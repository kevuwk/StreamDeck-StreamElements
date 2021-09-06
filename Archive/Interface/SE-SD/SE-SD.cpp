/*############################################################################################
############################### Should be built as x86 (WIN32) ###############################
############################################################################################*/

#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <json/json.h>
#include <fstream>

#pragma warning(disable : 4996)

namespace
{
    std::size_t callback(
        const char* in,
        std::size_t size,
        std::size_t num,
        std::string* out)
    {
        const std::size_t totalBytes(size * num);
        out->append(in, totalBytes);
        return totalBytes;
    }
}

int Get();
int Update(const char* szName);
int Close();


void WriteLog(const char* szText);


std::string sChannel;
std::string sAPI;
int iTotal = -1;

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR lpCmdLine, int cmdshow)
{
    std::string cmdline = lpCmdLine;

    std::string arr[10];
    std::string search = " ";
    int spacePos;
    int currPos = 0;
    int k = 0;
    int prevPos = 0;

    do
    {
        spacePos = cmdline.find(search, currPos);
        if (spacePos >= 0 && k < 9)
        {
            currPos = spacePos;
            arr[k] = cmdline.substr(prevPos, currPos - prevPos);
            currPos++;
            prevPos = currPos;
            k++;
        }
        else
        {
            spacePos = -1;
        }
    } while (spacePos >= 0);
    if (k < 7)
    {
        arr[k] = cmdline.substr(prevPos, cmdline.length() - prevPos);
    }
    std::string name;
    const char* szType = NULL;
    const char* szName = NULL;

    if (strcmp(arr[1].c_str(), "get") == 0)
    {
        sChannel = arr[0];
        szType = arr[1].c_str();
        sAPI = arr[2];

        if (szType == NULL) { return 0; }
        if (sChannel.length() == 0) { return 0; }
        if (sAPI.length() == 0) { return -1; }

        return Get();
    }
    else if (strcmp(arr[1].c_str(), "close") == 0)
    {
            sChannel = arr[0];
            szType = arr[1].c_str();
            sAPI = arr[2];

            if (szType == NULL) { return 0; }
            if (sChannel.length() == 0) { return 0; }
            if (sAPI.length() == 0) { return -1; }

            return Close();
    }
    else
    {
        int i = 1;
        int j = 0;
        int iToken = 0;
        while (i <= k)
        {
            if (strcmp(arr[i].c_str(), "update") == 0)
            {
                iToken = i + 1;
                szType = arr[i].c_str();
                while (j < i)
                {
                    if (j > 0) { name.append(" "); }
                    name.append(arr[j]);
                    j++;
                }
                szName = name.c_str();
            }
            i++;
        }
        sChannel = arr[iToken];
        iToken++;
        sAPI = arr[iToken];
        iToken++;
        if (arr[iToken].length() > 0)
        {
            iTotal = atoi(arr[iToken].c_str());
        }
    }


    if (szName == NULL) { return 0; }
    if (szType == NULL) { return 0; }
    if (sChannel.length() == 0) { return 0; }
    if (sAPI.length() == 0) { return -1; }


    if (strcmp(szType, "update") == 0)
    {
        return Update(szName);
    }
    
    return 0;
}


int Get()
{
    CURL* hnd = curl_easy_init();

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    std::string sURL = "https://api.streamelements.com/kappa/v2/store/";
    sURL.append(sChannel.c_str());
    sURL.append("/items/?limit=&offset=");
    curl_easy_setopt(hnd, CURLOPT_URL, sURL.c_str());

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Accept: ");
    std::string sAuth = "authorization: Bearer ";
    sAuth.append(sAPI.c_str());
    headers = curl_slist_append(headers, sAuth.c_str());
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);


    long httpCode(0);
    std::unique_ptr<std::string> httpData(new std::string());

    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, callback);

    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, httpData.get());

    curl_easy_perform(hnd);
    curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &httpCode);

    if (httpCode == 200)
    {
        Json::Value jsonData;
        Json::Reader jsonReader;

        if (jsonReader.parse(*httpData.get(), jsonData))
        {
            auto jBegin = jsonData.begin();

            Json::Value::iterator iter;
            for (iter = jsonData.begin(); iter != jsonData.end(); iter++)
            {
                Json::Value vMember = *iter;
                if (vMember)
                {
                    if (vMember.isMember("_id"))
                    {
                        if (vMember.isMember("name"))
                        {
                            std::string sName(vMember["name"].asString());
                            std::cout << sName.c_str() << " ";
                            if (vMember["enabled"].asBool())
                            {
                                std::cout << "enabled" << " ";
                                if (vMember.isMember("quantity"))
                                {
                                    std::cout << vMember["quantity"]["total"].asString() << " ";
                                    std::cout << vMember["quantity"]["current"].asString() << " ";
                                }
                            }
                            else
                            {
                                std::cout << "disabled" << " ";
                            }
                        }
                    }
                }
            }
        }
        else
        {
            return 0;
        }
    }
    return 0;
}

