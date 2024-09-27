// PageReport3.cpp: 实现文件
//

#include "pch.h"
#include "MeasureBot.h"
#include "PageReport3.h"
#include "afxdialogex.h"


#include <OpenXLSX.hpp>
#include <iostream>
#include <cmath>


//#include "ChartCtrl\ChartMouseListener.h"  //鼠标响应头文件
#include  "ChartCtrl\ChartAxisLabel.h"  //鼠标响应头文件
#include "ChartCtrl/ChartBarSerie.h"



#include "libxl.h"
using namespace libxl;
// PageReport3 对话框

IMPLEMENT_DYNAMIC(PageReport3, CDialogEx)

PageReport3::PageReport3(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG3, pParent)
{

}

PageReport3::~PageReport3()
{
}

void PageReport3::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl1);
	DDX_Control(pDX, IDC_CUSTOM1, m_ChartCtrl);
	DDX_Control(pDX, IDC_COMBO1, m_formulas);
	DDX_Control(pDX, IDC_LIST3, m_ListCtrl2);
}


BEGIN_MESSAGE_MAP(PageReport3, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &PageReport3::OnBnClickedButton1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &PageReport3::OnCbnSelchangeCombo1)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON2, &PageReport3::OnBnClickedButton2)
END_MESSAGE_MAP()


// PageReport3 消息处理程序


BOOL PageReport3::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	pdlg = (CMeasureBotDlg*)AfxGetMainWnd();
	{
		SCROLLINFO scrollinfo;
		GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		scrollinfo.nPage = 20; //设置滑块大小
		scrollinfo.nMax = 200; //设置滚动条的最大位置0–100
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
	}
	if(0){
		SCROLLINFO scrollinfo;
		GetScrollInfo(SB_HORZ, &scrollinfo, SIF_ALL);
		scrollinfo.nPage = 50; //设置滑块大小
		scrollinfo.nMax = 100; //设置滚动条的最大位置0–100
		SetScrollInfo(SB_HORZ, &scrollinfo, SIF_ALL);
	}

	m_formulas.InsertString(0, _T("循环迭代"));
	m_formulas.InsertString(1, _T("对边删除"));
	m_formulas.InsertString(2, _T("3333333"));
	m_formulas.SetWindowTextW(_T("请选择数据"));

	m_ImageList.Create(32, 32, ILC_COLOR16 | ILC_MASK, 3, 0);
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON2));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON2));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON2));
	m_ListCtrl.SetCellMargin(1.2);
	m_ListCtrl2.SetCellMargin(1.2);

	getMapinfo();
	initList();
	//initList1();
	initList2();

	//initChart();
	isInited = 1;

	for (int i = 0; i < 20; i++) {
		expComplete[i] = -1;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void PageReport3::getMapinfo() {

	Json::Value list;
	pdlg->m_page_track.readTracklist(list);

	Json::Value arr = list["list"];
	for (int i = 0; i < arr.size(); i++) {
		int type = arr[i]["model"].asInt();
		if (type == 2) {
			igroup = arr[i]["igroup"].asInt();
			igroup1 = arr[i]["igroup1"].asInt();
			igap = arr[i]["igap"].asInt();
			igap1 = arr[i]["igap1"].asInt();
			break;
		}
	}
	
}

void PageReport3::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (isInited) {
		CRect rect;
		this->GetClientRect(&rect);
		{
			rect.top += 60;
			rect.bottom = rect.top + igroup1*40+40;
			rect.left += 5;
			rect.right -= 10;// = rect.left + 80 * igroup + 100 + 80 + 10;
			m_ListCtrl.MoveWindow(&rect);
		}
		if(0){
			rect.top = rect.bottom+10;
			rect.bottom = rect.top + igroup1 * 40 + 40;
			m_ListCtrl1.MoveWindow(&rect);
		}
		{
			rect.top = rect.bottom+10;
			rect.bottom = rect.top + igroup1 * 40+40 ;
			m_ListCtrl2.MoveWindow(&rect);
		}
	}

	// TODO: 在此处添加消息处理程序代码
}
void PageReport3::resetList() {
	getMapinfo();
	m_ListCtrl.DeleteAllItems();
	while (m_ListCtrl.DeleteColumn(0));
	initList();

	m_ListCtrl2.DeleteAllItems();
	while (m_ListCtrl2.DeleteColumn(0));
	initList2();

	CRect rect;
	this->GetClientRect(&rect);
	OnSize(0, rect.right, rect.bottom);
}

