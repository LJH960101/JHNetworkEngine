#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <thread>
using namespace std;

#define LOG_INTERGRATED "LogIntergrated"
#define LOG_FILE_TYPE ".log"

enum ELogLevel
{
	Waring,
	Error,
	Critical
};
enum ELogType {
	LogTemp,
	LogSystem,
	NetworkManager,
	NetworkSystem,
	GuestNetworkSystem,
	ServerNetworkSystem,
	ClientNetworkSystem
};

struct FLog
{
	ELogType type;
	ELogLevel level;
	string log;
};

class CLogSystem
{
public:
	CLogSystem();
	~CLogSystem();
	// Push Log to queue by many args.
	void WriteLog(const ELogType& logType, const ELogLevel& logLevel, string& format);
	// Push Log to queue by simple string.
	void WriteLogS(string& format);
	void Join();
private:
	bool _bOnWrite;
	thread* _writeLogThread;
	mutex _bWriteMutex;
	mutex _fileMutex;
	mutex _queueMutex;
	queue<FLog> _logQueue;
	string _LogTypeToString(const ELogType& logType);
	string _LogLevelToString(const ELogLevel& logLevel);
	// Real Write Log
	static void _WriteLog(CLogSystem* logSystem);
};