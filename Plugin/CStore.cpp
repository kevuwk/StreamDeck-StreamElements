#include "pch.h"
#include "CStore.h"


CStore::CStore(std::string inContext, const json& inPayload)
{
	m_bUpdated = false;
	m_sContext = inContext;
	for (auto it = inPayload.begin(); it != inPayload.end(); ++it) {
		std::stringstream buffer;
		buffer << it.key() << " : " << it.value();
		//mConnectionManager->LogMessage(buffer.str());
		if (strcmp(it.key().c_str(), "settings") == 0)
		{
			for (auto its = it.value().begin(); its != it.value().end(); ++its)
			{
				std::stringstream buffers;


				if (strcmp(its.key().c_str(), "amountString") == 0)
				{
					buffers << its.value();
					//mConnectionManager->LogMessage("Found amountString");
					//mConnectionManager->LogMessage(buffers.str());
					//mConnectionManager->LogMessage(buffers.str().substr(1, buffers.str().length() - 2).c_str());
					m_sAmount = buffers.str().substr(1, buffers.str().length() - 2);
				}
				else if (strcmp(its.key().c_str(), "channelString") == 0)
				{
					buffers << its.value();
					//mConnectionManager->LogMessage("Found channelString");
					//mConnectionManager->LogMessage(buffers.str());
					//mConnectionManager->LogMessage(buffers.str().substr(1, buffers.str().length() - 2).c_str());
					m_sChannel = buffers.str().substr(1, buffers.str().length() - 2);
				}
				else if (strcmp(its.key().c_str(), "displayString") == 0)
				{
					buffers << its.value();
					//mConnectionManager->LogMessage("Found displayString");
					//mConnectionManager->LogMessage(buffers.str());
					//mConnectionManager->LogMessage(buffers.str().substr(1, buffers.str().length() - 2).c_str());
					m_sDisplay = buffers.str().substr(1, buffers.str().length() - 2);
				}
				else if (strcmp(its.key().c_str(), "itemString") == 0)
				{
					buffers << its.value();
					//mConnectionManager->LogMessage("Found itemString");
					//mConnectionManager->LogMessage(buffers.str());
					//mConnectionManager->LogMessage(buffers.str().substr(1, buffers.str().length() - 2).c_str());
					m_sItem = buffers.str().substr(1, buffers.str().length() - 2);
				}
				//else { buffers << its.key() << " : " << its.value(); mConnectionManager->LogMessage(buffers.str()); }
			}
		}
	}
}

void CStore::UpdateSettings(const json& inPayload)
{
	for (auto it = inPayload.begin(); it != inPayload.end(); ++it) {
		std::stringstream buffer;
		buffer << it.key() << " : " << it.value();
		//mConnectionManager->LogMessage(buffer.str());
		if (strcmp(it.key().c_str(), "settings") == 0)
		{
			for (auto its = it.value().begin(); its != it.value().end(); ++its)
			{
				std::stringstream buffers;


				if (strcmp(its.key().c_str(), "amountString") == 0)
				{
					buffers << its.value();
					//mConnectionManager->LogMessage("Found amountString");
					//mConnectionManager->LogMessage(buffers.str());
					//mConnectionManager->LogMessage(buffers.str().substr(1, buffers.str().length() - 2).c_str());
					m_sAmount = buffers.str().substr(1, buffers.str().length() - 2);
				}
				else if (strcmp(its.key().c_str(), "channelString") == 0)
				{
					buffers << its.value();
					//mConnectionManager->LogMessage("Found channelString");
					//mConnectionManager->LogMessage(buffers.str());
					//mConnectionManager->LogMessage(buffers.str().substr(1, buffers.str().length() - 2).c_str());
					m_sChannel = buffers.str().substr(1, buffers.str().length() - 2);
				}
				else if (strcmp(its.key().c_str(), "displayString") == 0)
				{
					buffers << its.value();
					//mConnectionManager->LogMessage("Found displayString");
					//mConnectionManager->LogMessage(buffers.str());
					//mConnectionManager->LogMessage(buffers.str().substr(1, buffers.str().length() - 2).c_str());
					m_sDisplay = buffers.str().substr(1, buffers.str().length() - 2);
				}
				else if (strcmp(its.key().c_str(), "itemString") == 0)
				{
					buffers << its.value();
					//mConnectionManager->LogMessage("Found itemString");
					//mConnectionManager->LogMessage(buffers.str());
					//mConnectionManager->LogMessage(buffers.str().substr(1, buffers.str().length() - 2).c_str());
					m_sItem = buffers.str().substr(1, buffers.str().length() - 2);
				}
				//else { buffers << its.key() << " : " << its.value(); mConnectionManager->LogMessage(buffers.str()); }
			}
		}
	}
}