#pragma once
#include "afxdialogex.h"
#include "SettingManager.h"
#include "TemplateRender.h"

// CTemplateDlg 对话框

class CTemplateDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTemplateDlg)

public:
	CTemplateDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTemplateDlg();

public:
	CTemplateItem m_template;

private:
	CAdSettingItem m_adSettings[AD_TYPE_MAX];

	// 初始窗口宽高
	int m_initSizeX = 0;
	int m_initSizeY = 0;

	// 预览图片显示缩放比例
	float m_scaleRatio = 1.0f;

	//预览图片显示相对于控件窗口的偏移
	int m_imgOffsetX = 0;
	int m_imgOffsetY = 0;

	// 准备编辑或删除等操作的广告
	std::wstring m_adId;

private:
	// 初始化控件，只调一次
	void InitControls();

	void UpdatePreviewCtrl();

	// 获取当前鼠标位置的广告
	CAdItem* GetCurrentPosAd();

	// 预览图片控件内的位置，转化为真实模板图片的位置
	void ImageCtrlToRealImage(LPPOINT pt);

	void HandleDropFile(HDROP hDrop);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEMPLATE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_nameEdit;
	CEdit m_groupNameEdit;
	CStatic m_previewImageCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CStatic m_nameStatic;
	CStatic m_groupNameStatic;
	CButton m_okBtn;
	CButton m_cancelBtn;
	afx_msg void OnBnClickedOk();
	afx_msg void OnStnDblclickPreviewImage();
	afx_msg void OnStnClickedPreviewImage();
	afx_msg void OnAdEdit();
	afx_msg void OnAdDelete();
	afx_msg void OnTemplatePreviewText();
};
