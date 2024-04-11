// TempManagerDlg.cpp: 实现文件
//

#include "pch.h"
#include "adtool.h"
#include "afxdialogex.h"
#include "TempManagerDlg.h"
#include "SettingManager.h"
#include "TemplateDlg.h"


// CTempManagerDlg 对话框

IMPLEMENT_DYNAMIC(CTempManagerDlg, CDialogEx)

CTempManagerDlg::CTempManagerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TEMPLATE_MANAGER_DIALOG, pParent)
{

}

CTempManagerDlg::~CTempManagerDlg()
{
}

void CTempManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_GROUP, m_groupEdit);
	DDX_Control(pDX, IDC_LIST_TEMPLATE, m_templateList);
}


BEGIN_MESSAGE_MAP(CTempManagerDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CTempManagerDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CTempManagerDlg::OnBnClickedButtonSearch)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_TEMPLATE, &CTempManagerDlg::OnNMRClickListTemplate)
	ON_COMMAND(ID_TEMPLATE_EDIT, &CTempManagerDlg::OnTemplateEdit)
	ON_COMMAND(ID_TEMPLATE_DELETE, &CTempManagerDlg::OnTemplateDelete)
END_MESSAGE_MAP()


void CTempManagerDlg::InitControlData(const std::vector<CTemplateItem>& templates)
{
	// 初始化列表数据
	m_templateList.DeleteAllItems();
	for (unsigned int j=0; j<templates.size(); j++)
	{		
		m_templateList.InsertItem(j, templates[j].m_name.c_str());
		m_templateList.SetItemText(j, 1, templates[j].m_groupName.c_str());
		m_templateList.SetItemText(j, 2, std::to_wstring(templates[j].m_ads.size()).c_str());
	}
}


void CTempManagerDlg::OnBnClickedButtonAdd()
{
	CTemplateDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		InitControlData(CSettingManager::GetInstance()->m_templates);
	}
}


BOOL CTempManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 初始化控件样式
	m_templateList.InsertColumn(0, _T("模板"), LVCFMT_LEFT, 180);
	m_templateList.InsertColumn(1, _T("分组"), LVCFMT_LEFT, 180);
	m_templateList.InsertColumn(2, _T("广告数"), LVCFMT_CENTER, 70);
	m_templateList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	InitControlData(CSettingManager::GetInstance()->m_templates);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CTempManagerDlg::OnBnClickedButtonSearch()
{
	CString groupName;
	m_groupEdit.GetWindowText(groupName);
	if (groupName.IsEmpty())
	{
		MessageBox(L"分组不能为空", L"提示", MB_OK);
		return;
	}

	const auto templates = CSettingManager::GetInstance()->m_templates;
	std::vector<CTemplateItem> results;
	for (const auto& item : templates)
	{
		if (item.m_groupName.find(groupName) != -1)
		{
			results.push_back(item);
		}
	}

	InitControlData(results);
}


void CTempManagerDlg::OnNMRClickListTemplate(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	CMenu menu;
	menu.LoadMenu(IDR_MENU_TEMPLATE);

	CMenu* pContextMenu = menu.GetSubMenu(0);
	if (pContextMenu == nullptr)
	{
		return;
	}

	CPoint point;
	GetCursorPos(&point);
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}


void CTempManagerDlg::OnTemplateEdit()
{
	int selIndex = m_templateList.GetNextItem(-1, LVNI_SELECTED);
	if (selIndex < 0)
	{
		return;
	}

	// 获取模板ID
	CString templateName = m_templateList.GetItemText(selIndex, 0);
	std::wstring id;
	auto& templates = CSettingManager::GetInstance()->m_templates;
	for (auto it = templates.begin(); it != templates.end(); it++)
	{
		if (it->m_name == (LPCTSTR)templateName)
		{
			id = it->m_id;
			break;
		}
	}
	if (id.empty())
	{
		return;
	}

	CTemplateDlg dlg;
	dlg.m_id = id;
	if (dlg.DoModal() == IDOK)
	{
		InitControlData(CSettingManager::GetInstance()->m_templates);
	}
}


void CTempManagerDlg::OnTemplateDelete()
{	
	if (MessageBox(L"您确定要删除选择的模板吗？", L"提示", MB_OKCANCEL) == IDCANCEL)
	{
		return;
	}

	int selIndex = m_templateList.GetNextItem(-1, LVNI_SELECTED);
	if (selIndex < 0)
	{
		return;
	}
	CString templateName = m_templateList.GetItemText(selIndex, 0);
	auto& templates = CSettingManager::GetInstance()->m_templates;
	for (auto it = templates.begin(); it != templates.end(); it++)
	{
		if (it->m_name == (LPCTSTR)templateName)
		{
			templates.erase(it);
			break;
		}
	}
	CSettingManager::GetInstance()->Save();
	InitControlData(templates);
}
