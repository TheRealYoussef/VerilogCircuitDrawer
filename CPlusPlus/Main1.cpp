#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#ifdef _WIN32
#include <windows.h>
#endif
//#include "MyListener.h"
#include "ServiceListener.h"
#include "MyListener.h"

using namespace std;

int main()
{
	ServiceListener DigitalListener(L"http://localhost:8080");
	ShellExecuteA(NULL, "open", "Digital.html", NULL, NULL, SW_SHOWNORMAL);
	system("pause");
	return 0;

}
