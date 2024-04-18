// AdDlg.cpp: 实现文件
//

#include "pch.h"
#include "adtool.h"
#include "afxdialogex.h"
#include "AdDlg.h"


// CAdDlg 对话框

IMPLEMENT_DYNAMIC(CAdDlg, CDialogEx)

CAdDlg::CAdDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_AD_DIALOG, pParent)
{

}

CAdDlg::~CAdDlg()
{
}

void CAdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_X, m_editX);
	DDX_Control(pDX, IDC_EDIT_Y, m_editY);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_editWidth);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_editHeight);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_comboType);
	DDX_Control(pDX, IDC_COMBO_IMAGE_ALIGN, m_comboImageAlign);
	DDX_Control(pDX, IDC_CHECK_TEXT, m_checkVerShow);
	DDX_Control(pDX, IDC_COMBO_FONT, m_comboFont);
	DDX_Control(pDX, IDC_COMBO_TEXT_ALIGN, m_comboTextAlign);
}


BEGIN_MESSAGE_MAP(CAdDlg, CDialogEx)
	ON_COMMAND(ID_AD_EDIT, &CAdDlg::OnAdEdit)
	ON_COMMAND(ID_AD_DELETE, &CAdDlg::OnAdDelete)
	ON_BN_CLICKED(IDOK, &CAdDlg::OnBnClickedOk)
END_MESSAGE_MAP()


CString CAdDlg::GetGuid()
{
	GUID guid;
	if (CoCreateGuid(&guid) == S_OK)
	{
		wchar_t guidString[40] = { 0 };
		if (StringFromGUID2(guid, guidString, 40) != 0)
		{
			return guidString;
		}
	}

	return L"";
}

void CAdDlg::InitControls()
{
	// 初始化广告类别
	std::wstring adNames[AD_TYPE_MAX];
	CSettingManager::GetInstance()->GetAdNames(adNames);
	m_comboType.Clear();
	for (int i = 0; i < AD_TYPE_MAX; i++)
	{
		m_comboType.AddString(adNames[i].c_str());
	}

	// 初始化图片对齐
	m_comboImageAlign.Clear();
	m_comboImageAlign.AddString(L"上");
	m_comboImageAlign.AddString(L"下");
	m_comboImageAlign.AddString(L"左");
	m_comboImageAlign.AddString(L"右");
	m_comboImageAlign.AddString(L"居中");

	// 初始化字体
	m_comboFont.Clear();
	m_comboFont.AddString(FONT_FANGZHENG_DAHEI);
	m_comboFont.AddString(FONT_FANGZHENG_ZONGYI);
	m_comboFont.AddString(FONT_FANGZHENG_XINGKAI);
	m_comboFont.AddString(FONT_FANGZHENG_JIANTI);

	// 文字对齐
	m_comboTextAlign.Clear();
	m_comboTextAlign.AddString(L"上");
	m_comboTextAlign.AddString(L"下");
	m_comboTextAlign.AddString(L"左");
	m_comboTextAlign.AddString(L"右");

	// 初始化数据
	if (m_ad.m_id.empty())  // 添加广告
	{
		m_comboType.SetCurSel(0);
		m_comboImageAlign.SetCurSel(0);
		m_comboFont.SetCurSel(0);
		m_comboTextAlign.SetCurSel(0);
	}
	else
	{
		m_editX.SetWindowText(std::to_wstring(m_ad.m_region.left).c_str());
		m_editY.SetWindowText(std::to_wstring(m_ad.m_region.top).c_str());
		m_editWidth.SetWindowText(std::to_wstring(m_ad.m_region.right-m_ad.m_region.left).c_str());
		m_editHeight.SetWindowText(std::to_wstring(m_ad.m_region.bottom-m_ad.m_region.top).c_str());
		m_comboType.SetCurSel(m_ad.m_type - 1);
		m_comboImageAlign.SetCurSel(m_ad.m_imageAlign - 1);
		if (!m_ad.m_bHorizon)
		{
			m_checkVerShow.SetCheck(BST_CHECKED);
		}
		m_comboFont.SelectString(-1, m_ad.m_fontName.c_str());
		m_comboTextAlign.SetCurSel(m_ad.m_textAlign - 1);
	}
}

void CAdDlg::OnAdEdit()
{
	// TODO: 在此添加命令处理程序代码
}


void CAdDlg::OnAdDelete()
{
	// TODO: 在此添加命令处理程序代码
}


BOOL CAdDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CAdDlg::OnBnClickedOk()
{
	CString text;
	m_editX.GetWindowText(text);
	if (text.IsEmpty())
	{
		MessageBox(L"X不能为空", L"提示", MB_OK);
		return;
	}
	m_ad.m_region.left = _wtoi(text);

	m_editY.GetWindowText(text);
	if (text.IsEmpty())
	{
		MessageBox(L"Y不能为空", L"提示", MB_OK);
		return;
	}
	m_ad.m_region.top = _wtoi(text);

	m_editWidth.GetWindowText(text);
	if (text.IsEmpty())
	{
		MessageBox(L"宽不能为空", L"提示", MB_OK);
		return;
	}
	m_ad.m_region.right = m_ad.m_region.left + _wtoi(text);

	m_editHeight.GetWindowText(text);
	if (text.IsEmpty())
	{
		MessageBox(L"高不能为空", L"提示", MB_OK);
		return;
	}
	m_ad.m_region.bottom = m_ad.m_region.top + _wtoi(text);

	m_ad.m_type = m_comboType.GetCurSel() + 1;
	m_ad.m_imageAlign = m_comboImageAlign.GetCurSel() + 1;
	m_ad.m_bHorizon = !(m_checkVerShow.GetCheck() == BST_CHECKED);
	m_ad.m_textAlign = m_comboTextAlign.GetCurSel() + 1;
	m_comboFont.GetWindowText(text);
	m_ad.m_fontName = text;
	if (m_ad.m_bHorizon)
	{
		if (m_ad.m_textAlign != AD_ALIGN_TOP && m_ad.m_textAlign != AD_ALIGN_BOTTOM)
		{
			MessageBox(L"文字水平显示，请选择上对齐或下对齐", L"提示", MB_OK);
			return;
		}
	}
	else
	{
		if (m_ad.m_textAlign != AD_ALIGN_LEFT && m_ad.m_textAlign != AD_ALIGN_RIGHT)
		{
			MessageBox(L"文字垂直显示，请选择左对齐或右对齐", L"提示", MB_OK);
			return;
		}
	}

	if (m_ad.m_id.empty())
	{
		m_ad.m_id = GetGuid();
	}

	CDialogEx::OnOK();
}
