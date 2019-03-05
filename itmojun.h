#ifndef __ITMOJUN_H__
#define __ITMOJUN_H__

#ifdef __cplusplus
extern "C" {
#endif


extern void __cdecl GetDeviceCmd(const char* szUid, const char* szDevid, char* szCmd);
extern void __cdecl GetPCCmd(const char* szPcId, char* szCmd);
extern int  __cdecl UploadDeviceData(const char* szUid, const char* szDevid, const char* szDevData);
extern void __cdecl AutoRun();
extern void __cdecl CommInit();
extern int  __cdecl CommSend(const char *strCommPort, const char* pszData);
extern int  __cdecl CommRecv(const char *strCommPort, char* pszBuff, int iSize);


#ifdef __cplusplus
}
#endif

#pragma comment(lib,"ws2_32.lib")

#endif
