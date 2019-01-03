#include "pch.h"
#include <iostream>
#include <string>
#include <conio.h>
#include <Windows.h>
#include "LogSystem.h"
using namespace std;

int main()
{
	CLogSystem myLogSystem;
	string text = "Test Msg";
	for (int i = 0; i < 1000; ++i) {
		myLogSystem.WriteLogS(text);
	}
	cout << "End of for";
	myLogSystem.Join();

	return 0;
}