void PageReport3::initList() {

	
	m_ListCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);
	m_ListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES);// | LVS_EX_FULLROWSELECT);
	// 假设 m_gridList 已经创建
	//m_ListCtrl.SetCellMargin(1.2);
	m_ListCtrl.EnableVisualStyles(true);
	m_ListCtrl.InsertHiddenLabelColumn();
	//	m_ListCrl.InsertHiddenLabelColumn();	// Requires one never uses column 0

	//添加列,0---序号，第二个参数显示内容，第三个参数显示对齐方式，第四个参数表头显示的宽度
	m_ListCtrl.InsertColumn(1, _T("X/Y(MM)"), LVCFMT_CENTER, 100);

	int half_mod_width = theApp.getInt(_T("sample"), _T("modwidth"))/2;

	for (int i = 2; i <= igroup + 1; i++) {
		CString colname;
		colname.Format(L"%d",(i-2)*igap+ half_mod_width);
		//colname.Format(L"%d", (i - 2) * igap + pdlg->m_page_track.igap);
		m_ListCtrl.InsertColumn(i, colname, LVCFMT_CENTER, 80);

	}
	m_ListCtrl.InsertColumn(m_ListCtrl.GetColumnCount(), _T("Average"), LVCFMT_CENTER, 80);
	for (int i = 0; i < igroup1 ; i++) {
		m_ListCtrl.InsertItem(m_ListCtrl.GetItemCount(), _T("行"));
		CString colname;
		colname.Format(L"%d", (i ) * igap1);
		m_ListCtrl.SetItemText(i, 1, colname);
	}


	return;
	//插入行内容 尾部添加行
	srand(static_cast<unsigned int>(time(0)));
	for (int i = 2; i <= igroup + 1; i++) {
		for (int j = 0; j < igroup1; j++) {
			float tmpval = theApp.RandomFloat1(20,30);
			CString tmpstr;
			tmpstr.Format(L"%.2e", tmpval);
			m_ListCtrl.SetItemText(j, i, tmpstr);
			dataTab[j][i] = tmpval;
		}
	}
	return;
	
}
void PageReport3::compute2(int ydx) {
	float totalEven = 0.0;
	int totalcount = 0;
	float maxval = 0.0;
	float minval = 1000000.0;
	for (int i = 2 + ydx; i < igroup + 2 - ydx; i++) {
		for (int j = ydx; j < igroup1 - ydx; j++) {
			if (maxval < dataTab[j][i])
				maxval = dataTab[j][i];
			if (minval > dataTab[j][i])
				minval = dataTab[j][i];
			totalEven += dataTab[j][i];
			totalcount++;
		}
	}
	if (totalcount <= 0) {
		::MessageBox(NULL, _T("数据计算失败4"), _T("compute！"), MB_OK);
		return;
	};
	totalEven = totalEven / totalcount;

	float retEven = 1 - (maxval - minval) / totalEven;
	retEven = retEven * 100;
	CString tmpstr;
	tmpstr.Format(L"%.2f%%", retEven);
	GetDlgItem(IDC_EDIT1)->SetWindowTextW(tmpstr);
}
float PageReport3::compute2(int ydx,int ydy) {
	double totalEven = 0.0;
	int totalcount = 0;
	int retcount = 0;

	for (int i = 2 + ydx; i < igroup + 2 - ydx; i++) {
		for (int j = ydy; j < igroup1 - ydy; j++) {

			totalEven += dataTab[j][i];
			totalcount++;
		}
	}
	if (totalcount <= 0) {
		::MessageBox(NULL, _T("数据计算失败2"), _T("compute！"), MB_OK);
		return 0;
	};
	totalEven = totalEven / totalcount;
	m_totalEven_ = totalEven;

	{
		CString tmpstr;
		tmpstr.Format(L"%.2e", totalEven);
		m_ListCtrl.SetItemText(igroup1 / 2, m_ListCtrl.GetColumnCount() - 1, tmpstr);
	}

	//if (ydx == 0) 
	{
		mymemset(MinEven, 50, 1000.0);
		for (int i = 2 + ydx; i < igroup + 2 - ydx; i++) {
			for (int j = ydy; j < igroup1 - ydy; j++) {
				EvenTab[j][i] = (1 - std::abs(dataTab[j][i] - totalEven) / totalEven) * 100;
				if (MinEven[j] > EvenTab[j][i]) {
					if (EvenTab[j][i] > 90.0) {
						MinEven[j] = EvenTab[j][i];
					}
				}
			}
		}
	}

	if(1){
		mymemset(ColEven, 200, 1000.0);
		for (int i = 2 + ydx; i < igroup + 2 - ydx; i++) {
			for (int j = ydy; j < igroup1 - ydy; j++) {
				//EvenTab[j][i] = (1 - std::abs(dataTab[j][i] - totalEven) / totalEven) * 100;
				if (ColEven[i] > EvenTab[j][i]) {
					if (EvenTab[j][i] > 90.0) {
						ColEven[i] = EvenTab[j][i];
					}
				}
			}
		}
	}


	m_ListCtrl.m_redCells.clear();
	m_ListCtrl2.m_redCells.clear();
	for (int i = 2 + ydx; i < igroup + 2 - ydx; i++) {
		for (int j = ydy; j < igroup1 - ydy; j++) {
			{
				CPoint cell;
				cell.x = j;
				cell.y = i;
				m_ListCtrl.m_redCells.push_back(cell);
			}
			if (EvenTab[j][i] > 90) {
				CPoint cell;
				cell.x = j;
				cell.y = i;
				m_ListCtrl2.m_redCells.push_back(cell);
				retcount++;
			}
			CString tmpstr;
			tmpstr.Format(L"%.2f%%", EvenTab[j][i]);
			m_ListCtrl2.SetItemText(j, i, tmpstr);

		}
	}
	m_ListCtrl.RedrawWindow();

	for (int j = ydy; j < igroup1 - ydy; j++) {
		CString tmpstr;
		tmpstr.Format(L"%.2f%%", MinEven[j]);
		m_ListCtrl2.SetItemText(j, m_ListCtrl2.GetColumnCount() - 1, tmpstr);
	}
	m_ListCtrl2.RedrawWindow();

	if (totalcount <= 0) {
		::MessageBox(NULL, _T("数据计算失败3"), _T("compute！"), MB_OK);
		return 0;
	}
	return retcount * 100.0 / totalcount;
}

