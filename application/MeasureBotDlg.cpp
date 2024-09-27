
// MeasureBotDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MeasureBot.h"
#include "MeasureBotDlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"
#include "ThreadMc.h"
#include "ThreadX20.h"
#include "ThreadRtuSmacq.h"
#include "ThreadRtuLx.h"

#include <OpenXLSX.hpp>
#include <iostream>
#include <cmath>

#include "libxl.h"
using namespace libxl;

using namespace std;
using namespace OpenXLSX;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void On32773();
	afx_msg void On32792();
	afx_msg void OnMenuCtrl();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
	EnableActiveAccessibility();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)

	ON_COMMAND(ID_32773, &CAboutDlg::On32773)
	ON_COMMAND(ID_32804, &CAboutDlg::OnMenuCtrl)
END_MESSAGE_MAP()


// CMeasureBotDlg 对话框


MultiCard g_MultiCard;

IMPLEMENT_DYNAMIC(CMeasureBotDlg, CDialogEx);

CMeasureBotDlg::CMeasureBotDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MEASUREBOT_DIALOG, pParent)
{
	EnableActiveAccessibility();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = nullptr;
	DebugLog::writeLogF("--------mesure start--------%ld\n", pts_time::current());
}

CMeasureBotDlg::~CMeasureBotDlg()
{
	// 如果该对话框有自动化代理，则
	//  此对话框的返回指针为 null，所以它知道
	//  此代理知道该对话框已被删除。
	if (m_pAutoProxy != nullptr)
		m_pAutoProxy->m_pDialog = nullptr;

	DebugLog::writeLogF("--------mesure end--------%ld\n", pts_time::current());

}

void CMeasureBotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_ctrl_pages);
	DDX_Control(pDX, IDC_TAB2, m_data_pages);

	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
}

BEGIN_MESSAGE_MAP(CMeasureBotDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_MYMESSAGE_SIGNAL, &CMeasureBotDlg::OnMyMessageSignal)
	ON_BN_CLICKED(IDOK, &CMeasureBotDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMeasureBotDlg::OnBnClickedCancel)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB2, &CMeasureBotDlg::OnTcnSelchangeTab2)
	ON_BN_CLICKED(IDC_BUTTON16, &CMeasureBotDlg::OnBnClickedExortExcel)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMeasureBotDlg::OnTcnSelchangeTab1)
	ON_WM_SIZE()
	ON_COMMAND(ID_32779, &CMeasureBotDlg::On32779)
	ON_WM_TIMER()
	ON_COMMAND(ID_32773, &CMeasureBotDlg::OnConnectDevs)
	ON_COMMAND(ID_32774, &CMeasureBotDlg::OnDisconnect)
	ON_COMMAND(ID_32776, &CMeasureBotDlg::On32776)
	ON_COMMAND(ID_32780, &CMeasureBotDlg::On32780)
	ON_COMMAND(ID_32785, &CMeasureBotDlg::On32785)
	ON_COMMAND(ID_32787, &CMeasureBotDlg::On32787)
	ON_COMMAND(ID_32789, &CMeasureBotDlg::On32789)
	ON_COMMAND(ID_32790, &CMeasureBotDlg::On32790)
	ON_COMMAND(ID_32791, &CMeasureBotDlg::On32791)
	ON_COMMAND(ID_32786, &CMeasureBotDlg::On32786)
	ON_BN_CLICKED(IDC_BUTTON1, &CMeasureBotDlg::OnBnClickedSave)
	ON_COMMAND(ID_32792, &CMeasureBotDlg::On32792)
	ON_COMMAND(ID_32793, &CMeasureBotDlg::OnAutoIrraStart)
	ON_COMMAND(ID_32794, &CMeasureBotDlg::OnIrraStop)
	ON_COMMAND(ID_32783, &CMeasureBotDlg::On32783)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_COMMAND(ID_32797, &CMeasureBotDlg::On32797)
	ON_COMMAND(ID_32795, &CMeasureBotDlg::On32795)
	ON_COMMAND(ID_32796, &CMeasureBotDlg::On32796)
	ON_COMMAND(ID_32798, &CMeasureBotDlg::On32798)
	ON_COMMAND(ID_32799, &CMeasureBotDlg::On32799)
	ON_COMMAND(ID_32803, &CMeasureBotDlg::OnMenuSetting)
	ON_COMMAND(ID_32802, &CMeasureBotDlg::OnMenuStat)
	ON_COMMAND(ID_32804, &CMeasureBotDlg::OnMenuCtrl)
	ON_COMMAND(ID_32805, &CMeasureBotDlg::OnMenuHandMov)
	ON_COMMAND(ID_32806, &CMeasureBotDlg::OnMenuModX)
	ON_COMMAND(ID_32807, &CMeasureBotDlg::OnMenuMody)
	ON_COMMAND(ID_32808, &CMeasureBotDlg::OnMenuModGrid)
END_MESSAGE_MAP()


// CMeasureBotDlg 消息处理程序


BOOL CMeasureBotDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	return CDialogEx::OnNotify(wParam, lParam, pResult);
}


BOOL CMeasureBotDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	//CMenu* pSysMenu = GetSystemMenu(FALSE);
	//if (pSysMenu != nullptr)
	//{
	//	BOOL bNameValid;
	//	CString strAboutMenu;
	//	bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
	//	ASSERT(bNameValid);
	//	if (!strAboutMenu.IsEmpty())
	//	{
	//		pSysMenu->AppendMenu(MF_SEPARATOR);
	//		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	//	}
	//}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	if(0){
		SCROLLINFO scrollinfo;
		GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		scrollinfo.nPage = 20; //设置滑块大小
		scrollinfo.nMax = 200; //设置滚动条的最大位置0–100
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
	}
	if (0) {
		SCROLLINFO scrollinfo;
		GetScrollInfo(SB_HORZ, &scrollinfo, SIF_ALL);
		scrollinfo.nPage = 50; //设置滑块大小
		scrollinfo.nMax = 100; //设置滚动条的最大位置0–100
		SetScrollInfo(SB_HORZ, &scrollinfo, SIF_ALL);
	}


	initbot();
	ShowWindow(SW_MAXIMIZE);


	// TODO: 在此添加额外的初始化代码

	//EnumCommPort();
	m_ctrl_pages.InsertItem(0, _T(" 参数配置 "));
	m_ctrl_pages.InsertItem(1, _T(" 测量控制 "));
	m_ctrl_pages.InsertItem(2, _T(" 设备运行 "));
	//m_ctrl_pages.InsertItem(3, _T(" 电源 "));
	//m_ctrl_pages.InsertItem(4, _T(" 光表 "));

	m_ctrl_mc.Create(IDD_DIALOG_MC, GetDlgItem(IDC_TAB1));
	m_ctrl_cfg.Create(IDD_DIALOG_CFG, GetDlgItem(IDC_TAB1));
	m_ctrl_stat.Create(IDD_DIALOG_SAMPLE, GetDlgItem(IDC_TAB1));
	
	m_ctrl_lx.Create(IDD_DIALOG_POWER, GetDlgItem(IDC_TAB1));
	m_ctrl_x20.Create(IDD_DIALOG_X20, GetDlgItem(IDC_TAB1));


	//m_ctrl_cfg.ShowWindow(true);
	m_ctrl_pages.ShowWindow(false);
	m_ctrl_pages.SetCurSel(4);

	/*for (int i = 0; i < m_ctrl_pages.GetItemCount(); ++i)
	{
		m_ctrl_pages.SetItemState(i, 0, 0);
	}*/
	//--------------------
	m_data_pages.InsertItem(2, _T(" MD报表 "));
	m_data_pages.InsertItem(3, _T(" DA报表 "));
	m_data_pages.InsertItem(1, _T(" CD报表 "));
