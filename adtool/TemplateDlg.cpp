﻿// TemplateDlg.cpp: 实现文件
//

#include "pch.h"
#include "adtool.h"
#include "afxdialogex.h"
#include "TemplateDlg.h"
#include "SettingManager.h"
#include "TemplateRender.h"
#include "AdDlg.h"
#include "ImPath.h"
#include "GetTextDlg.h"

#define WM_SHOW_MAX		WM_USER+100

IMPLEMENT_DYNAMIC(CTemplateDlg, CDialogEx)

CTemplateDlg::CTemplateDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TEMPLATE_DIALOG, pParent)
{
	
}

CTemplateDlg::~CTemplateDlg()
{
}

void CTemplateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NAME, m_nameEdit);
	DDX_Control(pDX, IDC_EDIT_GROUPNAME, m_groupNameEdit);
	DDX_Control(pDX, IDC_PREVIEW_IMAGE, m_previewImageCtrl);
	DDX_Control(pDX, IDC_STATIC_NAME, m_nameStatic);
	DDX_Control(pDX, IDC_STATIC_GROUPNAME, m_groupNameStatic);
	DDX_Control(pDX, IDOK, m_okBtn);
	DDX_Control(pDX, IDCANCEL, m_cancelBtn);
	DDX_Control(pDX, IDC_STATIC_POSITION, m_positionStatic);
}


BEGIN_MESSAGE_MAP(CTemplateDlg, CDialogEx)
	ON_WM_DROPFILES()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CTemplateDlg::OnBnClickedOk)
	ON_STN_DBLCLK(IDC_PREVIEW_IMAGE, &CTemplateDlg::OnStnDblclickPreviewImage)
	ON_STN_CLICKED(IDC_PREVIEW_IMAGE, &CTemplateDlg::OnStnClickedPreviewImage)
	ON_COMMAND(ID_AD_EDIT, &CTemplateDlg::OnAdEdit)
	ON_COMMAND(ID_AD_DELETE, &CTemplateDlg::OnAdDelete)
	ON_COMMAND(ID_AD_PREVIEW, &CTemplateDlg::OnAdPreview)
	ON_COMMAND(ID_AD_COPY, &CTemplateDlg::OnAdCopy)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


void CTemplateDlg::InitControls()
{
	m_nameEdit.SetWindowText(m_template.m_name.c_str());
	m_groupNameEdit.SetWindowText(m_template.m_groupNames.c_str());
	m_previewImageCtrl.SetCallback(this);
	UpdatePreviewCtrl();
}

void CTemplateDlg::RejustControlPos()
{
	// 调整控件的位置
	CRect wndRect;
	GetClientRect(&wndRect);
	// 水平居中显示
	int offsetX = (wndRect.Width() - m_initSizeX) / 2;
	UINT controls[] = { IDC_STATIC_NAME , IDC_EDIT_NAME , IDC_STATIC_GROUPNAME , IDC_EDIT_GROUPNAME , IDOK , IDCANCEL, IDC_STATIC_POSITION };
	for (int i = 0; i < ARRAYSIZE(controls); i++)
	{
		CWnd* ctrlWnd = GetDlgItem(controls[i]);
		CRect rect;
		ctrlWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.OffsetRect(offsetX, 0);
		ctrlWnd->MoveWindow(&rect);
	}
	// 预览控件撑满
	CRect previewRect;
	m_previewImageCtrl.GetWindowRect(&previewRect);
	ScreenToClient(&previewRect);
	int widthChange = wndRect.Width() - m_initSizeX;
	int heightChange = wndRect.Height() - m_initSizeY;
	previewRect.right += widthChange;
	previewRect.bottom += heightChange;
	m_previewImageCtrl.MoveWindow(&previewRect);
	// 确定取消按钮往下移动
	CWnd* okCancelBtn[] = { &m_okBtn, &m_cancelBtn, &m_positionStatic };
	for (int i = 0; i < ARRAYSIZE(okCancelBtn); i++)
	{
		CRect rect;
		okCancelBtn[i]->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.OffsetRect(0, heightChange);
		okCancelBtn[i]->MoveWindow(&rect);
	}
	Invalidate();

	UpdatePreviewCtrl();
}