void PageReport3::compute(int ydx) {

	{
		float maxval = 0.0;
		float minval = 1000000.0;
		for (int i = 2 + ydx; i < igroup + 2 - ydx; i++) {
			float totalEven = 0.0;
			int totalcount = 0;
			for (int j = ydx; j < igroup1 - ydx; j++) {
				if (maxval < dataTab[j][i])
					maxval = dataTab[j][i];
				if (minval > dataTab[j][i])
					minval = dataTab[j][i];
				totalEven += dataTab[j][i];
				totalcount++;
			}
			if (totalcount <= 0) {
				::MessageBox(NULL, _T("数据计算失败"), _T("compute！"), MB_OK);

				return;
			}
			ColIrra[i] = totalEven / totalcount;

		}
	}


	if (!isComputed) return;
	{
		float maxval = 0.0;

		if (0) {
			mymemset(ColEven, 200, 1000.0);

			for (int i = 2 + ydx; i < igroup + 2 - ydx; i++) {
				float minval = 1000000.0;
				float totalEven = 0.0;
				int totalcount = 0;
				for (int j = ydx; j < igroup1 - ydx; j++) {
					if (maxval < EvenTab[j][i])
						maxval = EvenTab[j][i];
					if (minval > EvenTab[j][i])
						minval = EvenTab[j][i];
					totalEven += EvenTab[j][i];
					totalcount++;
				}
				ColEven[i] = minval;

			}
		}
	}
}


void PageReport3::initList1() {

	m_ListCtrl1.SetImageList(&m_ImageList, LVSIL_SMALL);
	m_ListCtrl1.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	// 假设 m_gridList 已经创建
	//m_ListCtrl1.SetCellMargin(1.2);
	m_ListCtrl1.EnableVisualStyles(true);
	m_ListCtrl1.InsertHiddenLabelColumn();
	//	m_ListCrl.InsertHiddenLabelColumn();	// Requires one never uses column 0

	//添加列,0---序号，第二个参数显示内容，第三个参数显示对齐方式，第四个参数表头显示的宽度
	m_ListCtrl1.InsertColumn(1, _T("X/Y(MM)"), LVCFMT_CENTER, 100);

	for (int i = 2; i <= igroup + 1; i++) {
		CString colname;
		colname.Format(L"%d", (i - 2) * igap);
		m_ListCtrl1.InsertColumn(i, colname, LVCFMT_CENTER, 80);

	}
	m_ListCtrl1.InsertColumn(m_ListCtrl1.GetColumnCount(), _T("Average"), LVCFMT_CENTER, 80);

	for (int i = 0; i < igroup1; i++) {
		m_ListCtrl1.InsertItem(m_ListCtrl1.GetItemCount(), _T("行"));
		CString colname;
		colname.Format(L"%d", (i)*igap1);
		m_ListCtrl1.SetItemText(i, 1, colname);
	}

	return;

	//插入行内容 尾部添加行
	srand(static_cast<unsigned int>(time(0)));
	for (int i = 0; i < 10; i++) {
		for (int j = 1; j < 12; j++) {
			//m_ListCtrl1.SetItemText(i, j, RandomFloat());
		}
	}
}
void PageReport3::initList2() {

	m_ListCtrl2.SetImageList(&m_ImageList, LVSIL_SMALL);

	m_ListCtrl2.SetExtendedStyle(LVS_EX_GRIDLINES);// | LVS_EX_FULLROWSELECT);
	// 假设 m_gridList 已经创建
	//m_ListCtrl2.SetCellMargin(1.2);
	m_ListCtrl2.EnableVisualStyles(true);
	m_ListCtrl2.InsertHiddenLabelColumn();
	//	m_ListCrl.InsertHiddenLabelColumn();	// Requires one never uses column 0

	//添加列,0---序号，第二个参数显示内容，第三个参数显示对齐方式，第四个参数表头显示的宽度
	m_ListCtrl2.InsertColumn(1, _T("X/Y(MM)"), LVCFMT_CENTER, 100);

	for (int i = 2; i <= igroup + 1; i++) {
		CString colname;
		colname.Format(L"%d", (i - 2) * igap);
		m_ListCtrl2.InsertColumn(i, colname, LVCFMT_CENTER, 80);

	}
	m_ListCtrl2.InsertColumn(m_ListCtrl2.GetColumnCount(), _T("Min"), LVCFMT_CENTER, 80);
	for (int i = 0; i < igroup1; i++) {
		m_ListCtrl2.InsertItem(m_ListCtrl2.GetItemCount(), _T("行"));
		CString colname;
		colname.Format(L"%d", (i)*igap1);
		m_ListCtrl2.SetItemText(i, 1, colname);
	}

	return;

	//插入行内容 尾部添加行
	srand(static_cast<unsigned int>(time(0)));
	for (int i = 0; i < 10; i++) {
		for (int j = 1; j < 12; j++) {
			//m_ListCtrl2.SetItemText(i, j, RandomFloat());
		}
	}
}

