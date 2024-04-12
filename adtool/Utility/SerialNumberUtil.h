#pragma once

#include <string>

class CSerialNumberUtil
{
public:
	// 随机生成一个序列号
	static std::wstring GetSerialNumber();

	// 校验序列号是否有效
	static bool IsValid(std::wstring sn);
};

