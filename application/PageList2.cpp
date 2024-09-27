// PageList2.cpp: 实现文件
//

#include "pch.h"
#include "MeasureBot.h"
#include "PageList2.h"
#include "afxdialogex.h"

#include ".\CGridListCtrlEx\CGridColumnTraitDateTime.h"
#include ".\CGridListCtrlEx\CGridColumnTraitEdit.h"
#include ".\CGridListCtrlEx\CGridColumnTraitCombo.h"
#include ".\CGridListCtrlEx\CGridColumnTraitHyperLink.h"
#include ".\CGridListCtrlEx\CGridRowTraitXP.h"
#include ".\CGridListCtrlEx\ViewConfigSection.h"


#include "ChartCtrl\ChartMouseListener.h"  //鼠标响应头文件
#include  "ChartCtrl\ChartAxisLabel.h"  //鼠标响应头文件
#include "ChartCtrl/ChartBarSerie.h"

#include "libxl.h"
using namespace libxl;
// PageList2 对话框

IMPLEMENT_DYNAMIC(PageList2, CDialogEx)

PageList2::PageList2(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOGList2, pParent)
{

}

PageList2::~PageList2()
{
}

void PageList2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Control(pDX, IDC_CUSTOM3, m_ChartCtrl);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl1);
	DDX_Control(pDX, IDC_CUSTOM1, m_ChartCtrl1);
	DDX_Control(pDX, IDC_CUSTOM2, m_ChartCtrl2);
	DDX_Control(pDX, IDC_COMBO1, m_formulas);
}


BEGIN_MESSAGE_MAP(PageList2, CDialogEx)
	ON_WM_SIZE()
	ON_MESSAGE(WM_MYMESSAGE_PAGE_LIST, &PageList2::OnMyMessageSignal)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &PageList2::OnLvnItemchangedList1)
	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, &PageList2::OnHdnItemdblclickList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &PageList2::OnNMDblclkList1)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_COMBO1, &PageList2::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON1, &PageList2::OnBnClickedButton1)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST2, &PageList2::OnLvnEndlabeleditList2)
	ON_BN_CLICKED(IDC_BUTTON2, &PageList2::OnBnClickedCompute)
END_MESSAGE_MAP()


// PageList2 消息处理程序
LRESULT PageList2::OnMyMessageSignal(WPARAM wParam, LPARAM data) {
	int msg_id = wParam;
	std::string* pMsg = reinterpret_cast<std::string*>(data);
	return 0;
}
void PageList2::initChart() {
	pBottomAxis = m_ChartCtrl.CreateStandardAxis(CChartCtrl::BottomAxis);
	pBottomAxis->SetMinMax(-5, 105);
	//pBottomAxis->SetAutomatic(true);
	pBottomAxis->GetLabel()->SetText(_T("WORK DISTANCE（MM）"));
	pBottomAxis->GetLabel()->SetColor(RGB(255, 255, 255));



	pLeftAxis = m_ChartCtrl.CreateStandardAxis(CChartCtrl::LeftAxis);
//	pLeftAxis->SetMinMax(0, 30);
	pLeftAxis->SetAutomatic(true);
	pLeftAxis->GetLabel()->SetText(_T("Max Irradiance（MM）"));
	pLeftAxis->GetLabel()->SetColor(RGB(255, 255, 255));

	CChartStandardAxis* pRightAxis = m_ChartCtrl.CreateStandardAxis(CChartCtrl::RightAxis);
	pRightAxis->SetMinMax(0, 100);
	//pRightAxis->SetAutomatic(true);
	pRightAxis->GetLabel()->SetText(_T("Irradiance Evenness（%）"));
	pRightAxis->GetLabel()->SetColor(RGB(255, 255, 255));

	TChartString str1;
	CString title;
	title.Format(L"%s %s %s Irradiance CD Irradiance & Evenness", theApp.getString(_T("sample"), _T("model")), theApp.getString(_T("sample"), _T("sn")), theApp.getString(_T("sample"), _T("wavelength")));
	m_ChartCtrl.GetTitle()->AddString(TChartString(title));

	m_ChartCtrl.GetTitle()->SetColor(RGB(255, 255, 255));   //标题字体白色
	m_ChartCtrl.GetLeftAxis()->SetTextColor(RGB(255, 255, 255));  //左坐标轴白色
	m_ChartCtrl.GetBottomAxis()->SetTextColor(RGB(255, 255, 255));  //底部坐标轴白色
	//m_ChartCtrl.GetRightAxis()->GetGrid()->SetColor(RGB(0, 255, 255));
	m_ChartCtrl.GetRightAxis()->SetTextColor(RGB(255, 255, 255));  //底部坐标轴白色

	m_ChartCtrl.SetBorderColor(RGB(255, 255, 255));  //边框颜色白色
	m_ChartCtrl.SetBackColor(RGB(85, 85, 85));  //背景颜色深灰色

	pLineEven = m_ChartCtrl.CreateLineSerie(false, true);
	pBarLen = m_ChartCtrl.CreateBarSerie();

	COLORREF SerieColor = RGB(0, 255, 0);
	pLineEven->SetColor(SerieColor);//   pLineSeries要在头文件中初始化CChartLineSerie *pLineSeries
	pLineEven->SetName(L"Evenness");
	pLineEven->SetWidth(2);

	COLORREF SerieColor1 = RGB(0, 0, 255);
	pBarLen->SetColor(SerieColor1);
	pBarLen->SetName(L"Length");
	//pBarLen->SetBarWidth(20);
	//pBarLen->SetGroupId(1);

	m_ChartCtrl.GetLegend()->SetVisible(true);
	m_ChartCtrl.GetLegend()->DockLegend(CChartLegend::DockSide::dsDockTop);
	m_ChartCtrl.GetLegend()->SetHorizontalMode(true);

	m_ChartCtrl.SetZoomEnabled(false);
	

	/*for (int i = 0; i < 5; i++) {
		pBarIrra->AddPoint(i*10+1,i+10.0);
	}*/
}