void PageReport3::initChart() {
	pBottomAxis = m_ChartCtrl.CreateStandardAxis(CChartCtrl::BottomAxis);
	pBottomAxis->SetMinMax(0, 100);
	//pBottomAxis->SetAutomatic(true);
	pBottomAxis->GetLabel()->SetText(_T("WORK DISTANCE（MM）"));
	pBottomAxis->GetLabel()->SetColor(RGB(255, 255, 255));



	pLeftAxis = m_ChartCtrl.CreateStandardAxis(CChartCtrl::LeftAxis);
	pLeftAxis->SetMinMax(0, 30);
	//pLeftAxis->SetAutomatic(true);
	pLeftAxis->GetLabel()->SetText(_T("PEEK IRRADIANCE（W/cm2）"));
	pLeftAxis->GetLabel()->SetColor(RGB(255, 255, 255));



	TChartString str1;
	CString title;
	title.Format(L"%s interval %s Irradiance On MD", L"UV3004 W267_3.8mm", L"395nm");
	m_ChartCtrl.GetTitle()->AddString(TChartString(title));

	m_ChartCtrl.GetTitle()->SetColor(RGB(255, 255, 255));   //标题字体白色
	m_ChartCtrl.GetLeftAxis()->SetTextColor(RGB(255, 255, 255));  //左坐标轴白色
	m_ChartCtrl.GetBottomAxis()->SetTextColor(RGB(255, 255, 255));  //底部坐标轴白色
	//m_ChartCtrl.GetRightAxis()->GetGrid()->SetColor(RGB(0, 255, 255));

	m_ChartCtrl.SetBorderColor(RGB(255, 255, 255));  //边框颜色白色
	m_ChartCtrl.SetBackColor(RGB(85, 85, 85));  //背景颜色深灰色

	pBarIrra = m_ChartCtrl.CreateLineSerie();

	COLORREF SerieColor = RGB(0, 255, 0);
	pBarIrra->SetColor(SerieColor);//   pLineSeries要在头文件中初始化CChartLineSerie *pLineSeries
	//pBarIrra->SetBarWidth(20);
	//pBarIrra->SetGroupId(1);
	pBarIrra->SetName(L"光强");

	m_ChartCtrl.SetZoomEnabled(false);
	//m_ChartCtrl.RemoveAllSeries();//先清空


	pBarIrra->AddPoint( 10.0 + 1,  10.0);
	/*for (int i = 0; i < 5; i++) {
		pBarIrra->AddPoint(i*10+1,i+10.0);
	}*/
}


void PageReport3::listExport1bak(string outfname) {
	int count = m_ListCtrl.GetItemCount(); // 获取列表中的项目数
	if (count <= 0) return;

	string tplate = theApp.tostr(theApp.getString(_T("File"), _T("template_path")));
	tplate += "/Track_PA.xlsx";
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
	if (sheet == NULL) return;
	DebugLog::writeLogF("%s", theApp.tostr(CString(sheet->name())).c_str());

	int tabpos = 0;
	for (int32_t row = sheet->firstRow(); row < sheet->lastRow(); ++row)
	{
		CellType cell_type = sheet->cellType(row, 0);
		if (cell_type == CELLTYPE_STRING)
		{
			wstring ret = sheet->readStr(row, 0);
			if (ret == L"Irradiance (mw/cm2)") {
				tabpos = row;
				break;
			}
		}
	}

	tabpos += 1;
	//sheet->insertRow()
	if(igroup1>30)
		sheet->insertRow(tabpos + 6, tabpos + 6 + (igroup1-25));
	sheet->setMerge(tabpos, tabpos + 1, igroup + 3, igroup + 3);
	sheet->copyCell(tabpos, 5, tabpos, igroup + 3);

	sheet->delMerge(tabpos, 5);
	sheet->setMerge(tabpos + 2, tabpos + 2 + igroup1, igroup + 3, igroup + 3);
	sheet->copyCell(tabpos + 2, 5, tabpos + 2, igroup + 3);

	sheet->clear(tabpos + 2, tabpos + 2, 5, 5);

	for (int i = 2; i <= igroup + 2; i++) {
		if (i > 4) {
			if (i % 2 == 0) {
				sheet->copyCell(tabpos, 2, tabpos, i);
				sheet->copyCell(tabpos + 1, 2, tabpos + 1, i);
			}
			else {
				sheet->copyCell(tabpos, 3, tabpos, i);
				sheet->copyCell(tabpos + 1, 3, tabpos + 1, i);
			}
		}
		CString colname;
		colname.Format(L"%d", (i - 2) * igap);
		sheet->writeStr(tabpos + 1, i, colname);
		CString colname1;
		colname1.Format(L"%d", (i - 2) * igap - (igroup * igap) / 2);
		sheet->writeStr(tabpos, i, colname1);

	}
	//	m_ListCtrl1.InsertColumn(m_ListCtrl1.GetColumnCount(), _T("Average"), LVCFMT_CENTER, 80);


	for (int i = 0; i <= igroup1; i++) {

		if (i == igroup1) {

			sheet->setMerge(tabpos + 2 + i, tabpos + 2 + i, 0, 1);
			sheet->copyCell(tabpos + 2, 0, tabpos + 2 + i, 0);
			sheet->writeStr(tabpos + 2 + i, 0, L"Average");
		}
		else {
			sheet->copyCell(tabpos + 2, 1, tabpos + 2 + i, 1);
			CString colname;
			colname.Format(L"%d", (i)*igap1);
			sheet->writeStr(tabpos + 2 + i, 1, colname);
		}

	}

	for (int i = 2; i <= igroup + 1; i++)
	{
		for (int j = 0; j < igroup1; j++)
		{
			sheet->copyCell(tabpos + 2, 2, tabpos + 2 + j, i);
		}
	}
	for (int i = 2; i <= igroup + 1; i++) {
		sheet->copyCell(tabpos + 1 + igroup1, i, tabpos + 2 + igroup1, i);
	}

	Format* format = book->addFormat();
	//Format* format = sheet->cellFormat(tabpos + 2, 2);

	format->setFillPattern(FILLPATTERN_SOLID);//填充样式
	format->setPatternForegroundColor(COLOR_GREEN);//填充颜色：红色

	for (int i = 2; i <= igroup + 1; i++)
	{
		for (int j = 0; j < igroup1; j++)
		{
			//sheet->copyCell(tabpos + 2, 2, tabpos + 2 + j, i );
			CString text;
			text = m_ListCtrl.GetItemText(j, i);
			string valstr = "--";
			if (!text.IsEmpty()) {
				valstr = theApp.tostr(text);
			}
			if (m_ListCtrl.isCellRed(j, i)) {
				//sheet->setCellFormat(tabpos + 2 + j, i, format);//
				//DebugLog::writeLogF("---x=%d  y=%d",j,i-2);

				IFormatT<TCHAR>* pformat;//申请格式变量
				CString strIn = sheet->readStr(tabpos + 2 + j, i, &pformat);//读取需要填充的单元格格式
				IFormatT<TCHAR>* pformat1 = book->addFormat(pformat);
				pformat1->setFillPattern(FILLPATTERN_SOLID);//填充样式
				pformat1->setPatternForegroundColor(COLOR_YELLOW_CL);//填充颜色：红色
				sheet->setCellFormat(tabpos + 2 + j, i, pformat1);

				sheet->writeNum(tabpos + 2 + j, i, std::stof(valstr));

			}
			else {
				sheet->writeNum(tabpos + 2 + j, i, std::stof(valstr));

			}

		}
	}

	sheet->writeNum(tabpos + 2, igroup + 3, m_totalEven_);

	for (int i = 2; i <= igroup + 1; i++) {
		CString tmpstr;
		tmpstr.Format(L"%.2f", ColIrra[i]);
		sheet->copyCell(tabpos + 1 + igroup1, i, tabpos + 2 + igroup1, i);
		sheet->writeStr(tabpos + 2 + igroup1, i, tmpstr);
	}

	book->save(CString(outfname.c_str()));

	return;
}
bool PageReport3::appendSheets(string outfname, int dist)
{
	libxl::Book* book = xlCreateXMLBook();
	if (nullptr == book)
	{
		DebugLog::writeLogF("create libxl book failed");
		return false;
	}

	CString fname(outfname.c_str());

	Sheet* TPsheet0;
	Sheet* TPsheet1;
	Sheet* TPsheet2;

	{
		string tplate = theApp.tostr(theApp.getString(_T("File"), _T("template_path")));
		tplate += "/Track_PA.xlsx";
		CString fname1(tplate.c_str());
		libxl::Book* book1 = xlCreateXMLBook();
		if (false == book1->load(fname1))
		{
			DebugLog::writeLogF(" libxl open template failed");
			return false;
		}
		TPsheet0 = book1->getSheet(0);
		TPsheet1 = book1->getSheet(1);
		TPsheet2 = book1->getSheet(2);

		if (dist == 0) {
			book1->save(fname);
		}
	}

	if (false == book->load(fname))
	{
		DebugLog::writeLogF(" libxl open template failed");
		return false;
	}
	if (dist == 0) {
		book->delSheet(0);
		book->delSheet(1);
		book->delSheet(2);
	//	book->delSheet(3);

	}
	int sheetIdx = (m_DistIdx + 0) * 3;
	int curH = pdlg->m_ctrl_mc.m_height_list.GetCurSel();
	CString sheetname; 
	{
		
		sheetname.Format(L"%dMM", theApp.getHtByidx(curH));
		book->insertSheet(sheetIdx, sheetname, TPsheet0);
	}
	{
		sheetname.Format(L"%d_powerdata", theApp.getHtByidx(curH));
		book->insertSheet(sheetIdx +1, sheetname, TPsheet1);
	}
	{
		sheetname.Format(L"%d_temp&flow", theApp.getHtByidx(curH));
		book->insertSheet(sheetIdx +2, sheetname, TPsheet2);
	}

	book->save(fname);
	
	return true;
	
}