//	m_data_pages.InsertItem(4, _T(" 图表 "));
	m_data_pages.InsertItem(0, _T(" 测量轨迹 "));

	m_page_chart.Create(IDD_DIALOGChart, GetDlgItem(IDC_TAB2));
	m_page_list.Create(IDD_DIALOGList, GetDlgItem(IDC_TAB2));
	m_page_list2.Create(IDD_DIALOGList2, GetDlgItem(IDC_TAB2));
	m_page_list3.Create(IDD_DIALOG3, GetDlgItem(IDC_TAB2));
	m_page_track.Create(IDD_DIALOGTrack2, GetDlgItem(IDC_TAB2));

	//m_data_pages.SetPadding(CSize(-1000, -1000));         //   适当调整这个值.  
	//m_data_pages.SetItemSize(CSize(0, 0));


	m_data_pages.ShowWindow(false);
	m_data_pages.SetCurSel(0);


	

	{
		CTime time;
		time = time.GetCurrentTime();
		CString stime;
		stime = time.Format("%y-%m-%d %H:%M:%S");
		// 添加状态栏
		UINT array[2] = { 12301,12302 };
		m_Statusbar.Create(this);
		m_Statusbar.SetIndicators(array, sizeof(array) / sizeof(UINT));

		//显示状态栏
		CRect rect;
		GetWindowRect(rect);
		m_Statusbar.SetPaneInfo(0, array[0], 0, rect.Width() / 3);
		m_Statusbar.SetPaneInfo(1, array[1], 0, rect.Width() / 3 * 2);
		m_Statusbar.SetPaneText(0, stime);
		m_Statusbar.SetPaneText(1, _T("LED 光照测试系统"));
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	}

	//testExport();
	/*{if (!m_wndToolBar.Create(this) || !m_wndToolBar.LoadToolBar(IDR_TOOLBAR1))
	{
		TRACE0("Failed to Create Dialog Toolbar\n ");
		EndDialog(IDCANCEL);
	}

	

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0); }*/
	
	//InitToolBar();
	CRect rect;
	GetClientRect(&rect);
	resizeWindow(rect.right, rect.bottom);
	CRect rect1;
	rect1.top = (rect.Height() - 30) / 2;
	rect1.bottom = rect1.top + 30;
	rect1.left = (rect.Width() - 400) / 2;
	rect1.right = rect1.left + 400;
	m_progress.MoveWindow(rect1);
	//m_progress.ShowWindow(true);

	windowInit = true;
	SetTimer( 1, 1000, NULL);
	//this->EnableWindow(false);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
