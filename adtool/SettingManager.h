#pragma once

#include <string>
#include <vector>

// ���ù����
#define BACKUP_AD_COUNT			7 

// �����𣬴�1-15
#define AD_TYPE_MIN			1
#define AD_TYPE_MAX			15

// ���뷽ʽ
#define AD_ALIGN_TOP					1
#define AD_ALIGN_BOTTOM			2
#define AD_ALIGN_LEFT					3
#define AD_ALIGN_RIGHT				4

// ����
#define FONT_FANGZHENG_DAHEI			L"�������"
#define FONT_FANGZHENG_ZONGYI			L"��������"
#define FONT_FANGZHENG_XINGKAI		L"�����п�"
#define FONT_FANGZHENG_JIANTI			L"�������кڼ���"

// ���λ
class CAdItem
{
public:
	// id
	std::wstring m_id;

	// �������
	RECT m_region;

	// ������1��ʼ
	int m_type = AD_TYPE_MIN;

	// ͼƬ���뷽ʽ��1��ʼ
	int m_imageAlign = AD_ALIGN_TOP;

	// ��ʶ������ˮƽ���Ǵ�ֱ��ʾ
	bool m_bHorizon = true;

	// ����
	std::wstring m_fontName = FONT_FANGZHENG_DAHEI;

	// ���ֶ��뷽ʽ��1��ʼ
	int m_textAlign = AD_ALIGN_TOP;
};

// ģ��
class CTemplateItem
{
public:
	// id
	std::wstring m_id;

	// ģ������
	std::wstring m_name;

	// ��������
	std::wstring m_groupName;

	// λ��dataĿ¼��
	std::wstring m_imageFileName;	

	// ���
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

	// �������
	void GetAdNames(std::wstring adNames[AD_TYPE_MAX]);

	void Save();

private:
	void Load();

public:
	int m_nLogLevel = 2;  // debug	

	// ģ���б�
	std::vector<CTemplateItem> m_templates;

private:
	// ���ù������
	std::wstring m_backupAdNames[BACKUP_AD_COUNT];
};
