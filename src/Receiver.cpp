// Receiver.cpp : 구현 파일
//

#include "stdafx.h"
#include "CReceiver.h"
#include "afxdialogex.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#endif

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")


// Sender 와 메시지 통신
#define MSG_SENDER_CLOSE 1000
#define MSG_SENDER_SAVE 1001


#ifndef WM_COPYGLOBALDATA
#define WM_COPYGLOBALDATA 0x0049
#endif

#ifndef MSGFLT_ADD
#define MSGFLT_ADD 1
#endif

typedef BOOL(WINAPI *pfnChangeWindowMessageFilter)(UINT, DWORD);

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CReceiver 대화 상자
CReceiver::CReceiver(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RAINBOW_SW_DIALOG, pParent)
{
	// Motion 관련 인자들 ini 파일에서 읽어와서 초기화 시킨다.
	//Ini_Load_Data();

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// GDI+ 초기화
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);//ULONG_PTR	m_gdiplusToken(헤더에서 선언됨)

	// IO 상태 초기화
	//ResetIOStatus();
}

void CReceiver::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_PARENT, id_tab_parent);
}

CReceiver::~CReceiver()
{
	// Gdi+ 해제
	if(m_gdiplusToken != NULL) GdiplusShutdown(m_gdiplusToken);
}

BEGIN_MESSAGE_MAP(CReceiver, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PARENT, &CReceiver::OnSelchangeTabParent)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_DROPFILES()
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()


#define H_TIMER_3_FIRST_RUN			103


// CReceiver 메시지 처리기

BOOL CReceiver::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.


	// 파일 드래그앤 드롭 처리
	HMODULE hUser32Mod = LoadLibrary("USER32,DLL");
	if (hUser32Mod != NULL)
	{
		pfnChangeWindowMessageFilter pChangeWindowMessageFilter =
			(pfnChangeWindowMessageFilter)GetProcAddress(hUser32Mod, "ChangeWindowMessageFilter");

		if (pChangeWindowMessageFilter != NULL)
		{
			pChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
			pChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);
			pChangeWindowMessageFilter(WM_COPYGLOBALDATA, MSGFLT_ADD);
		}
	}

	DragAcceptFiles();

	RegistMessage();		// Curve와 메시지 통신을 위해 호출

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CReceiver::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


void CReceiver::OnDropFiles(HDROP hDropInfo)
{
	if (externripdlg->IsWindowVisible() == true)
	{
		externripdlg->OnDropFiles(hDropInfo);
	}
	else
		CDialogEx::OnDropFiles(hDropInfo);
}


// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CReceiver::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CReceiver::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

}

void CReceiver::OnDestroy()
{
	CDialogEx::OnDestroy();
}

void CReceiver::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
}


// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CReceiver::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 탭 메뉴 선택 시 호출되는 이벤트 함수
void CReceiver::OnSelchangeTabParent(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	if (IDC_TAB_PARENT == pNMHDR->idFrom)
	{
		int iSelect = id_tab_parent.GetCurSel();
		switch (iSelect)
		{
		case 0:	
			break;
		case 1:
			break;
		case 2:

			break;
		case 3:
			break;
		default:
			break;
		}
	}
}

void CReceiver::RegistMessage()
{
	// 메시지 등록 : 문자열을 사용자 메시지로 등록하여 메시지번호를 받는다.
	// WM_USER와는 조금 다른 개념.
	// (WM_USER + 추가번호)는 다음 어떤 어플리케이션과 메시지번호가 중복 될 가능성이 있으나,
	// RegisterWindowMessage()는 고유의 메시지번호를 받는 것이다.

	m_uRegisteredMessage = RegisterWindowMessage(_T("Sender_Message"));
}

LRESULT CReceiver::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == m_uRegisteredMessage)
	{
		switch (wParam)
		{
		case MSG_CURVE_SAVE:
		{
			printf("Message Save!!\n");
		}
		break;

		case MSG_CURVE_CLOSE:
		{
			printf("Message Close!!\n");
		}
			break;
		default:
			break;
		}
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}