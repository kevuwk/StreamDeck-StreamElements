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

/*#define kActionNameItem  "plugin.test.test.item"
#define kActionNameClose "plugin.test.test.close"
#define kActionNameCost  "plugin.test.test.cost"*/

#define kActionNameItem  "plugin.streamelements.store.item"
#define kActionNameClose "plugin.streamelements.store.close"
#define kActionNameCost  "plugin.streamelements.store.cost"

#define kButtonColourBlue "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAADCSURBVHhe7dwhEsIwFADR0DPV13LkWjx3ahGRhPXMeybfZueLqDyO5/saLG3zZEGgIFAQKAgUBAoChXwHvc59Tv/pc/85fWeDgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQK/g8KNigIFAQKAgWBgkBBoJ/GuAFOPAqEPozxcQAAAABJRU5ErkJggg=="
#define kButtonColourGreen "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAADDSURBVHhe7dyxDcIwEEBRk1EyTyRmjcQ8rJJQuMT8Hr3X+Fp/XeHKj/08rsHSNk8WBAoCBYGCQEGgIFDId9D7+ZrTf/rcf07f2aAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYL/g4INCgIFgYJAQaAgUBDopzFudu4J6Pf08/IAAAAASUVORK5CYII="
#define kButtonColourRed "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAADDSURBVHhe7dyhEcMwEABBxS2YGqb/igxDU4MdIBjleGaX6KluHgjp8T6e12BpmycLAgWBgkBBoCBQECjkO2h/nXP6T5/7z+k7GxQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUBAoCBQECgIFgYJAQaAgUPB/ULBBQaAgUBAoCBQECgL9NMYNUZoKEgTQxbkAAAAASUVORK5CYII="
#define kButtonColourYellow "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAAC6SURBVHhe7dyxEYAgEABBtAD7r9MG1IAULnd2E0j/5gMijucez2DpnCcLAgWBgkBBoCBQECjkO+i45uWnvvm3bFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQEGgIFAQKAgUBAoCBYGCQMH/QcEGBYGCQEGgIFAQKAi0NcYL/mELXpzOcG4AAAAASUVORK5CYII="
#define kButtonColourPurple "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAABICAYAAABV7bNHAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAADCSURBVHhe7dyxEcIwEABB4e7cg6nO9EB5NoFCxOXMbqJPdfOBIj3O/XUNlrZ5siBQECgIFAQKAgWBQr6Dnu9jTv/pc/85fWeDgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQKAgWBgkBBoCBQECgIFAQK/g8KNigIFAQKAgWBgkBBoJ/GuAHXnAs70NuowgAAAABJRU5ErkJggg=="


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
