// PageChart.cpp: 实现文件
//

#include "pch.h"
#include "MeasureBot.h"
#include "PageChart.h"
#include "afxdialogex.h"

#include "ChartCtrl\ChartMouseListener.h"  //鼠标响应头文件
#include  "ChartCtrl\ChartAxisLabel.h"  //鼠标响应头文件
// PageChart 对话框

IMPLEMENT_DYNAMIC(PageChart, CDialogEx)

PageChart::PageChart(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOGChart, pParent)
{

}

PageChart::~PageChart()
{
}

void PageChart::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM1, m_ChartCtrl);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(PageChart, CDialogEx)
	ON_WM_SIZE()
	ON_MESSAGE(WM_MYMESSAGE_PAGE_CHART, &PageChart::OnMyMessageSignal)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// PageChart 消息处理程序


BOOL PageChart::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	pdlg = (CMeasureBotDlg*)AfxGetMainWnd();


	pBottomAxis =m_ChartCtrl.CreateStandardAxis(CChartCtrl::BottomAxis);
	pBottomAxis->SetMinMax(0, 60);
	//pBottomAxis->SetAutomatic(true);
	pBottomAxis->GetLabel()->SetText(_T("时间（秒）"));
	pBottomAxis->GetLabel()->SetColor(RGB(255, 255, 255));



	pLeftAxis =m_ChartCtrl.CreateStandardAxis(CChartCtrl::LeftAxis);
	pLeftAxis->SetMinMax(0, 30);
	pLeftAxis->GetLabel()->SetText(_T("光强（mW/cm2）"));
	pLeftAxis->GetLabel()->SetColor(RGB(255, 255, 255));

	//CChartStandardAxis* pTopAxis =
	//	m_ChartCtrl.CreateStandardAxis(CChartCtrl::TopAxis);
	//pTopAxis->SetMinMax(0, 10);
	CChartStandardAxis* pRightAxis =m_ChartCtrl.CreateStandardAxis(CChartCtrl::RightAxis);
	pRightAxis->SetMinMax(0, 15);
	pRightAxis->GetLabel()->SetText(_T("电流（mA）"));
	pRightAxis->GetLabel()->SetColor(RGB(255, 255, 255));


	/*TChartString str1;
	str1 = _T("实时测量曲线");
	m_ChartCtrl.GetTitle()->AddString(str1);*/

	m_ChartCtrl.GetTitle()->SetColor(RGB(255, 255, 255));   //标题字体白色
	m_ChartCtrl.GetLeftAxis()->SetTextColor(RGB(255, 255, 255));  //左坐标轴白色
	m_ChartCtrl.GetBottomAxis()->SetTextColor(RGB(255, 255, 255));  //底部坐标轴白色
	m_ChartCtrl.GetRightAxis()->SetTextColor(RGB(255, 255, 255));  //底部坐标轴白色
	//m_ChartCtrl.GetRightAxis()->GetGrid()->SetColor(RGB(0, 255, 255));

	m_ChartCtrl.SetBorderColor(RGB(255, 255, 255));  //边框颜色白色
	m_ChartCtrl.SetBackColor(RGB(85, 85, 85));  //背景颜色深灰色


	m_ChartCtrl.SetZoomEnabled(true);
//	m_ChartCtrl.RemoveAllSeries();//先清空
	pLineIrra = m_ChartCtrl.CreateLineSerie(false,true);
	pLineCur = m_ChartCtrl.CreateLineSerie();

	pLineTemp = m_ChartCtrl.CreateLineSerie();

	for (size_t i = 0; i < 2048; ++i)
	{
		x_secs[i] = i;
	}
	x_count = 0;

	COLORREF SerieColor = RGB(0, 255, 0);
	pLineIrra->SetColor(SerieColor);//   pLineSeries要在头文件中初始化CChartLineSerie *pLineSeries
	pLineIrra->SetWidth(2);
	pLineIrra->SetName(L"光强");
	
	

	COLORREF SerieColor1 = RGB(0, 0,255);
//	pLineCur->SetSmooth(true);
	pLineCur->SetName(L"电流");
	pLineCur->SetColor(SerieColor1);
	pLineCur->SetWidth(2);
	
	//m_ChartCtrl.GetLegend()->SetTransparent(true);
	m_ChartCtrl.GetLegend()->SetVisible(true);
	m_ChartCtrl.GetLegend()->DockLegend(CChartLegend::DockSide::dsDockTop);
	m_ChartCtrl.GetLegend()->SetHorizontalMode(true);


//	initList();
	isInited = true;

	SetTimer(1, 1000, NULL);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void PageChart::OnLeftAxisRadio()
{
	CChartAxis* pAxis = m_ChartCtrl.GetLeftAxis();
	//TChartString AxisLabel = pAxis->GetLabel()->GetText();
	double Min = 0, Max = 0;
	CString strBuff;
	pAxis->GetMinMax(Min, Max);

}


