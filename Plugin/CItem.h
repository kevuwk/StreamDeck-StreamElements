class CItem;

#ifndef __CITEM_H
#define __CITEM_H

#include <list>
#include <sstream>

using namespace std;

class CItem
{
public:

    CItem( std::string inContext, const json &inPayload);
    ~CItem(void) {};

    void                UpdateSettings(const json& payload);

    std::string         GetAction(void) { return m_sAction; }

    std::string         GetContext(void) { return m_sContext; }

    void                SetDisplay(std::string sDisplay) { m_sDisplay = sDisplay; }
    std::string         GetDisplay(void) { return m_sDisplay; }

    std::string         GetChannel(void) { return m_sChannel; }

    std::string         GetItem(void) { return m_sItem; }

    std::string         GetAmount(void) { return m_sAmount; }

    void                SetID(std::string sID) { m_sID = sID; }
    std::string         GetID(void) { return m_sID; }

    void                SetEnabled(bool bEnabled) { m_bEnabled = bEnabled; }
    bool                GetEnabled(void) { return m_bEnabled; }

    void                SetCost(int iCost) { m_iCost = iCost; }
    int                 GetCost(void) { return m_iCost; }

    void                SetDescription(std::string sDescription) { m_sDescription = sDescription; }
    std::string         GetDescription(void) { return m_sDescription; }

    void                SetSubscribers(bool bSubscribers) { m_bSubscribers = bSubscribers; }
    bool                GetSubscribers(void) { return m_bSubscribers; }

    bool                GetUpdated(void) { return m_bUpdated; }
    void                ResetUpdated(void) { m_bUpdated = false; }
    void                SetUpdated(void) { m_bUpdated = true; }

private:

    std::string m_sAction;

    std::string m_sContext;

    std::string m_sDisplay;
    std::string m_sChannel;
    std::string m_sItem;
    std::string m_sAmount;

    std::string m_sID;
    bool m_bEnabled;
    int m_iCost;
    std::string m_sDescription;
    bool m_bSubscribers;

    bool m_bUpdated;
};

#endif
