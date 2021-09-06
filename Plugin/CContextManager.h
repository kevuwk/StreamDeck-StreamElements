class CContextManager;

#ifndef __CCONTEXTMANAGER_H
#define __CCONTEXTMANAGER_H

//#include "common.h"
//#include <windows.h>
//#include <stdio.h>
//#include <string.h>
//#include <conio.h>

#include "CItem.h"
#include "CStore.h"
#include "CCost.h"
#include <list>

class CContextManager
{
public:

	CContextManager(void) {};
	~CContextManager(void) { RemoveAll(); }

	CItem* AddItem(std::string inContext, const json& inPayload);
	CStore* AddStore(std::string inContext, const json& inPayload);
	CCost* AddCost(std::string inContext, const json& inPayload);
	
	CItem* GetItem(std::string inContext);
	CItem* GetItem(std::string sChannel, std::string sItem);

	CStore* GetStore(std::string inContext);

	CCost* GetCost(std::string inContext);
	CCost* GetCost(std::string sChannel, std::string sItem);

	list <std::string>					GetChannels(void);

	void								ResetUpdated(void);
	
	bool								Exists(CItem* pContext);

	void                                Remove(CItem* pItem);
	void                                Remove(CStore* pStore);
	void                                Remove(CCost* pCost);
	void                                RemoveAll(void);

	list < CItem* > ::iterator       IterItemBegin(void) { return m_Items.begin(); }
	list < CItem* > ::iterator       IterItemEnd(void) { return m_Items.end(); }

	list < CStore* > ::iterator       IterStoreBegin(void) { return m_Stores.begin(); }
	list < CStore* > ::iterator       IterStoreEnd(void) { return m_Stores.end(); }

	list < CCost* > ::iterator       IterCostBegin(void) { return m_Costs.begin(); }
	list < CCost* > ::iterator       IterCostEnd(void) { return m_Costs.end(); }

private:

	list < CItem* >						m_Items;
	list < CStore* >					m_Stores;
	list < CCost* >						m_Costs;

};

#endif