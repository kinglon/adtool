#include "pch.h"
#include "SettingManager.h"
#include <json/json.h>
#include <fstream>
#include "ImPath.h"
#include "ImCharset.h"

CSettingManager::CSettingManager()
{
    for (int i = 0; i < BACKUP_AD_COUNT; i++)
    {
        m_backupAdNames[i] = L"±¸ÓÃ";
        m_backupAdNames[i] += std::to_wstring(i+1);
    }

    Load();
}

CSettingManager* CSettingManager::GetInstance()
{
	static CSettingManager* pInstance = new CSettingManager();
	return pInstance;
}

void CSettingManager::Load()
{
    std::wstring strConfFilePath = CImPath::GetConfPath() + L"configs.json";
    std::ifstream inputFile(strConfFilePath);
    if (!inputFile.is_open())
    {
        LOG_ERROR(L"failed to open the basic configure file : %s", strConfFilePath.c_str());
        return;
    }

    std::string jsonString;
    std::string line;
    while (std::getline(inputFile, line))
    {
        jsonString += line;
    }
    inputFile.close();

    Json::Value root;
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();
    std::string errors;
    bool parsingSuccessful = reader->parse(jsonString.c_str(), jsonString.c_str() + jsonString.size(), &root, &errors);
    delete reader;
    if (!parsingSuccessful)
    {
        LOG_ERROR(L"failed to parse the basic configure");
        return;
    }

    if (root.isMember("log_level"))
    {
        m_nLogLevel = root["log_level"].asInt();
    }

    // todo by yejinlong, load other config
}

void CSettingManager::Save()
{
    Json::Value root = Json::objectValue;
    root["log_level"] = m_nLogLevel;

    // todo by yejinlong, save other config

    std::wstring strConfFilePath = CImPath::GetConfPath() + L"configs.json";
    std::ofstream outputFile(strConfFilePath);
    if (outputFile.is_open())
    {
        Json::StreamWriterBuilder writer;
        std::string jsonString = Json::writeString(writer, root);
        outputFile << jsonString;
        outputFile.close();
    }
    else
    {
        LOG_ERROR(L"failed to open the basic configure file : %s", strConfFilePath.c_str());
    }
}