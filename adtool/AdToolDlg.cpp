
// AdToolDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "adtool.h"
#include "AdToolDlg.h"
#include "afxdialogex.h"
#include "SettingManager.h"
#include "TempManagerDlg.h"
#include <gdiplus.h>
#include "GenImageDlg.h"

using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define WM_SHOW_MAX  (WM_USER+100)

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAdToolDlg 对话框



CAdToolDlg::CAdToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ADTOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAdToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_GROUP_COMBO, m_groupCombo);
	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_previewImageCtrl);
}

BEGIN_MESSAGE_MAP(CAdToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_TEMPLATE_MANAGER_BTN, &CAdToolDlg::OnBnClickedTemplateManagerBtn)
	ON_BN_CLICKED(IDC_PREVIEW_BTN, &CAdToolDlg::OnBnClickedPreviewBtn)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_GEN_IMAGE_BTN, &CAdToolDlg::OnBnClickedGenImageBtn)
END_MESSAGE_MAP()


void CAdToolDlg::InitControls()
{
	// 调整控件的位置
	CRect wndRect;
	GetClientRect(&wndRect);
	int offsetX = (wndRect.Width() - m_initSizeX) / 2;
	CWnd* pChildWnd = this->GetWindow(GW_CHILD);
	while (pChildWnd)
	{
		UINT nID = pChildWnd->GetDlgCtrlID();
		CRect rect;
		pChildWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		if (nID == IDC_STATIC_PREVIEW) // 预览控件撑满
		{
			int widthChange = wndRect.Width() - m_initSizeX;
			int heightChange = wndRect.Height() - m_initSizeY;
			rect.right += widthChange;
			rect.bottom += heightChange;
			pChildWnd->MoveWindow(&rect);
		}
		else  // 其他控件水平移动
		{
			rect.OffsetRect(offsetX, 0);
			pChildWnd->MoveWindow(&rect);
		}

		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
	}
	Invalidate();

	// 初始化分组列表
	InitGroupControl();

	// 初始化所有广告为图片模式
	for (int controlId = IDC_AD_RADIO_1; controlId <= IDC_AD_RADIO_30; controlId++)
	{
		if ((controlId - IDC_AD_RADIO_1) % 2 == 0)
		{
			((CButton*)GetDlgItem(controlId))->SetCheck(BST_CHECKED);
		}
	}

	// 初始化广告的名字
	std::wstring adNames[AD_TYPE_MAX];
	CSettingManager::GetInstance()->GetAdNames(adNames);;
	for (int i = 0; i < AD_TYPE_MAX; i++)
	{
		GetDlgItem(IDC_AD_STATIC_1 + i)->SetWindowText(adNames[i].c_str());
	}
}

void CAdToolDlg::InitGroupControl()
{
	std::set<std::wstring> groups;
	auto& templates = CSettingManager::GetInstance()->m_templates;
	for (const auto& item : templates)
	{
		groups.insert(groups.end(), item.m_groupName);
	}

	m_groupCombo.Clear();
	for (const auto& item : groups)
	{
		m_groupCombo.AddString(item.c_str());
	}

	if (groups.size() > 0)
	{
		m_groupCombo.SetCurSel(0);
	}
}

std::vector<CTemplateItem> CAdToolDlg::GetTemplatesByGroupName(std::wstring groupName)
{
	std::vector<CTemplateItem> result;
	const auto& templates = CSettingManager::GetInstance()->m_templates;
	for (const auto& item : templates)
	{
		if (item.m_groupName == groupName)
		{
			result.push_back(item);
		}
	}

	return result;
}

std::vector<CAdItem> CAdToolDlg::GetAdsFromTemplates(const std::vector<CTemplateItem>& templates)
{
	std::vector<CAdItem> ads;
	for (const auto& item : templates)
	{
		for (const auto& ad : item.m_ads)
		{
			bool exist = false;
			for (unsigned int i = 0; i < ads.size(); i++)
			{				
				if (ad.m_type == ads[i].m_type)
				{
					exist = true;
					break;
				}
			}
			if (!exist)
			{
				ads.push_back(ad);
			}
		}
	}

	return ads;
}

