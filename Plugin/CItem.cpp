#include "pch.h"
#include "CItem.h"


CItem::CItem(std::string inContext, const json& inPayload)
{
	m_bUpdated = false;
	m_sContext = inContext;
	for (auto it = inPayload.begin(); it != inPayload.end(); ++it) {
		std::stringstream buffer;
		buffer << it.key() << " : " << it.value();
		if (strcmp(it.key().c_str(), "settings") == 0)
		{
			for (auto its = it.value().begin(); its != it.value().end(); ++its)
			{
				std::stringstream buffers;

				if (strcmp(its.key().c_str(), "amountString") == 0)
				{
					buffers << its.value();
					m_sAmount = buffers.str().substr(1, buffers.str().length() - 2);
				}
				else if (strcmp(its.key().c_str(), "channelString") == 0)
				{
					buffers << its.value();
					m_sChannel = buffers.str().substr(1, buffers.str().length() - 2);
				}
				else if (strcmp(its.key().c_str(), "displayString") == 0)
				{
					buffers << its.value();
					m_sDisplay = buffers.str().substr(1, buffers.str().length() - 2);
				}
				else if (strcmp(its.key().c_str(), "itemString") == 0)
				{
					buffers << its.value();
					m_sItem = buffers.str().substr(1, buffers.str().length() - 2);
				}
			}
		}
	}
}

void CItem::UpdateSettings(const json& inPayload)
{
	for (auto it = inPayload.begin(); it != inPayload.end(); ++it) {
		std::stringstream buffer;
		buffer << it.key() << " : " << it.value();
		if (strcmp(it.key().c_str(), "settings") == 0)
		{
			for (auto its = it.value().begin(); its != it.value().end(); ++its)
			{
				std::stringstream buffers;


				if (strcmp(its.key().c_str(), "amountString") == 0)
				{
					buffers << its.value();
					m_sAmount = buffers.str().substr(1, buffers.str().length() - 2);
				}
				else if (strcmp(its.key().c_str(), "channelString") == 0)
				{
					buffers << its.value();
					m_sChannel = buffers.str().substr(1, buffers.str().length() - 2);
				}
				else if (strcmp(its.key().c_str(), "displayString") == 0)
				{
					buffers << its.value();
					m_sDisplay = buffers.str().substr(1, buffers.str().length() - 2);
				}
				else if (strcmp(its.key().c_str(), "itemString") == 0)
				{
					buffers << its.value();
					m_sItem = buffers.str().substr(1, buffers.str().length() - 2);
				}
			}
		}
	}
}