void CMeasureBotDlg::testExport() {
	// First, create a new document and access the sheet named 'Sheet1'.
  // New documents contain a single worksheet named 'Sheet1'
	XLDocument doc;
	doc.create("./Demo01.xlsx");
	auto wks = doc.workbook().worksheet("Sheet1");

	// The individual cells can be accessed by using the .cell() method on the worksheet object.
	// The .cell() method can take the cell address as a string, or alternatively take a XLCellReference
	// object. By using an XLCellReference object, the cells can be accessed by row/column coordinates.
	// The .cell() method returns an XLCell object.

	// The .value() method of an XLCell object can be used for both getting and setting the cell value.
	// Setting the value of a cell can be done by using the assignment operator on the .value() method
	// as shown below. Alternatively, a .set() can be used. The cell values can be floating point numbers,
	// integers, strings, and booleans. It can also accept XLDateTime objects, but this requires special
	// handling (see later).
	wks.cell("A1").value() = 3.14159265358979323846;
	wks.cell("B1").value() = 42;
	wks.cell("C1").value() = "  Hello OpenXLSX!  ";
	wks.cell("D1").value() = true;
	wks.cell("E1").value() = std::sqrt(-2); // Result is NAN, resulting in an error value in the Excel spreadsheet.

	// As mentioned, the .value() method can also be used for getting tha value of a cell.
	// The .value() method returns a proxy object that cannot be copied or assigned, but
	// it can be implicitly converted to an XLCellValue object, as shown below.
	// Unfortunately, it is not possible to use the 'auto' keyword, so the XLCellValue
	// type has to be explicitly stated.
	XLCellValue A1 = wks.cell("A1").value();
	XLCellValue B1 = wks.cell("B1").value();
	XLCellValue C1 = wks.cell("C1").value();
	XLCellValue D1 = wks.cell("D1").value();
	XLCellValue E1 = wks.cell("E1").value();

	// The cell value can be implicitly converted to a basic c++ type. However, if the type does not
	// match the type contained in the XLCellValue object (if, for example, floating point value is
	// assigned to a std::string), then an XLValueTypeError exception will be thrown.
	// To check which type is contained, use the .type() method, which will return a XLValueType enum
	// representing the type. As a convenience, the .typeAsString() method returns the type as a string,
	// which can be useful when printing to console.
	double vA1 = wks.cell("A1").value();
	int vB1 = wks.cell("B1").value();
	std::string vC1 = wks.cell("C1").value();
	bool vD1 = wks.cell("D1").value();
	double vE1 = wks.cell("E1").value();

	cout << "Cell A1: (" << A1.typeAsString() << ") " << vA1 << endl;
	cout << "Cell B1: (" << B1.typeAsString() << ") " << vB1 << endl;
	cout << "Cell C1: (" << C1.typeAsString() << ") " << vC1 << endl;
	cout << "Cell D1: (" << D1.typeAsString() << ") " << vD1 << endl;
	cout << "Cell E1: (" << E1.typeAsString() << ") " << vE1 << endl << endl;

	// Instead of using implicit (or explicit) conversion, the underlying value can also be retrieved
	// using the .get() method. This is a templated member function, which takes the desired type
	// as a template argument.
	cout << "Cell A1: (" << A1.typeAsString() << ") " << A1.get<double>() << endl;
	cout << "Cell B1: (" << B1.typeAsString() << ") " << B1.get<int64_t>() << endl;
	cout << "Cell C1: (" << C1.typeAsString() << ") " << C1.get<std::string>() << endl;
	cout << "Cell D1: (" << D1.typeAsString() << ") " << D1.get<bool>() << endl;
	cout << "Cell E1: (" << E1.typeAsString() << ") " << E1.get<double>() << endl << endl;

	// XLCellValue objects can also be copied and assigned to other cells. This following line
	// will copy and assign the value of cell C1 to cell E1. Note tha only the value is copied;
	// other cell properties of the target cell remain unchanged.
	wks.cell("F1").value() = wks.cell(XLCellReference("C1")).value();
	XLCellValue F1 = wks.cell("F1").value();
	cout << "Cell F1: (" << F1.typeAsString() << ") " << F1.get<std::string_view>() << endl << endl;

	// Date/time values is a special case. In Excel, date/time values are essentially just a
	// 64-bit floating point value, that is rendered as a date/time string using special
	// formatting. When retrieving the cell value, it is just a floating point value,
	// and there is no way to identify it as a date/time value.
	// If, however, you know it to be a date time value, or if you want to assign a date/time
	// value to a cell, you can use the XLDateTime class, which falilitates conversion between
	// Excel date/time serial numbers, and the std::tm struct, that is used to store
	// date/time data. See https://en.cppreference.com/w/cpp/chrono/c/tm for more information.

	// An XLDateTime object can be created from a std::tm object:
	std::tm tm;
	tm.tm_year = 121;
	tm.tm_mon = 8;
	tm.tm_mday = 1;
	tm.tm_hour = 12;
	tm.tm_min = 0;
	tm.tm_sec = 0;
	XLDateTime dt(tm);
	//    XLDateTime dt (43791.583333333299);

		// The std::tm object can be assigned to a cell value in the same way as shown previously.
	wks.cell("G1").value() = dt;

	// And as seen previously, an XLCellValue object can be retrieved. However, the object
	// will just contain a floating point value; there is no way to identify it as a date/time value.
	XLCellValue G1 = wks.cell("G1").value();
	cout << "Cell G1: (" << G1.typeAsString() << ") " << G1.get<double>() << endl;

	// If it is known to be a date/time value, the cell value can be converted to an XLDateTime object.
	auto result = G1.get<XLDateTime>();

	// The Excel date/time serial number can be retrieved using the .serial() method.
	cout << "Cell G1: (" << G1.typeAsString() << ") " << result.serial() << endl;

	// Using the .tm() method, the corresponding std::tm object can be retrieved.
	auto tmo = result.tm();
	cout << "Cell G1: (" << G1.typeAsString() << ") " << std::asctime(&tmo);

	doc.save();
	doc.close();

}


//初始化工具栏
void CMeasureBotDlg::InitToolBar()
{
	CString strPath;
	HICON hIcon;

	//创建图像列表
	m_ImageList.Create(32, 32, ILC_COLOR32 | ILC_MASK, 0, 0);

	//向图像列表中添加图标
	for (int i = 0; i < 10; i++)
	{
		//获取图片的路径
		strPath.Format(TEXT(".\\res\\ico\\%02d.ico"), i);
		//加载图标
		hIcon = (HICON)::LoadImage(NULL, strPath, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);

		m_ImageList.Add(hIcon);
	}

	//创建工具栏
	m_ToolBarCtrl.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 154231);
	//工具栏支持自动化
	m_ToolBarCtrl.EnableAutomation();
	//设置工具栏的图像列表
//	m_ToolBarCtrl.SetImageList(&m_ImageList);

	TBBUTTON button[11];

	for (int j = 0; j < 11; j++)
	{
		button[j].dwData = 0;
		//工具栏按钮为可用
		button[j].fsState = TBSTATE_ENABLED;
		//工具栏按钮样式
		button[j].fsStyle = TBSTYLE_BUTTON;
	}

	//设置工具栏按钮的命令ID值
	button[0].idCommand = 1;// ID_ADDDATA;
	//设置图标索引
	button[0].iBitmap = 0;
	//设置工具栏按钮名称
	button[0].iString = m_ToolBarCtrl.AddStrings(TEXT("添加"));


	//设置工具栏按钮的命令ID值
	button[1].idCommand = 1;//ID_UPDATEDATA;
	//设置图标索引
	button[1].iBitmap = 1;
	//设置工具栏按钮名称
	button[1].iString = m_ToolBarCtrl.AddStrings(TEXT("修改"));


	//设置工具栏按钮的命令ID值
	button[2].idCommand = 2;//ID_DELETEDATA;
	//设置图标索引
	button[2].iBitmap = 2;
	//设置工具栏按钮名称
	button[2].iString = m_ToolBarCtrl.AddStrings(TEXT("删除"));

	button[3].fsStyle = TBSTYLE_SEP;



	//设置工具栏按钮的命令ID值
	button[4].idCommand = 3;//ID_FIRSTDATA;
	//设置图标索引
	button[4].iBitmap = 3;
	//设置工具栏按钮名称
	button[4].iString = m_ToolBarCtrl.AddStrings(TEXT("第一条"));


	//设置工具栏按钮的命令ID值
	button[5].idCommand = 4;//ID_PREVIOUSDATA;
	//设置图标索引
	button[5].iBitmap = 4;
	//设置工具栏按钮名称
	button[5].iString = m_ToolBarCtrl.AddStrings(TEXT("上一条"));


	//设置工具栏按钮的命令ID值
	button[6].idCommand = 5;//ID_NEXTDATA;
	//设置图标索引
	button[6].iBitmap = 5;
	//设置工具栏按钮名称
	button[6].iString = m_ToolBarCtrl.AddStrings(TEXT("下一条"));


	//设置工具栏按钮的命令ID值
	button[7].idCommand = 6;//ID_LASTDATA;
	//设置图标索引
	button[7].iBitmap = 6;
	//设置工具栏按钮名称
	button[7].iString = m_ToolBarCtrl.AddStrings(TEXT("末一条"));

	button[8].fsStyle = TBSTYLE_SEP;

	//设置工具栏按钮的命令ID值
	button[9].idCommand = 7;//ID_SAVEDATA;
	//设置图标索引
	button[9].iBitmap = 7;
	//设置工具栏按钮名称
	button[9].iString = m_ToolBarCtrl.AddStrings(TEXT("保存"));


	//设置工具栏按钮的命令ID值
	button[10].idCommand = 8;//ID_CANCELDATA;
	//设置图标索引
	button[10].iBitmap = 8;
	//设置工具栏按钮名称
	button[10].iString = m_ToolBarCtrl.AddStrings(TEXT("取消"));

	//向工具栏中添加按钮
	m_ToolBarCtrl.AddButtons(11, button);
	//自动调整工具栏的大小
	m_ToolBarCtrl.AutoSize();
	//设置工具栏的样式
	m_ToolBarCtrl.SetStyle(TBSTYLE_FLAT | CCS_TOP);

}


void CMeasureBotDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMeasureBotDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMeasureBotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 当用户关闭 UI 时，如果控制器仍保持着它的某个
//  对象，则自动化服务器不应退出。  这些
//  消息处理程序确保如下情形: 如果代理仍在使用，
//  则将隐藏 UI；但是在关闭对话框时，
//  对话框仍然会保留在那里。

void CMeasureBotDlg::OnClose()
{
	if (CanExit())
		CDialogEx::OnOK();
}

void CMeasureBotDlg::OnOK()
{
	if (CanExit())
		CDialogEx::OnOK();
}

void CMeasureBotDlg::OnCancel()
{
	if (CanExit())
		CDialogEx::OnCancel();
}

BOOL CMeasureBotDlg::CanExit()
{
	// 如果代理对象仍保留在那里，则自动化
	//  控制器仍会保持此应用程序。
	//  使对话框保留在那里，但将其 UI 隐藏起来。
	if (m_pAutoProxy != nullptr)
	{
		//ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}

//-------------------follow bot funcs-------------------------
LRESULT CMeasureBotDlg::OnMyMessageSignal(WPARAM wParam, LPARAM data) {

	int msg_id = wParam;
	std::string* msg = reinterpret_cast<std::string*>(data);

	Json::Reader reader;
	Json::Value message;
	if (!reader.parse(*msg, message)) {
		return 0;
	}

	if (msg_id == MC_MESSAGE) {

	}
	else if (msg_id == L5000_MESSAGE) {
		if (message["cmd"].asString() == "Connect") {
			if (message["code"].asInt() == 0) {
				m_ctrl_stat.ed_power.SetWindowText(_T("已连接"));
				m_ctrl_lx.m_connected = 1;
				m_ctrl_lx.sendCommand(string("setRatio"));
				m_ctrl_lx.initChannel();
			}else if (message["code"].asInt() == -2001) {
				m_ctrl_stat.ed_power.SetWindowText(_T("连接失败"));
				m_ctrl_lx.m_connected = 0;
			}
		}
		else if (message["cmd"].asString() == "Stat") {

			int pow = message["pow"].asInt();
			int cur = message["cur"].asInt();
			int stat= message["stat"].asInt();
			int err= message["err"].asInt();
			int warn= message["warn"].asInt();

			int chanbits= message["chns"].asInt();
			m_pow_chnbits = chanbits;

			static char buf[128] = { 0 };
			memset(buf, 0, 128);
			for (int i = 15; i >= 0; i--) {
				if (chanbits & 1 << i)
				{

				}
				else {
					m_ctrl_stat.ed_power_list.SetItemText(i, 1, L"未启用");
					m_ctrl_stat.ed_power_list.SetItemText(i, 2, L"--"); 
					m_ctrl_stat.ed_power_list.SetItemText(i, 3, L"--");
				//	m_ctrl_stat.ed_power_list.SetItemText(i, 4, L"--");
				}
			}
		}
	}
	else if (msg_id == SMACQ_MESSAGE) {
		if (message["cmd"].asString() == "Connect") {
			if (message["code"].asInt() == 0) {
				m_ctrl_stat.ed_smacq.SetWindowText(_T("已连接"));
				m_ctrl_stat.m_connected = 1;
				m_ctrl_stat.sendCommand(string("Automeasure"));
			}
			else if (message["code"].asInt() == -3001) {
				m_ctrl_stat.ed_smacq.SetWindowText(_T("连接失败"));
				m_ctrl_stat.m_connected = 0;
			}
		}
	}
	else if (msg_id == X20_MESSAGE) {
		if (message["cmd"].asString() == "Connect") {
			if (message["code"].asInt() == 0) {
				m_ctrl_stat.ed_x20.SetWindowText(_T("已连接"));
				m_ctrl_x20.m_connected = 1;
				m_ctrl_x20.sendCommand(string("Set"));
			}
			else if (message["code"].asInt() == -4001) {
				m_ctrl_stat.ed_x20.SetWindowText(_T("连接失败"));
				m_ctrl_x20.m_connected = 0;
			}
		}
		else if (message["cmd"].asString() == "Set") {
			if (message["code"].asInt() == 0) {
				m_ctrl_x20.sendCommand(string("AutoMeasure"));
			}
			else if (message["code"].asInt() == -4002) {
				m_ctrl_stat.ed_x20.SetWindowText(_T("光表参数设置失败"));
			}
		}
		else if (message["cmd"].asString() == "Measure") {
			float val = message["val"].asDouble();
			string unit = message["unit"].asString();

			CString boxmsg;
			boxmsg.Format(_T("当前光强值：%.6f %s"),val, unit.c_str());
			::MessageBox(NULL, boxmsg, _T("测量完成"), MB_OK);
		}
		else if (message["cmd"].asString() == "AutoMeasure") {
			float val = message["val"].asDouble();
			string unit = message["unit"].asString();
			int unitIdx = message["unit_idx"].asInt();
			double dbtm = message["tm"].asDouble();


			handleAutoMeasure(val,unit, unitIdx,unsigned long(dbtm));
			
		}
	}

	delete msg;

	return 1;
}

void CMeasureBotDlg::handleAutoMeasure(float val, string& unit, int unitNo,unsigned long tm) {
	//real view
	/*if (unit == "W/cm2" && m_ctrl_stat.selUnit == 1) {
		val = val * 1000.0;
	}
	if (unit == "" && m_ctrl_stat.selUnit == 2) {
		val = val / 1000.0;
	}*/
	if ((unitNo == 17 || unitNo == 18)) {
		if (m_ctrl_stat.selUnit == 1) {
			val = val * 1000.0;
		}
	}
	else if ((unitNo == 1 || unitNo == 3))
	{
		if (m_ctrl_stat.selUnit == 1) {
			val = val * 1000.0 /10000;
		}
		else if (m_ctrl_stat.selUnit == 2) {
			val = val  / 10000;
		}
	}

	CString edMsg;
	edMsg.Format(L"%.2e", val);
	m_ctrl_stat.ed_irra.SetWindowText(edMsg);


	if (start_irra) {
		if (time(NULL) - irra_start_tm >= 60) {
			start_irra = false;
			dumpIrraList();
			irraList_.clear();
		}
		else {
			IrraItem_ item;
			item.tm = time(NULL);//tm
			item.val = val;
			irraList_.push_back(item);
		}
	
	}

	//
	int trackmod = m_ctrl_mc.m_models.GetCurSel() + 1;
	if (trackmod == 1) {
		if (m_ctrl_mc.m_isMeasure == 1 && m_ctrl_mc.m_task_stat == 1) {
			m_page_list2.onIrraVal(val, m_ctrl_stat.selUnit, tm);
		}
	}
	if (trackmod == 2) {
		if (m_ctrl_mc.m_isMeasure == 1 && m_ctrl_mc.m_task_stat == 1) {
			m_page_list.onIrraVal(val, m_ctrl_stat.selUnit, tm);
		}
	}
	if (trackmod == 3) {
		if (m_ctrl_mc.m_task_stat == 1) {
			m_page_list3.onIrraVal(val, m_ctrl_stat.selUnit, tm);
		}

	}
}

string WStringToUTF8(const wchar_t* lpwcszWString)
{
	char* pElementText;
	int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
	::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);
	string strReturn(pElementText);
	delete[] pElementText;
	return strReturn;
}

void CMeasureBotDlg::initbot() {
	
	ThreadMc::startThread(&mcQueue_);
	ThreadX20::startThread(&x20Queue_);
	ThreadRtuSmacq::startThread(&smacqQueie_);
	ThreadRtuLx::startThread(&lxQueie_);

}
void CMeasureBotDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void CMeasureBotDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
//	CDialogEx::OnCancel();
}



