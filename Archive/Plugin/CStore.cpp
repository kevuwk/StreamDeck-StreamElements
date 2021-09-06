#include "pch.h"
#include "CStore.h"


CStore::CStore(std::string inContext, const json& inPayload)
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

				if (strcmp(its.key().c_str(), "channelString") == 0)
				{
					buffers << its.value();
					m_sChannel = buffers.str().substr(1, buffers.str().length() - 2);
				}
			}
		}
	}
}

void CStore::UpdateSettings(const json& inPayload)
{
	for (auto it = inPayload.begin(); it != inPayload.end(); ++it) {
		std::stringstream buffer;
		buffer << it.key() << " : " << it.value();
		if (strcmp(it.key().c_str(), "settings") == 0)
		{
			for (auto its = it.value().begin(); its != it.value().end(); ++its)
			{
				std::stringstream buffers;

				if (strcmp(its.key().c_str(), "channelString") == 0)
				{
					buffers << its.value();
					m_sChannel = buffers.str().substr(1, buffers.str().length() - 2);
				}
			}
		}
	}
}