void PageList2::initChart1() {
	pBottomAxis1 = m_ChartCtrl1.CreateStandardAxis(CChartCtrl::BottomAxis);
	//pBottomAxis1->SetMinMax(idriftx, idriftx+iheight);
	int m_led_width = theApp.getInt(_T("sample"), _T("width"));
	pBottomAxis1->SetMinMax(0, m_led_width);
	//pBottomAxis->SetAutomatic(true);
	pBottomAxis1->GetLabel()->SetText(_T("Position (mm)"));
	pBottomAxis1->GetLabel()->SetColor(RGB(255, 255, 255));



	pLeftAxis1 = m_ChartCtrl1.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pLeftAxis1->SetMinMax(0, 20);
//	pLeftAxis1->SetAutomatic(true);
	pLeftAxis1->GetLabel()->SetText(_T("Intensity (W/cm2)"));
	pLeftAxis1->GetLabel()->SetColor(RGB(255, 255, 255));

	CChartStandardAxis* pRightAxis = m_ChartCtrl1.CreateStandardAxis(CChartCtrl::RightAxis);
	pRightAxis->SetMinMax(50, 120);
	pRightAxis->GetLabel()->SetText(_T("Homogeneity (%)"));
	pRightAxis->GetLabel()->SetColor(RGB(255, 255, 255));

	TChartString str1;
	CString title;
	title.Format(L"%s & %s ", L"Irradiance", L"Homogeneity");
	m_ChartCtrl1.GetTitle()->AddString(TChartString(title));

	m_ChartCtrl1.GetTitle()->SetColor(RGB(255, 255, 255));   //标题字体白色
	m_ChartCtrl1.GetLeftAxis()->SetTextColor(RGB(255, 255, 255));  //左坐标轴白色
	m_ChartCtrl1.GetBottomAxis()->SetTextColor(RGB(255, 255, 255));  //底部坐标轴白色
	//m_ChartCtrl1.GetRightAxis()->GetGrid()->SetColor(RGB(0, 255, 255));
	m_ChartCtrl1.GetRightAxis()->SetTextColor(RGB(255, 255, 255));  //底部坐标轴白色

	m_ChartCtrl1.SetBorderColor(RGB(255, 255, 255));  //边框颜色白色
	m_ChartCtrl1.SetBackColor(RGB(85, 85, 85));  //背景颜色深灰色

	pLine1 = m_ChartCtrl1.CreateLineSerie();
	pLine2 = m_ChartCtrl1.CreateLineSerie(false,true);
	pLine3 = m_ChartCtrl1.CreateLineSerie(false, true);

	{
		for (int i = 0+1; i < imodnum+1; i++) {
			CChartLineSerie* pLine4 = m_ChartCtrl1.CreateLineSerie();
			COLORREF SerieColor = RGB(255, 255, 255);
			pLine4->SetColor(SerieColor);
			pLine4->AddPoint(i* igap1, 0);
			pLine4->AddPoint(i* igap1, 50);
		}
	}

	
	COLORREF SerieColor = RGB(0, 255, 0);
	pLine1->SetColor(SerieColor);//   pLineSeries要在头文件中初始化CChartLineSerie *pLineSeries
	pLine1->SetName(L"Irradiance");
	pLine1->SetWidth(2);


	COLORREF SerieColor1 = RGB(0, 0, 255);
	pLine2->SetColor(SerieColor1);
	pLine2->SetName(L"Homogeneity");
	pLine2->SetWidth(2);

	pLine3->SetColor(RGB(255, 0, 0));
	pLine3->AddPoint(-100, 90);
	pLine3->AddPoint(1000, 90);

	m_ChartCtrl1.GetLegend()->SetVisible(true);
	m_ChartCtrl1.GetLegend()->DockLegend(CChartLegend::DockSide::dsDockTop);
	m_ChartCtrl1.GetLegend()->SetHorizontalMode(true);

	m_ChartCtrl1.SetZoomEnabled(false);


}
void PageList2::initChart2() {

}
void PageList2::initListByModule()
{

	m_ListCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);

	m_ListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	// 假设 m_gridList 已经创建
//	m_ListCtrl.SetCellMargin(1.2);
	m_ListCtrl.EnableVisualStyles(true);
	m_ListCtrl.InsertHiddenLabelColumn();

	Json::Value list;
	pdlg->m_page_track.readTracklist(list);

	Json::Value arr = list["list"];
	for (int i = 0; i < arr.size(); i++) {
		int type = arr[i]["model"].asInt();
		if (type == 0) {
			iheight = arr[i]["iheight"].asInt();
			idriftx = arr[i]["idriftx"].asInt();
		//	idrifty = arr[i]["idrifty"].asInt();
			igap1 = arr[i]["igap1"].asInt();
			break;
		}
	}


	//int m_led_height = theApp.getInt(_T("sample"), _T("height"));
	int m_led_width = theApp.getInt(_T("sample"), _T("width"));
	//int halfLen = m_led_width / 2;
	//int halfGroup = halfLen / igap1;
	//int groupmod = halfLen % igap1;
	//if (groupmod != 0)halfGroup += 1;

	//igroup = halfGroup * 2 + 1;
	int modnum = theApp.getInt(_T("sample"), _T("modcount"));

	igroup =  modnum;
	igroup = iheight;
	imodnum = modnum;
	
	//添加列,0---序号，第二个参数显示内容，第三个参数显示对齐方式，第四个参数表头显示的宽度
	m_ListCtrl.InsertColumn(1, _T("Distance(MM)"), LVCFMT_CENTER, 120);

	//for (int i = igroup/2 *(-1); i <= igroup + 1; i++) 

	memset(XVal, 0, 100);
	for (int i = 0; i < 100; i++) {
		XVal[i] = 0;
	}
	
	for (int i = 2; i < modnum + 2; i++)
	{
		int idis;
		CString colTitle;
		colTitle.Format(L"Module % d", i-1);
		XVal[i] = (i-2) * (m_led_width / modnum)- idriftx;
		DebugLog::writeLogF("---Xval: i=%d val=%d",i, XVal[i]);
		m_ListCtrl.InsertColumn(i, colTitle, LVCFMT_CENTER, 90);
	}
	int rowidx = 0;
	{
		m_ListCtrl.InsertItem(m_ListCtrl.GetItemCount(), _T("行"));
		CString disstr;
		disstr.Format(L"%d", 0);
		m_ListCtrl.SetItemText(rowidx++, 1, disstr);
	}
	{
		m_ListCtrl.InsertItem(m_ListCtrl.GetItemCount(), _T("行"));
		CString disstr;
		disstr.Format(L"%d", 5);
		m_ListCtrl.SetItemText(rowidx++, 1, disstr);
	}
	for (int i = 10; i < 101; i += 10) {
		m_ListCtrl.InsertItem(m_ListCtrl.GetItemCount(), _T("行"));
		CString disstr;
		disstr.Format(L"%d", i);
		m_ListCtrl.SetItemText(rowidx++, 1, disstr);
	}

	return;
	//return;
	//插入行内容 尾部添加行
	srand(static_cast<unsigned int>(time(0)));
	for (int i = 0; i < 12; i++) {
		for (int j = 2; j < modnum + 2; j++) {
			float tmpval = theApp.RandomFloat1(0, 2);
			CString tmpstr;
			tmpstr.Format(L"%.2e", tmpval);
			m_ListCtrl.SetItemText(i, j, tmpstr);
			dataTab[i][j] = tmpval;
		}
	}

	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < iheight; j++) {
			dataTab[i][j+2] = theApp.RandomFloat1(0, 2);
		}
	}


	isDataready = true;

}

void PageList2::initList()
{
	
	m_ListCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);

	m_ListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	// 假设 m_gridList 已经创建
