#pragma once
#include "afxdialogex.h"
#include <vector>
#include "SettingManager.h"
#include "TemplateRender.h"
#include <thread>

// CGenImageDlg 对话框

class CGenImageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGenImageDlg)

public:
	CGenImageDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CGenImageDlg();

public:
	void SetParams(const std::vector<CTemplateItem>& templates, CAdSettingItem adSettings[AD_TYPE_MAX], std::wstring savePath);

	bool IsSuccess() { return m_success; }

private:
	void ThreadProc();

private:
	std::wstring m_savePath;

	std::vector<CTemplateItem> m_templates;

	CAdSettingItem m_adSettings[AD_TYPE_MAX];

	std::thread* m_thread = nullptr;

	bool m_requestClose = false;

	bool m_threadExit = false;

	bool m_success = true;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GEN_IMAGE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CStatic m_progressStatic;
	CProgressCtrl m_progress;
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