bool PageReport3::listExport1(string outfname, int dist) {


	int count = m_ListCtrl.GetItemCount(); // 获取列表中的项目数
	if (count <= 0) return false;

	libxl::Book* book = xlCreateXMLBook();	// 与2003相比，仅仅这里有点区别而已
	if (nullptr == book)
	{
		DebugLog::writeLogF("create libxl book failed");
		return false;
	}
	Sheet* sheet;
	
	CString fname(outfname.c_str());

	if (false == book->load(fname))
	{
		DebugLog::writeLogF(" libxl open template failed");
		return false;
	}
	sheet = book->getSheet(m_DistIdx*3);
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
		tempstr.Format(L"%.1f", pdlg->m_ctrl_stat.curTemp.temp2);
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
			if (ret == L"Irradiance (mw/cm2)") {
				tabpos = row;
				break;
			}
		}
	}

	tabpos += 1;
	//sheet->insertRow()
	if (igroup1 > 30)
	{
		sheet->insertRow(tabpos + 6, tabpos + 6 + (igroup1 - 25));
	}
	else {
		//int delrow = 30 - igroup1;
		sheet->removeRow(igroup1+5,30);
	}
	sheet->setMerge(tabpos, tabpos+1, igroup+2, igroup+2);
	sheet->copyCell(tabpos, 5, tabpos, igroup+2);

	sheet->delMerge(tabpos, 5);
	sheet->setMerge(tabpos+2, tabpos + 2 +igroup1, igroup + 2, igroup + 2);
	sheet->copyCell(tabpos + 2, 5, tabpos + 2, igroup + 2);

	sheet->clear(tabpos + 2, tabpos + 2, 5, 5);

	for (int i = 2; i < igroup + 2; i++) {
		if (i > 4) {
			if (i % 2 == 0) {
				sheet->copyCell(tabpos, 2, tabpos, i);
				sheet->copyCell(tabpos+1, 2, tabpos+1, i);
			}
			else {
				sheet->copyCell(tabpos, 3, tabpos, i);
				sheet->copyCell(tabpos+1, 3, tabpos+1, i);
			}
		}
		CString colname;
		colname.Format(L"%d", (i - 2) * igap);
		sheet->writeStr(tabpos + 1, i, colname);
		CString colname1;
		colname1.Format(L"%d", (i - 2) * igap - (igroup*igap)/2);
		sheet->writeStr(tabpos, i, colname1);

	}