//	m_ListCtrl.SetCellMargin(1.2);
	m_ListCtrl.EnableVisualStyles(true);
	m_ListCtrl.InsertHiddenLabelColumn();

	Json::Value list;
	pdlg->m_page_track.readTracklist(list);

	Json::Value arr = list["list"];
	for (int i = 0; i < arr.size(); i++) {
		int type = arr[i]["model"].asInt();
		if (type == 0) {
			iheight = arr[i]["iheight"].asInt();
			idriftx = arr[i]["idriftx"].asInt();
			idriftx = arr[i]["idriftx"].asInt();
			igap1 = arr[i]["igap1"].asInt();
			break;
		}
	}


	//int m_led_height = theApp.getInt(_T("sample"), _T("height"));
	int m_led_width = theApp.getInt(_T("sample"), _T("width"));
	int halfLen = m_led_width / 2;
	int halfGroup = halfLen / igap1;
	int groupmod = halfLen % igap1;
	if (groupmod != 0)halfGroup += 1;

	igroup = halfGroup * 2 + 1;

	//m_ListCtrl.InsertHiddenLabelColumn();	// Requires one never uses column 0

	//添加列,0---序号，第二个参数显示内容，第三个参数显示对齐方式，第四个参数表头显示的宽度
	m_ListCtrl.InsertColumn(1, _T("Distance(MM)"), LVCFMT_CENTER, 100);

	//for (int i = igroup/2 *(-1); i <= igroup + 1; i++) 

	memset(XVal, 0, 200);
	int igap = halfGroup * (-1);
	for (int i = 2; i < igroup + 2; i++)
	{
		int idis;
		CString colTitle;
		colTitle.Format(L" % d", igap * igap1);
		XVal[i] = igap * igap1;
		m_ListCtrl.InsertColumn(i, colTitle, LVCFMT_CENTER, 90);
		igap++;
	}
	int rowidx = 0;
	{
		m_ListCtrl.InsertItem(m_ListCtrl.GetItemCount(), _T("行"));
		CString disstr;
		disstr.Format(L"%d", 0);
		m_ListCtrl.SetItemText(rowidx++, 1, disstr);
	}
	{
		m_ListCtrl.InsertItem(m_ListCtrl.GetItemCount(), _T("行"));
		CString disstr;
		disstr.Format(L"%d", 5);
		m_ListCtrl.SetItemText(rowidx++, 1, disstr);
	}
	for (int i = 10; i < 101; i += 10) {
		m_ListCtrl.InsertItem(m_ListCtrl.GetItemCount(), _T("行"));
		CString disstr;
		disstr.Format(L"%d", i);
		m_ListCtrl.SetItemText(rowidx++, 1, disstr);
	}

	return;
	//return;
	//插入行内容 尾部添加行
	srand(static_cast<unsigned int>(time(0)));
	for (int i = 0; i < 12; i++) {
		for (int j = 2; j < igroup + 2; j++) {
			float tmpval = theApp.RandomFloat1(0, 2);
			CString tmpstr;
			tmpstr.Format(L"%.2e", tmpval);
			m_ListCtrl.SetItemText(i, j, tmpstr);
			dataTab[i][j] = tmpval;
		}
	}

	isDataready = true;

}
void PageList2::resetList() {
	m_ListCtrl.DeleteAllItems();
	while (m_ListCtrl.DeleteColumn(0));
	//initList();
	initListByModule();

	CRect rect;
	this->GetClientRect(&rect);
	OnSize(0, rect.right, rect.bottom);
}

void PageList2::initList1()
{
	
	
	m_ListCtrl1.SetImageList(&m_ImageList1, LVSIL_SMALL);

	m_ListCtrl1.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	// 假设 m_gridList 已经创建
//	m_ListCtrl1.SetCellMargin(1.2);
	m_ListCtrl1.EnableVisualStyles(true);
	m_ListCtrl1.InsertHiddenLabelColumn();

	CString irraUnit = theApp.getString(_T("X20"), _T("irraunit"));
	CString titleTxt;

	//添加列,0---序号，第二个参数显示内容，第三个参数显示对齐方式，第四个参数表头显示的宽度
	m_ListCtrl1.InsertColumn(1, _T("Distance(MM)"), LVCFMT_CENTER, 140);

	titleTxt.Format(L"Average(%s)", irraUnit);
	m_ListCtrl1.InsertColumn(2, titleTxt, LVCFMT_CENTER, 140);
	titleTxt.Format(L"Max(%s)", irraUnit);
	m_ListCtrl1.InsertColumn(3, titleTxt, LVCFMT_CENTER, 120);
	m_ListCtrl1.InsertColumn(4, _T("Evenness(%)"), LVCFMT_CENTER, 120);
	//m_ListCtrl1.InsertColumn(5, _T("无效长度(MM)"), LVCFMT_CENTER, 120);
	{
		CGridColumnTrait* pTrait = new CGridColumnTraitEdit;
		m_ListCtrl1.InsertColumnTrait(5, _T("Left Margin(MM)"), LVCFMT_CENTER, 140, 5, pTrait);
	}
	{
		m_ListCtrl1.InsertColumn(6, _T("Effective Len(MM)"), LVCFMT_CENTER, 140);
		//CGridColumnTrait* pTrait = new CGridColumnTraitEdit;
		//m_ListCtrl1.InsertColumnTrait(6, _T("无效长度(MM)"), LVCFMT_CENTER, 100, 6, pTrait);
	}
	{
		CGridColumnTrait* pTrait = new CGridColumnTraitEdit;
		m_ListCtrl1.InsertColumnTrait(7, _T("Threshold"), LVCFMT_CENTER, 120, 5, pTrait);
	}
	
	//m_ListCtrl1.InsertColumn(6, _T("有效长度(MM)"), LVCFMT_CENTER, 120);

	int rowidx = 0;
	{
		m_ListCtrl1.InsertItem(m_ListCtrl.GetItemCount(), _T("行"));
		CString disstr;
		disstr.Format(L"%d", 0);
		m_ListCtrl1.SetItemText(rowidx++, 1, disstr);
	}
	{
		m_ListCtrl1.InsertItem(m_ListCtrl.GetItemCount(), _T("行"));
		CString disstr;
		disstr.Format(L"%d", 5);
		m_ListCtrl1.SetItemText(rowidx++, 1, disstr);
	}
	for (int i = 10; i < 101; i += 10) {
		m_ListCtrl1.InsertItem(m_ListCtrl1.GetItemCount(), _T("第一行"));
		CString disstr;
		disstr.Format(L"%d", i);
		m_ListCtrl1.SetItemText(rowidx++, 1, disstr);
	}

	return;
	//插入行内容 尾部添加行
	srand(static_cast<unsigned int>(time(0)));
	for (int i = 0; i < 12; i++) {
		for (int j = 2; j <= igroup + 1; j++) {
			m_ListCtrl1.SetItemText(i, j, theApp.RandomFloat());
		}
	}

}
void PageList2::onPause() {
	preTm = 0;
}
void PageList2::onNewLine() {
	curPoxI = 2;
	preTm = 0;
	for (int i = 0; i < 100; i++) {
		GroupMax[i] = 0.0;
	}
//	memset(dataTab[curRowIdx()],-1,1200);
	for (int j = 2; j < imodnum + 2; j++) {
		m_ListCtrl.SetItemText(curRowIdx(), j, L"");
	}
	for (int i = 0; i < 1200; i++) {
		dataTab[curRowIdx()][i] = -1.0;
	}

	DebugLog::writeLogF("CD:start one new line idx=%d", curRowIdx());
}
void PageList2::onIrraVal(float val, int unit, unsigned long tm) {

	if (pdlg->m_ctrl_mc.m_isMeasure == 1) {
		curIrra = val;
		if (curPoxI < 1199) {
			if (preTm == 0) {
				preTm = tm;
			}
			else {
				int dur = tm - preTm;
				preTm = tm;
				
				int distance =(int)getSpeedScale(pdlg->m_ctrl_mc.m_curSpeed)* dur / 1000;
				DebugLog::writeLogF("----CD:compute distance :dis=%d dur=%d speed=%d", distance, dur, pdlg->m_ctrl_mc.m_curSpeed);
				if (dur < 0 || distance < 0) {
					DebugLog::writeLogF("CD:compute distance err dur=%d dist=%d!", dur, distance);
				}
				if (distance < 1)distance = 1;
				curPoxI += distance;
				if (curPoxI >= 1199) {
					curPoxI = 1199;
					DebugLog::writeLogF("CD:compute distance err!");
				}
			}
			dataTab[curRowIdx()][curPoxI] = val;
			DebugLog::writeLogF("--- row %d : col %d :val=%.2e", curRowIdx(), curPoxI, val);
		}
		else {
			int fortest = 0;
		}
	}
}
void PageList2::onPosVal(float tx, float ty, string axis, float dist, int gidx, unsigned long tm){
	if (dist < 0) return;
	if (pdlg->m_ctrl_mc.m_isMeasure != 1)return;
	int x = tx + pdlg->m_page_track.getSrart('x');// +pdlg->m_page_track.m_led_driftx;
	int y = ty + pdlg->m_page_track.getSrart('y');// pdlg->m_page_track.m_led_drifty;
	for (int i = 2; i < imodnum + 2; i++) {
		int nextX = 1200;
		if (i + 1 < imodnum + 2) {
			nextX = XVal[i + 1];
		}
		
		if (x >=XVal[i] && x<= nextX) {
		//	DebugLog::writeLogF("cd--pos:idx =%d x=%d %.2f pre=%d next=%d irra=%.2e  irratm =%d postm=%d dur=%d", i, x, tx, XVal[i], nextX, curIrra, this->preTm, tm, tm - this->preTm);
			CString tmpstr;
			tmpstr.Format(L"%.2e", curIrra);
			if (GroupMax[i] < curIrra) {
				GroupMax[i] = curIrra;
				m_ListCtrl.SetItemText(curRowIdx(), i, tmpstr);
			}
	/*		if (x - XVal[i] < nextX - x) {

				if (setIdx == i) {
					if (x - XVal[i] < setDist) {
						if (dist > 0) {
							m_ListCtrl.SetItemText(curRowIdx(), i, tmpstr);
						}
						setDist = std::abs(x - XVal[i]);
					}
				}
				else {
					if (dist > 0) {
						m_ListCtrl.SetItemText(curRowIdx(), i, tmpstr);
					}
					setDist = std::abs(x - XVal[i]);
					setIdx = i;
				}
			}
			else {
				if (i + 1 < imodnum + 2) {
					if (dist > 0) {
						m_ListCtrl.SetItemText(curRowIdx(), i + 1, tmpstr);
					}
					setDist = std::abs(nextX - x);
					setIdx = i+1;
				}
			}
		*/
			break;
		}
	}
}
int PageList2::curRowIdx() {
	//int idx = 0;
	int curh = pdlg->m_ctrl_mc.m_height_list.GetCurSel();
	return curh;
}

