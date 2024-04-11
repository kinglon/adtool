#pragma once

#include "SettingManager.h"

class CAdSettingItem
{
public:
	// ��־��ͼƬģʽ���������ɱ༭
	bool m_imageMode = true;

	// ͼƬ����·��
	std::wstring m_imageFilePath;

	// �ı�����
	std::wstring m_textContent;
};

class CTemplateRender
{
public:
	// ��ȾͼƬ
	// @param tempItem, ģ��
	// @param adSettings, �������
	// @return ��ȾͼƬ�ľ���������߸����ͷ�
	HBITMAP Do(const CTemplateItem& tempItem, const CAdSettingItem adSettings[AD_TYPE_MAX]);

	// ����������ͼƬ
	static HBITMAP ScaleBitmap(HBITMAP bitmap, float scaleFactor);
};

