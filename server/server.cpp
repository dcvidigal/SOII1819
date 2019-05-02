#include <windows.h>
#include <iostream>
#include <tchar.h>

#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include <strsafe.h>

using namespace std;

#define TAM 256
#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")



void clientCOM(TCHAR buf[256], HANDLE hPipe);

DWORD WINAPI InstanceThread(LPVOID);
VOID GetAnswerToRequest(LPTSTR, LPTSTR, LPDWORD);

int _tmain(int argc, LPTSTR argv[]) {
	
	//BOOL ret;
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif
	TCHAR buf[TAM];
	BOOL fConnected = FALSE;
	DWORD dwThreadId = 0;
	HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL;

	while (1) {
		_tprintf(TEXT("[servidor] Criar PipeServer\n"));
		hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES, sizeof(buf), sizeof(buf), 0, NULL);// cria pipe name

		if (hPipe == INVALID_HANDLE_VALUE) {
			_tprintf(TEXT("[ERRO] Criar Named Pipe! (CreateNamedPipe)"));
			exit(-1);
		}
		if (ConnectNamedPipe(hPipe, NULL)) {
			_tprintf(TEXT("[servidor] cliente ligado ao pipe (ConnectNamedPipe\n"));

			//cria uma thread para este cliente
			hThread = CreateThread(NULL, 0, InstanceThread, (LPVOID) hPipe, 0, &dwThreadId);
			if (hThread == NULL) {
				_tprintf(TEXT("[ERRO] Criar thread"));
			} else {
				CloseHandle(hThread);
			}
		}
		else {
			CloseHandle(hPipe);
		}
	}

	return 1;
}

DWORD WINAPI InstanceThread(LPVOID lpvParam)
{
	// This routine is a thread processing function to read from and reply to a client
// via the open pipe connection passed from the main loop. Note this allows
// the main loop to continue executing, potentially creating more threads of
// of this procedure to run concurrently, depending on the number of incoming
// client connections.

	HANDLE hHeap = GetProcessHeap();
	TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, TAM * sizeof(TCHAR));
	TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, TAM * sizeof(TCHAR));

	DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
	BOOL fSuccess = FALSE;
	HANDLE hPipe = NULL;

	// Do some extra error checking since the app will keep running even if this
  // thread fails.

	if (lpvParam == NULL){
		/*printf("\nERROR - Pipe Server Failure:\n");
		printf("   InstanceThread got an unexpected NULL value in lpvParam.\n");
		printf("   InstanceThread exitting.\n");*/
		if (pchReply != NULL) {
			HeapFree(hHeap, 0, pchReply);
		}
		if (pchRequest != NULL) {
			HeapFree(hHeap, 0, pchRequest);
		}
		return (DWORD)-1;
	}

	if (pchRequest == NULL){
		/*printf("\nERROR - Pipe Server Failure:\n");
		printf("   InstanceThread got an unexpected NULL heap allocation.\n");
		printf("   InstanceThread exitting.\n");*/
		if (pchReply != NULL) {
			HeapFree(hHeap, 0, pchReply);
		}
		return (DWORD)-1;
	}

	if (pchReply == NULL){
		/*printf("\nERROR - Pipe Server Failure:\n");
		printf("   InstanceThread got an unexpected NULL heap allocation.\n");
		printf("   InstanceThread exitting.\n");*/
		if (pchRequest != NULL) {
			HeapFree(hHeap, 0, pchRequest);
		}
		return (DWORD)-1;
	}


	hPipe = (HANDLE) lpvParam;
	while (1) {
		//Ler Cliente
		fSuccess = ReadFile(hPipe,
			pchRequest, //buff que recebe mensagem
			TAM * sizeof(TCHAR),
			&cbBytesRead, // numero de bytes lidos
			NULL);

		if (!fSuccess || cbBytesRead == 0) {
			if (GetLastError() == ERROR_BROKEN_PIPE){
				_tprintf(TEXT("InstanceThread: client disconnected.\n"), GetLastError());
			}else{
				_tprintf(TEXT("InstanceThread ReadFile failed, GLE=%d.\n"), GetLastError());
			}
			break;
		}

		// Processa a mensagem.
		GetAnswerToRequest(pchRequest, pchReply, &cbReplyBytes);


		//responde ao cliente
		fSuccess = WriteFile(hPipe, pchReply, cbReplyBytes, &cbWritten, NULL);
		if (!fSuccess || cbReplyBytes != cbWritten)
		{
			_tprintf(TEXT("InstanceThread WriteFile failed, GLE=%d.\n"), GetLastError());
			break;
		}
	}
	// Flush the pipe to allow the client to read the pipe's contents 
	// before disconnecting. Then disconnect the pipe, and close the 
	// handle to this pipe instance. 
 
	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	HeapFree(hHeap, 0, pchRequest);
	HeapFree(hHeap, 0, pchReply);

	//printf("InstanceThread exitting.\n");
	return 1;
}

VOID GetAnswerToRequest(LPTSTR pchRequest,
	LPTSTR pchReply,
	LPDWORD pchBytes)
	// This routine is a simple function to print the client request to the console
	// and populate the reply buffer with a default data string. This is where you
	// would put the actual client request processing code that runs in the context
	// of an instance thread. Keep in mind the main thread will continue to wait for
	// and receive other client connections while the instance thread is working.
{
	_tprintf(TEXT("Client Request String:\"%s\"\n"), pchRequest);

	// Check the outgoing message to make sure it's not too long for the buffer.
	
	TCHAR* s;
	if (_tcscmp(pchRequest, TEXT("u1234")) == 0) {
		s = (TCHAR* )"1";
	}
	else {
		s = (TCHAR*)"0";
	}
	if (FAILED(StringCchCopy(pchReply, TAM, s)))
	{
		*pchBytes = 0;
		pchReply[0] = 0;
		//printf("StringCchCopy failed, no outgoing message.\n");
		return;
	}
	*pchBytes = (lstrlen(pchReply) + 1) * sizeof(TCHAR);
}


void clientCOM(TCHAR buf[256], HANDLE hPipe) {
	DWORD n;
	// cliente <- servidor

		
		
			/*_tprintf(TEXT("[Servidor] Frase: "));
			_fgetts(buf, 256, stdin);*/
			buf[_tcslen(buf) - 1] = '\0';
			if (!WriteFile(hPipe, buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL)) {
				_tprintf(TEXT("[Servidor] Escrever no pipe! (WriteFile)\n"));
				exit(-1);
			}
			_tprintf(TEXT("[Servidor] Enviei %d bytes ao leitor...(WriteFile)\n"), n);
	};