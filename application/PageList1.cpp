// PageList1.cpp: 实现文件
//

#include "pch.h"
#include "MeasureBot.h"
#include "PageList1.h"
#include "afxdialogex.h"

#include <OpenXLSX.hpp>
#include <iostream>
#include <cmath>

#include ".\CGridListCtrlEx\CGridColumnTraitDateTime.h"
#include ".\CGridListCtrlEx\CGridColumnTraitEdit.h"
#include ".\CGridListCtrlEx\CGridColumnTraitCombo.h"
#include ".\CGridListCtrlEx\CGridColumnTraitHyperLink.h"
#include ".\CGridListCtrlEx\CGridRowTraitXP.h"
#include ".\CGridListCtrlEx\ViewConfigSection.h"

#include "ChartCtrl\ChartMouseListener.h"  //鼠标响应头文件
#include  "ChartCtrl\ChartAxisLabel.h"  //鼠标响应头文件
#include "ChartCtrl/ChartBarSerie.h"

//#include <xlnt/xlnt.hpp>
//#include "xlnt/drawing/spreadsheet_drawing.hpp"

#include "libxl.h"
using namespace libxl;

// PageList1 对话框
using namespace std;
using namespace OpenXLSX;

IMPLEMENT_DYNAMIC(PageList1, CDialogEx)

PageList1::PageList1(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOGList, pParent)
{

}

PageList1::~PageList1()
{
}

void PageList1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Control(pDX, IDC_CUSTOM2, m_ChartCtrl);

}


BEGIN_MESSAGE_MAP(PageList1, CDialogEx)
	ON_WM_SIZE()
	ON_MESSAGE(WM_MYMESSAGE_PAGE_LIST, &PageList1::OnMyMessageSignal)

	ON_BN_CLICKED(IDC_BUTTON1, &PageList1::OnBnClickedButton1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &PageList1::OnLvnItemchangedList1)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, &PageList1::OnLvnColumnclickList1)
END_MESSAGE_MAP()


// PageList1 消息处理程序


BOOL PageList1::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	pdlg= (CMeasureBotDlg*)AfxGetMainWnd();

	m_ImageList.Create(32, 32, ILC_COLOR16 | ILC_MASK, 3, 0);
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON2));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON2));
	m_ListCtrl.SetCellMargin(1.2);
	curIdx = -1;
	initList();
	initChart();
	// TODO:  在此添加额外的初始化
	isInited = true;
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void PageList1::resetList() {

	m_ListCtrl.DeleteAllItems();
	while (m_ListCtrl.DeleteColumn(0));
	initList();
	
	CRect rect;
	this->GetClientRect(&rect);
	OnSize(0, rect.right, rect.bottom);
}
void PageList1::onMeasureStart() {
	memset(rowMax, 0.0, 100);
}

int PageList1::curRowIdx() {
	//int idx = 0;
	int curh = pdlg->m_ctrl_mc.m_height_list.GetCurSel();
	return curh;
}

