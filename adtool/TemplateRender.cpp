#include "pch.h"
#include "TemplateRender.h"
#include "ImPath.h"

HBITMAP CTemplateRender::Do(const CTemplateItem& tempItem, const CAdSettingItem adSettings[AD_TYPE_MAX], bool useDefaultAd)
{  
    // Load the original image
    std::wstring imageFilePath = tempItem.m_imageFileName;
    if (imageFilePath.find(L':') == -1)  // 相对路径
    {
        imageFilePath = CImPath::GetDataPath() + L"\\" + imageFilePath;
    }
    Gdiplus::Bitmap originalImage(imageFilePath.c_str());
    if (originalImage.GetLastStatus() != Gdiplus::Ok)
    {
        LOG_ERROR(L"failed to load template image, path is %s", imageFilePath.c_str());
        return NULL;
    }

    // Create a Graphics object from the original image
    Gdiplus::Graphics graphics(&originalImage);

    // 画广告
    for (const auto& ad : tempItem.m_ads)
    {
        auto& adSetting = adSettings[ad.m_type - 1];
        if (adSetting.m_imageMode)  // 画广告图片
        {
            if (adSetting.m_imageFilePath.empty())  // 没配置
            {
                if (useDefaultAd)
                {
                    PaintAdRect(graphics, ad);
                }
                continue;
            }

            Gdiplus::Bitmap adImage(adSetting.m_imageFilePath.c_str());
            if (adImage.GetLastStatus() != Gdiplus::Ok)
            {
                LOG_ERROR(L"failed to load ad image, path is %s", adSetting.m_imageFilePath.c_str());
                return NULL;
            }

            // 计算缩放比例
            int adRegionWidth = ad.m_region.right - ad.m_region.left;
            int adRegionHeight = ad.m_region.bottom - ad.m_region.top;
            float scaleRatio = min(adRegionWidth * 1.0f / adImage.GetWidth(), adRegionHeight * 1.0f / adImage.GetHeight());
            int displayWidth = int(adImage.GetWidth()*scaleRatio);
            int displayHeight = int(adImage.GetHeight() * scaleRatio);
            Gdiplus::Rect destRect;
            switch (ad.m_imageAlign)
            {
            case AD_ALIGN_TOP:
                destRect.X = (adRegionWidth - displayWidth) / 2 + ad.m_region.left;
                destRect.Y = ad.m_region.top;
                break;
            case AD_ALIGN_BOTTOM:
                destRect.X = (adRegionWidth - displayWidth) / 2 + ad.m_region.left;
                destRect.Y = ad.m_region.bottom - displayHeight;
                break;
            case AD_ALIGN_LEFT:
                destRect.X = ad.m_region.left;
                destRect.Y = (adRegionHeight - displayHeight) / 2 + ad.m_region.top;
                break;
            case AD_ALIGN_RIGHT:
                destRect.X = ad.m_region.right - displayWidth;
                destRect.Y = (adRegionHeight - displayHeight) / 2 + ad.m_region.top;
                break;
            default:
                break;
            }
            destRect.Width = displayWidth;
            destRect.Height = displayHeight;
            graphics.DrawImage(&adImage, destRect);
        }
        else
        {
            if (adSetting.m_textContent.empty())  // 没配置
            {
                if (useDefaultAd)
                {
                    PaintAdRect(graphics, ad);
                }
                continue;
            }

            Gdiplus::Color textColor;
            textColor.SetFromCOLORREF(CSettingManager::GetInstance()->m_textColor);
            Gdiplus::SolidBrush textBrush(textColor);

            Gdiplus::Font textFont(ad.m_fontName.c_str(), (Gdiplus::REAL)CSettingManager::GetInstance()->GetFontSize(adSetting.m_textContent.length()));

            Gdiplus::StringFormat stringFormat;
            stringFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
            if (!ad.m_bHorizon)
            {
                stringFormat.SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsDirectionVertical);
            }
            if (ad.m_textAlign == AD_ALIGN_LEFT || ad.m_textAlign == AD_ALIGN_TOP)
            {                
                stringFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentNear);                
            }
            else
            {
                stringFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
            }
           
            Gdiplus::RectF layoutRect((Gdiplus::REAL)ad.m_region.left, (Gdiplus::REAL)ad.m_region.top,
                (Gdiplus::REAL)(ad.m_region.right - ad.m_region.left), (Gdiplus::REAL)(ad.m_region.bottom - ad.m_region.top));
            graphics.DrawString(adSetting.m_textContent.c_str(), -1, &textFont, layoutRect, &stringFormat, &textBrush);
        }
    }

    HBITMAP resultBmp = NULL;
    Gdiplus::Status status = originalImage.GetHBITMAP(Gdiplus::Color::Black, &resultBmp);
    if (status != Gdiplus::Ok)
    {
        LOG_ERROR(L"failed to call GetHBITMAP, error is %d", status);
        return NULL;
    }
    return resultBmp;
}

void CTemplateRender::PaintAdRect(Gdiplus::Graphics& graphics, const CAdItem& ad)
{
    Gdiplus::Rect rect(ad.m_region.left, ad.m_region.top, ad.m_region.right - ad.m_region.left, ad.m_region.bottom - ad.m_region.top);
    Gdiplus::SolidBrush brush(Gdiplus::Color::White);
    graphics.FillRectangle(&brush, rect);
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