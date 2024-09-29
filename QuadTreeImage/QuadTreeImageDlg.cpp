
// QuadTreeImageDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "QuadTreeImage.h"
#include "QuadTreeImageDlg.h"
#include "afxdialogex.h"
#include <opencv2/opencv.hpp>
#include "GdiPlus.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <math.h>
#include "QuadTree.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




using namespace Gdiplus;
using namespace std;
using namespace cv;
#pragma comment(lib, "Gdiplus.lib")
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


// CQuadTreeImageDlg 대화 상자



CQuadTreeImageDlg::CQuadTreeImageDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_QUADTREEIMAGE_DIALOG, pParent)
	, nthreshold(0)
	, nmin(0)
	, m_compnum(0)
	, m_orgsize(0)
	, m_orgimg(0)
	, m_orgimg2(0)
	, compratio(0)
	, m_PSNRnum(0)
	, m_dBnum(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQuadTreeImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SelFile, m_selfile);
	DDX_Text(pDX, IDC_threshold, nthreshold);
	DDX_Text(pDX, IDC_min, nmin);
	DDX_Text(pDX, IDC_compnum, m_compnum);
	DDX_Text(pDX, IDC_orgsize, m_orgsize);
	DDX_Text(pDX, IDC_orgimg, m_orgimg);
	DDX_Text(pDX, IDC_orgimg2, m_orgimg2);
	DDX_Text(pDX, IDC_compratio, compratio);
	DDX_Text(pDX, IDC_PSNRnum, m_PSNRnum);
	DDX_Text(pDX, IDC_dBnum, m_dBnum);
}

BEGIN_MESSAGE_MAP(CQuadTreeImageDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_SelFile, &CQuadTreeImageDlg::OnEnChangeSelfile)
	ON_EN_CHANGE(IDC_threshold, &CQuadTreeImageDlg::OnEnChangethreshold)
	ON_EN_CHANGE(IDC_min, &CQuadTreeImageDlg::OnEnChangemin)
	ON_BN_CLICKED(IDC_run, &CQuadTreeImageDlg::OnBnClickedrun)
	ON_STN_CLICKED(IDC_compratio, &CQuadTreeImageDlg::OnStnClickedcompratio)
END_MESSAGE_MAP()


// CQuadTreeImageDlg 메시지 처리기

BOOL CQuadTreeImageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	
	check = 0;
	minArea = 4;
	threshold = 30;
	nthreshold = 30;
	nmin = 4;

	SetDlgItemInt(IDC_threshold, nthreshold);
	SetDlgItemInt(IDC_min, nmin);


	rect1 = { 50,70,300,320 };
	rect2 = { 500,70,300,320 };

	cv::Mat nothing = cv::imread("nothing.png", cv::IMREAD_UNCHANGED);

	for (int i = 0; i < nothing.rows; i++)
		for (int j = 0; j < nothing.cols; j++) {
			nothing.at<cv::Vec4b>(i, j)[0] = cv::saturate_cast<uchar>(1.0 * 0xff);
			nothing.at<cv::Vec4b>(i, j)[1] = cv::saturate_cast<uchar>(1.0 * 0xff);
			nothing.at<cv::Vec4b>(i, j)[2] = cv::saturate_cast<uchar>(1.0 * 0x0);
			nothing.at<cv::Vec4b>(i, j)[3] = 0x00;
		}
	cv::imwrite("contours.png", nothing);

	ImgPath = L"nothing.png";
	

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CQuadTreeImageDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CQuadTreeImageDlg::OnPaint()
{
	/*
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

	}
	*/
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.
	CDialog::OnPaint();
	Graphics ScreenG(dc.GetSafeHdc());

	Bitmap* pBitmap = (Bitmap*)Image::FromFile(ImgPath);
	Bitmap* tBitmap;
	check ? tBitmap = (Bitmap*)Image::FromFile(L"temp.bmp") : tBitmap = (Bitmap*)Image::FromFile(L"contours.png");


	/*
	dc.Rectangle(45, 65, 455, 475);
	dc.Rectangle(495, 65, 905, 475);
	rect1 = { 50,70,300,320 };
	rect2 = { 500,70,300,320 };
	*/
	/*
	dc.Rectangle(45, 65, 45+m_orgimg+10, 65+ m_orgimg2+10);
	dc.Rectangle(495, 65, 495+ m_orgimg+10, 65+ m_orgimg2+10);
	ScreenG.DrawImage(pBitmap, rect1.left, rect1.top, m_orgimg, m_orgimg2);
	ScreenG.DrawImage(tBitmap, rect2.left, rect2.top, m_orgimg, m_orgimg2);
	*/
	dc.Rectangle(225 - m_orgimg / 2 - 5, 65, 225 + m_orgimg / 2 + 5, 65 + m_orgimg2 + 10);
	dc.Rectangle(700- m_orgimg / 2 -5, 65, 700 + m_orgimg / 2 + 5, 65 + m_orgimg2 + 10);
	ScreenG.DrawImage(pBitmap, 225 - m_orgimg / 2, rect1.top, m_orgimg, m_orgimg2);
	ScreenG.DrawImage(tBitmap, 700 - m_orgimg / 2, rect2.top, m_orgimg, m_orgimg2);

	delete pBitmap;
	delete tBitmap;
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CQuadTreeImageDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CQuadTreeImageDlg::OnEnChangeSelfile()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_selfile.GetWindowText(ImgPath);

	size_t getVal;
	wcstombs_s(&getVal, path, (ImgPath.GetLength() + 1), ImgPath, _TRUNCATE);

	ImageQuadTree();

	Invalidate();
}





