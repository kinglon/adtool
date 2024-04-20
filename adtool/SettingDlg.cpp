// SettingDlg.cpp: 实现文件
//

#include "pch.h"
#include "adtool.h"
#include "afxdialogex.h"
#include "SettingDlg.h"
#include "SettingManager.h"


// CSettingDlg 对话框

IMPLEMENT_DYNAMIC(CSettingDlg, CDialogEx)

CSettingDlg::CSettingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SETTING_DIALOG, pParent)
{

}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ADNAME, m_comboAdName);
	DDX_Control(pDX, IDC_EDIT_ADNAME, m_editAdName);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_AD, &CSettingDlg::OnBnClickedButtonModifyAd)
	ON_BN_CLICKED(IDOK, &CSettingDlg::OnBnClickedOk)
END_MESSAGE_MAP()


void CSettingDlg::OnBnClickedButtonModifyAd()
{
	CString adName;
	m_editAdName.GetWindowText(adName);
	if (adName.IsEmpty())
	{
		MessageBox(L"广告名字不能为空", L"提示", MB_OK);
		return;
	}

	int curSel = m_comboAdName.GetCurSel();
	CSettingManager::GetInstance()->m_backupAdNames[curSel] = (LPCTSTR)adName;
	CSettingManager::GetInstance()->Save();

	m_comboAdName.ResetContent();
	for (int i = 0; i < BACKUP_AD_COUNT; i++)
	{
		m_comboAdName.AddString(CSettingManager::GetInstance()->m_backupAdNames[i].c_str());
	}
	m_comboAdName.SetCurSel(curSel);
}


void CSettingDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


BOOL CSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	for (int i = 0; i < BACKUP_AD_COUNT; i++)
	{
		m_comboAdName.AddString(CSettingManager::GetInstance()->m_backupAdNames[i].c_str());
	}
	m_comboAdName.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
