
// adtool.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "adtool.h"
#include "AdToolDlg.h"

#include "LogUtil.h"
#include "DumpUtil.h"
#include "ImPath.h"
#include "ImCharset.h"
#include "SettingManager.h"
#include <gdiplus.h>
#include "GetTextDlg.h"
#include <fstream>
#include "SerialNumberUtil.h"

using namespace Gdiplus;
using namespace std;

#pragma comment(lib, "gdiplus.lib") 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAdToolApp

BEGIN_MESSAGE_MAP(CAdToolApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAdToolApp 构造

CAdToolApp::CAdToolApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CAdToolApp 对象

CAdToolApp theApp;
CLogUtil* g_dllLog = nullptr;

bool CheckSN()
{
	// 本地读取序列号
	std::string sn;
	std::wstring snFilePath = CImPath::GetLocalAppDataPath(APPNAME) + L"{B410BF0F-4A08-4971-8A29-45517306A784}";
	ifstream file(snFilePath.c_str());	
	if (file.is_open()) 
	{
		getline(file, sn);
		file.close();
	}
	if (!sn.empty())
	{
		if (CSerialNumberUtil::IsValid(CImCharset::UTF8ToUnicode(sn.c_str())))
		{
			return true;
		}
	}

	// 弹窗输入序列号
	CGetTextDlg getTextDlg;
	getTextDlg.m_strWndTitle = L"软件激活";
	getTextDlg.m_textFieldName = L"注册码";
	if (getTextDlg.DoModal() == IDCANCEL)
	{
		return false;
	}
	if (!CSerialNumberUtil::IsValid((LPCTSTR)getTextDlg.m_textContent))
	{
		MessageBox(NULL, L"序列号无效", L"提示", MB_OK);
		return false;
	}

	// 保存序列号
	ofstream of(snFilePath.c_str(), ios::trunc);
	if (of.is_open()) 
	{
		of << (CImCharset::UnicodeToUTF8((LPCTSTR)getTextDlg.m_textContent).c_str());			
		of.close();
	}

	return true;
}

BOOL CAdToolApp::InitInstance()
{
	CWinApp::InitInstance();


	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	// 单实例
	const wchar_t* mutexName = L"{4ED33E4A-D47A-4D0A-8523-168D74420098}";
	HANDLE mutexHandle = CreateMutexW(nullptr, TRUE, mutexName);
	if (mutexHandle == nullptr || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		AfxMessageBox(L"程序已经在运行");
		return FALSE;
	}	

	CoInitialize(nullptr);

	g_dllLog = CLogUtil::GetLog(L"main");

	//初始化崩溃转储机制
	CDumpUtil::SetDumpFilePath(CImPath::GetDumpPath().c_str());
	CDumpUtil::Enable(true);

	int nLogLevel = CSettingManager::GetInstance()->GetLogLevel();
	g_dllLog->SetLogLevel((ELogLevel)nLogLevel);

	// Initialize GDI+
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
		
	if (!CheckSN())
	{
		return FALSE;
	}

	CAdToolDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// Shutdown GDI+
	GdiplusShutdown(gdiplusToken);

	CloseHandle(mutexHandle);

	CoUninitialize();


	// 删除上面创建的 shell 管理器。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

