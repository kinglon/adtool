#pragma once


class IMyStaticCallback
{
public:
	virtual void OnStaticMouseMove(HWND hwnd, CPoint point) = 0;
};

class CMyStatic : public CStatic
{
	DECLARE_DYNAMIC(CMyStatic)

public:
	CMyStatic();
	virtual ~CMyStatic();

public:
	void SetCallback(IMyStaticCallback* callback) { m_callback = callback;  }

private:
	IMyStaticCallback* m_callback = nullptr;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