int Update(const char* szName)
{
    CURL* hnd = curl_easy_init();

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");

    std::string sURL = "https://api.streamelements.com/kappa/v2/store/";
    sURL.append(sChannel.c_str());
    sURL.append("/items/?limit=&offset=");
    curl_easy_setopt(hnd, CURLOPT_URL, sURL.c_str());

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Accept: ");
    std::string sAuth = "authorization: Bearer ";
    sAuth.append(sAPI.c_str());

    headers = curl_slist_append(headers, sAuth.c_str());
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    long httpCode(0);
    std::unique_ptr<std::string> httpData(new std::string());

    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, callback);

    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, httpData.get());

    curl_easy_perform(hnd);
    curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &httpCode);

    if (httpCode == 200)
    {
        Json::Value jsonData;
        Json::Reader jsonReader;

        if (jsonReader.parse(*httpData.get(), jsonData))
        {
            auto jBegin = jsonData.begin();

            Json::Value::iterator iter;
            for (iter = jsonData.begin(); iter != jsonData.end(); iter++)
            {
                Json::Value vMember = *iter;
                if (vMember)
                {
                    if (vMember.isMember("_id"))
                    {
                        if (vMember.isMember("name"))
                        {
                            std::string sName(vMember["name"].asString());
                            if (strcmp(sName.c_str(), szName) == 0)
                            {
                                if (vMember["enabled"].asBool())
                                {
                                    std::string sURL = "https://api.streamelements.com/kappa/v2/store/";
                                    sURL.append(sChannel.c_str());
                                    sURL.append("/items/");
                                    sURL.append(vMember["_id"].asString());

                                    CURL* hndp = curl_easy_init();
                                    curl_easy_setopt(hndp, CURLOPT_URL, sURL.c_str());

                                    curl_easy_setopt(hndp, CURLOPT_CUSTOMREQUEST, "PUT");
                                    std::string sUpdate;
                                    sUpdate.append("{");
                                    //if number provided
                                    if (iTotal > 0)
                                    {
                                        sUpdate.append("\"quantity\":{\"total\":");
                                        sUpdate.append(std::to_string(iTotal));
                                        sUpdate.append(",\"current\":");
                                        sUpdate.append(std::to_string(iTotal));
                                        sUpdate.append("},");
                                    }
                                    else if (iTotal < 0)
                                    {
                                        sUpdate.append("\"quantity\":{\"total\":-1,\"current\":-1},");
                                    }
                                    //
                                    sUpdate.append("\"enabled\":false,\"name\":\"");
                                    sUpdate.append(vMember["name"].asString());
                                    sUpdate.append("\",\"cost\":");
                                    sUpdate.append(vMember["cost"].asString());
                                    sUpdate.append(",\"description\":\"");
                                    sUpdate.append(vMember["description"].asString());
                                    sUpdate.append("\"}");
                                    curl_easy_setopt(hndp, CURLOPT_POSTFIELDS, sUpdate.c_str());

                                    struct curl_slist* headersp = NULL;
                                    headersp = curl_slist_append(headersp, "Accept: ");
                                    headersp = curl_slist_append(headersp, sAuth.c_str());
                                    headersp = curl_slist_append(headersp, "Content-Type: application/json");
                                    curl_easy_setopt(hndp, CURLOPT_HTTPHEADER, headersp);


                                    curl_easy_setopt(hndp, CURLOPT_WRITEFUNCTION, callback);

                                    long httpCodep(0);
                                    std::unique_ptr<std::string> httpDatap(new std::string());

                                    curl_easy_setopt(hndp, CURLOPT_WRITEDATA, httpDatap.get());

                                    curl_easy_perform(hndp);
                                    curl_easy_getinfo(hndp, CURLINFO_RESPONSE_CODE, &httpCodep);
                                    curl_easy_cleanup(hndp);
                                    if (httpCodep == 200)
                                    {
                                        return 2;
                                    }
                                    else
                                    {
                                        return 3;
                                    }
                                }
                                else
                                {
                                    std::string sURL = "https://api.streamelements.com/kappa/v2/store/";
                                    sURL.append(sChannel.c_str());
                                    sURL.append("/items/");
                                    sURL.append(vMember["_id"].asString());

                                    CURL* hndp = curl_easy_init();
                                    curl_easy_setopt(hndp, CURLOPT_URL, sURL.c_str());

                                    curl_easy_setopt(hndp, CURLOPT_CUSTOMREQUEST, "PUT");
                                    std::string sUpdate;
                                    sUpdate.append("{");
                                    //if number provided
                                    if (iTotal >= 0)
                                    {
                                        sUpdate.append("\"quantity\":{\"total\":");
                                        sUpdate.append(std::to_string(iTotal));
                                        sUpdate.append(",\"current\":");
                                        sUpdate.append(std::to_string(iTotal));
                                        sUpdate.append("},");
                                    }
                                    else if (iTotal < 0)
                                    {
                                        sUpdate.append("\"quantity\":{\"total\":-1,\"current\":-1},");
                                    }
                                    sUpdate.append("\"enabled\":true,\"name\":\"");
                                    sUpdate.append(vMember["name"].asString());
                                    sUpdate.append("\",\"cost\":");
                                    sUpdate.append(vMember["cost"].asString());
                                    sUpdate.append(",\"description\":\"");
                                    sUpdate.append(vMember["description"].asString());
                                    sUpdate.append("\"}");
                                    curl_easy_setopt(hndp, CURLOPT_POSTFIELDS, sUpdate.c_str());

                                    struct curl_slist* headersp = NULL;
                                    headersp = curl_slist_append(headersp, "Accept: ");
                                    headersp = curl_slist_append(headersp, sAuth.c_str());
                                    headersp = curl_slist_append(headersp, "Content-Type: application/json");
                                    curl_easy_setopt(hndp, CURLOPT_HTTPHEADER, headersp);


                                    curl_easy_setopt(hndp, CURLOPT_WRITEFUNCTION, callback);

                                    long httpCodep(0);
                                    std::unique_ptr<std::string> httpDatap(new std::string());

                                    curl_easy_setopt(hndp, CURLOPT_WRITEDATA, httpDatap.get());

                                    curl_easy_perform(hndp);
                                    curl_easy_getinfo(hndp, CURLINFO_RESPONSE_CODE, &httpCodep);
                                    curl_easy_cleanup(hndp);
                                    if (httpCodep == 200)
                                    {
                                        return 3;
                                    }
                                    else
                                    {
                                        return 2;
                                    }
                                }
                                return 0;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            return 0;
        }
    }
    return 0;
}


int Close(void)
{
    CURL* hnd = curl_easy_init();

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");

    std::string sURL = "https://api.streamelements.com/kappa/v2/store/";
    sURL.append(sChannel.c_str());
    sURL.append("/items/?limit=&offset=");
    curl_easy_setopt(hnd, CURLOPT_URL, sURL.c_str());

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Accept: ");
    std::string sAuth = "authorization: Bearer ";
    sAuth.append(sAPI.c_str());
    headers = curl_slist_append(headers, sAuth.c_str());
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    long httpCode(0);
    std::unique_ptr<std::string> httpData(new std::string());

    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, callback);


    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, httpData.get());

    curl_easy_perform(hnd);
    curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &httpCode);

    if (httpCode == 200)
    {
        Json::Value jsonData;
        Json::Reader jsonReader;

        if (jsonReader.parse(*httpData.get(), jsonData))
        {
            auto jBegin = jsonData.begin();

            Json::Value::iterator iter;
            for (iter = jsonData.begin(); iter != jsonData.end(); iter++)
            {
                Json::Value vMember = *iter;
                if (vMember)
                {
                    if (vMember.isMember("_id"))
                    {
                        if (vMember.isMember("name"))
                        {
                            std::string sName(vMember["name"].asString());

                            if (vMember["enabled"].asBool())
                            {

                            std::string sURL = "https://api.streamelements.com/kappa/v2/store/";
                            sURL.append(sChannel.c_str());
                            sURL.append("/items/");
                            sURL.append(vMember["_id"].asString());

                            CURL* hndp = curl_easy_init();
                            curl_easy_setopt(hndp, CURLOPT_URL, sURL.c_str());

                            curl_easy_setopt(hndp, CURLOPT_CUSTOMREQUEST, "PUT");
                            std::string sUpdate;
                            sUpdate.append("{");
                            sUpdate.append("\"quantity\":{\"total\":");
                            sUpdate.append(vMember["quantity"]["total"].asString());
                            sUpdate.append(",\"current\":");
                            sUpdate.append(vMember["quantity"]["total"].asString());
                            sUpdate.append("},");
                            sUpdate.append("\"enabled\":false,\"name\":\"");
                            sUpdate.append(vMember["name"].asString());
                            sUpdate.append("\",\"cost\":");
                            sUpdate.append(vMember["cost"].asString());
                            sUpdate.append(",\"description\":\"");
                            sUpdate.append(vMember["description"].asString());
                            sUpdate.append("\"}");
                            curl_easy_setopt(hndp, CURLOPT_POSTFIELDS, sUpdate.c_str());

                            struct curl_slist* headersp = NULL;
                            headersp = curl_slist_append(headersp, "Accept: ");
                            headersp = curl_slist_append(headersp, sAuth.c_str());
                            headersp = curl_slist_append(headersp, "Content-Type: application/json");
                            curl_easy_setopt(hndp, CURLOPT_HTTPHEADER, headersp);


                            curl_easy_setopt(hndp, CURLOPT_WRITEFUNCTION, callback);

                            long httpCodep(0);
                            std::unique_ptr<std::string> httpDatap(new std::string());

                            curl_easy_setopt(hndp, CURLOPT_WRITEDATA, httpDatap.get());

                            curl_easy_perform(hndp);
                            curl_easy_getinfo(hndp, CURLINFO_RESPONSE_CODE, &httpCodep);
                            curl_easy_cleanup(hndp);

                            }
                        }
                    }
                }
            }
        }
        else
        {
            return 0;
        }
    }
    return 1;
}


void WriteLog(const char* szText)
{
    std::ofstream outfile3;
    outfile3.open("SE-Log.txt", std::ios_base::app); // append instead of overwrite
    outfile3 << szText;
    outfile3 << "\n";
    outfile3.close();
}
