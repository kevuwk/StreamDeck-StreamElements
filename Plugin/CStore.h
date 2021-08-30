class CStore;

#ifndef __CSTORE_H
#define __CSTORE_H

#include <list>
#include <sstream>

using namespace std;

class CStore
{
public:

    CStore( std::string inContext, const json &inPayload);
    ~CStore(void) {};

    void                UpdateSettings(const json& payload);

    std::string         GetAction(void) { return m_sAction; }

    std::string         GetContext(void) { return m_sContext; }

    std::string         GetChannel(void) { return m_sChannel; }

    bool                GetUpdated(void) { return m_bUpdated; }
    void                ResetUpdated(void) { m_bUpdated = false; }
    void                SetUpdated(void) { m_bUpdated = true; }

private:

    std::string m_sAction;

    std::string m_sContext;

    std::string m_sChannel;

    bool m_bUpdated;
};

#endif
