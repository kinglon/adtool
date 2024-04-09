#pragma once

class CSettingManager
{
protected:
	CSettingManager();

public:
	static CSettingManager* GetInstance();

public:
	int GetLogLevel() { return m_nLogLevel; }

	int GetRowCount() { return m_rowCount; }

	void SetRowCount(int rowCount);

	int GetColumnCount() { return m_columnCount; }

	void SetColumnCount(int columnCount);

	int GetFrontCount() { return m_frontCount; }

	void SetFrontCount(int frontCount);

	int GetBackCount() { return m_backCount; }

	void SetBackCount(int backCount);

	CRect GetCaptureRect() { return m_captureRect; }

	void SetCaptureRect(CRect rect);

	int GetRecordFrameRate() { return m_recordFrameRate; }

	int GetCacheDuration() { return m_cacheDuration; }

	void Save();

private:
	void Load();

private:
	int m_nLogLevel = 2;  // debug

	int m_rowCount = 4;

	int m_columnCount = 4;

	int m_frontCount = 20;  // 抓怕前20张

	int m_backCount = 20;  // 抓拍后20张

	CRect m_captureRect = CRect(300, 300, 600, 600);

	int m_recordFrameRate = 30;

	int m_cacheDuration = 119;
};
