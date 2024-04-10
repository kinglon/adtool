#pragma once

#include <string>
#include <vector>

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
#define FONT_FANGZHENG_DAHEI			L"方正大黑"
#define FONT_FANGZHENG_ZONGYI			L"方正综艺"
#define FONT_FANGZHENG_XINGKAI		L"方正行楷"
#define FONT_FANGZHENG_JIANTI			L"方正正中黑简体"

// 广告位
class CAdItem
{
public:
	// id
	std::wstring m_id;

	// 位置
	int m_x = 0;
	int m_y = 0;

	// 宽高
	int m_width = 100;
	int m_height = 100;

	// 广告类别，1开始
	int m_type = AD_TYPE_MIN;

	// 图片对齐方式
	int m_imageAlign = AD_ALIGN_TOP;

	// 标识文字是水平还是垂直显示
	bool m_bHorizon = true;

	// 字体
	std::wstring m_fontName = FONT_FANGZHENG_DAHEI;

	// 文字对齐方式
	int m_textAlign = AD_ALIGN_CENTER;
};

// 模板
class CTemplateItem
{
public:
	// id
	std::wstring m_id;

	// 位于data目录下
	std::wstring m_imageFileName;

	// 分组名字
	std::wstring m_groupName;

	// 广告
	std::vector<CAdItem> m_ads;
};


class CSettingManager
{
protected:
	CSettingManager();

public:
	static CSettingManager* GetInstance();

public:
	int GetLogLevel() { return m_nLogLevel; }

	void Save();

private:
	void Load();

public:
	int m_nLogLevel = 2;  // debug

	// 备用广告名字
	std::wstring m_backupAdNames[BACKUP_AD_COUNT];

	// 模板列表
	std::vector<CTemplateItem> m_templates;
};
