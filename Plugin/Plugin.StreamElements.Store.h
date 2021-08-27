//==============================================================================
/**
@file       Plugin.StreamElements.Store.h

@brief      StreamElements Store plugin

**/
//==============================================================================

#include "Common/ESDBasePlugin.h"
#include <mutex>
#include <json.hpp>
#include "CContextManager.h"

#define kActionNameItem  "plugin.streamelements.store.item"
#define kActionNameClose "plugin.streamelements.store.close"

class CallBackTimer;

class StreamElementsStore : public ESDBasePlugin
{
public:
	
	StreamElementsStore();
	virtual ~StreamElementsStore();
	
	void KeyDownForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID) override;
	void KeyUpForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID) override;
	
	void WillAppearForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID) override;
	void WillDisappearForAction(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID) override;
	
	void DeviceDidConnect(const std::string& inDeviceID, const json &inDeviceInfo) override;
	void DeviceDidDisconnect(const std::string& inDeviceID) override;
	
	void SendToPlugin(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID) override;

	void DidReceiveSettings(const std::string& inAction, const std::string& inContext, const json &inPayload, const std::string& inDeviceID) override;
	void DidReceiveGlobalSettings(const json &inPayload) override;

	

private:
	
	void UpdateTimer();
	std::string ReformDisplay(std::string sText);

	std::string m_sAPI;

	CContextManager* m_pContextManager;
	
	std::mutex mVisibleContextsMutex;
	std::set<std::string> mVisibleContexts;
	
	CallBackTimer *mTimer;
};
