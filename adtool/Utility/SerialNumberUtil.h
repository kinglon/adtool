#pragma once

#include <string>

class CSerialNumberUtil
{
public:
	// �������һ�����к�
	static std::wstring GetSerialNumber();

	// У�����к��Ƿ���Ч
	static bool IsValid(std::wstring sn);
};

