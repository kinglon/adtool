// MyStatic.cpp: 实现文件
//

#include "pch.h"
#include "adtool.h"
#include "MyStatic.h"


// CMyStatic

IMPLEMENT_DYNAMIC(CMyStatic, CStatic)

CMyStatic::CMyStatic()
{

}

CMyStatic::~CMyStatic()
{
}


BEGIN_MESSAGE_MAP(CMyStatic, CStatic)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// CMyStatic 消息处理程序




void CMyStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_callback)
	{
		m_callback->OnStaticMouseMove(m_hWnd, point);
	}

	CStatic::OnMouseMove(nFlags, point);
}
