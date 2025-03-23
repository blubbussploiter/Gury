#include "stdout.h"

#include "../Gury/Studio/pch.h"
#include "../Gury/Studio/MainFrm.h"
#include "../Gury/Main/framework.h"

void RBX::StandardOut::print(MessageType type, const char* format, ...)
{
	va_list argPtr;
	std::string _fmt, date;

	_SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	va_start(argPtr, format);
	_fmt = G3D::vformat(format, argPtr);

	date = RBX::Format("%s %s %d %d:%d:%d %d - ", 
		RBX::Date::daysOfWeek[SystemTime.wDayOfWeek],
		RBX::Date::months[SystemTime.wMonth],
		SystemTime.wDay,
		SystemTime.wHour,
		SystemTime.wMinute,
		SystemTime.wSecond,
		SystemTime.wYear);

	RBX::Log::writeEntry(date.c_str());
	RBX::Log::writeEntry(_fmt.c_str());
	RBX::Log::writeEntry("\r\n");
	CMainFrame::mainFrame->m_wndOutput.AddText(type, date, _fmt);

	va_end(argPtr);
}