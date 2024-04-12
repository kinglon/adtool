
// AdToolDlg.h: 头文件
//

#pragma once

#include <set>
#include "TemplateRender.h"
#include "SettingManager.h"

// CAdToolDlg 对话框
class CAdToolDlg : public CDialogEx
{
// 构造
public:
	CAdToolDlg(CWnd* pParent = nullptr);	// 标准构造函数

private:
	void InitControls();

	// 初始化分组控件
	void InitGroupControl();

	std::vector<CTemplateItem> GetTemplatesByGroupName(std::wstring groupName);

	// 从指定的模板中收集不同类别的广告（一个类别只收集一次)
	std::vector<CAdItem> GetAdsFromTemplates(const std::vector<CTemplateItem>& templates);

	// 从广告列表按规则生成一个模板
	bool MakeTemplateItemFromAds(const std::vector<CAdItem>& ads, CTemplateItem& tempItem);

	CLSID GetEncoderClsid(const WCHAR* format);

	// 从界面获取广告输入设置
	void GetAdSettings(CAdSettingItem adSettings[AD_TYPE_MAX]);

	// 检查广告配置是否正确
	bool CheckAdSettings(const std::vector<CAdItem>& ads, CAdSettingItem adSettings[AD_TYPE_MAX]);

private:
	int m_initSizeX = 0;
	int m_initSizeY = 0;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CComboBox m_groupCombo;
	afx_msg void OnBnClickedTemplateManagerBtn();
	afx_msg void OnBnClickedPreviewBtn();
	CStatic m_previewImageCtrl;
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedGenImageBtn();
};
