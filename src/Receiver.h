
// Receiver.h : 헤더 파일
//

#pragma once
#include <Shlwapi.h>
#include <tchar.h>
#include <cstdio>
#include <math.h>
#include "afxcmn.h"

#pragma comment(lib, "Shlwapi.lib")

// CReceiver 대화 상자
class CReceiver : public CDialogEx
{
// 생성입니다.
public:
	CReceiver(CWnd* pParent = NULL);	// 표준 생성자입니다.
	~CReceiver();	// 표준 소멸자입니다.
	
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RAINBOW_SW_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeTabParent(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	virtual void PostNcDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// Sender.exe와 메시지 통신을 하기 위해 쓰이는 함수 및 변수
	unsigned int m_uRegisteredMessage;		// 메시지 변수
	void RegistMessage();	// 메시지 등록

};
