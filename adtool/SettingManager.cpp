#include "pch.h"
#include "SettingManager.h"
#include <json/json.h>
#include <fstream>
#include "Utility/ImPath.h"
#include "Utility/ImCharset.h"

CSettingManager::CSettingManager()
{
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

    if (root.isMember("row_count"))
    {
        m_rowCount = root["row_count"].asInt();
    }

    if (root.isMember("column_count"))
    {
        m_columnCount = root["column_count"].asInt();
    }

    if (root.isMember("front_count"))
    {
        m_frontCount = root["front_count"].asInt();
    }

    if (root.isMember("back_count"))
    {
        m_backCount = root["back_count"].asInt();
    }

    if (root.isMember("capture_rect"))
    {
        m_captureRect.left = root["capture_rect"]["x"].asInt();
        m_captureRect.top = root["capture_rect"]["y"].asInt();
        m_captureRect.bottom = m_captureRect.top + root["capture_rect"]["height"].asInt();
        m_captureRect.right = m_captureRect.left + root["capture_rect"]["width"].asInt();
    }

    if (root.isMember("record_frame_rate"))
    {
        m_recordFrameRate = root["record_frame_rate"].asInt();
    }

    if (root.isMember("cache_duration"))
    {
        m_cacheDuration = root["cache_duration"].asInt();
    }
}

void CSettingManager::Save()
{
    Json::Value root = Json::objectValue;
    root["log_level"] = m_nLogLevel;    
    root["row_count"] = m_rowCount;    
    root["column_count"] = m_columnCount;    
    root["front_count"] = m_frontCount;    
    root["back_count"] = m_backCount;
    root["record_frame_rate"] = m_recordFrameRate;
    root["cache_duration"] = m_cacheDuration;

    Json::Value captureRect = Json::objectValue;
    captureRect["x"] = m_captureRect.left;
    captureRect["y"] = m_captureRect.top;
    captureRect["width"] = m_captureRect.Width();
    captureRect["height"] = m_captureRect.Height();
    root["capture_rect"] = captureRect;

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

void CSettingManager::SetRowCount(int rowCount)
{
    m_rowCount = rowCount;
    Save();
}

void CSettingManager::SetColumnCount(int columnCount)
{
    m_columnCount = columnCount;
    Save();
}

void CSettingManager::SetFrontCount(int frontCount)
{
    m_frontCount = frontCount;
    Save();
}

void CSettingManager::SetBackCount(int backCount)
{
    m_backCount = backCount;
    Save();
}

void CSettingManager::SetCaptureRect(CRect rect)
{
    m_captureRect = rect;
    Save();
}