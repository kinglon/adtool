#include "pch.h"
#include "TemplateRender.h"
#include "ImPath.h"

HDC CTemplateRender::m_hdc = NULL;

HBITMAP CTemplateRender::Do(const CTemplateItem& tempItem, const CAdSettingItem adSettings[AD_TYPE_MAX], bool useDefaultAd)
{  
    if (m_hdc == NULL)
    {
        LOG_ERROR(L"dc is null");
        return NULL;
    }

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
            case AD_ALIGN_CENTER:
                destRect.X = (adRegionWidth - displayWidth) / 2 + ad.m_region.left;
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
            textColor.SetFromCOLORREF(ad.m_textColor);
            Gdiplus::SolidBrush textBrush(textColor);            
            
            Gdiplus::StringFormat stringFormat;  
            if (ad.m_bHorizon)
            {
                if (ad.m_textAlign == AD_ALIGN_LEFT)
                {
                    stringFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
                }
                else if (ad.m_textAlign == AD_ALIGN_RIGHT)
                {
                    stringFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
                }
                else
                {
                    stringFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
                }

                if (ad.m_textVerAlign == AD_ALIGN_TOP)
                {
                    stringFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
                }
                else if (ad.m_textVerAlign == AD_ALIGN_BOTTOM)
                {
                    stringFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
                }
                else
                {
                    stringFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
                }
            }
            else
            {
                stringFormat.SetFormatFlags(stringFormat.GetFormatFlags() | Gdiplus::StringFormatFlags::StringFormatFlagsDirectionVertical);
                if (ad.m_textVerAlign == AD_ALIGN_TOP)
                {
                    stringFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
                }
                else if (ad.m_textVerAlign == AD_ALIGN_BOTTOM)
                {
                    stringFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
                }
                else
                {
                    stringFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
                }

                if (ad.m_textAlign == AD_ALIGN_LEFT)
                {
                    stringFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
                }
                else if (ad.m_textAlign == AD_ALIGN_RIGHT)
                {
                    stringFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
                }
                else
                {
                    stringFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
                }                
            }

            HFONT adFont = CreateAdFont(ad, adSetting.m_textContent, &graphics, &stringFormat);
            Gdiplus::Font textFont(m_hdc, adFont);           
            Gdiplus::RectF layoutRect((Gdiplus::REAL)ad.m_region.left, (Gdiplus::REAL)ad.m_region.top,
                (Gdiplus::REAL)(ad.m_region.right - ad.m_region.left), (Gdiplus::REAL)(ad.m_region.bottom - ad.m_region.top));
            graphics.DrawString(adSetting.m_textContent.c_str(), -1, &textFont, layoutRect, &stringFormat, &textBrush);

            DeleteObject(adFont);
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

HFONT CTemplateRender::CreateAdFont(const CAdItem& ad, const std::wstring& text, 
    Gdiplus::Graphics* graphics, const Gdiplus::StringFormat* stringFormat)
{
    if (text.empty())
    {
        return NULL;
    }

    int adWidth = ad.m_region.right - ad.m_region.left;
    int adHeight = ad.m_region.bottom - ad.m_region.top;
   
    // 横着显示，在[1, 高度]之间使用二分法寻找一个字号使得整个字符串宽度在广告区内
    // 竖着显示，在[1, 宽度]之间使用二分法寻找一个字号使得整个字符串高度在广告区内
    int maxSize = 1;
    int minSize = 1;
    if (ad.m_bHorizon)
    {
        maxSize = adHeight;
    }
    else
    {
        maxSize = adWidth;
    }

    HFONT desiredFont = NULL;
    while (true)
    {
        // 创建字体
        int middleSize = (maxSize + minSize) / 2;
        HFONT font = CreateFont(middleSize * -1, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, ad.m_fontName.c_str());
        if (font == NULL)
        {
            LOG_ERROR(L"failed to create the font (%s %d号) ", ad.m_fontName.c_str(), middleSize);
            return desiredFont;
        }

        // 计算文本占用空间        
        Gdiplus::RectF boundingBox;
        Gdiplus::Font gdiFont(m_hdc, font);
        Gdiplus::Status status = graphics->MeasureString(text.c_str(), text.length(), &gdiFont,
            Gdiplus::PointF(0, 0), stringFormat, &boundingBox);        
        if (status != Gdiplus::Ok)
        {
            LOG_ERROR(L"failed to call MeasureString, error is 0x%x", status);
            DeleteObject(font);
            return desiredFont;
        }

        // 判断是否能显示得下
        bool enough = true;
        if (ad.m_bHorizon && boundingBox.Width > (Gdiplus::REAL)adWidth)
        {
            enough = false;
        }
        else if (!ad.m_bHorizon && (boundingBox.Height > (Gdiplus::REAL)adHeight || boundingBox.Width > (Gdiplus::REAL)adWidth))
        {
            enough = false;
        }

        // 继续二分法查找
        if (!enough)  // 显示不下
        {
            DeleteObject(font);
            maxSize = middleSize;
        }
        else // 显示得下
        {
            DeleteObject(desiredFont);
            desiredFont = font;
            minSize = middleSize;
        }

        if (minSize + 1 >= maxSize)  // 查找结束
        {
            return desiredFont;
        }
    }

    return NULL;
}

void CTemplateRender::PaintAdRect(Gdiplus::Graphics& graphics, const CAdItem& ad)
{
    // 画黑白相间框
    Gdiplus::Rect rect(ad.m_region.left, ad.m_region.top, ad.m_region.right - ad.m_region.left, ad.m_region.bottom - ad.m_region.top);
    Gdiplus::Pen blackPen(Gdiplus::Color::Black, 6);
    graphics.DrawRectangle(&blackPen, rect);
    Gdiplus::Pen whitePen(Gdiplus::Color::White, 6);
    whitePen.SetDashStyle(Gdiplus::DashStyleDash);
    graphics.DrawRectangle(&whitePen, rect);
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