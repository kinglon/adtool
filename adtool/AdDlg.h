#pragma once
#include "afxdialogex.h"
#include "SettingManager.h"

// CAdDlg 对话框

class CAdDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAdDlg)

public:
	CAdDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAdDlg();

public:
	CAdItem m_ad;

public:
	static CString GetGuid();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AD_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAdEdit();
	afx_msg void OnAdDelete();
};
