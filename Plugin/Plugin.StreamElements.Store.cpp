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
			m_pContextManager->ResetUpdated();

			list <std::string> sChannels = m_pContextManager->GetChannels();
			list <std::string> ::iterator sChannel;
			for (sChannel = sChannels.begin(); sChannel != sChannels.end(); sChannel++)
			{
				//mConnectionManager->LogMessage(sChannel->c_str());
				bool bStoreEnabled = false;

				std::string sCommand;
				sCommand.append("SE-SD.exe ");
				sCommand.append(sChannel->c_str());
				sCommand.append(" get ");
				sCommand.append(m_sAPI);

				std::array<char, 128> buffer;
				std::string sResult;
				std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(sCommand.c_str(), "r"), _pclose);
				if (!pipe) {
					throw std::runtime_error("popen() failed!");
				}
				while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
					sResult += buffer.data();
				}

				std::string arr[100];
				std::string search = " ";
				int spacePos;
				int currPos = 0;
				int k = 0;
				int prevPos = 0;

				do
				{
					spacePos = sResult.find(search, currPos);
					if (spacePos >= 0 && k < 100)
					{
						currPos = spacePos;
						arr[k] = sResult.substr(prevPos, currPos - prevPos);
						currPos++;
						prevPos = currPos;
						k++;
					}
					else
					{
						spacePos = -1;
					}
				} while (spacePos >= 0);
				arr[k] = sResult.substr(prevPos, sResult.length() - prevPos);

				int i = 0;
				int f = 0;

				while (i < k)
				{
					if (strcmp(arr[i].c_str(), "enabled") == 0)
					{
						int k = 0;
						std::string sItem;
						while (k < (i - f))
						{
							sItem.append(arr[f + k].c_str());
							if (k != (i - f - 1)) sItem.append(" ");
							k++;
						}
						std::string ssChannel = sChannel->c_str();
						CItem* pItem = m_pContextManager->GetItem(ssChannel, sItem);
						if (pItem)
						{
							if (atoi(arr[i + 1].c_str()) > 0)
							{
								if (atoi(arr[i + 2].c_str()) == 0)
								{
									mConnectionManager->SetImage("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAADCSURBVHhe7dwhEsIwFADR0DPV13LkWjx3ahGRhPXMeybfZueLqDyO5/saLG3zZEGgIFAQKAgUBAoChXwHvc59Tv/pc/85fWeDgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQK/g8KNigIFAQKAgWBgkBBoJ/GuAFOPAqEPozxcQAAAABJRU5ErkJggg==", pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
									std::string sTitle = pItem->GetDisplay();
									mConnectionManager->SetTitle(sTitle, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
								}
								else
								{
									mConnectionManager->SetImage("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAADDSURBVHhe7dyxDcIwEEBRk1EyTyRmjcQ8rJJQuMT8Hr3X+Fp/XeHKj/08rsHSNk8WBAoCBYGCQEGgIFDId9D7+ZrTf/rcf07f2aAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYL/g4INCgIFgYJAQaAgUBDopzFudu4J6Pf08/IAAAAASUVORK5CYII=", pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
									std::string sTitle = pItem->GetDisplay();
									sTitle.append("\n");
									sTitle.append(std::to_string(atoi(arr[i + 2].c_str())));
									mConnectionManager->SetTitle(sTitle, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
								}
							}
							else
							{
								mConnectionManager->SetImage("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAADDSURBVHhe7dyxDcIwEEBRk1EyTyRmjcQ8rJJQuMT8Hr3X+Fp/XeHKj/08rsHSNk8WBAoCBYGCQEGgIFDId9D7+ZrTf/rcf07f2aAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYL/g4INCgIFgYJAQaAgUBDopzFudu4J6Pf08/IAAAAASUVORK5CYII=", pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
								std::string sTitle = ReformDisplay(pItem->GetDisplay());
								mConnectionManager->SetTitle(sTitle, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
							}
							pItem->SetUpdated();
							bStoreEnabled = true;
						}

						f = i + 3;
						i = i + 3;
					}
					else if (strcmp(arr[i].c_str(), "disabled") == 0)
					{
						int k = 0;
						std::string sItem;
						while (k < (i - f))
						{
							sItem.append(arr[f + k].c_str());
							if (k != (i - f - 1)) sItem.append(" ");
							k++;
						}
						std::string ssChannel = sChannel->c_str();
						CItem* pItem = m_pContextManager->GetItem(ssChannel, sItem);
						if (pItem)
						{
							mConnectionManager->SetImage("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAADDSURBVHhe7dyhEcMwEABBxS2YGqb/igxDU4MdIBjleGaX6KluHgjp8T6e12BpmycLAgWBgkBBoCBQECjkO2h/nXP6T5/7z+k7GxQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUPB/ULBBQaAgUBAoCBQECgL9NMYNUZoKEgTQxbkAAAAASUVORK5CYII=", pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
							std::string sTitle = pItem->GetDisplay();
							mConnectionManager->SetTitle(sTitle, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
							pItem->SetUpdated();
						}
						f = i + 1;
						i++;
					}
					else { i++; }
				}
				// update any close store buttons
				list < CStore* > ::iterator iter;
				for (iter = m_pContextManager->IterStoreBegin(); iter != m_pContextManager->IterStoreEnd(); iter++)
				{
					CStore* pStore = (*iter);
					if (strcmp(sChannel->c_str(), pStore->GetChannel().c_str()) == 0)
					{
						if (bStoreEnabled)
						{
							mConnectionManager->SetImage("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAADDSURBVHhe7dyxDcIwEEBRk1EyTyRmjcQ8rJJQuMT8Hr3X+Fp/XeHKj/08rsHSNk8WBAoCBYGCQEGgIFDId9D7+ZrTf/rcf07f2aAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYL/g4INCgIFgYJAQaAgUBDopzFudu4J6Pf08/IAAAAASUVORK5CYII=", pStore->GetContext(), kESDSDKTarget_HardwareAndSoftware);
						}
						else
						{
							mConnectionManager->SetImage("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAADDSURBVHhe7dyhEcMwEABBxS2YGqb/igxDU4MdIBjleGaX6KluHgjp8T6e12BpmycLAgWBgkBBoCBQECjkO2h/nXP6T5/7z+k7GxQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUPB/ULBBQaAgUBAoCBQECgL9NMYNUZoKEgTQxbkAAAAASUVORK5CYII=", pStore->GetContext(), kESDSDKTarget_HardwareAndSoftware);
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
					mConnectionManager->SetImage("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAAC6SURBVHhe7dyxEYAgEABBtAD7r9MG1IAULnd2E0j/5gMijucez2DpnCcLAgWBgkBBoCBQECjkO+i45uWnvvm3bFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQMH/QcEGBYGCQEGgIFAQKAi0NcYL/mELXpzOcG4AAAAASUVORK5CYII=", pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
					std::string sTitle = ReformDisplay(pItem->GetDisplay());
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
		if (strcmp(inAction.c_str(), kActionNameItem) == 0)
		{
			CItem* pItem = m_pContextManager->GetItem(inContext);
			if (pItem)
			{
				std::string sCommand;
				sCommand.append("SE-SD.exe ");
				sCommand.append(pItem->GetItem());
				sCommand.append(" update ");
				sCommand.append(pItem->GetChannel());
				sCommand.append(" ");
				sCommand.append(m_sAPI);


				if (atoi(pItem->GetAmount().c_str()) > 0)
				{
					sCommand.append(" ");
					sCommand.append(pItem->GetAmount());
				}

				std::string sTitle = pItem->GetDisplay();

				int iResult = system(sCommand.c_str());
				if (iResult == 1)
				{
					//purple
					mConnectionManager->SetImage("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAADCSURBVHhe7dyxEcIwEABB4e7cg6nO9EB5NoFCxOXMbqJPdfOBIj3O/XUNlrZ5siBQECgIFAQKAgWBQr6Dnu9jTv/pc/85fWeDgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQK/g8KNigIFAQKAgWBgkBBoJ/GuAHXnAs70NuowgAAAABJRU5ErkJggg==", pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
					mConnectionManager->SetTitle(sTitle, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
				}
				else if (iResult == 2)
				{
					mConnectionManager->SetImage("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAADDSURBVHhe7dyhEcMwEABBxS2YGqb/igxDU4MdIBjleGaX6KluHgjp8T6e12BpmycLAgWBgkBBoCBQECjkO2h/nXP6T5/7z+k7GxQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUPB/ULBBQaAgUBAoCBQECgL9NMYNUZoKEgTQxbkAAAAASUVORK5CYII=", pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
					mConnectionManager->SetTitle(sTitle, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
				}
				else if (iResult == 3)
				{ //green
					mConnectionManager->SetImage("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAADDSURBVHhe7dyxDcIwEEBRk1EyTyRmjcQ8rJJQuMT8Hr3X+Fp/XeHKj/08rsHSNk8WBAoCBYGCQEGgIFDId9D7+ZrTf/rcf07f2aAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYL/g4INCgIFgYJAQaAgUBDopzFudu4J6Pf08/IAAAAASUVORK5CYII=", pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
					if (atoi(pItem->GetAmount().c_str()) > 0)
					{
						sTitle.append("\n");
						sTitle.append(pItem->GetAmount());
					}
					mConnectionManager->SetTitle(sTitle, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
				}
				else
				{// yellow 
					mConnectionManager->SetImage("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAAC6SURBVHhe7dyxEYAgEABBtAD7r9MG1IAULnd2E0j/5gMijucez2DpnCcLAgWBgkBBoCBQECjkO+i45uWnvvm3bFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQMH/QcEGBYGCQEGgIFAQKAi0NcYL/mELXpzOcG4AAAAASUVORK5CYII=", pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
					mConnectionManager->SetTitle(sTitle, pItem->GetContext(), kESDSDKTarget_HardwareAndSoftware);
				}
			}
		}
		else if (strcmp(inAction.c_str(), kActionNameClose) == 0)
		{
			//do something
			CStore* pStore = m_pContextManager->GetStore(inContext);
			if (pStore)
			{
				std::string sCommand;
				sCommand.append("SE-SD.exe ");
				sCommand.append(pStore->GetChannel());
				sCommand.append(" close ");
				sCommand.append(m_sAPI);

				int iResult = system(sCommand.c_str());
				if (iResult == 1)
				{
					mConnectionManager->SetImage("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAADDSURBVHhe7dyhEcMwEABBxS2YGqb/igxDU4MdIBjleGaX6KluHgjp8T6e12BpmycLAgWBgkBBoCBQECjkO2h/nXP6T5/7z+k7GxQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUPB/ULBBQaAgUBAoCBQECgL9NMYNUZoKEgTQxbkAAAAASUVORK5CYII=", pStore->GetContext(), kESDSDKTarget_HardwareAndSoftware);
				}
				else
				{
					mConnectionManager->SetImage("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAAC6SURBVHhe7dyxEYAgEABBtAD7r9MG1IAULnd2E0j/5gMijucez2DpnCcLAgWBgkBBoCBQECjkO+i45uWnvvm3bFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQMH/QcEGBYGCQEGgIFAQKAi0NcYL/mELXpzOcG4AAAAASUVORK5CYII=", pStore->GetContext(), kESDSDKTarget_HardwareAndSoftware);
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
}

void StreamElementsStore::WillDisappearForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID)
{

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
}

void StreamElementsStore::DidReceiveSettings(const std::string& inAction, const std::string& inContext, const json& inPayload, const std::string& inDeviceID)
{
	//std::string sSerialized = inPayload.dump();
	//mConnectionManager->LogMessage(inAction);
	if (strcmp(inAction.c_str(), kActionNameItem) == 0)
	{
		CItem* pItem = m_pContextManager->GetItem(inContext);
		if (!pItem)
		{
			//mConnectionManager->LogMessage("dr creating new context");
			pItem = m_pContextManager->AddItem(inContext, inPayload);
			//mConnectionManager->LogMessage("item");
			//mConnectionManager->LogMessage(pContext->GetItem());
			pItem->SetDisplay(ReformDisplay(pItem->GetDisplay()));
			mConnectionManager->SetTitle(pItem->GetDisplay(), inContext, kESDSDKTarget_HardwareAndSoftware);
		}
		else
		{
			//mConnectionManager->LogMessage("dr updating existing context");
			pItem->UpdateSettings(inPayload);
			//mConnectionManager->LogMessage("item");
			//mConnectionManager->LogMessage(pContext->GetItem());
			pItem->SetDisplay(ReformDisplay(pItem->GetDisplay()));
			mConnectionManager->SetTitle(pItem->GetDisplay(), inContext, kESDSDKTarget_HardwareAndSoftware);

		}
	}
	else if (strcmp(inAction.c_str(), kActionNameClose) == 0)
	{
		CStore* pStore = m_pContextManager->GetStore(inContext);
		if (!pStore)
		{
			//mConnectionManager->LogMessage("dr creating new context");
			pStore = m_pContextManager->AddStore(inContext, inPayload);
			//mConnectionManager->LogMessage("item");
			//mConnectionManager->LogMessage(pContext->GetItem());
		}
		else
		{
			//mConnectionManager->LogMessage("dr updating existing context");
			pStore->UpdateSettings(inPayload);
			//mConnectionManager->LogMessage("item");
			//mConnectionManager->LogMessage(pContext->GetItem());
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