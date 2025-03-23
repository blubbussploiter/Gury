
#include <fstream>

#include "../Gury/Studio/pch.h"
#include "../Gury/Studio/MainFrm.h"
#include "../Gury/Main/framework.h"

#include "log.h"

std::ofstream* log_outStream;

std::string dayOfWeek(WORD d)
{
	switch (d)
	{
	case 1:
		return "Monday";
	case 2:
		return "Tuesday";
	case 3:
		return "Wednesday";
	case 4:
		return "Thursday";
	case 5:
		return "Friday";
	case 6:
		return "Saturday";
	case 7:
		return "Sunday";
	}
	return "";
}

void RBX::Log::cleanup()
{
	if (log_outStream)
	{
		log_outStream->close();
	}
}

void RBX::Log::writeEntry(const char* message, bool writeToUi)
{

	if (writeToUi)
	{
		CMainFrame::mainFrame->m_wndOutput.AddTextQuick(message);
	}

	if (!log_outStream || log_outStream && !log_outStream->is_open())
	{
		log_outStream = new std::ofstream(ConFileInPath("\\gury-log.txt"));
	}
	else
	{
		if (log_outStream->is_open())
		{
			log_outStream->write(message, strlen(message));
			log_outStream->flush();
		}
		else
		{
			MessageBox(0, "Cannot write to log file", "Gury error", 0);
		}
	}
}
