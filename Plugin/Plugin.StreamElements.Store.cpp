//==============================================================================
/**
@file       Plugin.StreamElements.Store.cpp

@brief      StreamElements Store plugin

**/
//==============================================================================

#include "Plugin.StreamElements.Store.h"
#include <atomic>


#include "Common/ESDConnectionManager.h"
#include <fstream>

#include <..\curl\curl.h>

using json = nlohmann::json;


#pragma warning(disable : 4996)

    std::size_t callback(
        const char* in,
        std::size_t size,
        std::size_t num,
        std::string* out)
    {
        const std::size_t totalBytes(size * num);
        out->append(in, totalBytes);
        return totalBytes;
    }

int iTimer = 0;

class CallBackTimer
{
public:
    CallBackTimer() :_execute(false) { }

    ~CallBackTimer()
    {
        if( _execute.load(std::memory_order_acquire) )
        {
            stop();
        };
    }

    void stop()
    {
        _execute.store(false, std::memory_order_release);
        if(_thd.joinable())
            _thd.join();
    }

    void start(int interval, std::function<void(void)> func)
    {
        if(_execute.load(std::memory_order_acquire))
        {
            stop();
        };
        _execute.store(true, std::memory_order_release);
        _thd = std::thread([this, interval, func]()
        {
            while (_execute.load(std::memory_order_acquire))
            {
                func();
                std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            }
        });
    }

    bool is_running() const noexcept
    {
        return (_execute.load(std::memory_order_acquire) && _thd.joinable());
    }

private:
    std::atomic<bool> _execute;
    std::thread _thd;
};

StreamElementsStore::StreamElementsStore()
{
	m_sAPI = "";
	m_pContextManager = new CContextManager();

	mTimer = new CallBackTimer();
	mTimer->start(1000, [this]()
	{
		this->UpdateTimer();
	});
}

StreamElementsStore::~StreamElementsStore()
{
	if(mTimer != nullptr)
	{
		mTimer->stop();
		
		delete mTimer;
		mTimer = nullptr;
	}
	
	if(m_pContextManager != nullptr)
	{
		delete m_pContextManager;
		m_pContextManager = nullptr;
	}
}

