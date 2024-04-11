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
}


BEGIN_MESSAGE_MAP(CAdDlg, CDialogEx)
	ON_COMMAND(ID_AD_EDIT, &CAdDlg::OnAdEdit)
	ON_COMMAND(ID_AD_DELETE, &CAdDlg::OnAdDelete)
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


void CAdDlg::OnAdEdit()
{
	// TODO: 在此添加命令处理程序代码
}


void CAdDlg::OnAdDelete()
{
	// TODO: 在此添加命令处理程序代码
}
