
// RainBow_SWDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "RainBow_SW.h"
#include "RainBow_SWDlg.h"
#include "afxdialogex.h"

// GDI+ 관련 헤더
#include <gdiplus.h> //gdi+
#pragma comment(lib, "Gdiplus.lib")

using namespace Gdiplus;

#ifdef _DEBUG
//#define new DEBUG_NEW
#endif

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")


// Curve 와 메시지 통신
#define MSG_CURVE_CLOSE 1000
#define MSG_CURVE_SAVE 1001


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


// CRainBow_SWDlg 대화 상자



CRainBow_SWDlg::CRainBow_SWDlg(CWnd* pParent /*=NULL*/)
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

void CRainBow_SWDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_PARENT, id_tab_parent);
}

CRainBow_SWDlg::~CRainBow_SWDlg()
{
	// Gdi+ 해제
	if(m_gdiplusToken != NULL) GdiplusShutdown(m_gdiplusToken);
}

BEGIN_MESSAGE_MAP(CRainBow_SWDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PARENT, &CRainBow_SWDlg::OnSelchangeTabParent)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_DROPFILES()
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()


#define H_TIMER_3_FIRST_RUN			103


// CRainBow_SWDlg 메시지 처리기

BOOL CRainBow_SWDlg::OnInitDialog()
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

	// 프리뷰 이미지 관련 메모리 초기화
	HDC hdc = ::GetDC(printdlg->printview->printarea->m_hWnd);
	graphics = new Gdiplus::Graphics(hdc);
	graphics->SetSmoothingMode(SmoothingModeHighQuality);
	bmp = NULL;
	bmp_Thumbnail = NULL;

	// 더블 버퍼링을 위한 PrintArea 다이얼로그 위치 정보 얻는다.
	printdlg->printview->printarea->GetClientRect(&rect_draw_printarea);

	// 더블 버퍼링을 할 때 쓰일 브러시 및 메모리 할당
	whiteBrush = new Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 255, 255));
	grayBrush = new Gdiplus::SolidBrush(Gdiplus::Color(255, 128, 128, 128));
	buf_bitmap = new Bitmap(rect_draw_printarea.Width(), rect_draw_printarea.Height());
	memDC = new Gdiplus::Graphics(buf_bitmap);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CRainBow_SWDlg::OnSysCommand(UINT nID, LPARAM lParam)
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


void CRainBow_SWDlg::OnDropFiles(HDROP hDropInfo)
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

void CRainBow_SWDlg::OnPaint()
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

void CRainBow_SWDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch (nIDCtl)
	{
	case IDC_TAB_PARENT:
	{
		CDC dc;

		dc.Attach(lpDrawItemStruct->hDC);
		RECT rect = lpDrawItemStruct->rcItem;

		TCHAR buffer[256] = { 0 };

		int nTabIndex = lpDrawItemStruct->itemID;

		if (!tab->GetItem(nTabIndex, &tabItem)) return;
		//-----------------(1)------------------ Tab Background Color
		BOOL bCurrent = (nTabIndex != id_tab_parent.GetCurSel());
		CBrush brush;
		if (bCurrent)
		{
			brush.CreateSolidBrush(RGB(220, 220, 220)); // active tab color >> 이 부분 변경하면 돼~   다저블루 색깔
			dc.SelectObject(&brush);
			dc.FillRect(&lpDrawItemStruct->rcItem, &brush);
		}
		else
		{
			brush.CreateSolidBrush(RGB(255, 0, 0));
			// Non-active tab color
		}
		//-----------------(2)------------------ Tab Text & Color

		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(RGB(0, 0, 0));
		dc.DrawText(tabItem.pszText, &lpDrawItemStruct->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		dc.Detach();
	}
		break;
	default:
		CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
		break;
	}
}


void CRainBow_SWDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

}

void CRainBow_SWDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	// Grapics 객체 메모리 해제
	delete graphics;

	// 더블 버퍼링을 할 때 쓰인 브러시 및 메모리 해제
	delete whiteBrush;
	delete grayBrush;
	delete buf_bitmap;
	delete memDC;
	delete bmp_Thumbnail;
}


void CRainBow_SWDlg::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
}


// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CRainBow_SWDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// 탭 메뉴 선택 시 호출되는 이벤트 함수
void CRainBow_SWDlg::OnSelchangeTabParent(NMHDR *pNMHDR, LRESULT *pResult)
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