void PageList1::onNewLine() {
	curPoxI = 0;
	preTm = 0;

	for (int j = 2; j <= igroup + 1; j++) {
		m_ListCtrl.SetItemText(curRowIdx(), j, L"");
	}
	//memset(dataTab[curRowIdx()][curIdx], -1, 500);

	DebugLog::writeLogF("CD:start one new line idx=%d", curRowIdx());
}
void PageList1::onIrraVal(float val, int unit, unsigned long tm) {
	if (curIdx >= 0 && curIdx<100) {
		if (val > rowMax[curIdx]) {
			rowMax[curIdx] = val;
			CString tmpstr;
			tmpstr.Format(L"%.2e",val);
			m_ListCtrl.SetItemText(curRowIdx(), curIdx+2, tmpstr);
		}
	}
	if (pdlg->m_ctrl_mc.m_isMeasure == 1) {
		if (curPoxI < 499) {
			if (preTm == 0) {
				preTm = tm;
			}
			else {
				int dur = tm - preTm;
				int distance = pdlg->m_ctrl_mc.m_curSpeed * dur / 1000;
				if (dur < 0 || distance < 0) {
					DebugLog::writeLogF("MD:compute distance err dur=%d dist=%d!", dur, distance);
				}
				if (distance < 1)distance = 1;
				curPoxI += distance;
				if (curPoxI >= 499) {
					curPoxI = 499;
					DebugLog::writeLogF("MD:compute distance err!");
				}
			}
			dataTab[curRowIdx()][curIdx][curPoxI] = val;
		}
	}
}
void PageList1::onPosVal(float x, float y, string axis, float dist,int idx) {
	if (idx >= 0) {
		curIdx = idx;
		if (dist < 0) {
			curIdx += 1;
		}

		if (moudles % 2 == 0 && idx+1 >moudles/2 && curIdx>0) {
			curIdx += 1;
		}
	}
}
void PageList1::initList() {

	m_ListCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);

	m_ListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	// 假设 m_gridList 已经创建

	m_ListCtrl.EnableVisualStyles(true);
	m_ListCtrl.InsertHiddenLabelColumn();

	Json::Value list;
	pdlg->m_page_track.readTracklist(list);

	Json::Value arr = list["list"];
	for (int i = 0; i < arr.size(); i++) {
		int type = arr[i]["model"].asInt();
		if (type == 1) {
			moudles = arr[i]["igroup"].asInt();
			iheight = arr[i]["iheight"].asInt();
			if (iheight > 500) {
				DebugLog::writeLogF("MD:get iheight err %d", iheight);
				iheight = 500;
			}
			break;
		}
	}
	igroup = moudles;
	if (igroup % 2 == 0)igroup += 1;

	//添加列,0---序号，第二个参数显示内容，第三个参数显示对齐方式，第四个参数表头显示的宽度
	m_ListCtrl.InsertColumn(1, _T("Distance(MM)"), LVCFMT_CENTER, 120);

	for (int i = 2; i <= igroup + 1; i++) {
		CString colTitle;
		if (i == (igroup + 1) / 2 + 1) {
			colTitle.Format(L"LampCenter");
			m_ListCtrl.InsertColumn(i, colTitle, LVCFMT_CENTER, 100);
		}
		else {
			if(moudles %2 == 0 && i > (igroup + 1) / 2 + 1)
				colTitle.Format(L"Module %d", i - 2);
			else
				colTitle.Format(L"Module %d", i - 1);

			m_ListCtrl.InsertColumn(i, colTitle, LVCFMT_CENTER, 90);
		}
		
	

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
	//插入行内容 尾部添加行
	srand(static_cast<unsigned int>(time(0)));
	for (int i = 0; i < 12; i++) {
		for (int j = 2; j <= igroup+1; j++) {
			m_ListCtrl.SetItemText(i, j, theApp.RandomFloat());
		}
	}
}
void PageList1::initChart() {
	pBottomAxis = m_ChartCtrl.CreateStandardAxis(CChartCtrl::BottomAxis);
	pBottomAxis->SetMinMax(-2, 105);
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
	title.Format(L"%s %s %s Irradiance On MD", theApp.getString(_T("sample"), _T("model")), theApp.getString(_T("sample"), _T("sn")), theApp.getString(_T("sample"), _T("wavelength")));
	m_ChartCtrl.GetTitle()->AddString(TChartString(title));

	m_ChartCtrl.GetTitle()->SetColor(RGB(255, 255, 255));   //标题字体白色
	m_ChartCtrl.GetLeftAxis()->SetTextColor(RGB(255, 255, 255));  //左坐标轴白色
	m_ChartCtrl.GetBottomAxis()->SetTextColor(RGB(255, 255, 255));  //底部坐标轴白色
	//m_ChartCtrl.GetRightAxis()->GetGrid()->SetColor(RGB(0, 255, 255));

	m_ChartCtrl.SetBorderColor(RGB(255, 255, 255));  //边框颜色白色
	m_ChartCtrl.SetBackColor(RGB(85, 85, 85));  //背景颜色深灰色

	pBarIrra = m_ChartCtrl.CreateBarSerie();

	COLORREF SerieColor = RGB(0, 255, 0);
	pBarIrra->SetColor(SerieColor);//   pLineSeries要在头文件中初始化CChartLineSerie *pLineSeries
	pBarIrra->SetBarWidth(20);
	//pBarIrra->SetGroupId(1);
	pBarIrra->SetName(L"光强");

	m_ChartCtrl.SetZoomEnabled(false);
	//m_ChartCtrl.RemoveAllSeries();//先清空


	/*for (int i = 0; i < 5; i++) {
		pBarIrra->AddPoint(i*10+1,i+10.0);
	}*/
}