//	m_ListCtrl1.InsertColumn(m_ListCtrl1.GetColumnCount(), _T("Average"), LVCFMT_CENTER, 80);


	for (int i = 0; i <= igroup1; i++) {
		
		if (i == igroup1) {

			sheet->setMerge(tabpos + 2 + i, tabpos + 2 + i, 0, 1);
			sheet->copyCell(tabpos + 2, 0, tabpos + 2 + i, 0);
			sheet->writeStr(tabpos + 2 + i, 0, L"Average");
		}
		else {
			sheet->copyCell(tabpos + 2, 1, tabpos + 2 + i, 1);
			CString colname;
			colname.Format(L"%d", (i)*igap1);
			sheet->writeStr(tabpos + 2 + i, 1, colname);
		}
		
	}

	for (int i = 2; i <= igroup + 1; i++)
	{
		for (int j = 0; j < igroup1; j++)
		{
			if (m_ListCtrl.isCellRed(j, i)) {
				sheet->copyCell(0, 26, tabpos + 2 + j, i);
			}
			else {
				sheet->copyCell(0, 25, tabpos + 2 + j, i);
			}
		}
	}
	for (int i = 2; i <= igroup + 1; i++) {
		sheet->copyCell(0, 25, tabpos + 2 + igroup1, i);
	}


	for (int i = 2; i <= igroup + 1; i++)
	{
		for (int j = 0; j < igroup1; j++)
		{
			//sheet->copyCell(tabpos + 2, 2, tabpos + 2 + j, i );
			CString text;
			text = m_ListCtrl.GetItemText(j, i);
			string valstr = "--";
			if (!text.IsEmpty()) {
				valstr = theApp.tostr(text);
			}

			sheet->writeNum(tabpos + 2 + j, i, std::stof(valstr));

		}
	}

	sheet->writeNum(tabpos + 2, igroup+2, m_totalEven_);

	for (int i = 2; i <= igroup + 1; i++) {
		CString tmpstr;
		tmpstr.Format(L"%.2f", ColIrra[i]);
		sheet->writeStr(tabpos + 2 + igroup1, i, tmpstr);
	}

	book->save(CString(outfname.c_str()));

	return true;
}
bool PageReport3::listExport2(string outfname, int dist) {
	if (!isComputed) return false;

	CString fname(outfname.c_str());
	libxl::Book* book = xlCreateXMLBook();	// 与2003相比，仅仅这里有点区别而已
	if (nullptr == book)
	{
		DebugLog::writeLogF("create libxl book failed");
		return false;;
	}

	if (false == book->load(fname))
	{
		DebugLog::writeLogF(" libxl open template failed");
		return false;;
	}

	Sheet* sheet = book->getSheet(m_DistIdx*3);
	if (sheet == NULL)
	{
		return false;;
	}
	DebugLog::writeLogF("%s", theApp.tostr(CString(sheet->name())).c_str());

	int tabpos = 0;
	for (int32_t row = sheet->firstRow(); row < sheet->lastRow(); ++row)
	{
		CellType cell_type = sheet->cellType(row, 0);
		if (cell_type == CELLTYPE_STRING)
		{
			wstring ret = sheet->readStr(row, 0);
			if (ret == L"Evenness Degree (%)") {
				tabpos = row;
				break;
			}
		}
	}

	tabpos += 1;
	if(igroup1>30)
		sheet->insertRow(tabpos + 6, tabpos + 6 + igroup1);
	//sheet->insertRow()
	sheet->setMerge(tabpos, tabpos + 1, igroup + 2, igroup + 2);
	sheet->copyCell(tabpos, 5, tabpos, igroup + 2);
	sheet->delMerge(tabpos, 5);

	/*sheet->setMerge(tabpos + 2, tabpos + 2 + igroup1, igroup + 3, igroup + 3);
	sheet->copyCell(tabpos + 2, 5, tabpos + 2, igroup + 3);*/
	sheet->delMerge(tabpos+2, 5);
	//sheet->clear(tabpos + 2, tabpos + 2, 5, 5);

	for (int i = 2; i < igroup + 2; i++) {
		if (i > 4) {
			if (i % 2 == 0) {
				sheet->copyCell(tabpos, 2, tabpos, i);
				sheet->copyCell(tabpos + 1, 2, tabpos + 1, i);
			}
			else {
				sheet->copyCell(tabpos, 3, tabpos, i);
				sheet->copyCell(tabpos + 1, 3, tabpos + 1, i);
			}
		}
		CString colname;
		colname.Format(L"%d", (i - 2) * igap);
		sheet->writeStr(tabpos + 1, i, colname);
		CString colname1;
		colname1.Format(L"%d", (i - 2) * igap - (igroup * igap) / 2);
		sheet->writeStr(tabpos, i, colname1);

	}
	//	m_ListCtrl1.InsertColumn(m_ListCtrl1.GetColumnCount(), _T("Average"), LVCFMT_CENTER, 80);

	
	for (int i = 0; i <= igroup1; i++) {

		if (i == igroup1) {

			sheet->setMerge(tabpos + 2 + i, tabpos + 2 + i, 0, 1);
			sheet->copyCell(tabpos + 2, 0, tabpos + 2 + i, 0);
			sheet->writeStr(tabpos + 2 + i, 0, L"Min Evenness");
		}
		else {
			sheet->copyCell(tabpos + 2, 1, tabpos + 2 + i, 1);
			CString colname;
			colname.Format(L"%d", (i)*igap1);
			sheet->writeStr(tabpos + 2 + i, 1, colname);
		}

	}
	

	for (int i = 2; i <= igroup + 1; i++)
	{
		for (int j = 0; j < igroup1; j++)
		{
			if (m_ListCtrl2.isCellRed(j, i)) {
				sheet->copyCell(0, 24, tabpos + 2 + j, i);
			}
			else {
				sheet->copyCell(0, 23, tabpos + 2 + j, i);
			}
		}
	}
	for (int i = 2; i <= igroup + 1; i++) {
		sheet->copyCell(0, 23, tabpos + 2 + igroup1, i);
	}

	for (int i = 2; i <= igroup + 1; i++)
	{
		for (int j = 0; j < igroup1; j++)
		{
			//sheet->copyCell(tabpos + 2, 2, tabpos + 2 + j, i);
			CString text;
			text = m_ListCtrl2.GetItemText(j, i);
			string valstr = "--";
			if (!text.IsEmpty()) {
				valstr = theApp.tostr(text);
			}
			//sheet->writeNum(tabpos + 2 + j, i, std::stof(valstr));
			sheet->writeStr(tabpos + 2 + j, i, text);

		}
	}
	for (int j = 0; j < igroup1; j++) {
		if (MinEven[j] == 1000.0)continue;
		CString tmpstr;
		tmpstr.Format(L"%.2f%%", MinEven[j]);
		sheet->copyCell(0,23, tabpos + 2 + j, igroup + 2);
		sheet->writeStr(tabpos + 2 + j, igroup+2, tmpstr);
	}

	for (int i = 2; i <= igroup + 1; i++) {
		if (ColEven[i] == 1000.0) continue;
		CString tmpstr;
		tmpstr.Format(L"%.2f%%", ColEven[i]);
	//	sheet->copyCell(tabpos + 1 + igroup1, i, tabpos + 2 + igroup1, i);
		sheet->writeStr(tabpos + 2 + igroup1, i, tmpstr);
	}


	Format* format = book->addFormat();
	format->setFillPattern(FILLPATTERN_NONE);//填充样式
	for (int i = 23; i <= 26; i++) {
		sheet->copyCell(0, 27, 0, i);
		sheet->setCellFormat(0, i, format);

	}
	//sheet->copyCell(0, 0, 0, 0);

	book->save(CString(outfname.c_str()));

	return true;
}