// Bitmap 관련 함수
// 비트맵 얻어오는 함수
HBITMAP CRainBow_SWDlg::GetHBITMAP(CString str_Image_path)
{
	// Convert CString to WCHAR
	USES_CONVERSION;
	WCHAR* wstrImgPath = T2W(str_Image_path.GetBuffer());

	HBITMAP hBitmap = NULL;
	Gdiplus::Bitmap* pBmp = ::new Bitmap(wstrImgPath);
	if (!pBmp)
		return NULL;

	pBmp->GetHBITMAP(Gdiplus::Color(255, 255, 255), &hBitmap);

	::delete pBmp;

	return hBitmap;
}

int CRainBow_SWDlg::Thumbnail_Print(CRect rect_Thumbnail_size, Gdiplus::Bitmap* bmp, CString str_Thumbnail_fn)
{
	// 파일로 생성하기 위해 Bitmap 생성
	Bitmap bitmap(rect_Thumbnail_size.Width(), rect_Thumbnail_size.Height(), PixelFormat24bppRGB);

	// DC 생성
	Gdiplus::Graphics* memDC;
	memDC = new Gdiplus::Graphics(&bitmap);
	memDC->SetSmoothingMode(SmoothingModeHighQuality);

	// 버퍼 DC에 썸네일 이미지 그려준다.
	memDC->DrawImage
	(
		bmp,
		0,
		0,
		rect_Thumbnail_size.Width(),
		rect_Thumbnail_size.Height()
	);

	// 파일로 쓰는 작업
	CLSID tiffClsid;
	int iResult = GetEncoderClsid(L"image/tiff", &tiffClsid);
	if (iResult == -1)
	{
		printf("Encoder Error!!\n");
	}

	USES_CONVERSION;
	WCHAR* wstrImgPath = T2W(str_Thumbnail_fn.GetBuffer());

	bitmap.Save(wstrImgPath, &tiffClsid, NULL);

	delete memDC;

	return 0;
}