int PageList2::compute1(int ydx) {

	pBarLen->ClearSerie();
	pLineEven->ClearSerie();

	if (m_formulas.GetCurSel() == 0) {
		int leftIn[13] = { 0.0 };
		int effeclen[13] = { 0.0 };
		Json::Value root;
		Json::Value arr;
		this->readWidthlist(root);
		arr = root["list"];

		for (int i = 0; i < arr.size(); i++) {
			int iht = arr[i]["measure_height"].asInt();
			leftIn[i] = arr[i]["margin"].asInt();
			effeclen[i] = pdlg->m_page_track.m_led_width - leftIn[i] * 2;
		}
		int retcount = 0;
		for (int i = 0; i < 12; i++) {
			float rowval = 0.0;
			float maxval = 0.0;
			float minval = 10000000.0;
			int rowcount = 0;
			for (int j = 2+ leftIn[i]- idriftx; j < 2 + leftIn[i]- idriftx + effeclen[i]; j++) {
				if (dataTab[i][j] < 0) continue;
				rowval += dataTab[i][j];
				if (maxval < dataTab[i][j])
					maxval = dataTab[i][j];
				if (minval > dataTab[i][j])
					minval = dataTab[i][j];

				rowcount++;
			}
			if (rowcount <= 0) return -1;
			RowMax[i] = maxval;
			RowAvg[i] = rowval / rowcount;
		//	RowEven[i] = (1 - (maxval - minval) / RowAvg[i]) * 100;

			float minEven = 10000.0;
			for (int j = 2 + leftIn[i] - idriftx; j < 2 + leftIn[i] - idriftx + effeclen[i]; j++) {
				if (dataTab[i][j] < 0) continue;
				float tmpval = (1 - std::abs(dataTab[i][j] - RowAvg[i]) / RowAvg[i]) * 100;

				if (minEven>tmpval) {
					minEven = tmpval;
				}
			}
			RowEven[i] = minEven;

			pLineEven->AddPoint(theApp.getHtByidx(i), RowEven[i]);
			pBarLen->AddPoint(theApp.getHtByidx(i), RowAvg[i]);

			int col = 2;
			m_ListCtrl1.SetItemText(i, col++, theApp.f2Lstr(RowAvg[i]));
			m_ListCtrl1.SetItemText(i, col++, theApp.f2Lstr(maxval));
			{
				CString tmpstr;
				tmpstr.Format(L"%.2f%%", RowEven[i]);
				m_ListCtrl1.SetItemText(i, col++, tmpstr);
			}


		}

		for (int i = 0; i < 12; i++) {
			for (int j = 2; j < igroup + 2; j++) {
				EvenTab[i][j] = (1 - std::abs(dataTab[i][j] - RowAvg[i]) / RowAvg[i]) * 100;
			}
		}

		return retcount;
	}
	else if (m_formulas.GetCurSel() == 1) {
		float minEven = 90.0;
		float input_val[13] = { 0.0 };
		Json::Value root;
		Json::Value arr;
		this->readWidthlist(root);
		arr = root["list"];

		for (int i = 0; i < arr.size(); i++) {
			int iht = arr[i]["measure_height"].asInt();
			input_val[i] = arr[i]["inputval"].asDouble();
		}
		int leftpos[13] = {0};
		int rightpos[13] = {0};
		for (int i = 0; i < 12; i++) {
			for (int j = 2; j < 2 + igroup; j++) {
				if (dataTab[i][j] > input_val[i]) {
					leftpos[i] = j; 
					/*if (j < std::abs(pdlg->m_page_track.idriftx)) {
						leftpos[i] = std::abs(pdlg->m_page_track.idriftx);
					}*/
					break;
				}
			}
			for (int j = igroup + 1; j > 2; j--) {
				if (dataTab[i][j] > input_val[i]) {
					rightpos[i] = j;
					/*if (j > pdlg->m_page_track.iheight- std::abs(pdlg->m_page_track.idriftx)- pdlg->m_page_track.m_led_width) {
						rightpos[i] = pdlg->m_page_track.iheight - std::abs(pdlg->m_page_track.idriftx) - pdlg->m_page_track.m_led_width;
					}*/
					break;
				}
			}

			{
				CString disstr;
				disstr.Format(L"%d", leftpos[i]);
				m_ListCtrl1.SetItemText(i, 5, disstr);
			}
			{
				CString disstr;
				disstr.Format(L"%d",std::abs( rightpos[i]-leftpos[i]));
				m_ListCtrl1.SetItemText(i, 6, disstr);
			}
		

		}

		for (int i = 0; i < 12; i++) {
			float rowval = 0.0;
			float maxval = 0.0;
			float minval = 10000000.0;
			int rowcount = 0;
			for (int j = leftpos[i]; j < std::abs(rightpos[i]); j++) {
				if (dataTab[i][j] < 0) continue;
				rowval += dataTab[i][j];
				if (maxval < dataTab[i][j])
					maxval = dataTab[i][j];
				if (minval > dataTab[i][j])
					minval = dataTab[i][j];

				rowcount++;
			}
			if (rowcount <= 0) return -1;
			RowMax[i] = maxval;
			RowAvg[i] = rowval / rowcount;
			RowEven[i] = (1 - (maxval - minval) / RowAvg[i]) * 100;

			pLineEven->AddPoint(theApp.getHtByidx(i), RowEven[i]);
			pBarLen->AddPoint(theApp.getHtByidx(i), RowAvg[i]);

			int col = 2;
			m_ListCtrl1.SetItemText(i, col++, theApp.f2Lstr(RowAvg[i]));
			m_ListCtrl1.SetItemText(i, col++, theApp.f2Lstr(maxval));
			{
				CString tmpstr;
				tmpstr.Format(L"%.2f%%", RowEven[i]);
				m_ListCtrl1.SetItemText(i, col++, tmpstr);
			}
		}

		for (int i = 0; i < 12; i++) {
			for (int j = 2; j < igroup + 2; j++) {
				EvenTab[i][j] = (1 - std::abs(dataTab[i][j] - RowAvg[i]) / RowAvg[i]) * 100;
			}
		}

	}

	return 0;
}

