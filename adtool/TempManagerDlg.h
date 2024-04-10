#pragma once
#include "afxdialogex.h"


// CTempManagerDlg 对话框

class CTempManagerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTempManagerDlg)

public:
	CTempManagerDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTempManagerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEMPLATE_MANAGER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