BOOL CMeasureBotDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialogEx::PreTranslateMessage(pMsg);
}

int CMeasureBotDlg::EnumCommPort() {
	HDEVINFO hDevInfo;
	SP_DEVICE_INTERFACE_DATA ifData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA pDetail;
	DWORD requiredSize;
	GUID guid = GUID_DEVCLASS_PORTS; // GUID for serial ports

	hDevInfo = SetupDiGetClassDevs((LPGUID)&guid, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hDevInfo == INVALID_HANDLE_VALUE) {
		// Handle error
		return 1;
	}

	// Enumerate through all serial ports
	//m_serailSel.ResetContent();
	for (DWORD i = 0; i < 10; i++) { // Arbitrary limit to avoid infinite loop
		ZeroMemory(&ifData, sizeof(ifData));
		ifData.cbSize = sizeof(ifData);

		if (!SetupDiEnumDeviceInterfaces(hDevInfo, 0, (LPGUID)&guid, i, &ifData)) {
			if (GetLastError() == ERROR_NO_MORE_ITEMS) {
				break; // No more devices
			}
			// Handle error
			continue;
		}

		SetupDiGetDeviceInterfaceDetail(hDevInfo, &ifData, NULL, 0, &requiredSize, NULL);
		pDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)HeapAlloc(GetProcessHeap(), 0, requiredSize);
		if (pDetail == NULL) {
			// Handle error
			continue;
		}

		pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &ifData, pDetail, requiredSize, &requiredSize, NULL)) {
			// Handle error
			HeapFree(GetProcessHeap(), 0, pDetail);
			continue;
		}

		// pDetail->DevicePath contains the device path for the serial port
		// Output the device path or do something with it
		DebugLog::writeLogF("Serial port: %s\n", pDetail->DevicePath);


		CString peername;
		peername.Format(pDetail->DevicePath, i);
		//m_serailSel.AddString(peername);

		

		HeapFree(GetProcessHeap(), 0, pDetail);
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return 0;
}

string _UnicodeToUtf8(CString Unicodestr)
{
	wchar_t* unicode = Unicodestr.AllocSysString();
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
	char* szUtf8 = (char*)malloc(len + 1);
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, unicode, -1, szUtf8, len, NULL, NULL);
	string result = szUtf8;
	free(szUtf8);
	return result;
}



void CMeasureBotDlg::OnTcnSelchangeTab2(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	changePageTab();
	
}



void CMeasureBotDlg::OnBnClickedExortExcel()
{
	
}

int CMeasureBotDlg::changeCtrlTab() {
	int CurSel;

	CurSel = m_ctrl_pages.GetCurSel();

	switch (CurSel)
	{

	case 0:
		m_ctrl_mc.ShowWindow(false);
		m_ctrl_lx.ShowWindow(FALSE);
		m_ctrl_x20.ShowWindow(FALSE);
		m_ctrl_stat.ShowWindow(FALSE);
		m_ctrl_cfg.ShowWindow(true);
		if (m_ctrl_mc.m_task_stat == 0) {
			m_ctrl_cfg.m_Prop_conf.EnableWindow(true);
		}
		else {
			m_ctrl_cfg.m_Prop_conf.EnableWindow(false);
		}

		break;
	case 1:
		m_ctrl_mc.ShowWindow(true);
		m_ctrl_lx.ShowWindow(false);
		m_ctrl_x20.ShowWindow(FALSE);
		m_ctrl_stat.ShowWindow(FALSE);
		m_ctrl_cfg.ShowWindow(FALSE);

		break;
	case 2:
		m_ctrl_mc.ShowWindow(false);
		m_ctrl_lx.ShowWindow(FALSE);
		m_ctrl_x20.ShowWindow(false);
		m_ctrl_stat.ShowWindow(true);
		m_ctrl_cfg.ShowWindow(FALSE);

		break;
	case 3:
		m_ctrl_mc.ShowWindow(FALSE);
		m_ctrl_lx.ShowWindow(true);
		m_ctrl_x20.ShowWindow(FALSE);
		m_ctrl_stat.ShowWindow(false);
		m_ctrl_cfg.ShowWindow(FALSE);
		break;
	case 4:
		m_ctrl_mc.ShowWindow(FALSE);
		m_ctrl_lx.ShowWindow(FALSE);
		m_ctrl_x20.ShowWindow(true);
		m_ctrl_stat.ShowWindow(FALSE);
		m_ctrl_cfg.ShowWindow(false);
		break;

	default:
		break;
	}
	return 0;

}
int CMeasureBotDlg::changePageTab() {
	int CurSel;

	CurSel = m_data_pages.GetCurSel();

	switch (CurSel)
	{

	case 0:
		m_page_chart.ShowWindow(false);
		m_page_list.ShowWindow(FALSE);
		m_page_list2.ShowWindow(FALSE);
		m_page_list3.ShowWindow(FALSE);
		m_page_track.ShowWindow(true);
		m_page_track.SetFocus();


		break;
	case 1:
		m_page_chart.ShowWindow(false);
		m_page_list.ShowWindow(true);
		m_page_track.ShowWindow(FALSE);
		m_page_list2.ShowWindow(FALSE);
		m_page_list3.ShowWindow(FALSE);


		break;
	case 2:
		m_page_chart.ShowWindow(false);
		m_page_list.ShowWindow(false);
		m_page_list2.ShowWindow(true);
		m_page_list3.ShowWindow(FALSE);
		m_page_track.ShowWindow(FALSE);
		break;

	case 3:
		m_page_chart.ShowWindow(false);
		m_page_list.ShowWindow(false);
		m_page_list2.ShowWindow(false);
		m_page_track.ShowWindow(false);
		m_page_list3.ShowWindow(true);
		break;
	case 4:
		m_page_chart.ShowWindow(true);
		m_page_list.ShowWindow(false);
		m_page_list2.ShowWindow(false);
		m_page_track.ShowWindow(false);
		m_page_list3.ShowWindow(FALSE);
		break;

	default:
		break;
	}
	return 0;
}

void CMeasureBotDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
//	changeCtrlTab();
	
}



void CMeasureBotDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
}



void CMeasureBotDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	switch (nType)
	{
	case SIZE_MAXIMIZED:
		break;
	case SIZE_RESTORED:
		break;
	case SIZE_MINIMIZED:
		//ShowWindow(SW_HIDE);
		break;
	default:
		return;
	}
	if(windowInit)
	resizeWindow(cx,cy);
}
void CMeasureBotDlg::resizeWindow(int cx, int cy) {
	m_ctrl_pages.MoveWindow(5, 5, 475, cy-30);
	//m_ctrl_pages.MoveWindow(5, 5, 475, 1080 -100- 30);
	CRect rect;
	m_ctrl_pages.GetClientRect(&rect);
	rect.top += 0;// 30;
	rect.bottom -= 0;
	rect.left +=0;
	rect.right -= 4;
	m_ctrl_mc.MoveWindow(&rect);
	m_ctrl_lx.MoveWindow(&rect);
	m_ctrl_x20.MoveWindow(&rect);
	m_ctrl_stat.MoveWindow(&rect);
	m_ctrl_cfg.MoveWindow(&rect);

	m_data_pages.MoveWindow(5 + 475 + 10, 5, cx - 478, cy - 30);
	//m_data_pages.MoveWindow(5 + 475 + 10, 5, 1920 -40- 478, 1080-100 - 30);
	CRect rect1;
	m_data_pages.GetClientRect(&rect1);
	rect1.top += 0;// 30;
	rect1.bottom -= 29;
	rect1.left += 0;
	rect1.right -= 5;
	m_page_chart.MoveWindow(&rect1);
	m_page_list.MoveWindow(&rect1);
	CRect rect3;
	rect3.top = rect1.top;
	rect3.left = rect1.left;
	rect3.bottom = rect1.bottom + 500;
	rect3.right = rect1.right + 500;
	m_page_list2.MoveWindow(&rect1);
	m_page_list3.MoveWindow(&rect1);
	m_page_track.MoveWindow(&rect1);

	CRect rect2;
	GetWindowRect(rect2);
	m_Statusbar.SetPaneInfo(0, 12301, 0, rect2.Width() / 3);
	m_Statusbar.SetPaneInfo(1, 12302, 0, rect2.Width() / 3 * 2);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
}
void CMeasureBotDlg::checkDevs() {}
void CMeasureBotDlg::connetcDevs() {
	if (m_isConnected) {
		::MessageBox(NULL, _T("已连接，请勿重复连接！"), _T("提示"), MB_OK);
		return;
	}
	m_ctrl_mc.sendCommand(string("Connect"));
	m_ctrl_lx.sendCommand(string("Connect"));
	m_ctrl_x20.sendCommand(string("Connect"));
	m_ctrl_stat.sendCommand(string("Connect"));
	m_isConnected = true;
	OnMenuStat();
}
void CMeasureBotDlg::reconnectDevs() {
	m_ctrl_mc.sendCommand(string("ReConnect"));
	m_ctrl_lx.sendCommand(string("ReConnect"));
	m_ctrl_x20.sendCommand(string("ReConnect"));
	m_ctrl_stat.sendCommand(string("ReConnect"));
}
void CMeasureBotDlg::disconnectDevs() {
	m_ctrl_mc.sendCommand(string("DisConnect"));
	m_ctrl_lx.sendCommand(string("DisConnect"));
	m_ctrl_x20.sendCommand(string("DisConnect"));
	m_ctrl_stat.sendCommand(string("DisConnect"));
	PostQuitMessage(0);
}
int CMeasureBotDlg::startMesureTask() {
	return 0;
}
int CMeasureBotDlg::stopMesureTask() {
	return 0;
}

void CMeasureBotDlg::On32779()
{
	// TODO: 在此添加命令处理程序代码

	m_data_pages.SetCurSel(0);
	changePageTab();
}


