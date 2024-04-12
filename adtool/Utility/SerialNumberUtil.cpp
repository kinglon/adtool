#include "pch.h"
#include "SerialNumberUtil.h"

std::wstring CSerialNumberUtil::GetSerialNumber()
{
	srand((unsigned int)time(0));
	int num[4][5];  // 4个部分，每个部分5个字符，第3部分由其他的计算所得	
	for (int i = 0; i < 5; i++)
	{
		int sum = 0;
		for (int j = 0; j < 4; j++)
		{
			if (j == 2)
			{
				continue;
			}
			num[j][i] = rand() % 10;
			sum += num[j][i];
		}

		num[2][i] = sum % 10;
	}

	// 转为字符
	std::wstring sn;
	for (int i = 0; i < 4; i++)
	{
		if (!sn.empty())
		{
			sn += L'-';
		}

		for (int j = 0; j < 5; j++)
		{
			sn += (wchar_t)(num[i][j] + L'0');
		}
	}

	return sn;
}

bool CSerialNumberUtil::IsValid(std::wstring sn)
{
	// 移除-符号
	std::wstring newSn;
	for (wchar_t c : sn) 
	{
		if (c != L'-') 
		{
			newSn += c;
		}
	}
	if (newSn.length() != 20)
	{
		return false;
	}

	// 转为数字
	int num[4][5];
	for (int i = 0; i < 20; i++)
	{
		int n = (int)(newSn[i] - L'0');
		if (n < 0 || n > 9)
		{
			return false;
		}
		num[i / 5][i % 5] = n;
	}

	// 校验
	for (int i = 0; i < 5; i++)
	{
		int sum = 0;
		for (int j = 0; j < 4; j++)
		{
			if (j == 2)
			{
				continue;
			}			
			sum += num[j][i];
		}

		if (num[2][i] != sum % 10)
		{
			return false;
		}
	}

	return true;
}