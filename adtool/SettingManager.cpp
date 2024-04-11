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
        m_backupAdNames[i] = L"备用";
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

void CSettingManager::GetAdNames(std::wstring adNames[AD_TYPE_MAX])
{
    adNames[0] = L"广告区(长方形)";
    adNames[1] = L"广告区1(正方形)";
    adNames[2] = L"广告区2(通用小)";
    adNames[3] = L"窗贴广区";
    adNames[4] = L"水电标胶带";
    adNames[5] = L"电话区";
    adNames[6] = L"电话区(间距大)";
    adNames[7] = L"窗贴电话区";
    for (int i = 8; i < AD_TYPE_MAX; i++)
    {
        adNames[i] = m_backupAdNames[i-8];
    }
}

int CSettingManager::GetFontSize(int textLength)
{
    int distance = 1000;
    int fontSize = 10;
    for (const auto& item : m_textLength2Fonts)
    {
        int dis = abs(textLength - item.first);
        if (dis < distance)
        {
            distance = dis;
            fontSize = item.second;
        }
    }

    return fontSize;
}