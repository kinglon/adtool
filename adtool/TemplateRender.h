#pragma once

#include "SettingManager.h"

class CAdSettingItem
{
public:
	// 标志是图片模式，还是自由编辑
	bool m_imageMode = true;

	// 图片完整路径
	std::wstring m_imageFilePath;

	// 文本内容
	std::wstring m_textContent;
};

class CTemplateRender
{
public:
	// 渲染图片
	// @param tempItem, 模板
	// @param adSettings, 广告设置
	// @param useDefaultAd, 标志是否使用默认广告，如果没有设置广告信息，默认刷白色
	// @return 渲染图片的句柄，调用者负责释放
	static HBITMAP Do(const CTemplateItem& tempItem, const CAdSettingItem adSettings[AD_TYPE_MAX], bool useDefaultAd);

	// 按比例缩放图片
	static HBITMAP ScaleBitmap(HBITMAP bitmap, float scaleFactor);

private:
	static void PaintAdRect(Gdiplus::Graphics& graphics, const CAdItem& ad);

	static HFONT CreateAdFont(const CAdItem& ad, const std::wstring& text, 
		Gdiplus::Graphics* graphics, const Gdiplus::StringFormat* stringFormat);

private:
	static HDC m_hdc;
};