void PageList1::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (isInited) {
		CRect rect;
		this->GetClientRect(&rect);
		rect.top +=60;
		rect.bottom = rect.top+460;
		rect.left += 5;
		rect.right = rect.left+(90*(igroup))+100+30;
		m_ListCtrl.MoveWindow(&rect);

		CRect rect1;
		rect1.top = rect.bottom + 10;
		rect1.bottom = rect1.top + 400;
		rect1.left = rect.left;
		rect1.right = rect.left + 800;
		m_ChartCtrl.MoveWindow(&rect1);

	}
	// TODO: 在此处添加消息处理程序代码
}
LRESULT PageList1::OnMyMessageSignal(WPARAM wParam, LPARAM data) {
	int msg_id = wParam;
	std::string* pMsg = reinterpret_cast<std::string*>(data);
	return 0;
}

void PageList1::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	string report = theApp.tostr(theApp.getString(_T("File"), _T("report_path")));
	report += theApp.makeFname("\\MD");
	DeleteFile(CString(report.c_str()));
	//listExport1(report);
	listExport2(report);
	insertImg(report);
	powerExport(report);
	tempExport(report);
	rawExport(report);
	::MessageBox(NULL, CString(report.c_str()), _T("导出成功！"), MB_OK);

}
void PageList1::listExport1(string fname)
{
	int count = m_ListCtrl.GetItemCount(); // 获取列表中的项目数
	if (count <= 0) return;

	string tplate = theApp.tostr(theApp.getString(_T("File"), _T("template_path")));
	tplate += "/Track_MD.xlsx";
	XLDocument doc;
	doc.open(tplate);
	auto wks = doc.workbook().worksheet("MD");

	int tabpos = 0;
	for (int i = 1; i < 1000; i++) {
		string txt = wks.cell(i, 1).getString();
		if (txt == "Test Data") {
			tabpos = i;
			break;
		}

	}
	int row = tabpos+4;

	for (int i = 0; i < 10; i++)
	{
		int col = 'B';

		for (int j = 2; j <= igroup + 1; j++) {
			CString text = m_ListCtrl.GetItemText(i, j);
			string valstr = "0.0";
			if (!text.IsEmpty()) {
				valstr = theApp.tostr(text);
			}

			char pos[32] = { 0 };
			sprintf(pos, "%c%d", col, row);
			//wks.cell(pos).value() = std::stof(valstr);
			wks.cell(pos).value() = valstr;
			col++;
		}
		row++;
	}

	{
		
		std::time_t t = std::time(nullptr);  // 获取当前时间
		std::tm* tm = std::localtime(&t);

		XLDateTime dt(*tm);
		wks.cell("B1").value() = dt;
	}
	
	doc.saveAs(fname);
	doc.close();
	return;

}

void PageList1::insertImg(string fname) {

	libxl::Book* book = xlCreateXMLBook();
	book->load(CString(fname.c_str()));
	libxl::Sheet* sheet = book->getSheet(0);
	if (sheet == NULL) return;
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

	string jpgfile = "tmp123.jpg";
	CString fname1(jpgfile.c_str());
	CRect rect;
	m_ChartCtrl.GetClientRect(&rect);
	m_ChartCtrl.SaveAsImage(TChartString(fname1), rect, 24);
	int id = book->addPicture(fname1);
	sheet->setPicture(tabpos, 0, id);
	DeleteFile(fname1);

	book->save(CString(fname.c_str()));

}

