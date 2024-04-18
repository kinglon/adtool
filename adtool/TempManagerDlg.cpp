// TempManagerDlg.cpp: 实现文件
//

#include "pch.h"
#include "adtool.h"
#include "afxdialogex.h"
#include "TempManagerDlg.h"
#include "SettingManager.h"
#include "TemplateDlg.h"
#include "ImPath.h"
#include <fstream>
#include <json/json.h>
#include "ImCharset.h"


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
	DDX_Control(pDX, IDC_EDIT_TEMPLATE, m_templateEdit);
}


BEGIN_MESSAGE_MAP(CTempManagerDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CTempManagerDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CTempManagerDlg::OnBnClickedButtonSearch)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_TEMPLATE, &CTempManagerDlg::OnNMRClickListTemplate)
	ON_COMMAND(ID_TEMPLATE_EDIT, &CTempManagerDlg::OnTemplateEdit)
	ON_COMMAND(ID_TEMPLATE_DELETE, &CTempManagerDlg::OnTemplateDelete)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CTempManagerDlg::OnBnClickedButtonExport)
END_MESSAGE_MAP()


void CTempManagerDlg::InitControlData(const std::vector<CTemplateItem>& templates)
{
	// 初始化列表数据
	m_templateList.DeleteAllItems();
	for (unsigned int j=0; j<templates.size(); j++)
	{		
		m_templateList.InsertItem(j, templates[j].m_name.c_str());
		m_templateList.SetItemText(j, 1, templates[j].m_groupNames.c_str());
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
	m_templateList.InsertColumn(1, _T("分组"), LVCFMT_LEFT, 280);
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
	CString templateName;
	m_templateEdit.GetWindowText(templateName);

	const auto templates = CSettingManager::GetInstance()->m_templates;
	std::vector<CTemplateItem> results;
	for (const auto& item : templates)
	{
		if (!templateName.IsEmpty() && item.m_name.find(templateName) == -1)
		{
			continue;
		}

		if (!groupName.IsEmpty() && item.m_groupNames.find(groupName) == -1)
		{
			continue;
		}

		results.push_back(item);
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
	CTemplateItem templateItem;
	auto& templates = CSettingManager::GetInstance()->m_templates;
	for (auto it = templates.begin(); it != templates.end(); it++)
	{
		if (it->m_name == (LPCTSTR)templateName)
		{
			templateItem = *it;
			break;
		}
	}
	if (templateItem.m_id.empty())
	{
		return;
	}

	CTemplateDlg dlg;
	dlg.m_template = templateItem;
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
			// 删除模板图片
			DeleteFile((CImPath::GetDataPath() + L"\\" + it->m_imageFileName).c_str());
			templates.erase(it);
			break;
		}
	}
	CSettingManager::GetInstance()->Save();
	InitControlData(templates);
}


void CTempManagerDlg::OnBnClickedButtonExport()
{
	// 选择导入路径
	CFolderPickerDialog folderPickerDlg(nullptr, 0, this, sizeof(OPENFILENAME));
	if (folderPickerDlg.DoModal() == IDCANCEL)
	{
		return;
	}
	std::wstring exportPath = (LPCTSTR)folderPickerDlg.GetPathName();

	std::wstring dataPath = exportPath + L"\\data";
	std::wstring configFilePath = exportPath + L"\\Configs\\configs.json";
	if (!PathFileExists(dataPath.c_str()) || !PathFileExists(configFilePath.c_str()))
	{
		MessageBox(L"没有找到配置文件", L"提示", MB_OK);
		return;
	}

	// 拷贝data文件夹
	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile((dataPath + L"\\*.*").c_str(), &findData);
	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do 
		{
			if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
			{
				std::wstring sourceFilePath = dataPath + L"\\" + findData.cFileName;
				std::wstring destinationFilePath = CImPath::GetDataPath() + findData.cFileName;
				if (!CopyFile(sourceFilePath.c_str(), destinationFilePath.c_str(), FALSE))
				{
					MessageBox(L"拷贝图片失败", L"提示", MB_OK);
					return;
				}
			}
		} while (FindNextFile(hFind, &findData));
		FindClose(hFind);
	}

	// 读取模板配置	
	std::ifstream inputFile(configFilePath);
	if (!inputFile.is_open())
	{
		MessageBox(L"打开配置文件失败", L"提示", MB_OK);
		return;
	}

	std::string jsonString;
	std::string line;
	while (std::getline(inputFile, line))
	{
		jsonString += line;
	}
	inputFile.close();

	Json::Value root;
	Json::CharReaderBuilder builder;
	Json::CharReader* reader = builder.newCharReader();
	std::string errors;
	bool parsingSuccessful = reader->parse(jsonString.c_str(), jsonString.c_str() + jsonString.size(), &root, &errors);
	delete reader;
	if (!parsingSuccessful)
	{
		MessageBox(L"解析配置文件失败", L"提示", MB_OK);
		return;
	}

	if (root.isMember("template"))
	{
		auto& templates = CSettingManager::GetInstance()->m_templates;
		for (unsigned int i = 0; i < root["template"].size(); i++)
		{
			auto& value = root["template"][i];
			CTemplateItem item;
			item.m_id = CImCharset::UTF8ToUnicode(value["id"].asString().c_str());
			item.m_name = CImCharset::UTF8ToUnicode(value["name"].asString().c_str());
			item.m_groupNames = CImCharset::UTF8ToUnicode(value["group_name"].asString().c_str());
			item.m_imageFileName = CImCharset::UTF8ToUnicode(value["image_file"].asString().c_str());

			for (unsigned int j = 0; j < value["ad"].size(); j++)
			{
				auto& adValue = value["ad"][j];
				CAdItem adItem;
				adItem.m_id = CImCharset::UTF8ToUnicode(adValue["id"].asString().c_str());
				adItem.m_region.left = adValue["left"].asInt();
				adItem.m_region.top = adValue["top"].asInt();
				adItem.m_region.right = adValue["right"].asInt();
				adItem.m_region.bottom = adValue["bottom"].asInt();
				adItem.m_type = adValue["type"].asInt();
				adItem.m_bHorizon = adValue["horizon"].asBool();
				adItem.m_fontName = CImCharset::UTF8ToUnicode(adValue["font_name"].asString().c_str());
				adItem.m_imageAlign = adValue["image_align"].asInt();
				adItem.m_textAlign = adValue["text_align"].asInt();
				item.m_ads.push_back(adItem);
			}

			templates.push_back(item);
		}
		CSettingManager::GetInstance()->Save();
	}

	MessageBox(L"导入模板成功", L"提示", MB_OK);
	InitControlData(CSettingManager::GetInstance()->m_templates);
}
