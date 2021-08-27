class CItem;

#ifndef __CITEM_H
#define __CITEM_H

//#include "common.h"
#include <list>
//#include <fstream>
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

    bool m_bUpdated;
};

#endif