void CTemplateDlg::UpdatePreviewCtrl()
{
	// 没有设置模板图片，不显示预览
	if (m_template.m_imageFileName.empty())
	{
		return;
	}

	// 渲染图片
	HBITMAP bmp = CTemplateRender::Do(m_template, m_adSettings, true);
	if (bmp == NULL)
	{
		return;
	}
	
	// 缩放图片
	BITMAP bitmap;
	GetObject(bmp, sizeof(BITMAP), &bitmap);

	CRect previewCtrlRect;
	m_previewImageCtrl.GetWindowRect(&previewCtrlRect);

	if (bitmap.bmWidth <= previewCtrlRect.Width() && bitmap.bmHeight <= previewCtrlRect.Height())
	{
		m_scaleRatio = 1.0f;
	}
	else
	{
		m_scaleRatio = min(previewCtrlRect.Width() * 1.0f / bitmap.bmWidth, previewCtrlRect.Height() * 1.0f / bitmap.bmHeight);
		HBITMAP newBmp = CTemplateRender::ScaleBitmap(bmp, m_scaleRatio);
		if (newBmp != NULL)
		{
			DeleteObject(bmp);
			bmp = newBmp;
			GetObject(bmp, sizeof(BITMAP), &bitmap);  // 重新计算宽高
		}
	}

	// 居中显示，计算偏移值
	m_imgOffsetX = (previewCtrlRect.Width() - bitmap.bmWidth) / 2;
	m_imgOffsetY = (previewCtrlRect.Height() - bitmap.bmHeight) / 2;	

	// 设置图片到预览控件
	HBITMAP hOldBmp = m_previewImageCtrl.SetBitmap(bmp);
	if (hOldBmp)
	{
		DeleteObject(hOldBmp);
	}
	if (m_previewImageCtrl.GetBitmap() != bmp)
	{
		DeleteObject(bmp);
	}
}

CAdItem* CTemplateDlg::GetCurrentPosAd()
{
	auto& ads = m_template.m_ads;
	if (ads.size() == 0)
	{
		return nullptr;
	}

	// 获取鼠标位置
	POINT pt;
	GetCursorPos(&pt);

	// 转换为预览控件内位置
	m_previewImageCtrl.ScreenToClient(&pt);

	// 转换为真实图片的位置
	ImageCtrlToRealImage(&pt);

	// 按位置查询广告
	for (auto& ad : ads)
	{
		if (CRect(&ad.m_region).PtInRect(pt))
		{
			return &ad;
		}
	}

	return nullptr;
}

void CTemplateDlg::ImageCtrlToRealImage(LPPOINT pt)
{
	// 转换为显示的图片的位置
	pt->x -= m_imgOffsetX;
	pt->y -= m_imgOffsetY;

	// 按比例缩放转为原始图片的位置
	pt->x = int(pt->x / m_scaleRatio);
	pt->y = int(pt->y / m_scaleRatio);
}

void CTemplateDlg::HandleDropFile(HDROP hDropInfo)
{
	// 判断是否给预览图片控件
	POINT pt;
	if (!DragQueryPoint(hDropInfo, &pt))
	{
		return;
	}

	CRect imageCtrlRect;
	m_previewImageCtrl.GetWindowRect(&imageCtrlRect);
	ScreenToClient(imageCtrlRect);
	if (!imageCtrlRect.PtInRect(pt))
	{
		return;
	}

	// 获取拖拽的文件名
	wchar_t szFileName[MAX_PATH + 1];
	memset(szFileName, 0, sizeof(szFileName));
	DragQueryFile(hDropInfo, 0, szFileName, MAX_PATH);

	// 还没有设置模板图片，设置它为模板图片
	if (m_template.m_imageFileName.empty())
	{
		m_template.m_imageFileName = szFileName;
		UpdatePreviewCtrl();
		return;
	}

	// 转化为预览图片控件内的位置
	ClientToScreen(&pt);
	m_previewImageCtrl.ScreenToClient(&pt);

	// 转化为相对于真实模板图片的位置
	ImageCtrlToRealImage(&pt);

	// 判断是否给广告设置预览
	for (auto& ad : m_template.m_ads)
	{
		if (CRect(ad.m_region).PtInRect(pt))
		{
			m_adSettings[ad.m_type - 1].m_imageMode = true;
			m_adSettings[ad.m_type - 1].m_imageFilePath = szFileName;
			UpdatePreviewCtrl();
			return;
		}
	}

	// 不是设置给广告，就设置给模板图片	
	m_template.m_imageFileName = szFileName;
	UpdatePreviewCtrl();
}