void PageReport3::onIrraVal(float val, int unit, unsigned long tm) {
	lastVal = val;
	if (curXidx != -1 && curYidx != -1) {
		CString tmpstr;
		tmpstr.Format(L"%.2e", val);
		m_ListCtrl.SetItemText(curYidx, curXidx+2, tmpstr);
		dataTab[curYidx][curXidx + 2] = val;
		curXidx = -1;
		curYidx = -1;
	}
}
void PageReport3::onPosVal(int x, int y) {
	curXidx = x;
	curYidx = y;
	{
		CString tmpstr;
		tmpstr.Format(L"%.2e", lastVal);
		m_ListCtrl.SetItemText(curYidx, curXidx + 2, tmpstr);
		dataTab[curYidx][curXidx + 2] = lastVal;
	}
}
void PageReport3::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	string report = theApp.tostr(theApp.getString(_T("File"), _T("report_path")));
	report += theApp.makeFname("\\DA");

	if (m_DistIdx == 0) {
		DeleteFile(CString(report.c_str()));
	}
	int dist = 0;
	if (_access(report.c_str(), 0) != -1) {
		dist = 1;
	}
	compute(0);

	int curH = pdlg->m_ctrl_mc.m_height_list.GetCurSel();
	if (curH < 0)return;
	if (expComplete[curH] != -1) {
		::MessageBox(NULL, _T("当前数据已导出，请勿重复操作"), _T("导出失败！"), MB_OK);
		return;
	}

	if (false == appendSheets(report, dist)) {
		::MessageBox(NULL, _T("新建Tab页面失败"), _T("导出失败！"), MB_OK);
		return;
	}

	if (false == listExport1(report, dist)) {
		::MessageBox(NULL, _T("导出报表失败1"), _T("导出失败！"), MB_OK);
		return;
	}
	if (false == listExport2(report, dist)) {
		::MessageBox(NULL, _T("导出报表失败1"), _T("导出失败！"), MB_OK);
		return;
	}
	powerExport(report);
	tempExport(report);

	expComplete[curH] = curH;
	m_DistIdx++;
	::MessageBox(NULL, CString(report.c_str()), _T("导出成功！"), MB_OK);

}


