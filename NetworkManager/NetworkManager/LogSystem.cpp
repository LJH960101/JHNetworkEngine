#include "pch.h"
#include "LogSystem.h"
#include <iostream>
#include <fstream>
#include <ctime>

CLogSystem::CLogSystem()
{
}

CLogSystem::~CLogSystem()
{
}

void CLogSystem::WriteLog(const ELogType& logType, const ELogLevel& logLevel, string& format)
{
	// Calc Time
	time_t t = std::time(0);
	tm time;
	localtime_s(&time, &t);
	char buf[80];
	sprintf_s(buf, "(%d-%d %d:%d:%d)", time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);

	// Create LogText
	string logText = buf + _LogLevelToString(logLevel) + " : " + format + "\n";

	// Push to queue
	FLog newLog;
	newLog.type = logType;
	newLog.level = logLevel;
	newLog.log = logText;

	_queueMutex.lock();
	_logQueue.push(newLog);
	_queueMutex.unlock();

	// Do _WriteLog when not on write
	if (!_bWriteMutex.try_lock()) return;
	if (!_bOnWrite) {
		_bOnWrite = true;
		_bWriteMutex.unlock();
		_writeLogThread = new thread(&CLogSystem::_WriteLog, this);
	}
	else _bWriteMutex.unlock();
}

void CLogSystem::WriteLogS(string& format)
{
	WriteLog(LogTemp, Waring, format);
}

void CLogSystem::Join()
{
	if (_writeLogThread == nullptr) return;
	_writeLogThread->join();
}

string CLogSystem::_LogTypeToString(const ELogType& logType)
{
	switch (logType)
	{
	case LogTemp:
		return "LogTemp";
	case LogSystem:
		return "LogSystem";
	case NetworkManager:
		return "NetworkManager";
	case NetworkSystem:
		return "NetworkSystem";
	case GuestNetworkSystem:
		return "GuestNetworkSystem";
	case ServerNetworkSystem:
		return "ServerNetworkSystem";
	case ClientNetworkSystem:
		return "ClientNetworkSystem";
	default:
		return "UnkownLogType";
	}
}

string CLogSystem::_LogLevelToString(const ELogLevel & logLevel)
{
	switch (logLevel)
	{
	case Waring:
		return "Warning";
	case Error:
		return "Error";
	case Critical:
		return "Critical";
	default:
		return "UnknownLevel";
	}
}

void CLogSystem::_WriteLog(CLogSystem* logSystem)
{
	FLog currentLog;
	while (true) {
		// Pop log
		logSystem->_queueMutex.lock();
		currentLog = logSystem->_logQueue.front();
		logSystem->_logQueue.pop();
		logSystem->_queueMutex.unlock();

		// Create LogFile Name
		string fileName1 = logSystem->_LogTypeToString(currentLog.type);
		string fileName2 = LOG_INTERGRATED;
		fileName1.append(LOG_FILE_TYPE);
		fileName2.append(LOG_FILE_TYPE);

		// Write Log
		logSystem->_fileMutex.lock();
		ofstream output1(fileName1, ios::app);
		ofstream output2(fileName2, ios::app);
		output1 << currentLog.log;
		output2 << currentLog.log;
		output1.close();
		output2.close();
		logSystem->_fileMutex.unlock();

		// break when queue is empty
		logSystem->_queueMutex.lock();
		if (logSystem->_logQueue.empty()) {
			logSystem->_queueMutex.unlock();

			// disable onWrite
			logSystem->_bWriteMutex.lock();
			logSystem->_bOnWrite = false;
			logSystem->_bWriteMutex.unlock();

			break;
		}
		logSystem->_queueMutex.unlock();
	}
}
