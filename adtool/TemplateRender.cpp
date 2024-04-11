#include "pch.h"
#include "TemplateRender.h"

HBITMAP CTemplateRender::Do(const CTemplateItem& tempItem, const CAdSettingItem adSettings[AD_TYPE_MAX])
{
	// todo by yejinlong
	return NULL;
}

HBITMAP CTemplateRender::ScaleBitmap(HBITMAP bitmap, float scaleFactor)
{
	Gdiplus::Bitmap* originalImage = Gdiplus::Bitmap::FromHBITMAP(bitmap, NULL);
	if (originalImage == nullptr)
	{
		LOG_ERROR(L"failed to load image from HBITMAP");
		return NULL;
	}

	int imageWidth = originalImage->GetWidth();
	int imageHeight = originalImage->GetHeight();
	int displayWidth = (int)(imageWidth * scaleFactor);
	int displayHeight = (int)(imageHeight * scaleFactor);

	Gdiplus::Bitmap scaledImage(displayWidth, displayHeight, originalImage->GetPixelFormat());
	Gdiplus::Graphics graphics(&scaledImage);
	graphics.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQualityBicubic);
	graphics.DrawImage(originalImage, 0, 0, displayWidth, displayHeight);
	delete originalImage;
	originalImage = nullptr;

	HBITMAP hScaleImage = NULL;
	Gdiplus::Status status = scaledImage.GetHBITMAP(Gdiplus::Color::Black, &hScaleImage);
	if (status != Gdiplus::Ok)
	{
		LOG_ERROR(L"failed to call GetHBITMAP, error is %d", status);
		return NULL;
	}
	return hScaleImage;
}