BOOL PageList2::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	pdlg= (CMeasureBotDlg*)AfxGetMainWnd();

	m_ImageList.Create(24, 24, ILC_COLOR16 | ILC_MASK, 3, 0);
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON2));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON2));

	m_ImageList1.Create(28, 28, ILC_COLOR16 | ILC_MASK, 3, 0);
	m_ImageList1.Add(AfxGetApp()->LoadIcon(IDI_ICON2));
	m_ImageList1.Add(AfxGetApp()->LoadIcon(IDI_ICON2));

	m_ListCtrl.SetCellMargin(1.2);
	m_ListCtrl1.SetCellMargin(1.2);

	GetDlgItem(IDC_EDIT1)->EnableWindow(false);
	GetDlgItem(IDC_EDIT1)->SetWindowTextW(L"20");

	for (int i = 0; i < 15;i++) {
		for (int j = 0; j < 1200; j++) {
			dataTab[i][j] = -1.0;
			EvenTab[i][j] = -1.0;
		}
	}

	for (int i = 0; i < 100; i++) {
		GroupMax[i] = 0.0;
	}
//	initList();
	initListByModule();
	initList1();
	initChart();
	initChart1();

	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
	scrollinfo.nPage = 40; //设置滑块大小
	scrollinfo.nMax = 200; //设置滚动条的最大位置0–100
	SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);

	m_formulas.InsertString(0, _T("Specify width"));
	m_formulas.InsertString(1, _T("Specify min value"));
	m_formulas.SetWindowTextW(_T("Select style"));
	//m_formulas.SetCurSel(0);
	isInited = true;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void PageList2::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (isInited) {
		CRect rect;
		this->GetClientRect(&rect);
		rect.top += 50;
		rect.bottom = rect.top + 360;
		rect.left += 5;
		rect.right -= 10;// rect.left + imodnum * 90 + 120 + 20;
		m_ListCtrl.MoveWindow(&rect);

	/*	CRect rect0;
		rect0.top = rect.bottom + 5;
		rect0.bottom = rect0.top + 30;
		rect0.left = rect.left;
		rect0.right = rect.left +900;
		GetDlgItem(IDC_EDIT2)->MoveWindow(&rect0);*/


		CRect rect1;
		rect1.top = rect.bottom + 5;
		rect1.bottom = rect1.top + 420;
		rect1.left = rect.left;
		rect1.right = rect1.left + 900;
		m_ChartCtrl1.MoveWindow(&rect1);

		CRect rect2;
		rect2.top = rect1.bottom+5;
		rect2.bottom = rect2.top+420;
		rect2.left = rect.left;
		rect2.right = rect.left + 1050;
		m_ListCtrl1.MoveWindow(&rect2);

		CRect rect3;
		rect3.top = rect2.bottom+5;
		rect3.bottom = rect3.top + 420;
		rect3.left = rect.left;
		rect3.right = rect.left+800;
		m_ChartCtrl.MoveWindow(&rect3);

	}

	// TODO: 在此处添加消息处理程序代码
}


void PageList2::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (!isComputed)
		return;
	POSITION ps;
	int nIndex;
	ps = m_ListCtrl.GetFirstSelectedItemPosition();
	nIndex = m_ListCtrl.GetNextSelectedItem(ps);
	if (nIndex < 0) return;

	TChartString str1;
	CString title;
	title.Format(L"%s & %s (%dMM)", L"Irradiance", L"Homogeneity", theApp.getHtByidx(nIndex));
	m_ChartCtrl1.GetTitle()->RemoveAll();
	m_ChartCtrl1.GetTitle()->AddString(TChartString(title));

	pLine1->ClearSerie();
	pLine2->ClearSerie();

	//float halfMod = pdlg->m_page_track.m_mod_width / 2;
	CString text = m_ListCtrl1.GetItemText(nIndex, 5);
	int axstart = _ttoi(text);
	CString text1 = m_ListCtrl1.GetItemText(nIndex, 6);
	int effw = _ttoi(text1);
	//pBottomAxis1->SetMinMax(axstart, axstart+ std::abs(effw));
	pLeftAxis1->SetMinMax(0,3+ RowMax[nIndex]);
	

	int leftIn[13] = { 0.0 };
	int effeclen[13] = { 0.0 };
	Json::Value root;
	Json::Value arr;
	this->readWidthlist(root);
	arr = root["list"];

	for (int i = 0; i < arr.size(); i++) {
		int iht = arr[i]["measure_height"].asInt();
		leftIn[i] = arr[i]["margin"].asInt();
		effeclen[i] = pdlg->m_page_track.m_led_width - leftIn[i] * 2;
	}
	int count = 0;
	for (int i = 0- idriftx; i < igroup+ idriftx; i++) {
		if ((dataTab[nIndex][i + 2] > 0)) {
			TmpVal[count] = dataTab[nIndex][i + 2];
			AxisX[count] = i+ idriftx;
			TmpEven[count] = EvenTab[nIndex][i + 2];
			count++;
		}
		
	}
	/*pLine1->AddPoints(AxisX, (double*)&(dataTab[nIndex][2]),igroup);
	pLine2->AddPoints((double*)AxisX, (double*)&(EvenTab[nIndex][2]), igroup);*/
	pLine1->AddPoints(AxisX, (double*)TmpVal, count);
	pLine2->AddPoints((double*)AxisX, (double*)TmpEven, count);

	{
		int curMargin = leftIn[nIndex];
		int stIdx, edIdx;
		for (int i = 0; i < count; i++) {
			if (AxisX[i] > curMargin) {
				stIdx = i;
				break;
			}
		}
		for (int i = count - 1; i > 0; i--) {
			if (AxisX[i] < pdlg->m_page_track.m_led_width -curMargin) {
				edIdx = i;
				break;
			}
		}
		if (stIdx >= edIdx) {
			::MessageBox(NULL, _T("数据计算失败2_3"), _T("compute！"), MB_OK);
			return;
		}
		float tpMin = totalMinIrra * 1.0;
		int accLen = 0;
		int stpt = 0;
		int inCount = 0;
		float preVal = TmpEven[stIdx];
		if (TmpEven[stIdx] < tpMin) {
			inCount = 1;
			stpt = AxisX[stIdx];
		}

		for (int i = stIdx; i < edIdx; i++) {
			if (preVal <= tpMin && TmpEven[i] > tpMin) {
				inCount = 0;
				accLen += AxisX[i] - stpt;
			}
			if ((preVal > tpMin && TmpEven[i] <= tpMin)) {
				inCount = 1;
				stpt = AxisX[i];
			}
			preVal = TmpEven[i];
		}

		if (inCount == 1 && AxisX[edIdx - 1] > stpt) {
			accLen += (AxisX[edIdx -1] - stpt);
		}

		CString tips;
		tips.Format(L"Accumulated width:%d mm (<%d%%)", accLen, totalMinIrra);
		m_ChartCtrl1.GetTitle()->AddString(TChartString(tips));
	}

	if(0){
		int totalcount = 0;
		int count = 0;
		for (int j = 0- idriftx; j < igroup + idriftx; j++) {

			if (EvenTab[nIndex][j] < 0)
				continue;
			totalcount++;
			if (EvenTab[nIndex][j] < 90) {
				count++;
			}
		}
		if (totalcount <= 0) {
			::MessageBox(NULL, _T("数据计算失败"), _T("compute！"), MB_OK);
			return;
		}
		count = count * effeclen[nIndex] / totalcount;
		CString tips;
		tips.Format(L"Accumulated width:%d mm (<%d%%)", count, totalMinIrra);
		m_ChartCtrl1.GetTitle()->AddString(TChartString(tips));
		//GetDlgItem(IDC_EDIT2)->SetWindowTextW(tips);
	}
	//for (int j = 2; j < igroup+2 ; j++) {
	//	/*pLine1->AddPoint((j-2) * igap1 , dataTab[nIndex][j]);
	//	pLine2->AddPoint((j-2) * igap1 , EvenTab[nIndex][j]);*/
	//	pLine1->AddPoint((j - 2) , dataTab[nIndex][j]);
	//	pLine2->AddPoint((j - 2) , EvenTab[nIndex][j]);
	//}
	if(pResult !=NULL)
	*pResult = 0;
}


