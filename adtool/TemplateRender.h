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
	// @return 渲染图片的句柄，调用者负责释放
	HBITMAP Do(const CTemplateItem& tempItem, const CAdSettingItem adSettings[AD_TYPE_MAX]);

	// 按比例缩放图片
	static HBITMAP ScaleBitmap(HBITMAP bitmap, float scaleFactor);
};