// 프리뷰에 이미지 출력 하는 함수
int  CRainBow_SWDlg::PreView_Print(CString str_image)
{
	if (bmp_Thumbnail != NULL)
	{
		delete bmp_Thumbnail;		// 이전 이미지 제거
		bmp_Thumbnail = NULL;
	}

	double dXres = 0;
	double dYres = 0;

	//Convert CString to WCHAR
	USES_CONVERSION;
	WCHAR* wstrImgPath = T2W(str_image.GetBuffer());

	bmp = new Bitmap(wstrImgPath);
	if (bmp == NULL)
	{
		AfxMessageBox("Loading Image was failed!!", MB_ICONSTOP);
		return 1;
	}


	//dXres = (double)bmp->GetHorizontalResolution();
	//dYres = (double)bmp->GetVerticalResolution();
	this->m_iWidth_Pixel = (int)bmp->GetWidth();
	this->m_iHeight_Pixel = (int)bmp->GetHeight();
	this->dXRes = (double)bmp->GetHorizontalResolution();
	this->dYRes = (double)bmp->GetVerticalResolution();
	iDataFormat = ripping->GetPixelFormat(str_image, printdlg->int_file_format);
	printf("iDataFormat : %d\n", iDataFormat);

	this->dXRes = (double)FloatToIntResolution(this->dXRes);
	this->dYRes = (double)FloatToIntResolution(this->dYRes);

	if (iDataFormat == 1 || iDataFormat == 2 || iDataFormat == 4)
	{
		if (this->dXRes != 360 && this->dXRes != 720 && this->dXRes != 1080 && this->dXRes != 1440)
		{
			this->EnableWindow(FALSE);
			selectresolutiondlg = new CSelectResolutionDlg();
			if (selectresolutiondlg != NULL)
			{
				selectresolutiondlg->Create(IDD_SELECT_RESOLUTION_DIALOG);
				selectresolutiondlg->ShowWindow(TRUE);

				while (1)
				{
					if (m_bWait_SelectResolution == true)
					{
						this->EnableWindow(TRUE);
						printdlg->SetFocus();
						selectresolutiondlg = NULL;
						break;
					}
					Wait(100, 1);
				}
			}
		}
		else if (this->dYRes != 360 && this->dYRes != 720 && this->dYRes != 1080 && this->dYRes != 1440)
		{
			this->EnableWindow(FALSE);
			selectresolutiondlg = new CSelectResolutionDlg();
			if (selectresolutiondlg != NULL)
			{
				selectresolutiondlg->Create(IDD_SELECT_RESOLUTION_DIALOG);
				selectresolutiondlg->ShowWindow(TRUE);

				while (1)
				{
					if (m_bWait_SelectResolution == true)
					{
						this->EnableWindow(TRUE);
						printdlg->SetFocus();
						selectresolutiondlg = NULL;
						break;
					}
					Wait(100, 1);
				}
			}
		}

	}

	dXres = this->dXRes;
	dYres = this->dYRes;

	double_image_width_print_area_length = (double)bmp->GetWidth() / (double)dXres * (double)25.4;
	double_image_height_print_area_length = (double)bmp->GetHeight() / (double)dYres * (double)25.4;
	int_image_width_print_area_pix = (double)int_width_print_area_pix * (double)double_image_width_print_area_length / (double)int_width_print_area_length;
	int_image_height_print_area_pix = (double)int_height_print_area_pix * (double)double_image_height_print_area_length / (double)int_height_print_area_length;

	// 나중에 이미지 사이즈 Reset을 위해서 원본의 크기를 저장해놓는다.
	double_image_width_print_area_length_original = double_image_width_print_area_length;
	double_image_height_print_area_length_original = double_image_height_print_area_length;
	int_image_width_print_area_pix_original = int_image_width_print_area_pix;
	int_image_height_print_area_pix_original = int_image_height_print_area_pix;

#ifdef CP_PRINTER
	if (double_image_width_print_area_length > (double)int_width_print_area_length + (double)m_dMaxSizeOffset_mm
		|| double_image_height_print_area_length > (double)int_height_print_area_length + (double)m_dMaxSizeOffset_mm)
	{
		AfxMessageBox("This Image size is bigger than PrintArea!!", MB_ICONWARNING);
	}
#endif

	rect_image_print_area.left = 0;
	rect_image_print_area.top = 0;
	rect_image_print_area.right = int_image_width_print_area_pix;
	rect_image_print_area.bottom = int_image_height_print_area_pix;

	// 썸네일 이미지 생성
	Thumbnail_Print(rect_image_print_area, bmp, m_Str_TempPrintThumbnail);

	// 나중에 이미지 사이즈 Reset을 위해서 원본의 위치를 저장해 놓는다.
	rect_image_print_area_original.left = 0;
	rect_image_print_area_original.top = 0;
	rect_image_print_area_original.right = int_image_width_print_area_pix;
	rect_image_print_area_original.bottom = int_image_height_print_area_pix;

	printdlg->printview->printarea->InvalidateRect(&rect_image_print_area, FALSE);
	printdlg->printview->printarea->PrintImageCoordinate();

	
	delete bmp;
	bmp = NULL;


	// 썸네일 이미지 Bitmap Load
	wstrImgPath = T2W(m_Str_TempPrintThumbnail.GetBuffer());
	bmp_Thumbnail = new Bitmap(wstrImgPath);
	if (bmp_Thumbnail == NULL)
	{
		AfxMessageBox("Loading Thumbnail Image was failed!!", MB_ICONSTOP);
		return 1;
	}

	m_bWait_SelectResolution = false;

	return 0;
}

// 딜레이 기능 함수
void  CRainBow_SWDlg::Wait(DWORD dwMillisecond, int sleeptime)
{
	MSG msg;
	DWORD dwStart;
	dwStart = GetTickCount64();

	while (GetTickCount64() - dwStart < dwMillisecond)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Sleep(sleeptime);		// >> 이거 없으면 CPU  점유율 100찍는다.. 굉장히 중요
	}
}


BOOL CRainBow_SWDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}

LRESULT CRainBow_SWDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == m_uRegisteredMessage)
	{
		switch (wParam)
		{
		default:
			break;
		}
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}

// 해상도 Float >> Int 변형
int CRainBow_SWDlg::FloatToIntResolution(double dRes)
{
	if (dRes > 359 && dRes < 361) return 360;
	else if (dRes > 719 && dRes < 721) return 720;
	else if (dRes > 1079 && dRes < 1081) return 1080;
	else if (dRes > 1439 && dRes < 1441) return 1440;
	else return (int)dRes;
}