void PageList2::OnHdnItemdblclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	POSITION ps;
	int nIndex;
	ps = m_ListCtrl.GetFirstSelectedItemPosition();
	nIndex = m_ListCtrl.GetNextSelectedItem(ps);
	*pResult = 0;
}


void PageList2::OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	POSITION ps;
	int nIndex;
	ps = m_ListCtrl.GetFirstSelectedItemPosition();
	nIndex = m_ListCtrl.GetNextSelectedItem(ps);
	*pResult = 0;
}


void PageList2::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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


void PageList2::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void PageList2::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_formulas.GetCurSel() == 0) {
		m_ListCtrl1.page2_style = 0;
		GetDlgItem(IDC_EDIT1)->EnableWindow(false);
		Json::Value root;
		Json::Value arr;
		this->readWidthlist(root);
		arr = root["list"];

		for (int i = 0; i < arr.size(); i++) {
			int iht = arr[i]["measure_height"].asInt();
			int margin = arr[i]["margin"].asInt();
			int eff_width = pdlg->m_page_track.m_led_width - margin * 2;

			CString tmpstr;
			tmpstr.Format(L"%d", margin);
			m_ListCtrl1.SetItemText(i, 5, tmpstr);

			tmpstr.Format(L"%d", eff_width);
			m_ListCtrl1.SetItemText(i, 6, tmpstr);

			tmpstr.Format(L"%.2f", 0.0);
			m_ListCtrl1.SetItemText(i, 7, tmpstr);

		}
	}
	else {
		m_ListCtrl1.page2_style = 1;

		GetDlgItem(IDC_EDIT1)->EnableWindow(true);
		Json::Value root;
		Json::Value arr;
		this->readWidthlist(root);
		arr = root["list"];

		for (int i = 0; i < arr.size(); i++) {
			int iht = arr[i]["measure_height"].asInt();
			int margin = 0;// arr[i]["margin"].asInt();
			int eff_width = pdlg->m_page_track.m_led_width - margin * 2;
			float inputval = arr[i]["inputval"].asDouble();

			CString tmpstr;
			tmpstr.Format(L"%d", margin);
			m_ListCtrl1.SetItemText(i, 5, tmpstr);

			tmpstr.Format(L"%d", eff_width);
			m_ListCtrl1.SetItemText(i, 6, tmpstr);

			tmpstr.Format(L"%.2e", inputval);
			m_ListCtrl1.SetItemText(i, 7, tmpstr);

		}
	}

	return;
	
}




void PageList2::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	string report = theApp.tostr(theApp.getString(_T("File"), _T("report_path")));
	report += theApp.makeFname("\\CD");
	DeleteFile(CString(report.c_str()));
	if (false == listExport1(report)) {
		::MessageBox(NULL, _T("导出报表失败"), _T("导出失败！"), MB_OK);
		return;
	};
	if(false==insertImg(report)) {
		::MessageBox(NULL, _T("导出图表失败"), _T("导出失败！"), MB_OK);
		return;
	};
	powerExport(report);
	tempExport(report);
	rawExport(report);
	::MessageBox(NULL, CString(report.c_str()), _T("导出成功！"), MB_OK);
}

