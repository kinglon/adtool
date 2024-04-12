#pragma once
#include "afxdialogex.h"


// CGetTextDlg 对话框

class CGetTextDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGetTextDlg)

public:
	CGetTextDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CGetTextDlg();

public:
	// 窗口标题
	std::wstring m_strWndTitle;

	// 文本字段名字
	std::wstring m_textFieldName;

	// 用户输入的文本内容
	CString m_textContent;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GET_TEXT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_textTitle;
	CEdit m_textEdit;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