void PageReport3::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码

	int count = m_ListCtrl.GetItemCount(); // 获取列表中的项目数
	if (count <= 0) return;

	libxl::Book* book = xlCreateXMLBook();	// 与2003相比，仅仅这里有点区别而已
	if (nullptr == book)
	{
		DebugLog::writeLogF("create libxl book failed");
		return;
	}
	Sheet* sheet;

	string tplate = theApp.tostr(theApp.getString(_T("File"), _T("template_path")));
	tplate += "/pt3_test_data.xlsx";
	CString fname(tplate.c_str());

	if (false == book->load(fname))
	{
		DebugLog::writeLogF(" libxl open template failed");
		return;
	}
	if (m_formulas.GetCurSel() == 0 && igroup == 13 && igroup1==7) {
		sheet = book->getSheet(0);
		if (sheet == NULL) return;
		for (int i = 2; i <= igroup + 1; i++) {
			for (int j = 0; j < igroup1; j++) {

				float tmpval = sheet->readNum(j + 6, i - 1);
				CString tmpstr;
				tmpstr.Format(L"%.2f", tmpval);
				m_ListCtrl.SetItemText(j, i, tmpstr);
				dataTab[j][i] = tmpval;
			}
		}
	}
	else if (m_formulas.GetCurSel() == 1 && igroup == 17 && igroup1 == 7) {
		sheet = book->getSheet(2);
		if (sheet == NULL) return;
		for (int i = 2; i <= igroup + 1; i++) {
			for (int j = 0; j < igroup1; j++) {

				float tmpval = sheet->readNum(j + 2, i);
				CString tmpstr;
				tmpstr.Format(L"%.2f", tmpval);
				m_ListCtrl.SetItemText(j, i, tmpstr);
				dataTab[j][i] = tmpval;
			}
		}
	}
	else if (m_formulas.GetCurSel() == 2 && igroup == 8 && igroup1 == 21) {
		sheet = book->getSheet(1);
		if (sheet == NULL) return;
		for (int i = 2; i <= igroup + 1; i++) {
			for (int j = 0; j < igroup1; j++) {

				float tmpval = sheet->readNum(j + 6, i-1);
				CString tmpstr;
				tmpstr.Format(L"%.2f", tmpval);
				m_ListCtrl.SetItemText(j, i, tmpstr);
				dataTab[j][i] = tmpval;
			}
		}
	}
	

	

}


void PageReport3::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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


void PageReport3::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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
		ScrollWindow( unit,0);
		break;
	case SB_LINEDOWN:           //Scroll one line down
		scrollinfo.nPos += 1;
		if (scrollinfo.nPos + scrollinfo.nPage > scrollinfo.nMax)  //此处一定要注意加上滑块的长度，再作判断
		{
			scrollinfo.nPos = scrollinfo.nMax;
			break;
		}
		SetScrollInfo(SB_HORZ, &scrollinfo, SIF_ALL);
		ScrollWindow( -unit,0);
		break;
	case SB_PAGEUP:            //Scroll one page up.
		scrollinfo.nPos -= 5;
		if (scrollinfo.nPos <= scrollinfo.nMin)
		{
			scrollinfo.nPos = scrollinfo.nMin;
			break;
		}
		SetScrollInfo(SB_HORZ, &scrollinfo, SIF_ALL);
		ScrollWindow( unit * 5,0);
		break;
	case SB_PAGEDOWN:        //Scroll one page down        
		scrollinfo.nPos += 5;
		if (scrollinfo.nPos + scrollinfo.nPage >= scrollinfo.nMax)  //此处一定要注意加上滑块的长度，再作判断
		{
			scrollinfo.nPos = scrollinfo.nMax;
			break;
		}
		SetScrollInfo(SB_HORZ, &scrollinfo, SIF_ALL);
		ScrollWindow( -unit * 5,0);
		break;
	case SB_ENDSCROLL:      //End scroll     
		break;
	case SB_THUMBPOSITION:  //Scroll to the absolute position. The current position is provided in nPos
		break;
	case SB_THUMBTRACK:                  //Drag scroll box to specified position. The current position is provided in nPos
		ScrollWindow( (scrollinfo.nPos - nPos) * unit,0);
		scrollinfo.nPos = nPos;
		SetScrollInfo(SB_HORZ, &scrollinfo, SIF_ALL);
		break;
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void PageReport3::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码

	if (1) {
		int idx = 0;
		int idy = 0;
		float maxret = 0;
		int tmpgroup = igroup1 > igroup?igroup:igroup1;
		int round = tmpgroup / 2 + tmpgroup % 2;

		for (int i = 0; i < round; i++) {

			for (int k = 2; k >=0; k--) {
				if (i == 0 && k != 0)continue;
				int ix = k == 1 ? 1 : 0;
				int iy = k == 2 ? 1 : 0;
				float count = compute2(i-ix, i-iy);
				if (count > maxret) {
					maxret = count;
					idx = i - ix;
					idy = i - iy;
				}
				pts_time::wait(500);
				

			}
		}
		compute2(idx,idy);
		isComputed = true;
		compute2(0);
	}
	else {
		int idx = 0;
		float maxret = 0;
		int tmpgroup = igroup1 > igroup ? igroup : igroup1;
		int round = tmpgroup / 2 + tmpgroup % 2;
		for (int i = 0; i < round; i++) 
		{
			float count = compute1(i);
			/*if (i == 3) {
				idx = i;
				break;
			}*/
			if (count > maxret) {
				maxret = count;
				idx = i;
			}

			pts_time::wait(500);
		}

		compute1(idx);
		isComputed = true;

		compute2(0);
	}
	
}

void PageReport3::powerExport(string outfname) {

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
	Sheet* sheet = book->getSheet(m_DistIdx*3+1);
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
		//	sheet->writeNum(i + 2, j + 3, powList[i].chn[j / 3].ntc);

		}
	}

	book->save(CString(outfname.c_str()));

	return;
}

void PageReport3::tempExport(string outfname) {

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
	Sheet* sheet = book->getSheet(m_DistIdx*3+2);
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