bool PageList2::insertImg(string fname) {
	libxl::Book* book = xlCreateXMLBook();
	book->load(CString(fname.c_str()));
	libxl::Sheet* sheet = book->getSheet(0);
	if (sheet == NULL) return false;
	int tabpos = 0;
	for (int32_t row = sheet->firstRow(); row < sheet->lastRow(); ++row)
	{
		CellType cell_type = sheet->cellType(row, 0);
		if (cell_type == CELLTYPE_STRING)
		{
			wstring ret = sheet->readStr(row, 0);
			if (ret == L"Data Chart") {
				tabpos = row;
			}
		}
	}

	tabpos += 1;

	string jpgfile = "tmp456.jpg";
	CString fname1(jpgfile.c_str());
	CRect rect;
	m_ChartCtrl.GetClientRect(&rect);
	m_ChartCtrl.SaveAsImage(TChartString(fname1), rect, 24);
	int id = book->addPicture(fname1);
	sheet->setPicture(tabpos, 0, id);
	DeleteFile(fname1);


	int leftIn[13] = { 0.0 };
	int effeclen[13] = { 0.0 };
	Json::Value root;
	Json::Value arr;
	this->readWidthlist(root);
	arr = root["list"];

	for (int i = 0; i < arr.size(); i++) {
		int iht = arr[i]["measure_height"].asInt();
		leftIn[i] = arr[i]["margin"].asInt();
		effeclen[i] = pdlg->m_page_track.m_led_width - leftIn[i] * 2;
	}

	tabpos += 26;
	for (int i = 0; i < 12; i++) {

		int nIndex = i;
		TChartString str1;
		CString title;
		title.Format(L"%s & %s (%dMM)", L"Irradiance", L"Homogeneity", theApp.getHtByidx(i));
		m_ChartCtrl1.GetTitle()->RemoveAll();
		m_ChartCtrl1.GetTitle()->AddString(TChartString(title));

		if(0){
			int totalcount = 0;
			int count = 0;
			for (int j = 0 - idriftx; j < igroup + idriftx; j++) {

				if (EvenTab[nIndex][j] < 0)
					continue;
				totalcount++;
				if (EvenTab[nIndex][j] < 90) {
					count++;
				}
			}
			if (totalcount <= 0) {
				::MessageBox(NULL, _T("数据计算失败2"), _T("compute！"), MB_OK);
				return false;
			};
			count = count * pdlg->m_page_track.m_led_width / totalcount;
			CString tips;
			tips.Format(L"Accumulated width:%d mm (<%d%%)", count, totalMinIrra);
			m_ChartCtrl1.GetTitle()->AddString(TChartString(tips));
			//GetDlgItem(IDC_EDIT2)->SetWindowTextW(tips);
		}

		pLine1->ClearSerie();
		pLine2->ClearSerie();


		CString text = m_ListCtrl1.GetItemText(nIndex, 5);
		int axstart = _ttoi(text);
		CString text1 = m_ListCtrl1.GetItemText(nIndex, 6);
		int effw = _ttoi(text1);
		//pBottomAxis1->SetMinMax(axstart, axstart + effw);



		/*for (int i = 0; i < igroup; i++) {
			AxisX[i] = i;
		}
		pLine1->AddPoints(AxisX, (double*)&(dataTab[nIndex][2]), igroup);
		pLine2->AddPoints((double*)AxisX, (double*)&(EvenTab[nIndex][2]), igroup);*/
		int count = 0;
		for (int i = 0 - idriftx; i < igroup + idriftx; i++) {
			if ((dataTab[nIndex][i + 2] > 0)) {
				TmpVal[count] = dataTab[nIndex][i + 2];
				AxisX[count] = i + idriftx;
				TmpEven[count] = EvenTab[nIndex][i + 2];
				count++;
			}

		}
		/*pLine1->AddPoints(AxisX, (double*)&(dataTab[nIndex][2]),igroup);
		pLine2->AddPoints((double*)AxisX, (double*)&(EvenTab[nIndex][2]), igroup);*/
		pLine1->AddPoints(AxisX, (double*)TmpVal, count);
		pLine2->AddPoints((double*)AxisX, (double*)TmpEven, count);

		{
			int curMargin = leftIn[nIndex];
			int stIdx, edIdx;
			for (int i = 0; i < count; i++) {
				if (AxisX[i] > curMargin) {
					stIdx = i;
					break;
				}
			}
			for (int i = count - 1; i > 0; i--) {
				if (AxisX[i] < pdlg->m_page_track.m_led_width - curMargin) {
					edIdx = i;
					break;
				}
			}
			if (stIdx >= edIdx) {
				::MessageBox(NULL, _T("数据计算失败2_2"), _T("compute！"), MB_OK);
				return false;
			}
			float tpMin = totalMinIrra * 1.0;
			int accLen = 0;
			int stpt = 0;
			int inCount = 0;
			float preVal = TmpEven[stIdx];
			if (TmpEven[stIdx] < tpMin) {
				inCount = 1;
				stpt = AxisX[stIdx];
			}

			for (int i = stIdx; i < edIdx; i++) {
				if (preVal <= tpMin && TmpEven[i] > tpMin) {
					inCount = 0;
					accLen += AxisX[i] - stpt;
				}
				if ((preVal > tpMin && TmpEven[i] <= tpMin)) {
					inCount = 1;
					stpt = AxisX[i];
				}
				preVal = TmpEven[i];
			}

			if (inCount == 1 && AxisX[edIdx - 1] > stpt) {
				accLen += (AxisX[edIdx - 1] - stpt);
			}

			CString tips;
			tips.Format(L"Accumulated width:%d mm (<%d%%)", accLen, totalMinIrra);
			m_ChartCtrl1.GetTitle()->AddString(TChartString(tips));
		}


		string jpgfile = "tmp789.jpg";
		CString fname1(jpgfile.c_str());
		CRect rect;
		m_ChartCtrl1.GetClientRect(&rect);
		m_ChartCtrl1.SaveAsImage(TChartString(fname1), rect, 24);
		int id = book->addPicture(fname1);
		sheet->setPicture(tabpos+i*25, 0, id);
		DeleteFile(fname1);
	}

	book->save(CString(fname.c_str()));

	return true;
}
bool PageList2::listExport1(string outfname) {
	int count = m_ListCtrl1.GetItemCount(); // 获取列表中的项目数
	if (count <= 0) return false;

	string tplate = theApp.tostr(theApp.getString(_T("File"), _T("template_path")));
	tplate += "/Track_CD.xlsx";
	CString fname(tplate.c_str());
	libxl::Book* book = xlCreateXMLBook();	// 与2003相比，仅仅这里有点区别而已
	if (nullptr == book)
	{
		DebugLog::writeLogF("create libxl book failed");
		return false;
	}

	if (false == book->load(fname))
	{
		DebugLog::writeLogF(" libxl open template failed");
		return false;
	}
	Sheet* sheet = book->getSheet(0);
	if (sheet == NULL) return false;
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
		CString tempstr;
		tempstr.Format(L"%.1f",pdlg->m_ctrl_stat.curTemp.temp2);
		sheet->writeStr(1, 1, tempstr);
	}
	{
		CString tempstr;
		pdlg->m_ctrl_mc.GetDlgItem(IDC_EDIT5)->GetWindowText(tempstr);
		sheet->writeStr(5, 1, tempstr);
	}
	{
		std::vector<RealStat_t>& powList = pdlg->m_ctrl_stat.powList;
		float maxCur = 0.0;
		for (int i = 0; i < powList.size(); i++) {
			for (int j = 0; j < 16 * 3; j += 3) {
				if (maxCur < powList[i].chn[j / 3].cur) {
					maxCur = powList[i].chn[j / 3].cur;
				}
			}
		}

		CString tempstr;
		tempstr.Format(L"%.1f", maxCur);
		sheet->writeStr(6, 1, tempstr);
	}
	{
		CString tempstr = theApp.getString(_T("sample"), _T("model"));
		sheet->writeStr(3, 1, tempstr);
	}
	{
		CString tempstr = theApp.getString(_T("sample"), _T("sn"));
		sheet->writeStr(4, 1, tempstr);
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

	CString irraUnit = theApp.getString(_T("X20"), _T("irraunit"));
	{
		CString titleTxt;
		titleTxt.Format(L"Average Irradiance (%s)", irraUnit);
		sheet->writeStr(tabpos + 2, 1, titleTxt);
	}
	{
		CString titleTxt;
		titleTxt.Format(L"Max Irradiance (%s)", irraUnit);
		sheet->writeStr(tabpos + 2, 2, titleTxt);
	}

	tabpos += 3;

	for (int i = 0; i < 12; i++)
	{

		for (int j = 1; j < 5; j++) {
			CString text;
			text = m_ListCtrl1.GetItemText(i, j + 1);
			if (j == 3) {
				sheet->writeStr(tabpos + i, j, text);
			}
			else {
				string valstr = "--";
				if (!text.IsEmpty()) {
					valstr = theApp.tostr(text);
				}				
				sheet->writeNum(tabpos + i, j, std::stof(valstr));
			}
		}
	}

	book->save(CString(outfname.c_str()));

	return true;
}
void PageList2::powerExport_bak(string outfname) {
	CString fname(outfname.c_str());
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
	if (sheet == NULL) return;
	int tabpos = 0;
	for (int32_t row = sheet->firstRow(); row < sheet->lastRow(); ++row)
	{
		CellType cell_type = sheet->cellType(row, 6);
		if (cell_type == CELLTYPE_STRING)
		{
			wstring ret = sheet->readStr(row, 6);
			if (ret == L"Test Data") {
				tabpos = row;
			}
		}
	}

	tabpos += 4;

	for (int i = 0; i < 10; i++)
	{

		for (int j = 7; j < 15; j++) {

			sheet->writeNum(tabpos + i, j, 10.01);

		}
	}

	book->save(CString(outfname.c_str()));

	return;
}
void PageList2::rawExport(string outfname) {
	CString fname(outfname.c_str());
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
	//Sheet* sheet = book->addSheet(L"raw data");
	Sheet* sheet = book->getSheet(1);
	if (sheet == NULL) return;
	for (int i = 2; i < 2 + igroup; i++) {
		sheet->writeNum(i - 1, 0, i-1);
		for (int j = 0; j < 12; j++) {
			if (dataTab[j][i] > 0) {
				sheet->writeNum(i - 1, j + 1, dataTab[j][i]);
			}
		}
	}

	book->save(CString(outfname.c_str()));
}
void PageList2::powerExport(string outfname) {

	CString fname(outfname.c_str());
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
	Sheet* sheet = book->getSheet(2);
	if (sheet == NULL) return;
	int tabpos = 0;

	//for (int j = 1; j < 16; j++) {

	//	for (int i = 1; i < 4; i++)
	//	{
	//		sheet->copyCell(0, 1, 0, j * 3 + i);
	//		CString header;
	//		header.Format(L"ch%d cur", j + 1);
	//		sheet->writeStr(0, j * 3 + i, header);
	//	}
	//}

	std::vector<RealStat_t>& powList = pdlg->m_ctrl_stat.powList;
	for (int i = 0; i < powList.size(); i++) {
		sheet->writeStr(i + 2, 0, theApp.tm2CStr(powList[i].tm));

		for (int j = 0; j < 16 * 3; j += 3) {
			sheet->writeNum(i + 2, j + 1, powList[i].chn[j / 3].cur);
			sheet->writeNum(i + 2, j + 2, powList[i].chn[j / 3].pow);
			sheet->writeNum(i + 2, j + 3, powList[i].chn[j / 3].ntc);

		}
	}

	book->save(CString(outfname.c_str()));

	return;
}