void PageChart::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (isInited) {
		CRect rect;
		this->GetClientRect(&rect);
	/*	rect.top += 60;
		rect.bottom = rect.top + 250;
		rect.left += 5;
		rect.right = rect.left + 80 * igroup + 100 + 10;
		m_ListCtrl.MoveWindow(&rect);*/

		CRect rect1;
		rect1.top = rect.top+10;
		rect1.bottom = rect.bottom - 10;
		rect1.left = rect.left+10;
		rect1.right = rect.right - 100;
		m_ChartCtrl.MoveWindow(&rect1);
	}
	// TODO: 在此处添加消息处理程序代码
}
LRESULT PageChart::OnMyMessageSignal(WPARAM wParam, LPARAM data) {
	int msg_id = wParam;
	std::string* pMsg = reinterpret_cast<std::string*>(data);
	return 0;
}


void PageChart::initList() {

	DWORD  style = m_ListCtrl.GetExtendedStyle();
	style |= LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;// | LVS_EX_CHECKBOXES;
	m_ListCtrl.SetExtendedStyle(style);

	Json::Value list;
	pdlg->m_page_track.readTracklist(list);

	Json::Value arr = list["list"];
	for (int i = 0; i < arr.size(); i++) {
		int type = arr[i]["model"].asInt();
		if (type == 2) {
			igroup = arr[i]["igroup"].asInt();
			break;
		}
	}

	if (igroup % 2 == 0)igroup += 1;
	//	m_ListCrl.InsertHiddenLabelColumn();	// Requires one never uses column 0

	//添加列,0---序号，第二个参数显示内容，第三个参数显示对齐方式，第四个参数表头显示的宽度
	m_ListCtrl.InsertColumn(1, _T("Distance(MM)"), LVCFMT_CENTER, 100);

	for (int i = 2; i <= igroup + 1; i++) {
		m_ListCtrl.InsertColumn(i, _T("-50"), LVCFMT_CENTER, 80);

	}
	int rowidx = 0;
	for (int i = 10; i < 101; i += 10) {
		m_ListCtrl.InsertItem(m_ListCtrl.GetItemCount(), _T("第一行"));
		CString disstr;
		disstr.Format(L"%d", i);
		m_ListCtrl.SetItemText(rowidx++, 0, disstr);
	}

	return;

	//插入行内容 尾部添加行
	srand(static_cast<unsigned int>(time(0)));
	for (int i = 0; i < 10; i++) {
		for (int j = 1; j < 12; j++) {
			//m_ListCtrl.SetItemText(i, j, RandomFloat());
		}
	}
}

void PageChart::OnTimer(UINT_PTR nIDEvent)
{
	if (isInited != 1) return;
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	static int count = 0;
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		x_count++;
		if(x_count>60)
			pBottomAxis->SetMinMax(x_count - 60, 60 + (x_count - 60) + 2);
		count++;
		if (count % 1==0) {

			{
				double y = 2 + static_cast<float>(rand()) / RAND_MAX * (6 - 2);
				double x = count;
				pLineIrra->AddPoint(x, y);
			}
			{
				double y = 3 + static_cast<float>(rand()) / RAND_MAX * (8 - 3);
				double x = count;
				pLineCur->AddPoint(x, y);
			}
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

void PageChart::drawFreeAdd(double tm, double val, int dtype) {
	pLineIrra->AddPoint(tm, val);
}

bool PageChart::LeftMoveArray(double* ptr, size_t length, double data)
{

	if (x_start + x_len >= 2048)
		return false;

	for (size_t i=x_start + 1; i < x_start + x_len; i++)
	{
		ptr[i - 1] = ptr[i];
	}
	ptr[length - 1] = data;

	return true;
}
void PageChart::drawMov60s(double tm, double val, int dtype) {
	if (dtype == 0) {

		drawFreeAdd(tm, val, dtype);
		return;

		x_count++;
		/*if (x_count == 60) {
			pBottomAxis->SetAutomatic(true);
		}*/
		if (x_count > 60)
		{
			pBottomAxis->SetMinMax(x_count-60, 60+ (x_count-60)+2);
			drawFreeAdd(tm, val, dtype);
			/*x_start++;
			LeftMoveArray(y_irra, x_len, val);
			LeftMoveArray(x_secs, x_len, tm);
			pLineIrra->ClearSerie();
			pLineIrra->AddPoints(&x_secs[x_start], &y_irra[x_start], x_len);*/

		}
		else {
			y_irra[x_count] = val;
			x_secs[x_count] = tm;
			drawFreeAdd(tm, val, dtype);
		}
	
	}

}