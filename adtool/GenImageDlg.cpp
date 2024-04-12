// GenImageDlg.cpp: 实现文件
//

#include "pch.h"
#include "adtool.h"
#include "afxdialogex.h"
#include "GenImageDlg.h"
#include <thread>
#include "TemplateRender.h"

#define WM_FINISH_ONE_ITEM		WM_USER+100


// CGenImageDlg 对话框

IMPLEMENT_DYNAMIC(CGenImageDlg, CDialogEx)

CGenImageDlg::CGenImageDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GEN_IMAGE_DIALOG, pParent)
{

}

CGenImageDlg::~CGenImageDlg()
{
}

void CGenImageDlg::SetParams(const std::vector<CTemplateItem>& templates, CAdSettingItem adSettings[AD_TYPE_MAX], std::wstring savePath)
{
	m_templates = templates;
	for (int i = 0; i < AD_TYPE_MAX; i++)
	{
		m_adSettings[i] = adSettings[i];
	}
	m_savePath = savePath;
}

void CGenImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PROG_VALUE, m_progressStatic);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
}


BEGIN_MESSAGE_MAP(CGenImageDlg, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CGenImageDlg 消息处理程序


BOOL CGenImageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_progress.SetRange32(0, (int)m_templates.size());
	m_progress.SetPos(0);
	
	// 启动图片生成线程
	m_thread = new std::thread(&CGenImageDlg::ThreadProc, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CGenImageDlg::ThreadProc()
{
	LOG_INFO(L"the thread of generating image is running");

	for (const auto& tempItem : m_templates)
	{
		if (m_requestClose)
		{
			m_success = false;
			break;
		}

		HBITMAP bmp = CTemplateRender::Do(tempItem, m_adSettings, false);
		if (bmp == NULL)
		{
			m_success = false;
			break;
		}

		CImage image;
		image.Attach(bmp);
		std::wstring imageSavePath = m_savePath + L"\\" + tempItem.m_name + L".png";
		HRESULT hr = image.Save(imageSavePath.c_str());
		image.Detach();
		DeleteObject(bmp);
		if (FAILED(hr))
		{
			LOG_ERROR(L"failed to save image, error is 0x%x, path is: %s", hr, imageSavePath.c_str());
			m_success = false;
			break;
		}

		PostMessage(WM_FINISH_ONE_ITEM);
	}

	m_threadExit = true;
	PostMessage(WM_CLOSE);
}

void CGenImageDlg::OnClose()
{
	m_requestClose = true;
	if (!m_threadExit)
	{
		return;
	}

	CDialogEx::OnClose();
}


BOOL CGenImageDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_FINISH_ONE_ITEM)
	{
		m_progress.SetPos(m_progress.GetPos() + 1);
		CString progressValue;
		progressValue.Format(L"%d/%d", m_progress.GetPos(), m_templates.size());
		m_progressStatic.SetWindowText(progressValue);
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