void PageList2::tempExport(string outfname) {

	CString fname(outfname.c_str());
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
	Sheet* sheet = book->getSheet(3);
	if (sheet == NULL) return;
	int tabpos = 0;


	std::vector<Smacq_stat_t>& tmpFlowList = pdlg->m_ctrl_stat.tmpFlowList;

	for (int i = 0; i < tmpFlowList.size(); i++) {
		sheet->writeStr(i + 1, 0, theApp.tm2CStr(tmpFlowList[i].tm));
		sheet->writeNum(i + 1, 1, tmpFlowList[i].tempin);
		sheet->writeNum(i + 1, 2, tmpFlowList[i].tempout);
		sheet->writeNum(i + 1, 3, tmpFlowList[i].flow);
		sheet->writeNum(i + 1, 4, tmpFlowList[i].temp1);
		sheet->writeNum(i + 1, 5, tmpFlowList[i].temp2);
	}

	book->save(CString(outfname.c_str()));

	return;
}


void PageList2::OnLvnEndlabeleditList2(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int row = pDispInfo->item.iItem;
	int col = pDispInfo->item.iSubItem;

	CString curtxt = pDispInfo->item.pszText;

	if (col == 5) {
		if (m_formulas.GetCurSel() != 0) return;
		int curmargin = _ttoi(curtxt);
		int usewidth = pdlg->m_page_track.m_led_width - curmargin * 2;
		this->writeWidthlist(theApp.getHtByidx(row), curmargin, usewidth, -1);
		CString widtxt;
		widtxt.Format(L"%d", usewidth);
		m_ListCtrl1.SetItemText(row, col + 1, widtxt);
	}
	else if (col == 7) {
		if (m_formulas.GetCurSel() != 1) return;
		float inputval = _ttof(curtxt);
		this->writeWidthlist(theApp.getHtByidx(row), -1, -1, inputval);
	}

//	m_ListCtrl1.SetItemText(3, 1, L"rrrrr");
	//POSITION ps;
	//int nIndex;
	//ps = m_ListCtrl1.GetFirstSelectedItemPosition();
	//nIndex = m_ListCtrl1.GetNextSelectedItem(ps);
	*pResult = 0;
}


void PageList2::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	return;
	//CDialogEx::OnOK();
}


void PageList2::OnBnClickedCompute()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_formulas.GetCurSel() < 0)
		return;
	if (!isDataready) return;
	if (compute1(0) < 0) {
		::MessageBox(NULL, _T("数据计算失败1"), _T("compute！"), MB_OK);
		return;
	}
	isComputed = true;
	totalMinIrra = theApp.getInt(_T("compute"), _T("minirra"));

	//m_ListCtrl.SetSelectionMark(0);
	m_ListCtrl.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
//	OnLvnItemchangedList1(NULL,NULL);
}

void PageList2::writeWidthlist(int height,int margin,int usewidth,float inputval)
{
	char path[512];
	getcwd(path, 511);
	string confPath = path;
	confPath += "/WidthList.json";

	int flen = 0;
	char buf[10240] = { 0 };

	FILE* pFile = fopen(confPath.c_str(), "r");
	if (pFile != NULL)
	{
		flen = fread(buf, sizeof(char), 10240, pFile);
		fclose(pFile);
	}


	if (flen > 0)
	{
		Json::Reader reader;
		Json::Value jmessage;
		if (reader.parse(buf, jmessage))
		{

			bool found = false;
			Json::Value arr = jmessage["list"];
			int total = arr.size();

			for (int i = 0; i < total; i++)
			{
				int measure_height = arr[i]["measure_height"].asInt();
				if (measure_height == height) {
					if (margin >= 0 && usewidth >= 0) {
						arr[i]["margin"] = margin;
						arr[i]["effective_width"] = usewidth;
					}
					else if (inputval >= 0) {
						arr[i]["inputval"] = inputval;
					}
				}

			}
			Json::Value root;
			root["list"] = arr;
			Json::FastWriter writer;
			string configStr = writer.write(root);
			pFile = fopen(confPath.c_str(), "w");
			fwrite(configStr.c_str(), 1, configStr.length(), pFile);
			fclose(pFile);

		}

		
	}

	return;

}

void PageList2::readWidthlist(Json::Value& list)
{
	char path[512];
	getcwd(path, 511);
	string confPath = path;
	confPath += "/WidthList.json";

	char buf[10240] = { 0 };
	int res = 0;

	FILE* pFile = fopen(confPath.c_str(), "r");
	if (!pFile)
	{
		return;
	}
	int flen = fread(buf, sizeof(char), 10240, pFile);
	fclose(pFile);

	if (flen > 0)
	{
		Json::Reader reader;
		Json::Value jmessage;
		if (reader.parse(buf, jmessage))
		{
		/*	Json::Value arr = jmessage["list"];
			int total = arr.size();
			for (int i = 0; i < total; i++)
			{
				Json::Value contitem = arr[i];
			}*/

			list = jmessage;
		}

	}

}