void CQuadTreeImageDlg::ImageQuadTree()
{
	// Input Image의 파일명
	//string CLUSTER[1] = { "test.PNG"};


	CT2CA convertedString(ImgPath);
	std::string path = std::string(convertedString);


	// QuadTree Clustering
	cv::Mat img = imread(path);
	//imshow("Original", img);
	cv::Mat result;


	result = QuadTree_Cluster(img, nmin, nthreshold); // src, minArea, threshold
	//imshow("Modified", result);
	cv::imwrite("temp.bmp", result);


}

cv::Mat CQuadTreeImageDlg::QuadTree_Cluster(cv::Mat src, int minArea, float threshold) {
	cv::Mat result;
	m_orgsize = src.rows * src.cols * 3;
	m_orgimg = src.rows;
	m_orgimg2 = src.cols;
	QuadTree clustering(src, minArea, threshold);

	result = clustering.getResultMat();
	check = 1;

	/*
	QuadTree::Node root = clustering.getTree();
	QuadTree::Node* p = root.Children;
	FILE* fp = NULL;
	fp = fopen("comp.bin", "wb");
	while (p != NULL) {
			fwrite(&root, sizeof(QuadTree::Node), 1, fp);
			
			p = p->Children;

	}
	fclose(fp);
	*/

	int teemp = 8*3*clustering.getNum();
	int teeemp = 4*clustering.getCnt()+1;
	compratio = ((double)teemp+ (double)teeemp)/8;
	m_compnum = ((double)m_orgsize- (double)compratio)/ (double)m_orgsize*100;
	
	m_PSNRnum = cv::PSNR(src, result);
	m_dBnum = 10 * log10(m_orgsize / compratio);

	CString str;
	str.Format(_T("%.2f"), m_compnum);
	SetDlgItemText(IDC_compnum, str);

	str.Format(_T("%f"), m_PSNRnum);
	SetDlgItemText(IDC_PSNRnum, str);

	str.Format(_T("%.3f"), compratio);
	SetDlgItemText(IDC_compratio, str);

	SetDlgItemInt(IDC_orgimg, m_orgimg);
	SetDlgItemInt(IDC_orgimg2, m_orgimg2);
	SetDlgItemInt(IDC_orgsize, m_orgsize);

	SetDlgItemInt(IDC_dBnum, m_dBnum);

	return result;
}


void CQuadTreeImageDlg::OnEnChangethreshold()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.


}


void CQuadTreeImageDlg::OnEnChangemin()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

}


void CQuadTreeImageDlg::OnBnClickedrun()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	nthreshold = GetDlgItemInt(IDC_threshold);
	nmin = GetDlgItemInt(IDC_min);
	ImageQuadTree();
	Invalidate();
}


void CQuadTreeImageDlg::OnStnClickedcompratio()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
