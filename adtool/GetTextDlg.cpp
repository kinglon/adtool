// GetTextDlg.cpp: 实现文件
//

#include "pch.h"
#include "adtool.h"
#include "afxdialogex.h"
#include "GetTextDlg.h"


// CGetTextDlg 对话框

IMPLEMENT_DYNAMIC(CGetTextDlg, CDialogEx)

CGetTextDlg::CGetTextDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GET_TEXT_DIALOG, pParent)
{

}

CGetTextDlg::~CGetTextDlg()
{
}

void CGetTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TITLE, m_textTitle);
	DDX_Control(pDX, IDC_EDIT_TEXT, m_textEdit);
}


BEGIN_MESSAGE_MAP(CGetTextDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CGetTextDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CGetTextDlg 消息处理程序


BOOL CGetTextDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(m_strWndTitle.c_str());
	m_textTitle.SetWindowTextW(m_textFieldName.c_str());

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CGetTextDlg::OnBnClickedOk()
{
	m_textContent = L"";
	m_textEdit.GetWindowText(m_textContent);
	CDialogEx::OnOK();
}
