// TemplateDlg.cpp: 实现文件
//

#include "pch.h"
#include "adtool.h"
#include "afxdialogex.h"
#include "TemplateDlg.h"


// CTemplateDlg 对话框

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
}


BEGIN_MESSAGE_MAP(CTemplateDlg, CDialogEx)
END_MESSAGE_MAP()


// CTemplateDlg 消息处理程序