void CMeasureBotDlg::OnTimer(UINT_PTR nIDEvent)
{
	static int count = 0;
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (0) {
		unsigned long tstm = pts_time::currentms();
		Json::Value root;
		root["tm"] = double(tstm);
		Json::FastWriter writer;
		string tmstr = writer.write(root);
		double dbtm = root["tm"].asDouble();
		unsigned long lltm = (unsigned long)dbtm;
		DebugLog::writeLogF("src tm = %ld  %s  after %f %ld", tstm, tmstr.c_str(), dbtm, lltm);
	}

	if (nIDEvent == 1)
	{
		count++;
		if (count == 1) {
			m_progress.ShowWindow(false);
			this->EnableWindow(true);
			m_data_pages.ShowWindow(TRUE);
			m_ctrl_pages.ShowWindow(true);
			m_data_pages.SetCurSel(0);
			m_ctrl_pages.SetCurSel(2);
			changeCtrlTab();
			changePageTab();



		}
		else if(count<1){
			m_progress.SetPos(count * 10);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CMeasureBotDlg::OnConnectDevs()
{
	// TODO: 在此添加命令处理程序代码
	connetcDevs();

}


void CAboutDlg::On32773()
{
	// TODO: 在此添加命令处理程序代码
}


void CMeasureBotDlg::OnDisconnect()
{
	// TODO: 在此添加命令处理程序代码
	disconnectDevs();
}


void CMeasureBotDlg::On32776()
{
	// TODO: 在此添加命令处理程序代码
	m_ctrl_pages.SetCurSel(0);
	changeCtrlTab();
}


void CMeasureBotDlg::On32780()
{
	// TODO: 在此添加命令处理程序代码
	m_ctrl_pages.SetCurSel(4);
	changeCtrlTab();
}


void CMeasureBotDlg::On32785()
{
	// TODO: 在此添加命令处理程序代码
	m_data_pages.SetCurSel(3);
	changePageTab();
}


void CMeasureBotDlg::On32787()
{
	// TODO: 在此添加命令处理程序代码
	m_data_pages.SetCurSel(0);
	changePageTab();
}


void CMeasureBotDlg::On32789()
{
	// TODO: 在此添加命令处理程序代码
	m_ctrl_lx.powerOnOff(1);
}


void CMeasureBotDlg::On32790()
{
	// TODO: 在此添加命令处理程序代码
	m_ctrl_lx.powerOnOff(0);

}


void CMeasureBotDlg::On32791()
{
	// TODO: 在此添加命令处理程序代码
	m_ctrl_pages.SetCurSel(3);
	changeCtrlTab();
}


void CMeasureBotDlg::On32786()
{
	// TODO: 在此添加命令处理程序代码
	m_data_pages.SetCurSel(2);
	changePageTab();
}


void CMeasureBotDlg::OnBnClickedSave()
{
	// TODO: 在此添加控件通知处理程序代码

}


void CMeasureBotDlg::On32792()
{
	// TODO: 在此添加命令处理程序代码
	m_ctrl_x20.sendCommand(string("Measure"));
}


void CMeasureBotDlg::OnAutoIrraStart()
{
	// TODO: 在此添加命令处理程序代码
	m_ctrl_x20.sendCommand(string("AutoMeasure"));
}


void CMeasureBotDlg::OnIrraStop()
{
	// TODO: 在此添加命令处理程序代码
	m_ctrl_x20.sendCommand(string("Stop"));
}


void CMeasureBotDlg::On32783()
{
	// TODO: 在此添加命令处理程序代码
	m_data_pages.SetCurSel(1);
	changePageTab();
}


void CMeasureBotDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
	int unit = 5;
	switch (nSBCode)
	{
	case SB_LINEUP:          //Scroll one line up
		scrollinfo.nPos -= 1;
		if (scrollinfo.nPos < scrollinfo.nMin)
		{
			scrollinfo.nPos = scrollinfo.nMin;
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, unit);
		break;
	case SB_LINEDOWN:           //Scroll one line down
		scrollinfo.nPos += 1;
		if (scrollinfo.nPos + scrollinfo.nPage > scrollinfo.nMax)  //此处一定要注意加上滑块的长度，再作判断
		{
			scrollinfo.nPos = scrollinfo.nMax;
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, -unit);
		break;
	case SB_PAGEUP:            //Scroll one page up.
		scrollinfo.nPos -= 5;
		if (scrollinfo.nPos <= scrollinfo.nMin)
		{
			scrollinfo.nPos = scrollinfo.nMin;
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, unit * 5);
		break;
	case SB_PAGEDOWN:        //Scroll one page down        
		scrollinfo.nPos += 5;
		if (scrollinfo.nPos + scrollinfo.nPage >= scrollinfo.nMax)  //此处一定要注意加上滑块的长度，再作判断
		{
			scrollinfo.nPos = scrollinfo.nMax;
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, -unit * 5);
		break;
	case SB_ENDSCROLL:      //End scroll     
		break;
	case SB_THUMBPOSITION:  //Scroll to the absolute position. The current position is provided in nPos
		break;
	case SB_THUMBTRACK:                  //Drag scroll box to specified position. The current position is provided in nPos
		ScrollWindow(0, (scrollinfo.nPos - nPos) * unit);
		scrollinfo.nPos = nPos;
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		break;
	}
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CMeasureBotDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_HORZ, &scrollinfo, SIF_ALL);
	int unit = 5;
	switch (nSBCode)
	{
	case SB_LINEUP:          //Scroll one line up
		scrollinfo.nPos -= 1;
		if (scrollinfo.nPos < scrollinfo.nMin)
		{
			scrollinfo.nPos = scrollinfo.nMin;
			break;
		}
		SetScrollInfo(SB_HORZ, &scrollinfo, SIF_ALL);
		ScrollWindow(unit, 0);
		break;
	case SB_LINEDOWN:           //Scroll one line down
		scrollinfo.nPos += 1;
		if (scrollinfo.nPos + scrollinfo.nPage > scrollinfo.nMax)  //此处一定要注意加上滑块的长度，再作判断
		{
			scrollinfo.nPos = scrollinfo.nMax;
			break;
		}
		SetScrollInfo(SB_HORZ, &scrollinfo, SIF_ALL);
		ScrollWindow(-unit, 0);
		break;
	case SB_PAGEUP:            //Scroll one page up.
		scrollinfo.nPos -= 5;
		if (scrollinfo.nPos <= scrollinfo.nMin)
		{
			scrollinfo.nPos = scrollinfo.nMin;
			break;
		}
		SetScrollInfo(SB_HORZ, &scrollinfo, SIF_ALL);
		ScrollWindow(unit * 5, 0);
		break;
	case SB_PAGEDOWN:        //Scroll one page down        
		scrollinfo.nPos += 5;
		if (scrollinfo.nPos + scrollinfo.nPage >= scrollinfo.nMax)  //此处一定要注意加上滑块的长度，再作判断
		{
			scrollinfo.nPos = scrollinfo.nMax;
			break;
		}
		SetScrollInfo(SB_HORZ, &scrollinfo, SIF_ALL);
		ScrollWindow(-unit * 5, 0);
		break;
	case SB_ENDSCROLL:      //End scroll     
		break;
	case SB_THUMBPOSITION:  //Scroll to the absolute position. The current position is provided in nPos
		break;
	case SB_THUMBTRACK:                  //Drag scroll box to specified position. The current position is provided in nPos
		ScrollWindow((scrollinfo.nPos - nPos) * unit, 0);
		scrollinfo.nPos = nPos;
		SetScrollInfo(SB_HORZ, &scrollinfo, SIF_ALL);
		break;
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CAboutDlg::On32792()
{
	// TODO: 在此添加命令处理程序代码
}


void CMeasureBotDlg::On32797()
{
	// TODO: 在此添加命令处理程序代码
	if (m_ctrl_x20.m_connected != 1) {
		::MessageBox(NULL, L"光强未连接，请连接后再开始测量", _T("提示"), MB_OK);
		return;
	}
	m_ctrl_x20.sendCommand(string("Measure"));
}


void CMeasureBotDlg::On32795()
{
	// TODO: 在此添加命令处理程序代码
	if (m_ctrl_lx.m_connected != 1) {
		::MessageBox(NULL, L"电源未连接", _T("提示"), MB_OK);
		return;
	}
	m_ctrl_lx.powerOnOff(1);
}


void CMeasureBotDlg::On32796()
{
	// TODO: 在此添加命令处理程序代码
	if (m_ctrl_lx.m_connected != 1) {
		::MessageBox(NULL, L"电源未连接", _T("提示"), MB_OK);
		return;
	}
	m_ctrl_lx.powerOnOff(0);
}


void CMeasureBotDlg::On32798()
{
	// TODO: 在此添加命令处理程序代码
	if (m_ctrl_x20.m_connected != 1) {
		::MessageBox(NULL, L"光强未连接，请连接后再开始测量", _T("提示"), MB_OK);
		return;
	}
	irraList_.clear();
	m_ctrl_x20.turnOnOff(1);
	start_irra = true;
	irra_start_tm = time(NULL);
}

void CMeasureBotDlg::dumpIrraList() {
	int count = irraList_.size(); // 获取列表中的项目数
	if (count <= 0) return;

	string report = theApp.tostr(theApp.getString(_T("File"), _T("report_path")));
	report += theApp.makeFname("Raw");

	string tplate = theApp.tostr(theApp.getString(_T("File"), _T("template_path")));
	tplate += "\\Raw_Irra.xlsx";
	CString fname(tplate.c_str());
	libxl::Book* book = xlCreateXMLBook();	// 与2003相比，仅仅这里有点区别而已
	if (nullptr == book)
	{
		DebugLog::writeLogF("create libxl book failed");
		return;
	}

	if (false == book->load(fname))
	{
		DebugLog::writeLogF(" libxl open template failed");
		return;
	}
	Sheet* sheet = book->getSheet(0);
	if (sheet == NULL) return ;
	DebugLog::writeLogF("%s", theApp.tostr(CString(sheet->name())).c_str());

	{

		int year, month, day, hour, minute, second;
		time_t current;
		time(&current);
		struct tm* l = localtime(&current);

		year = l->tm_year + 1900;
		month = l->tm_mon + 1;
		day = l->tm_mday;
		char buf[32] = { 0 };
		sprintf(buf, "%d/%d/%d", year, month, day);
		CString datestr(buf);
		sheet->writeStr(0, 1, datestr);
	}
	
	{
		CString tempstr = theApp.getString(_T("sample"), _T("sn"));
		sheet->writeStr(1, 1, tempstr);
	}

	int tabpos = 0;
	for (int32_t row = sheet->firstRow(); row < sheet->lastRow(); ++row)
	{
		CellType cell_type = sheet->cellType(row, 0);
		if (cell_type == CELLTYPE_STRING)
		{
			wstring ret = sheet->readStr(row, 0);
			if (ret == L"Test Data") {
				tabpos = row;
			}
		}
	}
	{
		CString maxTitle;
		maxTitle.Format(L"Irradiance @ %s (%s)", (theApp.getString(_T("X20"), _T("wavelength"))), (theApp.getString(_T("X20"), _T("irraunit"))));
		sheet->writeStr(tabpos + 2, 1, maxTitle);

	}

	tabpos += 3;
	for (int i = 0; i < irraList_.size(); i++) {
		sheet->copyCell(tabpos, 0, i + tabpos, 0);
		sheet->copyCell(tabpos, 1, i + tabpos, 1);
		sheet->writeStr(i + tabpos, 0, theApp.tm2CStr(irraList_[i].tm));
		{
			CString text;
			text.Format(L"%.2f", irraList_[i].val);
			sheet->writeNum(tabpos + i, 1, irraList_[i].val);
		}
	}
	

	book->save(CString(report.c_str()));

	return;
}


void CMeasureBotDlg::On32799()
{
	// TODO: 在此添加命令处理程序代码
	// 
	
	if (m_ctrl_x20.m_connected != 1) {
		::MessageBox(NULL, L"光强未连接，请连接后再开始测量", _T("提示"), MB_OK);
		return;
	}
	//m_ctrl_x20.turnOnOff(0);
	//save file
	start_irra = false;
	dumpIrraList();

}


void CMeasureBotDlg::OnMenuSetting()
{
	// TODO: 在此添加命令处理程序代码
	m_ctrl_pages.SetCurSel(0);
	changeCtrlTab();
}


void CMeasureBotDlg::OnMenuStat()
{
	// TODO: 在此添加命令处理程序代码
	m_ctrl_pages.SetCurSel(2);
	changeCtrlTab();
}


void CAboutDlg::OnMenuCtrl()
{
	// TODO: 在此添加命令处理程序代码
}


void CMeasureBotDlg::OnMenuCtrl()
{
	// TODO: 在此添加命令处理程序代码
	m_ctrl_pages.SetCurSel(1);
	changeCtrlTab();
	m_ctrl_mc.switchPan(2);

}


void CMeasureBotDlg::OnMenuHandMov()
{
	// TODO: 在此添加命令处理程序代码
	if (m_ctrl_mc.m_connected != 1) {
		::MessageBox(NULL, L"运动卡未连接", _T("提示"), MB_OK);
		return;
	}
	m_ctrl_mc.switchPan(1);
	m_ctrl_pages.SetCurSel(1);
	changeCtrlTab();

}


void CMeasureBotDlg::OnMenuModX()
{
	// TODO: 在此添加命令处理程序代码
	m_data_pages.SetCurSel(0);
	changePageTab();

	m_page_track.m_models.SetCurSel(0);
	m_page_track.updateTrackSel();
}


void CMeasureBotDlg::OnMenuMody()
{
	// TODO: 在此添加命令处理程序代码
	m_data_pages.SetCurSel(0);
	changePageTab();
	m_page_track.m_models.SetCurSel(1);
	m_page_track.updateTrackSel();
}


void CMeasureBotDlg::OnMenuModGrid()
{
	// TODO: 在此添加命令处理程序代码
	m_data_pages.SetCurSel(0);
	changePageTab();
	m_page_track.m_models.SetCurSel(2);
	m_page_track.updateTrackSel();
}
