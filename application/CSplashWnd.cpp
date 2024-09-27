// CSplashWnd.cpp: 实现文件
//

#include "pch.h"
#include "MeasureBot.h"
#include "CSplashWnd.h"
#include "afxdialogex.h"


// CSplashWnd 对话框

IMPLEMENT_DYNAMIC(CSplashWnd, CDialogEx)

CSplashWnd::CSplashWnd(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOGSplash, pParent)
{

}

CSplashWnd::~CSplashWnd()
{
}

void CSplashWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSplashWnd, CDialogEx)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CSplashWnd 消息处理程序
void CSplashWnd::Create(UINT nBitmapID)
{
	m_bitmap.LoadBitmap(nBitmapID);
	BITMAP bitmap;
	m_bitmap.GetBitmap(&bitmap);
	//CreateEx(0,AfxRegisterWndClass(0),"",WS_POPUP|WS_VISIBLE|WS_BORDER,0,0,bitmap.bmWidth,bitmap.bmHeight,NULL,0);

	int iWidth = GetSystemMetrics(SM_CXSCREEN); //获取屏幕水平分辨率
	int iHeight = GetSystemMetrics(SM_CYSCREEN); //获取屏幕垂直分辨率

	int xpos = (iWidth - bitmap.bmWidth) / 2;
	int ypos = (iHeight - bitmap.bmHeight) / 2;

	CreateEx(0,AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),NULL, WS_POPUP | WS_VISIBLE, 0, 0, iWidth, iHeight, NULL, NULL);
	//CreateEx(0, AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)), NULL, WS_POPUP | WS_VISIBLE, xpos, ypos, bitmap.bmWidth, bitmap.bmHeight, NULL, NULL);
}
void CSplashWnd::OnPaint()
{
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
	CPaintDC dc(this); // device context forpainting
	BITMAP bitmap;
	m_bitmap.GetBitmap(&bitmap);


	CDC dcComp;
	dcComp.CreateCompatibleDC(&dc);
	dcComp.SelectObject(&m_bitmap);
	// draw bitmap
	int iWidth = GetSystemMetrics(SM_CXSCREEN); //获取屏幕水平分辨率
	int iHeight = GetSystemMetrics(SM_CYSCREEN); //获取屏幕垂直分辨率
	//dc.BitBlt(0, 0, bitmap.bmWidth, bitmap.bmHeight, &dcComp, 0, 0, SRCCOPY);
	dc.StretchBlt(0, 0, iWidth, iHeight, &dcComp, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
}
void CSplashWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//CWnd::OnTimer(nIDEvent);
	DestroyWindow(); //销毁初始画面窗口
}