void PageList1::powerExport_bak(string outfname) {

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
	for (int32_t row = sheet->firstRow(); row < sheet->lastRow(); ++row)
	{
		CellType cell_type = sheet->cellType(row, 0);
		if (cell_type == CELLTYPE_STRING)
		{
			wstring ret = sheet->readStr(row, 0);
			if (ret == L"Power Data") {
				tabpos = row;
			}
		}
	}

	tabpos += 2;

	for (int i = 0; i < 10; i++)
	{

		for (int j = 1; j < 9; j++) {
		
			sheet->writeNum(tabpos + i, j, 10.01);

		}
	}

	book->save(CString(outfname.c_str()));

	return;
}
void PageList1::powerExport(string outfname) {

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

	/*for (int j = 1; j < 16 ; j ++) {

		for(int i=1;i<4;i++)
		{
			sheet->copyCell(0, 1, 0, j * 3 + i);
			CString header;
			header.Format(L"ch%d cur", j + 1);
			sheet->writeStr(0, j * 3 + i, header); 
		}
	}*/

	std::vector<RealStat_t> &powList = pdlg->m_ctrl_stat.powList;

	for (int i = 0; i < powList.size(); i++) {
		sheet->writeStr(i + 2, 0, theApp.tm2CStr(powList[i].tm));

		for (int j = 0; j < 16 * 3; j += 3) {
			sheet->writeNum(i+2, j+1, powList[i].chn[j/3].cur);
			sheet->writeNum(i + 2, j + 2, powList[i].chn[j / 3].pow);
			sheet->writeNum(i + 2, j + 3, powList[i].chn[j / 3].ntc);

		}
	}

	book->save(CString(outfname.c_str()));

	return;
}

void PageList1::tempExport(string outfname) {

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

void PageList1::rawExport(string outfname) {
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
	for (int k = 0; k < igroup; k++) {
		CString Linestr;
		Linestr.Format(L"Line %d", k + 1);
		sheet->copyCell(0,1,0,k+2);
		sheet->writeStr(0,k+2,Linestr);
	}

	int currow = 1;
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < iheight; j++) {
			sheet->writeNum(currow, 0, theApp.getHtByidx(i));
			sheet->writeNum(currow, 1, j);
			bool havedata = false;
			for (int k = 0; k < igroup; k++) {
				if (dataTab[i][k][j] > 0) {
					sheet->writeNum(currow, k + 2, dataTab[i][k][j]);
					havedata = true;
				}
			}
			if (havedata) {
				currow++;
			}
		}
		currow++;
	}

	book->save(CString(outfname.c_str()));
}

void PageList1::listExport2(string outfname) {
	int count = m_ListCtrl.GetItemCount(); // 获取列表中的项目数
	if (count <= 0) return;

	string tplate = theApp.tostr(theApp.getString(_T("File"), _T("template_path")));
	tplate += "/Track_MD.xlsx";
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
			if (ret == L"Test Data") {
				tabpos = row;
			}
		}
	}
	{
		CString maxTitle;
		maxTitle.Format(L"Max Irradiance @ %s (%s)", (theApp.getString(_T("X20"), _T("wavelength"))), (theApp.getString(_T("X20"), _T("irraunit"))));
		sheet->writeStr(tabpos + 2, 1, maxTitle);

	}

	tabpos += 3;
	for (int j = 1; j < igroup + 1; j++) {
		sheet->copyCell(tabpos, 1, tabpos, j);

		CString colTitle;
		if (j == (igroup + 1) / 2 ) {
			colTitle.Format(L"Lamp Center");
			sheet->writeStr(tabpos, j, colTitle);
		}
		else {
			if (moudles % 2 == 0 && j > (igroup + 1) / 2 + 1)
				colTitle.Format(L"Module %d", j - 1);
			else
				colTitle.Format(L"Module %d", j - 0);
			sheet->writeStr(tabpos, j, colTitle);

		}
	}

	tabpos += 1;

	for (int i = 0; i < 12; i++)
	{

		for (int j = 1; j < igroup + 1; j++) {
			if(j>1)
			sheet->copyCell(tabpos+i, 1, tabpos, j);

			CString text;
			text = m_ListCtrl.GetItemText(i, j+1);
			string valstr = "--";
			if (!text.IsEmpty()) {
				valstr = theApp.tostr(text);
			}
			sheet->writeNum(tabpos + i,j, std::stof(valstr));
			
		}
	}

	book->save(CString(outfname.c_str()));
	
	return;
}

