#pragma once

#include <random>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>

static std::wstring generateGuid()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<unsigned int> dis(0, 255);

	std::vector<unsigned char> bytes(16);
	for (int i = 0; i < 16; ++i) {
		bytes[i] = static_cast<unsigned char>(dis(gen));
	}

	// Set version to 4 (xxxxxxxx-xxxx-4xxx-...)
	bytes[6] = (bytes[6] & 0x0F) | 0x40;

	// Set variant to 1 (..., -Nxxx-..., where N is 8, 9, A, or B)
	bytes[8] = (bytes[8] & 0x3F) | 0x80;

	std::wstringstream ss;
	ss << L"{";
	ss << std::hex << std::setfill(L'0') << std::uppercase;

	for (int i = 0; i < 4; ++i) ss << std::setw(2) << static_cast<int>(bytes[i]);
	ss << L"-";
	for (int i = 4; i < 6; ++i) ss << std::setw(2) << static_cast<int>(bytes[i]);
	ss << L"-";
	for (int i = 6; i < 8; ++i) ss << std::setw(2) << static_cast<int>(bytes[i]);
	ss << L"-";
	for (int i = 8; i < 10; ++i) ss << std::setw(2) << static_cast<int>(bytes[i]);
	ss << L"-";
	for (int i = 10; i < 16; ++i) ss << std::setw(2) << static_cast<int>(bytes[i]);

	ss << L"}";

	return ss.str();
}