CTemplateItem CAdToolDlg::MakeTemplateItemFromAds(const std::vector<CAdItem>& ads)
{
	// todo by yejinlong, MakeTemplateItemFromAds
	return CTemplateItem();
}

void CAdToolDlg::GetAdSettings(CAdSettingItem adSettings[AD_TYPE_MAX])
{
	for (int controlId = IDC_AD_RADIO_1; controlId <= IDC_AD_RADIO_30; controlId+=2)
	{
		int index = (controlId - IDC_AD_RADIO_1) / 2;
		adSettings[index].m_imageMode = (((CButton*)GetDlgItem(controlId))->GetCheck()) == BST_CHECKED;
		if (adSettings[index].m_imageMode)
		{
			CString imagePath;
			GetDlgItem(controlId - IDC_AD_RADIO_1 + IDC_AD_EDIT_1)->GetWindowText(imagePath);
			adSettings[index].m_imageFilePath = imagePath;
		}
		else
		{
			CString textContent;
			GetDlgItem(controlId - IDC_AD_RADIO_1 + 1 + IDC_AD_EDIT_1)->GetWindowText(textContent);
			adSettings[index].m_textContent = textContent;
		}
	}
}

HBITMAP CAdToolDlg::AspectFitContainer(HBITMAP bmp, int sizeX, int sizeY)
{
	// todo by yejinlong
	return NULL;
}

bool CAdToolDlg::CheckAdSettings(const std::vector<CAdItem>& ads, CAdSettingItem adSettings[AD_TYPE_MAX])
{
	CString errorAd;
	for (const auto& ad : ads)
	{
		bool hasError = false;
		int adIndex = ad.m_type - 1;
		if (adSettings[adIndex].m_imageMode)
		{
			if (!::PathFileExists(adSettings[adIndex].m_imageFilePath.c_str()))
			{
				hasError = true;
			}
		}
		else
		{
			if (adSettings[adIndex].m_textContent.empty())
			{
				hasError = true;
			}
		}
		
		if (hasError)
		{
			if (!errorAd.IsEmpty())
			{
				errorAd += L'，';
			}
			CString text;
			GetDlgItem(IDC_AD_STATIC_1 + adIndex)->GetWindowText(text);
			errorAd += text;
		}
	}

	if (!errorAd.IsEmpty())
	{
		CString errorMsg;
		errorMsg.Format(L"广告(%s)配置有误，是否继续？", errorAd);
		if (MessageBox(errorMsg, L"提示", MB_YESNO) == IDNO)
		{
			return false;
		}
	}

	return true;
}

BOOL CAdToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//  允许拖拽文件
	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	ChangeWindowMessageFilter(0x0049, MSGFLT_ADD); // 0x0049 == WM_COPYGLOBALDATA		

	// 最大化
	CRect wndRect;
	GetClientRect(&wndRect);
	m_initSizeX = wndRect.Width();
	m_initSizeY = wndRect.Height();
	PostMessage(WM_SHOW_MAX);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAdToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAdToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAdToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CAdToolDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_SHOW_MAX)
	{
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAdToolDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (nType == SIZE_MAXIMIZED)
	{
		static bool bFirst = true;
		if (bFirst)
		{
			bFirst = false;
			InitControls();
		}
	}
}


void CAdToolDlg::OnBnClickedTemplateManagerBtn()
{
	CTempManagerDlg dlg;
	dlg.DoModal();
	InitGroupControl();
}


