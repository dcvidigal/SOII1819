
//DLL.h
// O bloco ifdef seguinte � o modo standard de criar macros que tornam a exporta��o de
// fun��es e vari�veis mais simples. Todos os ficheiros neste projeto DLL s�o
// compilados com o s�mbolo DLL_IMP_EXPORTS definido. Este s�mbolo n�o deve ser definido
// em nenhum projeto que use a DLL. Desta forma, qualquer outro projeto que inclua este
// este ficheiro ir� ver as fun��es e vari�veis DLL_IMP_API como sendo importadas de uma
// DLL.
#include <windows.h>
#include <iostream>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>


using namespace std;
typedef basic_string<TCHAR> tstring;
//Definir uma constante para facilitar a leitura do prot�tipo da fun��o
//Este .h deve ser inclu�do no projeto que o vai usar (modo impl�cito)
#define TAM 256
#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")
//Esta macro � definida pelo sistema caso estejamos na DLL (<DLL_IMP>_EXPORTS definida)
//ou na app (<DLL_IMP>_EXPORTS n�o definida) onde DLL_IMP � o nome deste projeto
#ifdef DLL_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif


struct MessageBuf
{
	string func;
	TCHAR  value[TAM];
};


extern "C"
{
	//Vari�vel global da DLL
	extern DLL_IMP_API int nDLL;


	//Fun��es a serem exportadas/importadas
	DLL_IMP_API int Load(TCHAR username[256]);
	//DLL_IMP_API int ReceiveBroadcast(void);
	//DLL_IMP_API int SendMessageA(void);
	//DLL_IMP_API int ReceiveMessage(void);
}

void serverWrite(TCHAR text[TAM]);