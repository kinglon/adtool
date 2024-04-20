#pragma once

#include <string>
#include <vector>
#include <set>
#include <sstream>

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
#define AD_ALIGN_CENTER			5

// ����
#define FONT_FANGZHENG_DAHEI			L"������ڼ���"
#define FONT_FANGZHENG_ZONGYI			L"�������ռ���"
#define FONT_FANGZHENG_XINGKAI		L"�����п�����"
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
	int m_imageAlign = AD_ALIGN_CENTER;

	// ��ʶ������ˮƽ���Ǵ�ֱ��ʾ
	bool m_bHorizon = true;

	// ����
	std::wstring m_fontName = FONT_FANGZHENG_DAHEI;

	// ���ֶ��뷽ʽ��1��ʼ
	int m_textAlign = AD_ALIGN_TOP;

	// ������ɫ
	COLORREF m_textColor = RGB(255, 255, 255);
};

// ģ��
class CTemplateItem
{
public:
	// id
	std::wstring m_id;

	// ģ������
	std::wstring m_name;

	// �������֣�������������߷ָ�
	std::wstring m_groupNames;

	// λ��dataĿ¼��
	std::wstring m_imageFileName;	

	// ���
	std::vector<CAdItem> m_ads;

public:
	// ��ȡ�����б�
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

	// �������
	void GetAdNames(std::wstring adNames[AD_TYPE_MAX]);

	void Save();

private:
	void Load();

public:
	int m_nLogLevel = 2;  // debug		

	// ģ���б�
	std::vector<CTemplateItem> m_templates;

	// ͼƬ���ɱ���·��
	std::wstring m_savePath;

	// ���ù������
	std::wstring m_backupAdNames[BACKUP_AD_COUNT];
};