void StreamElementsStore::UpdateTimer()
{
	//
	// Warning: UpdateTimer() is running in the timer thread
	//
	if (m_sAPI.length() == 0)
	{
		if (mConnectionManager != nullptr)
		{
			mConnectionManager->GetGlobalSettings();
		}
	}
	if (iTimer == 1)
	{
		if (mConnectionManager != nullptr)
		{
			//mConnectionManager->LogMessage("u1");
			m_pContextManager->ResetUpdated();

			list <std::string> sChannels = m_pContextManager->GetChannels();
			list <std::string> ::iterator sChannel;
			for (sChannel = sChannels.begin(); sChannel != sChannels.end(); sChannel++)
			{
				bool bStoreEnabled = false;

				std::string sURL = "https://api.streamelements.com/kappa/v2/store/";
				sURL.append(sChannel->c_str());
				sURL.append("/items/?limit=&offset=");
				std::unique_ptr<std::string> httpData(APIGet(sURL));
				if (httpData)
				{
					auto parsed_json = json::parse(*httpData.get());
					auto jBegin = parsed_json.begin();
					nlohmann::detail::iter_impl<nlohmann::json> iter;
					for (iter = parsed_json.begin(); iter != parsed_json.end(); iter++)
					{
						json jsonData = *iter;
						std::string sItem = jsonData["name"];
						bool bEnabled = jsonData["enabled"];
						int iTotal = jsonData["quantity"]["total"];
						int iCurrent = -1;
						if (!jsonData["quantity"]["current"].empty())
						{
							iCurrent = jsonData["quantity"]["current"];
						}
						std::string sID = jsonData["_id"];
						bool bSubscribers = jsonData["subscriberOnly"];

						// find store item
						std::string ssChannel = sChannel->c_str();
						CItem* pItem = m_pContextManager->GetItem(ssChannel, sItem);
						if (pItem)
						{
							pItem->SetID(sID);
							pItem->SetEnabled(bEnabled);
							pItem->SetCost(jsonData["cost"]);
							pItem->SetDescription(jsonData["description"]);
							pItem->SetSubscribers(bSubscribers);
							if (bEnabled)
							{
								if (iTotal > 0)
								{
									if (iCurrent == 0)
									{
										mConnectionManager->SetImage(kButtonColourBlue, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
										std::string sTitle = pItem->GetDisplay();
										mConnectionManager->SetTitle(sTitle, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
									}
									else
									{
										mConnectionManager->SetImage(kButtonColourGreen, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
										std::string sTitle = pItem->GetDisplay();
										sTitle.append("\n");
										sTitle.append(std::to_string(iCurrent));
										mConnectionManager->SetTitle(sTitle, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
									}
								}
								else
								{
									mConnectionManager->SetImage(kButtonColourGreen, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
									std::string sTitle = pItem->GetDisplay();
									mConnectionManager->SetTitle(sTitle, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
								}
								pItem->SetUpdated();
								bStoreEnabled = true;
							}
							else
							{
								mConnectionManager->SetImage(kButtonColourRed, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
								std::string sTitle = pItem->GetDisplay();
								mConnectionManager->SetTitle(sTitle, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
								pItem->SetUpdated();
							}
						}
						CCost* pCost = m_pContextManager->GetCost(ssChannel, sItem);
						if (pCost)
						{
							pCost->SetID(sID);
							pCost->SetEnabled(bEnabled);
							pCost->SetCost(jsonData["cost"]);
							pCost->SetDescription(jsonData["description"]);
							pCost->SetSubscribers(bSubscribers);
							std::string sTitle = pCost->GetDisplay();
							sTitle.append("\n");
							sTitle.append(std::to_string(pCost->GetCost()));
							mConnectionManager->SetTitle(sTitle, pCost->GetContext(), kESDSDKTarget_HardwareAndSoftware);
						}
					}
				}
				list < CStore* > ::iterator iter;
				for (iter = m_pContextManager->IterStoreBegin(); iter != m_pContextManager->IterStoreEnd(); iter++)
				{
					CStore* pStore = (*iter);
					if (strcmp(sChannel->c_str(), pStore->GetChannel().c_str()) == 0)
					{
						if (bStoreEnabled)
						{
							mConnectionManager->SetImage(kButtonColourGreen, pStore->GetContext(), kESDSDKTarget_HardwareAndSoftware);
						}
						else
						{
							mConnectionManager->SetImage(kButtonColourRed, pStore->GetContext(), kESDSDKTarget_HardwareAndSoftware);
						}
					}
				}

			}
			

			list < CItem* > ::iterator iter;
			for (iter = m_pContextManager->IterItemBegin(); iter != m_pContextManager->IterItemEnd(); iter++)
			{
				CItem* pItem = (*iter);
				if (!pItem->GetUpdated())
				{
					mConnectionManager->SetImage(kButtonColourYellow, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
					std::string sTitle = pItem->GetDisplay();
					mConnectionManager->SetTitle(sTitle, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
				}
			}
		}
		iTimer = 0;
	}
	iTimer++;
}

void StreamElementsStore::KeyDownForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID)
{
	if (mConnectionManager != nullptr)
	{
		//mConnectionManager->LogMessage("k1");
		if (strcmp(inAction.c_str(), kActionNameItem) == 0)
		{
			CItem* pItem = m_pContextManager->GetItem(inContext);
			if (pItem)
			{
				std::string sID = pItem->GetID();
				if (sID != "")
				{
					bool bEnabled = !pItem->GetEnabled();
					int iCost = pItem->GetCost();

					std::string sUpdate;
					sUpdate.append("{");

					int iTotal = atoi(pItem->GetAmount().c_str());
					if (iTotal >= 0)
					{
						sUpdate.append("\"quantity\":{\"total\":");
						sUpdate.append(std::to_string(iTotal));
						sUpdate.append(",\"current\":");
						sUpdate.append(std::to_string(iTotal));
						sUpdate.append("},");
					}
					else
					{
						sUpdate.append("\"quantity\":{\"total\":-1,\"current\":-1},");
					}
					if (bEnabled) { sUpdate.append("\"enabled\":true,\"name\":\""); }
					else { sUpdate.append("\"enabled\":false,\"name\":\""); }

					sUpdate.append(pItem->GetItem());
					sUpdate.append("\",\"cost\":");
					sUpdate.append(std::to_string(iCost));
					sUpdate.append(",\"description\":\"");
					sUpdate.append(pItem->GetDescription());
					
					if (pItem->GetSubscribers())
					{
						sUpdate.append("\",\"subscriberOnly\":true");
					}
					else
					{
						sUpdate.append("\",\"subscriberOnly\":false");
					}

					sUpdate.append("}");

					std::string sURL = "https://api.streamelements.com/kappa/v2/store/";
					sURL.append(pItem->GetChannel());
					sURL.append("/items/");
					sURL.append(sID);

					long httpCodep = APIPut(sURL, sUpdate);
					std::string sTitle = pItem->GetDisplay();
					if (httpCodep == 200)
					{

						if (bEnabled)
						{
							mConnectionManager->SetImage(kButtonColourGreen, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
							if (atoi(pItem->GetAmount().c_str()) > 0)
							{
								sTitle.append("\n");
								sTitle.append(pItem->GetAmount());
							}
							mConnectionManager->SetTitle(sTitle, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
							pItem->SetEnabled(true);
						}
						else
						{
							mConnectionManager->SetImage(kButtonColourRed, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
							mConnectionManager->SetTitle(sTitle, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
							pItem->SetEnabled(false);
						}
					}
					else
					{
							
						mConnectionManager->SetImage(kButtonColourPurple, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
						mConnectionManager->SetTitle(sTitle, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
					}
				}
			}
		}
		else if (strcmp(inAction.c_str(), kActionNameClose) == 0)
		{
			CStore* pStore = m_pContextManager->GetStore(inContext);
			if (pStore)
			{
				list < CItem* > ::iterator iter;
				for (iter = m_pContextManager->IterItemBegin(); iter != m_pContextManager->IterItemEnd(); iter++)
				{
					CItem* pItem = (*iter);
					if (strcmp(pStore->GetChannel().c_str(), pItem->GetChannel().c_str()) == 0)
					{
						if (pItem->GetEnabled())
						{
							bool bEnabled = !pItem->GetEnabled();
							int iCost = pItem->GetCost();

							std::string sUpdate;
							sUpdate.append("{");

							int iTotal = atoi(pItem->GetAmount().c_str());
							if (iTotal >= 0)
							{
								sUpdate.append("\"quantity\":{\"total\":");
								sUpdate.append(std::to_string(iTotal));
								sUpdate.append(",\"current\":");
								sUpdate.append(std::to_string(iTotal));
								sUpdate.append("},");
							}
							else
							{
								sUpdate.append("\"quantity\":{\"total\":-1,\"current\":-1},");
							}
							sUpdate.append("\"enabled\":false,\"name\":\"");

							sUpdate.append(pItem->GetItem());
							sUpdate.append("\",\"cost\":");
							sUpdate.append(std::to_string(iCost));
							sUpdate.append(",\"description\":\"");
							sUpdate.append(pItem->GetDescription());
							if (pItem->GetSubscribers())
							{
								sUpdate.append("\",\"subscriberOnly\":true");
							}
							else
							{
								sUpdate.append("\",\"subscriberOnly\":false");
							}

							sUpdate.append("}");

							std::string sURL = "https://api.streamelements.com/kappa/v2/store/";
							sURL.append(pItem->GetChannel());
							sURL.append("/items/");
							sURL.append(pItem->GetID());

							long httpCodep = APIPut(sURL, sUpdate);
							if (httpCodep == 200)
							{
								std::string sTitle = pItem->GetDisplay();
								mConnectionManager->SetImage(kButtonColourRed, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
								mConnectionManager->SetTitle(sTitle, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
								pItem->SetEnabled(false);
							}
						}
					}
					mConnectionManager->SetImage(kButtonColourRed, pStore->GetContext(), kESDSDKTarget_HardwareAndSoftware);
				}
			}
		}
		else if (strcmp(inAction.c_str(), kActionNameCost) == 0)
		{
			CCost* pCost = m_pContextManager->GetCost(inContext);
			if (pCost)
			{
				std::string sID = pCost->GetID();
				if (sID != "")
				{
					bool bEnabled = pCost->GetEnabled();
					std::string sCost = "";

					if (pCost->GetCost() == atoi(pCost->GetCost1().c_str()))
					{
						sCost = pCost->GetCost2();
					}
					else
					{
						sCost = pCost->GetCost1();
					}

					std::string sUpdate;
					sUpdate.append("{");
					if (bEnabled) { sUpdate.append("\"enabled\":true,\"name\":\""); }
					else { sUpdate.append("\"enabled\":false,\"name\":\""); }

					sUpdate.append(pCost->GetItem());
					sUpdate.append("\",\"cost\":");

						
					sUpdate.append(sCost);
					sUpdate.append(",\"description\":\"");
					sUpdate.append(pCost->GetDescription());
					if (pCost->GetSubscribers())
					{
						sUpdate.append("\",\"subscriberOnly\":true");
					}
					else
					{
						sUpdate.append("\",\"subscriberOnly\":false");
					}

					sUpdate.append("}");

					std::string sURL = "https://api.streamelements.com/kappa/v2/store/";
					sURL.append(pCost->GetChannel());
					sURL.append("/items/");
					sURL.append(sID);

					long httpCodep = APIPut(sURL, sUpdate);
					if (httpCodep == 200)
					{
						std::string sTitle = pCost->GetDisplay();
						sTitle.append("\n");
						sTitle.append(sCost);
						mConnectionManager->SetTitle(sTitle, pCost->GetContext(), kESDSDKTarget_HardwareAndSoftware);
					}
				}
			}
		}
	}

}

void StreamElementsStore::KeyUpForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID)
{
	// Nothing to do
}

void StreamElementsStore::WillAppearForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID)
{
	// just pass to DidReceiveSettings
	DidReceiveSettings(inAction, inContext, inPayload, inDeviceID);
	// send back store items?
}

void StreamElementsStore::WillDisappearForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID)
{
	//mConnectionManager->LogMessage("w1");
	// Remove the context
	//std::string sSerialized = inPayload.dump();
	//mConnectionManager->LogMessage(sSerialized);
	if (strcmp(inAction.c_str(), kActionNameItem) == 0)
	{
		CItem* pItem = m_pContextManager->GetItem(inContext);
		if (pItem)
		{
			m_pContextManager->Remove(pItem);
		}
	}
	else if (strcmp(inAction.c_str(), kActionNameClose) == 0)
	{
		CStore* pStore = m_pContextManager->GetStore(inContext);
		if (!pStore)
		{
			m_pContextManager->Remove(pStore);
		}
	}
	else if (strcmp(inAction.c_str(), kActionNameCost) == 0)
	{
		CCost* pCost = m_pContextManager->GetCost(inContext);
		if (!pCost)
		{
			m_pContextManager->Remove(pCost);
		}
	}
}

void StreamElementsStore::DeviceDidConnect(const std::string& inDeviceID, const json &inDeviceInfo)
{
	// Nothing to do
}

void StreamElementsStore::DeviceDidDisconnect(const std::string& inDeviceID)
{
	// Nothing to do
}

void StreamElementsStore::SendToPlugin(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID)
{
	// Nothing to do
	//mConnectionManager->LogMessage("test3");
	//std::string sSerialized = inPayload.dump();
	//mConnectionManager->LogMessage(sSerialized);
	//{"property_inspector":"propertyInspectorConnected"}

	for (auto it = inPayload.begin(); it != inPayload.end(); ++it)
	{
		std::stringstream buffer;
		buffer << it.key() << " : " << it.value();
		if (strcmp(it.key().c_str(), "property_inspector") == 0)
		{
			std::stringstream buffers;
			buffers << it.value();
			if (strcmp(buffers.str().substr(1, buffers.str().length() - 2).c_str(), "propertyInspectorConnected") == 0)
			{
				CItem* pItem = m_pContextManager->GetItem(inContext);
				if (pItem)
				{
					auto jsonItems = json::array();
					std::string sURL = "https://api.streamelements.com/kappa/v2/store/";
					sURL.append(pItem->GetChannel().c_str());
					sURL.append("/items/?limit=&offset=");
					std::unique_ptr<std::string> httpData(APIGet(sURL));
					if (httpData)
					{
						auto parsed_json = json::parse(*httpData.get());
						auto jBegin = parsed_json.begin();
						nlohmann::detail::iter_impl<nlohmann::json> iter;
						for (iter = parsed_json.begin(); iter != parsed_json.end(); iter++)
						{
							json jsonData = *iter;
							jsonItems.push_back(jsonData["name"]);
						}
					}
					std::string sAction = "itemList";
					mConnectionManager->SendToPropertyInspector(sAction, inContext, jsonItems);
				}
				CCost* pCost = m_pContextManager->GetCost(inContext);
				if (pCost)
				{
					auto jsonItems = json::array();
					std::string sURL = "https://api.streamelements.com/kappa/v2/store/";
					sURL.append(pCost->GetChannel().c_str());
					sURL.append("/items/?limit=&offset=");
					std::unique_ptr<std::string> httpData(APIGet(sURL));
					if (httpData)
					{
						auto parsed_json = json::parse(*httpData.get());
						auto jBegin = parsed_json.begin();
						nlohmann::detail::iter_impl<nlohmann::json> iter;
						for (iter = parsed_json.begin(); iter != parsed_json.end(); iter++)
						{
							json jsonData = *iter;
							jsonItems.push_back(jsonData["name"]);
						}
					}
					std::string sAction = "itemList";
					mConnectionManager->SendToPropertyInspector(sAction, inContext, jsonItems);
				}
			}
		}
	}
}

void StreamElementsStore::DidReceiveSettings(const std::string& inAction, const std::string& inContext, const json& inPayload, const std::string& inDeviceID)
{
	//std::string sSerialized = inPayload.dump();
	//mConnectionManager->LogMessage(inAction);
	//mConnectionManager->LogMessage("r1");
	if (strcmp(inAction.c_str(), kActionNameItem) == 0)
	{
		
		CItem* pItem = m_pContextManager->GetItem(inContext);
		if (!pItem)
		{
			pItem = m_pContextManager->AddItem(inContext, inPayload);
			pItem->SetDisplay(ReformDisplay(pItem->GetDisplay()));
			mConnectionManager->SetTitle(pItem->GetDisplay(), inContext, kESDSDKTarget_HardwareAndSoftware);
		}
		else
		{
			pItem->UpdateSettings(inPayload);
			pItem->SetDisplay(ReformDisplay(pItem->GetDisplay()));
			mConnectionManager->SetTitle(pItem->GetDisplay(), inContext, kESDSDKTarget_HardwareAndSoftware);
			if (pItem->GetItem().length() == 0)
			{
				auto jsonItems = json::array();
				std::string sURL = "https://api.streamelements.com/kappa/v2/store/";
				sURL.append(pItem->GetChannel().c_str());
				sURL.append("/items/?limit=&offset=");
				std::unique_ptr<std::string> httpData(APIGet(sURL));
				if (httpData)
				{
					auto parsed_json = json::parse(*httpData.get());
					auto jBegin = parsed_json.begin();
					nlohmann::detail::iter_impl<nlohmann::json> iter;
					for (iter = parsed_json.begin(); iter != parsed_json.end(); iter++)
					{
						json jsonData = *iter;
						jsonItems.push_back(jsonData["name"]);
					}
				}
				std::string sAction = "itemList";
				mConnectionManager->SendToPropertyInspector(sAction, inContext, jsonItems);
			}
		}
	}
	else if (strcmp(inAction.c_str(), kActionNameClose) == 0)
	{
		CStore* pStore = m_pContextManager->GetStore(inContext);
		if (!pStore)
		{
			pStore = m_pContextManager->AddStore(inContext, inPayload);
		}
		else
		{
			pStore->UpdateSettings(inPayload);
		}
	}
	else if (strcmp(inAction.c_str(), kActionNameCost) == 0)
	{
		CCost* pCost = m_pContextManager->GetCost(inContext);
		if (!pCost)
		{
			pCost = m_pContextManager->AddCost(inContext, inPayload);
			pCost->SetDisplay(ReformDisplay(pCost->GetDisplay()));
			mConnectionManager->SetTitle(pCost->GetDisplay(), inContext, kESDSDKTarget_HardwareAndSoftware);
		}
		else
		{
			pCost->UpdateSettings(inPayload);
			pCost->SetDisplay(ReformDisplay(pCost->GetDisplay()));
			mConnectionManager->SetTitle(pCost->GetDisplay(), inContext, kESDSDKTarget_HardwareAndSoftware);
			if (pCost->GetItem().length() == 0)
			{
				auto jsonItems = json::array();
				std::string sURL = "https://api.streamelements.com/kappa/v2/store/";
				sURL.append(pCost->GetChannel().c_str());
				sURL.append("/items/?limit=&offset=");
				std::unique_ptr<std::string> httpData(APIGet(sURL));
				if (httpData)
				{
					auto parsed_json = json::parse(*httpData.get());
					auto jBegin = parsed_json.begin();
					nlohmann::detail::iter_impl<nlohmann::json> iter;
					for (iter = parsed_json.begin(); iter != parsed_json.end(); iter++)
					{
						json jsonData = *iter;
						jsonItems.push_back(jsonData["name"]);
					}
				}
				std::string sAction = "itemList";
				mConnectionManager->SendToPropertyInspector(sAction, inContext, jsonItems);
			}
		}
	}
}

void StreamElementsStore::DidReceiveGlobalSettings(const json& inPayload)
{
	/*mConnectionManager->LogMessage("test1");
	std::string sSerialized = inPayload.dump();
	mConnectionManager->LogMessage(sSerialized);*/
	for (auto it = inPayload.begin(); it != inPayload.end(); ++it)
	{
		if (strcmp(it.key().c_str(), "settings") == 0)
		{
			for (auto its = it.value().begin(); its != it.value().end(); ++its)
			{
				std::stringstream buffers;
				if (strcmp(its.key().c_str(), "API") == 0)
				{
					//mConnectionManager->LogMessage("found API");
					buffers << its.value();
					m_sAPI = buffers.str().substr(1, buffers.str().length() - 2);
					//mConnectionManager->LogMessage(buffers.str().substr(1, buffers.str().length() - 2));
				}
			}
		}
	}
}


std::string StreamElementsStore::ReformDisplay(std::string sText)
{
	std::string arr[10];
	std::string search = "\\n";
	int spacePos;
	int currPos = 0;
	int k = 0;
	int prevPos = 0;

	do
	{
		spacePos = sText.find(search, currPos);
		if (spacePos >= 0 && k < 9)
		{
			currPos = spacePos+1;
			arr[k] = sText.substr(prevPos, currPos - prevPos -1);
			currPos++;
			prevPos = currPos;
			k++;
		}
		else
		{
			spacePos = -1;
		}
	} while (spacePos >= 0);
	arr[k] = sText.substr(prevPos, sText.length() - prevPos);
	int i = 0;
	std::string sReturn;
	while ( i <= k )
	{
		sReturn.append(arr[i]);
		if (i != k) { sReturn.append("\n"); }
		i++;
	}
	return sReturn;
}

long StreamElementsStore::APIPut (std::string sURL, std::string sUpdate)
{
	CURL* hndp = curl_easy_init();

	curl_easy_setopt(hndp, CURLOPT_URL, sURL.c_str());

	curl_easy_setopt(hndp, CURLOPT_CUSTOMREQUEST, "PUT");
	curl_easy_setopt(hndp, CURLOPT_POSTFIELDS, sUpdate.c_str());

	struct curl_slist* headersp = NULL;
	std::string sAuth = "authorization: Bearer ";
	sAuth.append(m_sAPI);
	headersp = curl_slist_append(headersp, "Accept: ");
	headersp = curl_slist_append(headersp, sAuth.c_str());
	headersp = curl_slist_append(headersp, "Content-Type: application/json");
	curl_easy_setopt(hndp, CURLOPT_HTTPHEADER, headersp);
	curl_easy_setopt(hndp, CURLOPT_WRITEFUNCTION, callback);

	long httpCodep(0);
	std::unique_ptr<std::string> httpDatap(new std::string());

	curl_easy_setopt(hndp, CURLOPT_WRITEDATA, httpDatap.get());

	curl_easy_perform(hndp);
	curl_easy_getinfo(hndp, CURLINFO_RESPONSE_CODE, &httpCodep);
	curl_easy_cleanup(hndp);

	return httpCodep;
}

std::unique_ptr<std::string> StreamElementsStore::APIGet (std::string sURL)
{
	CURL* hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(hnd, CURLOPT_URL, sURL.c_str());

	struct curl_slist* headers = NULL;
	headers = curl_slist_append(headers, "Accept: ");
	std::string sAuth = "authorization: Bearer ";
	sAuth.append(m_sAPI);
	headers = curl_slist_append(headers, sAuth.c_str());
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

	long httpCode(0);
	std::unique_ptr<std::string> httpData(new std::string());

	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, callback);

	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, httpData.get());

	curl_easy_perform(hnd);
	curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &httpCode);
	curl_easy_cleanup(hnd);
	if (httpCode == 200)
	{
		return httpData;
	}
	return NULL;
}