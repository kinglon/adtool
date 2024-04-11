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

private:
	// 初始化控件，只调用一次
	void InitControls();

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
	CEdit m_editX;
	CEdit m_editY;
	CEdit m_editWidth;
	CEdit m_editHeight;
	CComboBox m_comboType;
	CComboBox m_comboImageAlign;
	CButton m_checkVerShow;
	CComboBox m_comboFont;
	CComboBox m_comboTextAlign;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
