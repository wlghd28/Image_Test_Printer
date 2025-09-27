// RainBow_SWDlg.h : 헤더 파일
//

#pragma once
#include <Shlwapi.h>
#include <tchar.h>
#include <cstdio>
#include <math.h>
#include "afxcmn.h"
#include "Func_Def.h"

#pragma comment(lib, "Shlwapi.lib")

// libtiff 라이브러리
#include "tiff.h"
#include "tiffconf.h"
#include "tiffio.h"
#include "tiffvers.h"

// CRainBow_SWDlg 대화 상자
class CRipping;
class CRainBow_SWDlg : public CDialogEx
{
// 생성입니다.
public:
	CRainBow_SWDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	~CRainBow_SWDlg();	// 표준 소멸자입니다.
	
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
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// MFC 변수
	CTabCtrl id_tab_parent;
	int int_tab_margin;
	CRect rect_tab_position;

	// 딜레이 함수
	void  Wait(DWORD dwMillisecond, int sleeptime);

	//gdi+ 사용
	ULONG_PTR	m_gdiplusToken = NULL;

	// Bitmap 관련 변수
	Gdiplus::Bitmap* bmp;
	Gdiplus::Bitmap* buf_bitmap;		// 더블 버퍼링을 할 때 쓰일 버퍼 Bitmap 메모리
	Gdiplus::Graphics* memDC;			// 더블 버퍼링을 할 때 쓰일 버퍼 DC
	Gdiplus::Bitmap* bmp_Thumbnail;		// 썸네일 이미지 Bitmap 메모리

	// Bitmap 관련 함수
	HBITMAP GetHBITMAP(CString str_Image_path);
	Gdiplus::Graphics* graphics;
	CRect rect_draw_printarea;		// 더블 버퍼링을 위한 PrintArea 다이얼로그의 위치 정보를 얻을 변수
	Gdiplus::SolidBrush* whiteBrush;	// 더블 버퍼링을 위해 미리 그리는 메모리에 칠해줄 하얀색 브러시
	Gdiplus::SolidBrush* grayBrush;		// 더블 버퍼링을 위해 미리 그리는 메모리에 칠해줄 회색 브러시



	// 썸네일 이미지 생성 함수
	int Thumbnail_Print(CRect rect_Thumbnail_size, Gdiplus::Bitmap* bmp, CString str_Thumbnail_fn);

	// 프리뷰 출력하는 함수 (PrintArea 다이얼로그)
	int PreView_Print(CString str_image);

	// 읽어들인 이미지의 해상도
	double dXRes = 0;
	double dYRes = 0;

	// 읽어들인 이미지의 픽셀
	int m_iWidth_Pixel = 0;
	int m_iHeight_Pixel = 0;

	// 읽어들인 이미지의 데이터 포맷
	int iDataFormat = 0;

	// 인쇄 영역에 관한 변수
	int int_width_print_area_length;		// 인쇄영역의 가로 길이 (mm)
	int int_height_print_area_length;		// 인쇄영역의 세로 길이 (mm)
	int int_width_print_area_pix;			// 인쇄영역의 가로 픽셀(실제 좌표 길이)
	int int_height_print_area_pix;			// 인쇄영역의 세로 픽셀(실제 좌표 길이)

	double double_image_width_print_area_length;	// 읽어들인 이미지의 인쇄영역 가로 길이 (mm)
	double double_image_height_print_area_length;	// 읽어들인 이미지의 인쇄영역 세로 길이 (mm)
	int int_image_width_print_area_pix;		// 읽어들인 이미지의 가로 픽셀(실제 좌표 길이)
	int int_image_height_print_area_pix;	// 읽어들인 이미지의 세로 픽셀(실제 좌표 길이)

	double double_image_width_print_area_length_original;	// 읽어들인 이미지 원본의 인쇄영역 가로 길이 (mm)
	double double_image_height_print_area_length_original;	// 읽어들인 이미지 원본의 인쇄영역 세로 길이 (mm)
	int int_image_width_print_area_pix_original;		// 읽어들인 이미지 원본의 가로 픽셀(실제 좌표 길이)
	int int_image_height_print_area_pix_original;	// 읽어들인 이미지 원본의 세로 픽셀(실제 좌표 길이)
};