void PageList1::testExport() {
	
	// 打开模板文件
	string tplate = theApp.tostr(theApp.getString(_T("File"), _T("template_path")));
	tplate += "/template1.xlsx";
	XLDocument doc;
	//doc.create("./Demo01.xlsx");
	doc.open(tplate);
	auto wks = doc.workbook().worksheet("Sheet1");

	for (int i = 4; i <= 14; i++) {

		for (int j = 'B'; j <= 'K'; j++) {
			char buf[32] = { 0 };
			sprintf(buf,"%c%d",j,i);
			string cpos = buf;
			wks.cell(buf).value() = "123321";

		}
		
	}

	string report = theApp.tostr(theApp.getString(_T("File"), _T("report_path")));
	report += "/20240619_1.xlsx";
	doc.saveAs(report);
	doc.close();
	return;

	wks.cell("A1").value() = 3.14159265358979323846;
	wks.cell("B1").value() = 42;
	wks.cell("C1").value() = "  Hello OpenXLSX!  ";
	wks.cell("D1").value() = true;
	wks.cell("E1").value() = std::sqrt(-2); // Result is NAN, resulting in an error value in the Excel spreadsheet.

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

	//string report = theApp.tostr(theApp.getString(_T("File"), _T("report_path")));
	report += "/20240619.xlsx";
	doc.saveAs(report);
	doc.close();

}


void PageList1::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	POSITION ps;
	int nIndex;
	ps = m_ListCtrl.GetFirstSelectedItemPosition();
	nIndex = m_ListCtrl.GetNextSelectedItem(ps);
	*pResult = 0;
}

CString PageList1::getColname(int col) {
	 
	CString colTitle;
	if (col == (igroup + 1) / 2 + 1) {
		colTitle.Format(L"LampCenter");
	}
	else {
		if (moudles % 2 == 0 && col > (igroup + 1) / 2 + 1)
			colTitle.Format(L"Module %d", col - 2);
		else
			colTitle.Format(L"Module %d", col - 1);
	}

	return colTitle;

	
}
void PageList1::OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int iCol = pNMLV->iSubItem;
	if (iCol >= 2 && iCol<= igroup+1) {

		pBarIrra->ClearSerie();
		//eftAxis->SetMinMax(0, 30+ iCol);
		pLeftAxis->SetAutomatic(true);
		m_ChartCtrl.GetTitle()->RemoveAll();

		CString title;
		title.Format(L"%s interval %s Irradiance On MD(%s)", L"UV3004 W267_3.8mm", L"395nm", getColname(iCol));
		m_ChartCtrl.GetTitle()->AddString(TChartString(title));


		for (int i = 0; i < 10; i++)
		{
			for (int j = 2; j <= igroup+1; j++) {
				if (iCol == j) {
					CString text;
					text = m_ListCtrl.GetItemText(i, j);
					string valstr = "0.0";
					if (!text.IsEmpty()) {
						valstr = theApp.tostr(text);
					}

					pBarIrra->AddPoint((i + 1) * 10.0, std::stof(valstr));
					break;
				}

			}

		}
	}
	*pResult = 0;
}
