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
	// @param useDefaultAd, ��־�Ƿ�ʹ��Ĭ�Ϲ�棬���û�����ù����Ϣ��Ĭ��ˢ��ɫ
	// @return ��ȾͼƬ�ľ���������߸����ͷ�
	static HBITMAP Do(const CTemplateItem& tempItem, const CAdSettingItem adSettings[AD_TYPE_MAX], bool useDefaultAd);

	// ����������ͼƬ
	static HBITMAP ScaleBitmap(HBITMAP bitmap, float scaleFactor);

private:
	static void PaintAdRect(Gdiplus::Graphics& graphics, const CAdItem& ad);

	static HFONT CreateAdFont(const CAdItem& ad, const std::wstring& text, 
		Gdiplus::Graphics* graphics, const Gdiplus::StringFormat* stringFormat);

private:
	static HDC m_hdc;
};

