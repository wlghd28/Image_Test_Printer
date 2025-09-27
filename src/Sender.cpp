#define _CRT_SECURE_NO_WARNINGS
// Sender1.0.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <commdlg.h>
#include "framework.h"
#include "Sender.h"
#include "shlwapi.h"
#include "shellapi.h"
#pragma comment(lib, "Shlwapi.lib")
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#define MAX_LOADSTRING 100

//////////////////////////////////////////////////////////////////////////////
#define MSG_SENDER_CLOSE  1000
#define MSG_SENDER_SAVE  1001 

unsigned int m_uRegisteredMessage;
void RegistMessage();
//////////////////////////////////////////////////////////////////////////////

// 전역 변수:
HDC hdc;
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.
	//wprintf(L"%s\n", lpCmdLine);
	int iCnt = 0;
	LPSTR lpcstr = ::GetCommandLineA();
	//LPCWSTR lpcwstr = ::GetCommandLineW(); 
	//pStr = ::CommandLineToArgvW(lpcwstr, &iCnt);
	pStr = ::CommandLineToArgvA(lpcstr, &iCnt);
	if (iCnt > 1)
	{
		_tchar_LoadPoint_FilePath = (TCHAR*)pStr[1];
		strcpy(_FN_Save, (TCHAR*)pStr[1]);
	}
	if (iCnt > 2) _iNum = _ttoi(pStr[2]);

	//wprintf(L"%s\n", _tchar_LoadPoint_FilePath);

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_Sender11, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_Sender11));

	MSG msg;

	// 기본 메시지 루프입니다:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_Sender11));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_Sender11);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		750, 10, 713, 670, nullptr, nullptr, hInstance, nullptr);		// 700 -> 750 ,  830 -> 713

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//TCHAR NewTitle[] = _T("타이틀 바꾸기");
	//TCHAR NewTitle[] = _T("ABCDEFG");
	//TCHAR GetTitle[80];

	if (message == WM_CLOSE) {			// 프로그램에서 'X' 눌렀을 때
		//check = MessageBox(hWnd, TEXT("프로그램을 종료하시겠습니까?"), TEXT("종료 확인"), MB_OKCANCEL);
		check = MessageBox(hWnd, TEXT("Do you want quit program?"), TEXT("Program quit"), MB_OKCANCEL | MB_TOPMOST);
		::PostMessage(HWND_BROADCAST, m_uRegisteredMessage, MSG_CURVE_CLOSE, lParam);
	}
	if (check == IDCANCEL) {			// 닫기 취소
		return 0;
	}
	else if (message == WM_DESTROY)
	{
		PostQuitMessage(0);
	}

	if (Doolgi == -2) {								// 에디트에 좌표 숫자로 치고 엔터 누르면
		InvalidateRect(hWnd, NULL, FALSE);			// 점 이동
	}

	switch (message)
	{

	case WM_CREATE:
	{
		RegistMessage();
	}
		break;

	default:
		break;
	}
///////////////////////////////////////////////////////////////////////////


void RegistMessage()
{
	// 메시지 등록 : 문자열을 사용자 메시지로 등록하여 메시지번호를 받는다.
	// WM_USER와는 조금 다른 개념.
	// (WM_USER + 추가번호)는 다음 어떤 어플리케이션과 메시지번호가 중복 될 가능성이 있으나,
	// RegisterWindowMessage()는 고유의 메시지번호를 받는 것이다.

	m_uRegisteredMessage = RegisterWindowMessage(_T("Sender_Message"));
}
