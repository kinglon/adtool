#pragma once

#include <string>
#include <vector>
#include <set>
#include <sstream>

// 备用广告数
#define BACKUP_AD_COUNT			7 

// 广告类别，从1-15
#define AD_TYPE_MIN			1
#define AD_TYPE_MAX			15

// 对齐方式
#define AD_ALIGN_TOP					1
#define AD_ALIGN_BOTTOM			2
#define AD_ALIGN_LEFT					3
#define AD_ALIGN_RIGHT				4
#define AD_ALIGN_CENTER			5

// 字体
#define FONT_FANGZHENG_DAHEI			L"方正大黑简体"
#define FONT_FANGZHENG_ZONGYI			L"方正综艺简体"
#define FONT_FANGZHENG_XINGKAI		L"方正行楷简体"
#define FONT_FANGZHENG_JIANTI			L"方正正中黑简体"

// 广告位
class CAdItem
{
public:
	// id
	std::wstring m_id;

	// 广告区域
	RECT m_region;

	// 广告类别，1开始
	int m_type = AD_TYPE_MIN;

	// 图片对齐方式，1开始
	int m_imageAlign = AD_ALIGN_CENTER;

	// 标识文字是水平还是垂直显示
	bool m_bHorizon = true;

	// 字体
	std::wstring m_fontName = FONT_FANGZHENG_DAHEI;

	// 文字对齐方式，1开始
	int m_textAlign = AD_ALIGN_TOP;

	// 字体颜色
	COLORREF m_textColor = RGB(255, 255, 255);
};

// 模板
class CTemplateItem
{
public:
	// id
	std::wstring m_id;

	// 模板名字
	std::wstring m_name;

	// 分组名字，多个分组用竖线分隔
	std::wstring m_groupNames;

	// 位于data目录下
	std::wstring m_imageFileName;	

	// 广告
	std::vector<CAdItem> m_ads;

public:
	// 获取分组列表
	std::set<std::wstring> GetGroupList() const
	{
		std::set<std::wstring> groupList;
		std::wstringstream ss(m_groupNames);
		std::wstring token;
		while (std::getline(ss, token, L'|'))
		{
			// Remove the space at the beginning and end of the token
			token.erase(0, token.find_first_not_of(' '));
			token.erase(token.find_last_not_of(' ') + 1);
			groupList.insert(groupList.end(), token);
		}
		return groupList;
	}
};


class CSettingManager
{
protected:
	CSettingManager();

public:
	static CSettingManager* GetInstance();

public:
	int GetLogLevel() { return m_nLogLevel; }

	// 广告名字
	void GetAdNames(std::wstring adNames[AD_TYPE_MAX]);

	void Save();

private:
	void Load();

public:
	int m_nLogLevel = 2;  // debug		

	// 模板列表
	std::vector<CTemplateItem> m_templates;

	// 图片生成保存路径
	std::wstring m_savePath;

	// 备用广告名字
	std::wstring m_backupAdNames[BACKUP_AD_COUNT];
};
