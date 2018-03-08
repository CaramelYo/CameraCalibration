
// CameraCalibrationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CameraCalibration.h"
#include "CameraCalibrationDlg.h"
#include "afxdialogex.h"
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCameraCalibrationDlg dialog



CCameraCalibrationDlg::CCameraCalibrationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CAMERACALIBRATION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCameraCalibrationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCameraCalibrationDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &CCameraCalibrationDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CCameraCalibrationDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CCameraCalibrationDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CCameraCalibrationDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CCameraCalibrationDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CCameraCalibrationDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CCameraCalibrationDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CCameraCalibrationDlg::OnBnClickedButton8)
//	ON_EN_CHANGE(IDC_EDIT1, &CCameraCalibrationDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CCameraCalibrationDlg message handlers

BOOL CCameraCalibrationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	AllocConsole();

	//freopen("CONOUT$", "w", stdout);
	FILE *stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCameraCalibrationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCameraCalibrationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//to set the attribute of chess board
Size patternSize(11, 8), winSize(11, 11);
int nBoards = 21, cornerWidth = patternSize.width, cornerCount = patternSize.width * patternSize.height;
Mat *imgs = new Mat[nBoards], *grayImgs = new Mat[nBoards];

//to set the camera mat
Mat intrinsicMat(3, 3, CV_32FC1);
Mat distortionMat(1, 5, CV_32F);
vector<Mat> rvecs, tvecs;
bool isSetIntrinsicMat = false;

//to create the chess board points
vector<vector<Point3f>> objectsPoints;
vector<vector<Point2f>> imagesPoints;
vector<Point3f> objectPoints;
vector<Point2f> imagePoints;

bool FindCorners(const Mat &img, const Size &patternSize, vector<Point2f> &corners, const Size &winSize) {

	bool patternFound = findChessboardCorners(img, patternSize, corners, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FAST_CHECK);

	if (patternFound)
		cornerSubPix(img, corners, winSize, Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

	return patternFound;
}

void Calibrate(const Mat &src, const vector<vector<Point3f>> &objectsPoints, const vector<vector<Point2f>> &imagesPoints) {
	Size imageSize = src.size();

	calibrateCamera(objectsPoints, imagesPoints, imageSize, intrinsicMat, distortionMat, rvecs, tvecs, 0);
	isSetIntrinsicMat = true;

	//??
	//initUndistortRectifyMap(intrinsicMat, distortionMat, Mat(), Mat(), imageSize, CV_32F, map1, map2);
	//remap(src, dst, map1, map2, INTER_LINEAR);
}



void CCameraCalibrationDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here

	//to load the bmp
	Mat img = imread("database/1.bmp", CV_LOAD_IMAGE_COLOR), grayImg;

	//to check if img is null
	if (!img.data) {
		cout << "img GG" << endl;
		return;
	}

	cvtColor(img, grayImg, CV_BGR2GRAY);

	vector<Point2f> corners;

	bool patternFound = FindCorners(grayImg, patternSize, corners, winSize);

	drawChessboardCorners(img, patternSize, Mat(corners), patternFound);

	namedWindow("Corner Image", WINDOW_NORMAL);
	imshow("Corner Image", img);

	waitKey(0);
}

void CCameraCalibrationDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here

	if (!isSetIntrinsicMat) {
		//to set the corners in 3d
		for (int i = 0; i < cornerCount; ++i)
			objectPoints.push_back(Point3f(i / cornerWidth, i % cornerWidth, 0.0f));

		//to read all image and detect the corner
		for (int i = 0; i < nBoards; ++i) {
			int j = i + 1;

			imgs[i] = imread("database/" + to_string(j) + ".bmp", CV_LOAD_IMAGE_COLOR);

			if (!imgs[i].data) {
				cout << "img GG" << endl;
				return;
			}

			cvtColor(imgs[i], grayImgs[i], CV_BGR2GRAY);

			FindCorners(grayImgs[i], patternSize, imagePoints, winSize);
			if (imagePoints.size() == patternSize.area()) {
				objectsPoints.push_back(objectPoints);
				imagesPoints.push_back(imagePoints);
			}
			else {
				cout << "size GG" << endl;
				return;
			}
		}

		Calibrate(grayImgs[0], objectsPoints, imagesPoints);
	}

	//it cannot be outputed by printf
	//the problem is ptr???
	cout << "intrinsic Mat\n" << intrinsicMat << endl;
}

void CCameraCalibrationDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here

	if (!isSetIntrinsicMat)
		OnBnClickedButton2();

	Mat rotationMat, extrinsicMat(3, 4, CV_32FC1);

	//to change rvec[i] (3*1) to rotation matrix (3*3)
	Rodrigues(rvecs[0], rotationMat);

	rotationMat.col(0).copyTo(extrinsicMat.col(0));
	rotationMat.col(1).copyTo(extrinsicMat.col(1));
	rotationMat.col(2).copyTo(extrinsicMat.col(2));
	tvecs[0].copyTo(extrinsicMat.col(3));

	cout << "extrinsic matrix\n" << extrinsicMat << endl;
}


void CCameraCalibrationDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	if (!isSetIntrinsicMat)
		OnBnClickedButton2();

	cout << "distortion matrix\n" << distortionMat << endl;
}


void CCameraCalibrationDlg::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here
	if (!isSetIntrinsicMat)
		OnBnClickedButton2();

	//to create the points of pyramid
	vector<Point3f> pyramidObjectPoints;

	//corners
	pyramidObjectPoints.push_back(Point3f(-1, -1, 0));
	pyramidObjectPoints.push_back(Point3f(1, -1, 0));
	pyramidObjectPoints.push_back(Point3f(1, 1, 0));
	pyramidObjectPoints.push_back(Point3f(-1, 1, 0));
	//vertex
	pyramidObjectPoints.push_back(Point3f(0, 0, -2));

	for (int i = 0; i < 5; ++i) {
		vector<Point2f> pyramidImagePoints;
		projectPoints(pyramidObjectPoints, rvecs[i], tvecs[i], intrinsicMat, distortionMat, pyramidImagePoints);
		
		//cout << "pyramidImagePoints\n" << pyramidImagePoints << endl;

		Mat draw = imgs[i].clone();
		//color Scalar(BGR)
		Scalar color(0, 0, 255);
		int thickness = 3;
		for(int j = 0; j < 4; ++j) {
			line(draw, pyramidImagePoints[4], pyramidImagePoints[j], color, thickness);
			line(draw, pyramidImagePoints[j], pyramidImagePoints[(j + 1) % 4], color, thickness);
		}
		
		imshow("Pyramid Image", draw);
		waitKey(500);
	}
}

Mat eyes, medianEyes, cannyEyes, circledEyes;
void CCameraCalibrationDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here

	if (!eyes.data) 
		eyes = imread("database/Eyes.png", CV_LOAD_IMAGE_GRAYSCALE);

	medianBlur(eyes, medianEyes, 3);
	imshow("Median Smooth Eyes", medianEyes);
}


void CCameraCalibrationDlg::OnBnClickedButton7()
{
	// TODO: Add your control notification handler code here

	if (!medianEyes.data)
		OnBnClickedButton6();

	Canny(medianEyes, cannyEyes, 75, 150, 3);

	//to inverse the black and white
	//threshold(cannyEyes, cannyEyes, 128, 255, THRESH_BINARY_INV);
	
	imshow("Canny Eyes", cannyEyes);
}


void CCameraCalibrationDlg::OnBnClickedButton8()
{
	// TODO: Add your control notification handler code here

	if (!medianEyes.data)
		OnBnClickedButton7();
	
	if (!circledEyes.data) {
		circledEyes = medianEyes.clone();
		cvtColor(circledEyes, circledEyes, CV_GRAY2BGR);

		/*
		CString minS, highS, lowS;

		GetDlgItem(IDC_EDIT1)->GetWindowText(minS);
		GetDlgItem(IDC_EDIT2)->GetWindowText(highS);
		GetDlgItem(IDC_EDIT3)->GetWindowText(lowS);

		int min, high, low;

		min = _ttoi(minS);
		high = _ttoi(highS);
		low = _ttoi(lowS);
		*/

		vector<Vec3f> circles;
		HoughCircles(cannyEyes, circles, CV_HOUGH_GRADIENT, 2, 100, 100, 50);

		Scalar color(0, 0, 255);
		int thickness = 1;
		for (int i = 0; i < circles.size(); ++i) 
			circle(circledEyes, Point(circles[i][0], circles[i][1]), (int)circles[i][2], color, thickness);
	}
	
	imshow("Hough Circle Eyes", circledEyes);
}