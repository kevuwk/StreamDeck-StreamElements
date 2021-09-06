#include "pch.h"
#include "CContextManager.h"


CItem* CContextManager::AddItem(std::string inContext, const json& inPayload)
{
    CItem* pContext = new CItem(inContext, inPayload);
    if (pContext)
    {
        m_Items.push_back(pContext);
        return pContext;
    }
    return NULL;
}

CStore* CContextManager::AddStore(std::string inContext, const json& inPayload)
{
    CStore* pStore = new CStore(inContext, inPayload);
    if (pStore)
    {
        m_Stores.push_back(pStore);
        return pStore;
    }
    return NULL;
}

CCost* CContextManager::AddCost(std::string inContext, const json& inPayload)
{
    CCost* pCost = new CCost(inContext, inPayload);
    if (pCost)
    {
        m_Costs.push_back(pCost);
        return pCost;
    }
    return NULL;
}

CItem* CContextManager::GetItem(std::string inContext)
{
    list < CItem* > ::iterator iter;
    for (iter = m_Items.begin(); iter != m_Items.end(); iter++)
    {
        if (strcmp((*iter)->GetContext().c_str(), inContext.c_str()) == 0)
        {
            return *iter;
        }
    }
    return NULL;
}

CItem* CContextManager::GetItem(std::string sChannel, std::string sItem)
{
    list < CItem* > ::iterator iter;
    for (iter = m_Items.begin(); iter != m_Items.end(); iter++)
    {
        if (strcmp((*iter)->GetChannel().c_str(), sChannel.c_str()) == 0)
        {
            if (strcmp((*iter)->GetItem().c_str(), sItem.c_str()) == 0)
            {
                return *iter;
            }
        }
    }
    return NULL;
}

CStore* CContextManager::GetStore(std::string inContext)
{
    list < CStore* > ::iterator iter;
    for (iter = m_Stores.begin(); iter != m_Stores.end(); iter++)
    {
        if (strcmp((*iter)->GetContext().c_str(), inContext.c_str()) == 0)
        {
            return *iter;
        }
    }
    return NULL;
}

CCost* CContextManager::GetCost(std::string inContext)
{
    list < CCost* > ::iterator iter;
    for (iter = m_Costs.begin(); iter != m_Costs.end(); iter++)
    {
        if (strcmp((*iter)->GetContext().c_str(), inContext.c_str()) == 0)
        {
            return *iter;
        }
    }
    return NULL;
}

CCost* CContextManager::GetCost(std::string sChannel, std::string sItem)
{
    list < CCost* > ::iterator iter;
    for (iter = m_Costs.begin(); iter != m_Costs.end(); iter++)
    {
        if (strcmp((*iter)->GetChannel().c_str(), sChannel.c_str()) == 0)
        {
            if (strcmp((*iter)->GetItem().c_str(), sItem.c_str()) == 0)
            {
                return *iter;
            }
        }
    }
    return NULL;
}

void CContextManager::ResetUpdated()
{
    list < CItem* > ::iterator iter;
    for (iter = m_Items.begin(); iter != m_Items.end(); iter++)
    {
        (*iter)->ResetUpdated();
    }
}

list <std::string> CContextManager::GetChannels ( void )
{
    list <std::string> sChannels;
    list < CItem* > ::iterator iter;
    for (iter = m_Items.begin(); iter != m_Items.end(); iter++)
    {
        if ((*iter)->GetChannel() != "")
        {
            bool bExists = false;
            // is unique?
            list <std::string> ::iterator sIter;
            for (sIter = sChannels.begin(); sIter != sChannels.end(); sIter++)
            {
                if (strcmp((*iter)->GetChannel().c_str(), (sIter)->c_str()) == 0)
                {
                    bExists = true;
                }
            }
            if (!bExists) { sChannels.push_back((*iter)->GetChannel()); }
        }
    }
    list < CCost* > ::iterator iterc;
    for (iterc = m_Costs.begin(); iterc != m_Costs.end(); iterc++)
    {
        if ((*iterc)->GetChannel() != "")
        {
            bool bExists = false;
            // is unique?
            list <std::string> ::iterator sIter;
            for (sIter = sChannels.begin(); sIter != sChannels.end(); sIter++)
            {
                if (strcmp((*iterc)->GetChannel().c_str(), (sIter)->c_str()) == 0)
                {
                    bExists = true;
                }
            }
            if (!bExists) { sChannels.push_back((*iter)->GetChannel()); }
        }
    }
    return sChannels;
}

bool CContextManager::Exists(CItem* pContext)
{
    list < CItem* > ::iterator iter = m_Items.begin();
    for (; iter != m_Items.end(); iter++)
    {
        if ((*iter) == pContext)
            return true;
    }
    return false;
}


void CContextManager::Remove(CItem* pItem)
{
    m_Items.remove(pItem);
    delete pItem;
}

void CContextManager::Remove(CStore* pStore)
{
    m_Stores.remove(pStore);
    delete pStore;
}

void CContextManager::Remove(CCost* pCost)
{
    m_Costs.remove(pCost);
    delete pCost;
}

void CContextManager::RemoveAll(void)
{
    list < CItem* > ::iterator iter;
    for (iter = m_Items.begin(); iter != m_Items.end(); iter++)
    {
        delete* iter;
    }
    m_Items.clear();

    list < CStore* > ::iterator iters;
    for (iters = m_Stores.begin(); iters != m_Stores.end(); iters++)
    {
        delete* iters;
    }
    m_Stores.clear();

    list < CCost* > ::iterator iterc;
    for (iterc = m_Costs.begin(); iterc != m_Costs.end(); iterc++)
    {
        delete* iterc;
    }
    m_Costs.clear();
}