void CTemplateDlg::OnStaticMouseMove(HWND hwnd, CPoint point)
{
	if (hwnd != m_previewImageCtrl.m_hWnd)
	{
		return;
	}
	
	POINT pt;
	pt.x = point.x;
	pt.y = point.y;

	// 转换为真实图片的位置
	ImageCtrlToRealImage(&pt);

	CString pos;
	pos.Format(L"%d, %d", pt.x, pt.y);
	m_positionStatic.SetWindowText(pos);
}

BOOL CTemplateDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//  允许拖拽文件
	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	ChangeWindowMessageFilter(0x0049, MSGFLT_ADD); // 0x0049 == WM_COPYGLOBALDATA		

	// 获取初始尺寸
	CRect wndRect;
	GetClientRect(&wndRect);
	m_initSizeX = wndRect.Width();
	m_initSizeY = wndRect.Height();

	InitControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CTemplateDlg::OnDropFiles(HDROP hDropInfo)
{
	HandleDropFile(hDropInfo);	

	CDialogEx::OnDropFiles(hDropInfo);
}


BOOL CTemplateDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CTemplateDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (cx == 0 || cy == 0 || m_initSizeX == 0 || m_initSizeY == 0)
	{
		return;
	}

	if (cx != m_initSizeX || cy != m_initSizeY)
	{
		RejustControlPos();
		m_initSizeX = cx;
		m_initSizeY = cy;
	}
}


void CTemplateDlg::OnBnClickedOk()
{
	if (m_template.m_id.empty())
	{
		m_template.m_id = CAdDlg::GetGuid();
	}

	CString name;
	CString groupName;
	m_nameEdit.GetWindowText(name);
	m_groupNameEdit.GetWindowText(groupName);
	if (name.IsEmpty())
	{
		MessageBox(L"模板名称不能为空", L"提示", MB_OK);
		return;
	}
	if (groupName.IsEmpty())
	{
		MessageBox(L"分组名称不能为空", L"提示", MB_OK);
		return;
	}
	
	// 校验模板名字是否重复
	bool exist = false;
	auto& templates = CSettingManager::GetInstance()->m_templates;
	for (auto& item : templates)
	{
		if (item.m_id != m_template.m_id && item.m_name == (LPCTSTR)name)
		{
			exist = true;
			break;
		}
	}
	if (exist)
	{
		MessageBox(L"模板名称已存在", L"提示", MB_OK);
		return;
	}

	// 校验图片文件是否存在
	if (m_template.m_imageFileName.empty())
	{
		MessageBox(L"请设置模板图片", L"提示", MB_OK);
		return;
	}

	// 更新数据
	m_template.m_name = (LPCTSTR)name;
	m_template.m_groupNames = (LPCTSTR)groupName;

	// 如果设置为外部图片，拷贝文件到data目录
	if (m_template.m_imageFileName.find(L':') != -1)
	{
		std::wstring extension;
		int index = m_template.m_imageFileName.rfind(L'.');
		if (index != -1)
		{
			extension = m_template.m_imageFileName.substr(index);
		}
		std::wstring newFileName = std::wstring((LPCTSTR)CAdDlg::GetGuid()) + extension;
		std::wstring newFilePath = CImPath::GetDataPath() + newFileName;
		if (!CopyFile(m_template.m_imageFileName.c_str(), newFilePath.c_str(), TRUE))
		{
			LOG_ERROR(L"failed to copy image %s", newFilePath.c_str());
			MessageBox(L"拷贝模板图片失败", L"提示", MB_OK);
			return;
		}
		m_template.m_imageFileName = newFileName;
	}

	// 更新到配置
	exist = false;
	for (auto& item : templates)
	{
		if (item.m_id == m_template.m_id)
		{
			// 如果图片不一样，要先删除原来的模板图片
			if (item.m_imageFileName != m_template.m_imageFileName)
			{
				DeleteFile((CImPath::GetDataPath() + L"\\" + item.m_imageFileName).c_str());
			}
			item = m_template;
			exist = true;
			break;
		}
	}
	if (!exist)
	{
		templates.push_back(m_template);
	}
	CSettingManager::GetInstance()->Save();

	CDialogEx::OnOK();
}


