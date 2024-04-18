#pragma once
#include "afxdialogex.h"
#include "SettingManager.h"


// CTempManagerDlg 对话框

class CTempManagerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTempManagerDlg)

public:
	CTempManagerDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTempManagerDlg();

private:
	void InitControlData(const std::vector<CTemplateItem>& templates);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEMPLATE_MANAGER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAdd();
	virtual BOOL OnInitDialog();
	CEdit m_groupEdit;
	CListCtrl m_templateList;
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnNMRClickListTemplate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTemplateEdit();
	afx_msg void OnTemplateDelete();
	CEdit m_templateEdit;
	afx_msg void OnBnClickedButtonExport();
};
