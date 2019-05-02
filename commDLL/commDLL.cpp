#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <conio.h>
#include <stdio.h>

#include "commDLL.h"


#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")

//Para verificar ao carregar a dll que a aplicação irá ocupar mais memória
char ponteiro[40960];
//Definição da variável global
int nDLL = 1234;
//Exportar a função para ser utilizada fora da DLL


TCHAR buf[TAM];
HANDLE hPipe;
LPTSTR lpvMessage;
TCHAR  chBuf[TAM];
BOOL   fSuccess = FALSE;
DWORD  cbRead, cbToWrite, cbWritten, dwMode;
typedef basic_string<TCHAR> tstring;

int Load(TCHAR username[TAM]){
	int resp = 0;
	if (_tcslen(username) <= 0) return 1;
	lpvMessage = username;
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif
	
	// All pipe instances are busy, so wait for 20 seconds. 

	

	while (1) {
		hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
			return -1;
		}

		if (!WaitNamedPipe(PIPE_NAME, 20000))
		{
			printf("Could not open pipe: 20 second wait timed out.");
			return -1;
		}
	}
	

	cbToWrite = (lstrlen(lpvMessage) + 1) * sizeof(TCHAR);
	_tprintf(TEXT("Sending %d byte message: \"%s\"\n"), cbToWrite, lpvMessage);

	fSuccess = WriteFile(
		hPipe,                  // pipe handle 
		lpvMessage,             // message 
		cbToWrite,              // message length 
		&cbWritten,             // bytes written 
		NULL);                  // not overlapped 

	if (!fSuccess)
	{
		_tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError());
		return -1;
	}

//	printf("\nMessage sent to server, receiving reply as follows:\n");

	do
	{
		// Read from the pipe. 

		fSuccess = ReadFile(
			hPipe,    // pipe handle 
			chBuf,    // buffer to receive reply 
			TAM * sizeof(TCHAR),  // size of buffer 
			&cbRead,  // number of bytes read 
			NULL);    // not overlapped 

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

		resp = _wtoi(chBuf);

		_tprintf(TEXT("\"%s\"\n"), chBuf);
	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
	{
		_tprintf(TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	

	CloseHandle(hPipe);

	return resp;

}

void serverWrite(TCHAR text[TAM]) {
	
	//// cliente <- servidor

	///*_tprintf(TEXT("[Servidor] Frase: "));
	//_fgetts(buf, 256, stdin);*/
	//		
	//text[_tcslen(text) - 1] = '\0';
	//if (!WriteFile(hPipe, text, _tcslen(text) * sizeof(TCHAR), &n, NULL)) {
	//	_tprintf(TEXT("[Servidor] Escrever no pipe! (WriteFile)\n"));
	//	exit(-1);
	//}
	//_tprintf(TEXT("[Servidor] Enviei %d bytes ao leitor...(WriteFile)\n"), n);
	//	

};

