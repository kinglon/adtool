// TempManagerDlg.cpp: 实现文件
//

#include "pch.h"
#include "adtool.h"
#include "afxdialogex.h"
#include "TempManagerDlg.h"


// CTempManagerDlg 对话框

IMPLEMENT_DYNAMIC(CTempManagerDlg, CDialogEx)

CTempManagerDlg::CTempManagerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TEMPLATE_MANAGER_DIALOG, pParent)
{

}

CTempManagerDlg::~CTempManagerDlg()
{
}

void CTempManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTempManagerDlg, CDialogEx)
END_MESSAGE_MAP()


// CTempManagerDlg 消息处理程序
