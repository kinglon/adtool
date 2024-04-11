#pragma once
#include "afxdialogex.h"


// CTemplateDlg 对话框

class CTemplateDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTemplateDlg)

public:
	CTemplateDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTemplateDlg();

public:
	// 模板ID
	std::wstring m_id;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEMPLATE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