void CAdToolDlg::OnBnClickedPreviewBtn()
{
	CString groupName;
	m_groupCombo.GetWindowText(groupName);
	if (groupName.IsEmpty())
	{
		MessageBox(L"请选择一个分组", L"提示", MB_OK);
		return;
	}

	std::vector<CTemplateItem> templates = GetTemplatesByGroupName((LPCTSTR)groupName);
	if (templates.size() == 0)
	{
		MessageBox(L"没找到任何模板", L"提示", MB_OK);
		return;
	}

	// 从这些模板中，收集不同类别的广告
	std::vector<CAdItem> ads = GetAdsFromTemplates(templates);
	if (ads.size() == 0)
	{
		MessageBox(L"没配置任何广告", L"提示", MB_OK);
		return;
	}

	// 从界面获取广告输入配置
	CAdSettingItem adSettings[AD_TYPE_MAX];
	GetAdSettings(adSettings);

	// 从广告生成一个模板对象
	CTemplateItem tempItem = MakeTemplateItemFromAds(ads);

	// 渲染模板图片
	CTemplateRender render;
	HBITMAP bmp = render.Do(tempItem, adSettings);

	// 显示图片
	BITMAP bitmap;
	GetObject(bmp, sizeof(BITMAP), &bitmap);

	CRect previewCtrlRect;
	m_previewImageCtrl.GetWindowRect(&previewCtrlRect);

	if (bitmap.bmWidth > previewCtrlRect.Width() || bitmap.bmHeight > previewCtrlRect.Height())
	{
		// 显示不下等比例缩放
		HBITMAP fitBmp = AspectFitContainer(bmp, previewCtrlRect.Width(), previewCtrlRect.Height());
		DeleteObject(bmp);
		bmp = fitBmp;
	}

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


void CAdToolDlg::OnDropFiles(HDROP hDropInfo)
{
	// 获取拖拽的文件名
	wchar_t szFileName[MAX_PATH + 1];
	memset(szFileName, 0, sizeof(szFileName));
	DragQueryFile(hDropInfo, 0, szFileName, MAX_PATH);

	// 查找目标控件
	POINT pt;
	if (DragQueryPoint(hDropInfo, &pt))
	{
		for (int controlId = IDC_AD_EDIT_1; controlId <= IDC_AD_EDIT_30; controlId += 2)
		{
			CRect rect;
			GetDlgItem(controlId)->GetWindowRect(rect);
			ScreenToClient(&rect);
			if (rect.PtInRect(pt))
			{
				GetDlgItem(controlId)->SetWindowText(szFileName);
				break;
			}
		}
	}

	CDialogEx::OnDropFiles(hDropInfo);
}

void CAdToolDlg::OnBnClickedGenImageBtn()
{
	CString groupName;
	m_groupCombo.GetWindowText(groupName);
	if (groupName.IsEmpty())
	{
		MessageBox(L"请选择一个分组", L"提示", MB_OK);
		return;
	}

	std::vector<CTemplateItem> templates = GetTemplatesByGroupName((LPCTSTR)groupName);
	if (templates.size() == 0)
	{
		MessageBox(L"没找到任何模板", L"提示", MB_OK);
		return;
	}

	// 从这些模板中，收集不同类别的广告
	std::vector<CAdItem> ads = GetAdsFromTemplates(templates);
	if (ads.size() == 0)
	{
		MessageBox(L"没配置任何广告", L"提示", MB_OK);
		return;
	}

	// 从界面获取广告输入配置并校验
	CAdSettingItem adSettings[AD_TYPE_MAX];
	GetAdSettings(adSettings);

	// 校验广告配置
	if (!CheckAdSettings(ads, adSettings))
	{
		return;
	}

	// 选择保存路径
	BROWSEINFO   bInfo;
	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.hwndOwner = m_hWnd;	
	bInfo.lpszTitle = _T("选择保存路径");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST lpDlist = SHBrowseForFolder(&bInfo);   //显示选择对话框
	if (lpDlist == NULL)
	{
		return;
	}

	wchar_t savePath[MAX_PATH];
	SHGetPathFromIDList(lpDlist, savePath); //把项目标识列表转化成目录

	// 开始生成
	CGenImageDlg genImageDlg;
	genImageDlg.SetParams(templates, adSettings, savePath);
	genImageDlg.DoModal();

	if (genImageDlg.IsSuccess())
	{
		ShellExecute(NULL, L"open", savePath, NULL, NULL, SW_SHOW);
	}
	else
	{
		MessageBox(L"图片生成失败", L"提示", MB_OK);
	}
}
