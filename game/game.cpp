#include <iostream>
#include <sstream>
#include <windows.h>
#include <tchar.h>#include <conio.h>

#include <fcntl.h>
#include <io.h>
#include "commDLL.h"


int _tmain(int argc, TCHAR *argv[]) {
	//Usar a variável da Dll
	TCHAR username[TAM];
	string u;
	int loaded = 0;
	do
	{
		_tprintf(TEXT("Insira o nome de utilizador\n"));
		_fgetts(username, TAM, stdin);
		username[_tcslen(username) - 1] = '\0';
		loaded = Load(username);
		if (!loaded) _tprintf(TEXT("user incorrecto!"));
	} while (!loaded);
	_getchar_nolock();
	return 0;
}