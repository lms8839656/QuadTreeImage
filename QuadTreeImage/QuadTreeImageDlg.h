
// QuadTreeImageDlg.h: 헤더 파일
//
#include <opencv2/opencv.hpp>
#pragma once


// CQuadTreeImageDlg 대화 상자
class CQuadTreeImageDlg : public CDialog
{
// 생성입니다.
public:
	CQuadTreeImageDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_QUADTREEIMAGE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CMFCEditBrowseCtrl m_selfile;
	afx_msg void OnEnChangeSelfile();

	ULONG_PTR gdiplusToken;
	CString ImgPath;
	char path[256];
	char per[256];
	CRect rect1;
	CRect rect2;

	int check;

	int minArea;
	float threshold;

	cv::Mat QuadTree_Cluster(cv::Mat src, int minArea, float threshold);
	void ImageQuadTree();

	afx_msg void OnEnChangethreshold();
	float nthreshold;
	int nmin;
	afx_msg void OnEnChangemin();
	afx_msg void OnBnClickedrun();
	double m_compnum;
	int m_orgsize;
	int m_orgimg;
	int m_orgimg2;
	double compratio;
	double m_PSNRnum;
	afx_msg void OnStnClickedcompratio();
	int m_dBnum;
};