void CTemplateDlg::OnStnDblclickPreviewImage()
{
	if (m_template.m_imageFileName.empty())
	{
		return;
	}

	CAdItem* ad = GetCurrentPosAd();
	if (ad)  // 已经有广告，不能添加
	{
		return;
	}

	CAdDlg adDlg;
	if (adDlg.DoModal() == IDCANCEL)
	{
		return;
	}

	m_template.m_ads.push_back(adDlg.m_ad);
	UpdatePreviewCtrl();
}


void CTemplateDlg::OnStnClickedPreviewImage()
{
	CAdItem* ad = GetCurrentPosAd();
	if (ad == nullptr)
	{
		return;
	}
	
	m_adId = ad->m_id;

	CMenu menu;
	menu.LoadMenu(IDR_MENU_AD);

	CMenu* pContextMenu = menu.GetSubMenu(0);
	if (pContextMenu == nullptr)
	{
		return;
	}

	CPoint point;
	GetCursorPos(&point);
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);	
}


void CTemplateDlg::OnAdEdit()
{
	CAdItem* ad = nullptr;
	auto& ads = m_template.m_ads;
	for (unsigned int i=0; i<ads.size(); i++)
	{
		if (ads[i].m_id == m_adId)
		{
			ad = &ads[i];
			break;
		}
	}
	if (ad == nullptr)
	{
		return;
	}
	
	CAdDlg adDlg;
	adDlg.m_ad = *ad;
	if (adDlg.DoModal() == IDCANCEL)
	{
		return;
	}

	*ad = adDlg.m_ad;
	UpdatePreviewCtrl();
}


void CTemplateDlg::OnAdDelete()
{
	if (MessageBox(L"确定删除广告吗？", L"提示", MB_OKCANCEL) == IDCANCEL)
	{
		return;
	}

	auto& ads = m_template.m_ads;
	for (auto it=ads.begin(); it!=ads.end(); it++)
	{
		if (it->m_id == m_adId)
		{
			ads.erase(it);
			break;
		}
	}

	UpdatePreviewCtrl();
}

void CTemplateDlg::OnAdPreview()
{
	CGetTextDlg dlg;
	dlg.m_strWndTitle = L"广告语";
	dlg.m_textFieldName = L"预览文字";
	if (dlg.DoModal() == IDCANCEL)
	{
		return;
	}

	auto& ads = m_template.m_ads;
	for (auto it = ads.begin(); it != ads.end(); it++)
	{
		if (it->m_id == m_adId)
		{
			m_adSettings[it->m_type - 1].m_imageMode = false;
			m_adSettings[it->m_type - 1].m_imageFilePath = L"";
			m_adSettings[it->m_type - 1].m_textContent = (LPCTSTR)dlg.m_textContent;
			break;
		}
	}

	UpdatePreviewCtrl();
}


void CTemplateDlg::OnAdCopy()
{
	auto& ads = m_template.m_ads;
	for (auto it = ads.begin(); it != ads.end(); it++)
	{
		if (it->m_id == m_adId)
		{
			auto newAd = *it;
			newAd.m_id = CAdDlg::GetGuid();
			newAd.m_region.top += 20;  // 向下偏移20
			newAd.m_region.bottom += 20;
			ads.insert(m_template.m_ads.begin(), newAd);  // 放在最上面
			break;
		}
	}

	UpdatePreviewCtrl();
}


void CTemplateDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CDialogEx::OnMouseMove(nFlags, point);
}
