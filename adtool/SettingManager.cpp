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

    if (root.isMember("text_color"))
    {
        int r = root["text_color"]["r"].asInt();
        int g = root["text_color"]["g"].asInt();
        int b = root["text_color"]["b"].asInt();
        m_textColor = RGB(r, g, b);
    }

    if (root.isMember("template"))
    {
        m_templates.clear();
        for (unsigned int i = 0; i < root["template"].size(); i++)
        {
            auto& value = root["template"][i];
            CTemplateItem item;
            item.m_id = CImCharset::UTF8ToUnicode(value["id"].asString().c_str());
            item.m_name = CImCharset::UTF8ToUnicode(value["name"].asString().c_str());
            item.m_groupName = CImCharset::UTF8ToUnicode(value["group_name"].asString().c_str());
            item.m_imageFileName = CImCharset::UTF8ToUnicode(value["image_file"].asString().c_str());
            
            for (unsigned int j = 0; j < value["ad"].size(); j++)
            {
                auto& adValue = value["ad"][j];
                CAdItem adItem;
                adItem.m_id = CImCharset::UTF8ToUnicode(adValue["id"].asString().c_str());
                adItem.m_region.left = adValue["left"].asInt();
                adItem.m_region.top = adValue["top"].asInt();
                adItem.m_region.right = adValue["right"].asInt();
                adItem.m_region.bottom = adValue["bottom"].asInt();
                adItem.m_type = adValue["type"].asInt();
                adItem.m_bHorizon = adValue["horizon"].asBool();
                adItem.m_fontName = CImCharset::UTF8ToUnicode(adValue["font_name"].asString().c_str());
                adItem.m_imageAlign = adValue["image_align"].asInt();
                adItem.m_textAlign = adValue["text_align"].asInt();
                item.m_ads.push_back(adItem);
            }

            m_templates.push_back(item);
        }
    }

    if (root.isMember("backup_ad"))
    {
        for (unsigned int i = 0; i < root["backup_ad"].size() && i < BACKUP_AD_COUNT; i++)
        {
            m_backupAdNames[i] = CImCharset::UTF8ToUnicode(root["backup_ad"][i].asString().c_str());
        }
    }

    if (root.isMember("text_length_font_size"))
    {
        m_textLength2Fonts.clear();
        for (unsigned int i = 0; i < root["text_length_font_size"].size(); i++)
        {
            auto& value = root["text_length_font_size"][i];
            m_textLength2Fonts[value["text_length"].asInt()] = value["font_size"].asInt();
        }
    }
}

void CSettingManager::Save()
{
    Json::Value root = Json::objectValue;
    root["log_level"] = m_nLogLevel;

    root["text_color"]["r"] = GetRValue(m_textColor);
    root["text_color"]["g"] = GetGValue(m_textColor);
    root["text_color"]["b"] = GetBValue(m_textColor);

    for (auto& tempItem : m_templates)
    {
        Json::Value value;
        value["id"] = CImCharset::UnicodeToUTF8(tempItem.m_id.c_str());
        value["name"] = CImCharset::UnicodeToUTF8(tempItem.m_name.c_str());
        value["group_name"] = CImCharset::UnicodeToUTF8(tempItem.m_groupName.c_str());
        value["image_file"] = CImCharset::UnicodeToUTF8(tempItem.m_imageFileName.c_str());        
        for (auto& adItem : tempItem.m_ads)
        {
            Json::Value adValue;
            adValue["id"] = CImCharset::UnicodeToUTF8(adItem.m_id.c_str());
            adValue["left"] = adItem.m_region.left;
            adValue["top"] = adItem.m_region.top;
            adValue["right"] = adItem.m_region.right;
            adValue["bottom"] = adItem.m_region.bottom;
            adValue["type"] = adItem.m_type;
            adValue["horizon"] = adItem.m_bHorizon;
            adValue["font_name"] = CImCharset::UnicodeToUTF8(adItem.m_fontName.c_str());
            adValue["image_align"] = adItem.m_imageAlign;
            adValue["text_align"] = adItem.m_textAlign;
            value["ad"].append(adValue);
        }
        root["template"].append(value);
    }

    for (auto& item : m_backupAdNames)
    {
        root["backup_ad"].append(CImCharset::UnicodeToUTF8(item.c_str()));
    }

    for (auto& item : m_textLength2Fonts)
    {
        Json::Value value;
        value["text_length"] = item.first;
        value["font_size"] = item.second;
        root["text_length_font_size"].append(value);
    }

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