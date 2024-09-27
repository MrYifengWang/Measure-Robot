// PageTrack2.cpp: 实现文件
//

#include "pch.h"
#include "MeasureBot.h"
#include "PageTrack2.h"
#include "afxdialogex.h"

#define CLR_TRACK (RGB(0, 255, 0))
#define CLR_TRACK_LINE ( RGB(255, 0, 0))
#define CLR_TRACK_LINE_POS ( RGB(0, 135, 189))
#define CLR_SAMPLE (RGB(255, 255, 224))
#define CLR_BED (RGB(192, 192,192))
#define CLR_BED1 (RGB(172, 172,172))
#define CLR_AXIS (RGB(75,92,196))

// PageTrack2 对话框
#define USE_CENTER

IMPLEMENT_DYNAMIC(PageTrack2, CDialogEx)

PageTrack2::PageTrack2(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOGTrack2, pParent)
{

}

PageTrack2::~PageTrack2()
{
}

void PageTrack2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN1, sp_group);
	DDX_Control(pDX, IDC_SPIN2, sp_gap);
	DDX_Control(pDX, IDC_SPIN3, sp_height);
	DDX_Control(pDX, IDC_SPIN4, sp_width);
	DDX_Control(pDX, IDC_COMBO1, m_models);
	DDX_Control(pDX, IDC_SPIN5, sp_group1);
	DDX_Control(pDX, IDC_SPIN6, sp_gap1);
	DDX_Control(pDX, IDC_SPIN7, sp_driftx);
	DDX_Control(pDX, IDC_SPIN8, sp_drifty);
	DDX_Control(pDX, IDC_COMBO2, m_gap_idx);
	DDX_Control(pDX, IDC_EDIT10, m_gap_val);
	DDX_Control(pDX, IDC_CHECK1, m_checkAddon);
}


BEGIN_MESSAGE_MAP(PageTrack2, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &PageTrack2::OnBnClickedSave)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO1, &PageTrack2::OnCbnSelchangeCombo1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &PageTrack2::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, &PageTrack2::OnDeltaposSpin2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN3, &PageTrack2::OnDeltaposSpin3)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN4, &PageTrack2::OnDeltaposSpin4)
	ON_EN_CHANGE(IDC_EDIT1, &PageTrack2::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &PageTrack2::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, &PageTrack2::OnEnChangeEdit3)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN5, &PageTrack2::OnDeltaposSpin5)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN6, &PageTrack2::OnDeltaposSpin6)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN7, &PageTrack2::OnDeltaposSpin7)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN8, &PageTrack2::OnDeltaposSpin8)
	ON_BN_CLICKED(IDC_CHECK1, &PageTrack2::OnBnClickedCheck1)
	ON_EN_CHANGE(IDC_EDIT10, &PageTrack2::OnEnChangeEdit10)
	ON_CBN_SELCHANGE(IDC_COMBO2, &PageTrack2::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()


// PageTrack2 消息处理程序


BOOL PageTrack2::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	pdlg = (CMeasureBotDlg*)AfxGetMainWnd();

	for (int i = 0; i < 100; i++) {
		m_gapAddon[i] = 0.2;
	}
	//m_gapAddon[0] = 0.2

	m_led_height = theApp.getInt(_T("sample"), _T("height"));
	m_led_width = theApp.getInt(_T("sample"), _T("width"));
	m_mod_width = theApp.getInt(_T("sample"), _T("modwidth"));

	sp_driftx.SetRange32(-300, 300);    //调节的范围
	sp_driftx.SetBase(10); //十进制
	sp_driftx.SetBuddy(GetDlgItem(IDC_EDIT8));    //设置关联的编辑框

	sp_drifty.SetRange32(-300, 300);    //调节的范围
	sp_drifty.SetBase(10); //十进制
	sp_drifty.SetBuddy(GetDlgItem(IDC_EDIT9));    //设置关联的编辑框

	sp_group1.SetRange32(0, 30);    //调节的范围
	sp_group1.SetBase(10); //十进制
	sp_group1.SetBuddy(GetDlgItem(IDC_EDIT6));    //设置关联的编辑框

	sp_group.SetRange32(0, 30);    //调节的范围
	sp_group.SetBase(10); //十进制
	sp_group.SetBuddy(GetDlgItem(IDC_EDIT1));    //设置关联的编辑框

	sp_gap.SetRange32(0, 200);    //调节的范围
	sp_gap.SetBase(10); //十进制
	sp_gap.SetBuddy(GetDlgItem(IDC_EDIT2));    //设置关联的编辑框

	sp_gap1.SetRange32(0, 200);    //调节的范围
	sp_gap1.SetBase(10); //十进制
	sp_gap1.SetBuddy(GetDlgItem(IDC_EDIT7));    //设置关联的编辑框


	sp_height.SetRange32(0, 1000);    //调节的范围
	sp_height.SetBase(10); //十进制
	sp_height.SetBuddy(GetDlgItem(IDC_EDIT3));    //设置关联的编辑框

	sp_width.SetRange32(0, 1000);    //调节的范围
	sp_width.SetBase(10); //十进制
	sp_width.SetBuddy(GetDlgItem(IDC_EDIT4));    //设置关联的编辑框

	GetDlgItem(IDC_EDIT1)->SetWindowTextW(L"0");
	GetDlgItem(IDC_EDIT2)->SetWindowTextW(L"0");
	GetDlgItem(IDC_EDIT3)->SetWindowTextW(L"0");
	GetDlgItem(IDC_EDIT4)->SetWindowTextW(L"0");
	GetDlgItem(IDC_EDIT7)->SetWindowTextW(L"0");
	GetDlgItem(IDC_EDIT6)->SetWindowTextW(L"0");

	sp_driftx.SetPos(0);
	sp_drifty.SetPos(0);
	sp_group.SetPos(5);
	sp_group1.SetPos(5);
	sp_gap.SetPos(50);
	sp_gap1.SetPos(50);

	sp_height.SetPos(300);
	sp_width.SetPos(10);


	m_models.InsertString(0, _T("横向扫描"));
	m_models.InsertString(1, _T("纵向扫描"));
	m_models.InsertString(2, _T("网格定点"));
	m_models.SetWindowTextW(_T("选择模式"));
	//m_models.SetCurSel(0);

	m_led_driftx = theApp.getInt(_T("sample"), _T("driftx"));
	m_led_drifty = theApp.getInt(_T("sample"), _T("drifty"));

	isInited = 1;
	//SetTimer(1, 2000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

LRESULT PageTrack2::OnMyMessageSignal(WPARAM wParam, LPARAM data) {
	int msg_id = wParam;
	std::string* pMsg = reinterpret_cast<std::string*>(data);
	return 0;
}

void PageTrack2::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (isInited) {
		CRect rect;
		{
			rect.left = 10;
			rect.right = 1310;
			rect.top = 150;
			rect.bottom = 750;
		}
		GetDlgItem(IDC_STATIC)->MoveWindow(&rect);
		InitDC();
	}
}

void PageTrack2::onLedChanged() {
	adjustTrack();
	if (m_models.GetCurSel() == 1) {
		pdlg->m_page_list.resetList();
	}
	else if (m_models.GetCurSel() == 0) {
		pdlg->m_page_list2.resetList();
	}
	else if (m_models.GetCurSel() == 2) {
		pdlg->m_page_list3.resetList();
	}
}


void PageTrack2::OnBnClickedSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CString nostr;
	GetDlgItem(IDC_EDIT5)->GetWindowText(nostr);
	if (nostr.GetLength() == 0) {
		::MessageBox(NULL, _T("程序号不能为空!"), _T("ERROR"), MB_OK);
		return;
	}

	string trackno = CT2A(nostr);
	writeTracklist(trackno);
	if (m_models.GetCurSel() == 1) {
		pdlg->m_page_list.resetList();
	}else if (m_models.GetCurSel() == 0) {
		pdlg->m_page_list2.resetList();
	}
	else if (m_models.GetCurSel() == 2) {
		pdlg->m_page_list3.resetList();
	}

	pdlg->m_ctrl_pages.SetCurSel(1);
	pdlg->changeCtrlTab();
	pdlg->m_ctrl_mc.switchPan(2);
	pdlg->m_ctrl_mc.m_models.SetCurSel(m_models.GetCurSel());
	pdlg->m_ctrl_mc.m_height_list.SetCurSel(0);
	pdlg->m_ctrl_mc.OnCbnSelchangeCombo2();


}

void PageTrack2::refreshDC() {
	CRect rect;
	GetDlgItem(IDC_STATIC)->GetClientRect(&rect);
	CDC* pDC = GetDlgItem(IDC_STATIC)->GetDC();// GetWindowDC();
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), m_pMemoryDC, 0, 0, SRCCOPY);
	ReleaseDC(pDC);
}

void PageTrack2::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	if (m_pMemoryDC == NULL) return;
	refreshDC();

	//	updateTest();

}
void PageTrack2::InitDC()  // 双缓冲绘图
{
	if (m_pMemoryDC != NULL)
		return;
	m_pMemoryDC = new CDC();
	CRect rect;
	GetDlgItem(IDC_STATIC)->GetClientRect(&rect);

	m_pMemoryBitmap.CreateCompatibleBitmap(GetDlgItem(IDC_STATIC)->GetDC(), rect.Width(), rect.Height()); // 根据需要设置尺寸
	m_pMemoryDC->CreateCompatibleDC(GetDlgItem(IDC_STATIC)->GetDC());
	m_pMemoryDC->SelectObject(&m_pMemoryBitmap);

	if (0) {
		//m_pMemoryDC->SetMapMode(MM_LOMETRIC); // 设置为MM_LOMETRIC，以便左下角为(0,0)

		//XFORM form;
		//form.eM11 = 1;
		//form.eM12 = 0;
		//form.eM21 = -1;
		//form.eM22 = 1;
		//form.eDx = m_pMemoryDC->GetDeviceCaps(HORZRES);
		//form.eDy = 0;

		//m_pMemoryDC->SetWorldTransform(&form);


		XFORM xform;
		// 设置原点在左下角
		xform.eM11 = 1;
		xform.eM12 = 0;
		xform.eM21 = 0;
		xform.eM22 = 1;
		xform.eDx = rect.right;
		xform.eDy = rect.bottom;

		// 应用变换
		m_pMemoryDC->SetWorldTransform(&xform);
	}
	{

	}


	// 清除内存DC背景，以防背景图案影响绘图
	initAera();

	// 绘制你的图形
//	m_pMemoryDC->Ellipse(0, rt(0), 100, rt(100));
	// 将内存DC的内容绘制到对话框客户区
	refreshDC();

	//m_pMemoryDC->SetWorldTransform(&oldTransform); // 完成操作后恢复原始世界变换矩阵

	drawLed();

}

BOOL PageTrack2::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	return CDialogEx::DestroyWindow();

}


void PageTrack2::OnDestroy()
{
	CDialogEx::OnDestroy();

	// 删除对象并清理内存
	m_pMemoryDC->DeleteDC();
	delete m_pMemoryDC;
	m_pMemoryBitmap.DeleteObject();
	// TODO: 在此处添加消息处理程序代码
}


void PageTrack2::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	static int count = 0;
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		if (count < m_testlist.size()) {
			updated = 1;
			curX = m_testlist[count].x;
			curY = m_testlist[count].y;
			this->adjustTrack();
		}
		count++;
	}

	CDialogEx::OnTimer(nIDEvent);
}

int PageTrack2::nextsetp(char dir, int dis, CPoint& pt,char type,int step)
{
	//m_pMemoryDC->MoveTo(pt.x, pt.y);
	if (type == 'Y') {
		if (dir == 'y' && (dis > 0 ||(igroup%2==0 && step +1 == igroup/2))) {
			CBrush newbrush;
			newbrush.CreateSolidBrush(CLR_TRACK_LINE);//画刷
			m_pMemoryDC->SelectObject(&newbrush);
			m_pMemoryDC->Rectangle(pt.x - 2, pt.y - dis, pt.x + 2, pt.y);
			newbrush.DeleteObject();

		}
	}
	else if (type == 'X') {
		if (dir == 'x' && dis > 0) {
			CBrush newbrush;
			newbrush.CreateSolidBrush(CLR_TRACK_LINE);//画刷
			m_pMemoryDC->SelectObject(&newbrush);
			m_pMemoryDC->Rectangle(pt.x , pt.y - 2, pt.x + dis, pt.y+2);
			newbrush.DeleteObject();

		}
	}
	

	if (dir == 'x') {
		pt.x += dis;
	}
	else if (dir == 'y') {
		pt.y -= dis;
	}
	m_pMemoryDC->LineTo(pt);
	return 0;
}
int PageTrack2::nextsetp1(char dir, int dis, CPoint& pt, CPoint& end)
{
	if (dir == 'x') {
		pt.x += dis;
	}
	else if (dir == 'y') {
		pt.y -= dis;
	}
	DebugLog::writeLogF("---x=%d      y=%d---- %d %d", pt.x, pt.y, end.x, end.y);
	if (pt.x == end.x && pt.y == end.y) {
		m_pMemoryDC->LineTo(pt);
	}
	else {
		//return -1;
	}
	return 0;
}

void PageTrack2::drawAxis() {

	CPen pen(PS_SOLID, 2, CLR_AXIS);
	m_pMemoryDC->SelectObject(&pen);
	//x
	m_pMemoryDC->MoveTo(xbed - 4, rt(ybed - 4));
	m_pMemoryDC->LineTo(1270, rt(ybed - 4));
	//y
	m_pMemoryDC->MoveTo(xbed - 4, rt(ybed - 4));
	m_pMemoryDC->LineTo(xbed - 4, rt(570));

	m_pMemoryDC->TextOutW(0, rt(ybed-5), L"(-600,-250)");
	m_pMemoryDC->TextOutW(5, rt(590), L"+Y");
	m_pMemoryDC->TextOutW(1250, rt(ybed - 10), L"+X");

	{
		CBrush newbrush;
		newbrush.CreateSolidBrush(CLR_AXIS);//画刷
		m_pMemoryDC->SelectObject(&newbrush);
		m_pMemoryDC->Ellipse(xbed + 600 - 5, rt(ybed + 250) - 5, xbed + 600 + 5, rt(ybed + 250) + 5);
		newbrush.DeleteObject();
	}
	
	//x
	for (int i = 1; i <= 11; i++)
	{
		m_pMemoryDC->MoveTo(i * 100 + xbed, rt(ybed - 4));
		m_pMemoryDC->LineTo(i * 100 + xbed, rt(ybed - 9));
		CString ykd;
		ykd.Format(L"%d", i * 100-600);
		m_pMemoryDC->TextOutW(i * 100 - 8 + xbed, rt(ybed - 10), ykd);
	}
	//y
	for (int i = 1; i <= 10; i++)
	{
		m_pMemoryDC->MoveTo(xbed - 4, rt(i * 50  + ybed));
		m_pMemoryDC->LineTo(xbed - 9, rt(i * 50   +ybed));
		CString ykd;
		ykd.Format(L"%d", i * 50-250);
		m_pMemoryDC->TextOutW(5, rt(i * 50 + 4 + ybed), ykd);
	}
	pen.DeleteObject();

}


void PageTrack2::drawAxis1() {

	CPen pen(PS_SOLID, 2, CLR_AXIS);
	m_pMemoryDC->SelectObject(&pen);
	//x
	m_pMemoryDC->MoveTo(xbed - 4, rt(ybed - 4));
	m_pMemoryDC->LineTo(1270, rt(ybed - 4));
	//y
	m_pMemoryDC->MoveTo(xbed - 4, rt(ybed - 4));
	m_pMemoryDC->LineTo(xbed - 4, rt(570));

	m_pMemoryDC->TextOutW(5, rt(ybed), L"(0,0)");
	m_pMemoryDC->TextOutW(5, rt(590), L"+Y");
	m_pMemoryDC->TextOutW(1250, rt(ybed - 10), L"+X");

	//x
	for (int i = 1; i <= 11; i++)
	{
		m_pMemoryDC->MoveTo(i * 100 + xbed, rt(ybed - 4));
		m_pMemoryDC->LineTo(i * 100 + xbed, rt(ybed - 9));
		CString ykd;
		ykd.Format(L"%d", i * 100);
		m_pMemoryDC->TextOutW(i * 100 - 8 + xbed, rt(ybed - 10), ykd);
	}
	//y
	for (int i = 1; i <= 5; i++)
	{
		m_pMemoryDC->MoveTo(xbed - 4, rt(i * 100 + ybed));
		m_pMemoryDC->LineTo(xbed - 9, rt(i * 100 + ybed));
		CString ykd;
		ykd.Format(L"%d", i * 100);
		m_pMemoryDC->TextOutW(5, rt(i * 100 + 4 + ybed), ykd);
	}
	pen.DeleteObject();


}

void PageTrack2::initAera() {
	CRect rect;
	GetDlgItem(IDC_STATIC)->GetClientRect(&rect);
	m_pMemoryDC->FillSolidRect(0, 0, rect.Width(), rect.Height(), CLR_BED);

	CBrush newbrush;
	newbrush.CreateSolidBrush(CLR_BED1);//画刷
	CPen pen(PS_SOLID, 2, CLR_BED1);
	m_pMemoryDC->SelectObject(&pen);

	m_pMemoryDC->SelectObject(&newbrush);
	m_pMemoryDC->Rectangle(xbed, rt(ybed), xbed + rect.right - 96, rt(ybed + rect.bottom - 96));
	newbrush.DeleteObject();
	pen.DeleteObject();

	//	refreshDC();
	drawAxis();
}

void PageTrack2::drawBed() {
	CRect rect;
	GetDlgItem(IDC_STATIC)->GetClientRect(&rect);

	CBrush newbrush;
	newbrush.CreateSolidBrush(CLR_BED1);//画刷
	CPen pen(PS_SOLID, 2, CLR_BED1);
	m_pMemoryDC->SelectObject(&pen);

	m_pMemoryDC->SelectObject(&newbrush);
	m_pMemoryDC->Rectangle(xbed, rt(ybed), xbed + rect.right - 100, rt(ybed + rect.bottom - 100));
	newbrush.DeleteObject();
	pen.DeleteObject();

	refreshDC();
}

void PageTrack2::drawLed() {

	CBrush newbrush;
	newbrush.CreateSolidBrush(CLR_SAMPLE);//画刷
	CPen pen(PS_SOLID, 2, CLR_SAMPLE);
	m_pMemoryDC->SelectObject(&pen);

	m_pMemoryDC->SelectObject(&newbrush);
	m_pMemoryDC->Rectangle(xbed+ (xCt- m_led_width/2)+m_led_driftx, rt(ybed+(yCt- m_led_height/2)+ m_led_drifty), xbed + (xCt+m_led_width/2)+ m_led_driftx, rt(ybed + (yCt+m_led_height/2)+ m_led_drifty));
	//m_pMemoryDC->Rectangle(0, rt(0),  m_led_width, rt( m_led_height));
	newbrush.DeleteObject();
	pen.DeleteObject();
	{
		int cenX = xbed + (xCt - m_led_width / 2) + m_led_driftx + m_led_width / 2;
		int cenY = rt(ybed + (yCt - m_led_height / 2) + m_led_drifty + m_led_height / 2);
		CBrush newbrush;
		newbrush.CreateSolidBrush(CLR_AXIS);//画刷
		m_pMemoryDC->SelectObject(&newbrush);
		m_pMemoryDC->Ellipse(cenX - 5, rt(cenY) - 5, cenX + 5, rt(cenY) + 5);
		newbrush.DeleteObject();
	}

	refreshDC();
};

void PageTrack2::drawLed1() {

	CBrush newbrush;
	newbrush.CreateSolidBrush(CLR_SAMPLE);//画刷
	CPen pen(PS_SOLID, 2, CLR_SAMPLE);
	m_pMemoryDC->SelectObject(&pen);

	m_pMemoryDC->SelectObject(&newbrush);
	m_pMemoryDC->Rectangle(xbed, rt(ybed), xbed + m_led_width, rt(ybed + m_led_height));
	//m_pMemoryDC->Rectangle(0, rt(0),  m_led_width, rt( m_led_height));
	newbrush.DeleteObject();
	pen.DeleteObject();

	refreshDC();
};
void PageTrack2::drawTrackX() {
	CPen pen(PS_SOLID, 1, CLR_TRACK_LINE);
	m_pMemoryDC->SelectObject(&pen);

	m_pMemoryDC->MoveTo(dx(idriftx), rt(dy(idrifty)));
	CPoint point(dx(idriftx), rt(dy(idrifty)));

	for (int i = 0; i < igroup; i++) {
		nextsetp('y', igap, point);
		nextsetp('x', iheight, point);
		nextsetp('y', igap, point);
		nextsetp('x', -1 * iheight, point);
		
	}
	pen.DeleteObject();
	refreshDC();

}
void PageTrack2::drawTrackY() {
	CPen pen(PS_SOLID, 1, CLR_TRACK_LINE);
	m_pMemoryDC->SelectObject(&pen);

	m_pMemoryDC->MoveTo(dx(idriftx), rt(dy(idrifty)));

	CPoint point(dx(idriftx), rt(dy(idrifty)));

	for (int i = 0; i < igroup; i++) {
		nextsetp('x', igap, point,'Y');
		nextsetp('y', iheight, point, 'Y',i);
		nextsetp('x', igap, point, 'Y');
		nextsetp('y', -1 * iheight, point, 'Y',i);
		//nextsetp('x', igap, point);
	}
	pen.DeleteObject();
	refreshDC();
};
void PageTrack2::drawTrack3() {
	CPen pen(PS_SOLID, 2, CLR_TRACK_LINE);
	m_pMemoryDC->SelectObject(&pen);

	m_pMemoryDC->MoveTo(dx(idriftx), rt(dy(idrifty)));
	CBrush newbrush;
	newbrush.CreateSolidBrush(CLR_BED);//画刷
	m_pMemoryDC->SelectObject(&newbrush);
	int sx = dx(idriftx);
	for (int i = 0; i < igroup; i++) {
		int sy = rt(dy(idrifty)+5);
		for (int j = 0; j < igroup1; j++) {
			m_pMemoryDC->Ellipse(sx - 5, sy - 5, sx + 5, sy + 5);
			sy -= (igap1);
		}
		sx += igap;

	}
	newbrush.DeleteObject();
	pen.DeleteObject();

	refreshDC();
};
void PageTrack2::drawRealPos() {
	//if (updated == 0) return;

	CPen pen(PS_SOLID, 2, CLR_TRACK_LINE_POS);
	m_pMemoryDC->SelectObject(&pen);
	CBrush newbrush;
	newbrush.CreateSolidBrush(CLR_TRACK_LINE_POS);//画刷
	m_pMemoryDC->SelectObject(&newbrush);

	int px = (600+50+curX);// +(m_led_width / 2));// curX;
	//int py = rt(dy(curY));// +(m_led_height / 2)));// curY;
	int py = rt(dy(curY+ m_led_height / 2));
	m_pMemoryDC->Ellipse(px - 8, py - 8, px + 8, py + 8);
	refreshDC();
	updated = 0;
	newbrush.DeleteObject();
	pen.DeleteObject();

	return;
}
void PageTrack2::drawRealPoint() {
	CPen pen(PS_SOLID, 2, CLR_TRACK_LINE_POS);
	m_pMemoryDC->SelectObject(&pen);
	CBrush newbrush;
	newbrush.CreateSolidBrush(CLR_TRACK_LINE_POS);//画刷
	m_pMemoryDC->SelectObject(&newbrush);

	int sx = dx(idriftx);
	for (int i = 0; i < igroup; i++) {
		int sy = rt(dy(idrifty));
		for (int j = 0; j < igroup1; j++) {
			if (curX == i && curY == j) {
				m_pMemoryDC->Ellipse(sx - 8, sy - 8, sx + 8, sy + 8);

				CString tmpstr;
				tmpstr.Format(L"%d", sx - 650);
				pdlg->m_ctrl_stat.GetDlgItem(IDC_EDIT5)->SetWindowTextW(tmpstr);

				tmpstr.Format(L"%d", (sy-300)*-1);
				pdlg->m_ctrl_stat.GetDlgItem(IDC_EDIT6)->SetWindowTextW(tmpstr);
				break;
			}
			sy -= (igap1);
		}
		sx += igap;
	}
	refreshDC();
	updated = 0;
	newbrush.DeleteObject();
	pen.DeleteObject();

	return;
}

void PageTrack2::onPosVal(float x, float y, string axis, float dist, int idx) {
	curX = x;
	curY = y;
	this->adjustTrack();
}
void PageTrack2::drawEndpoint() {};

void PageTrack2::updateTrackSel() {
	int idx = m_models.GetCurSel();
	Json::Value list;
	readTracklist(list);
	Json::Value arr = list["list"];
	for (int i = 0; i < arr.size(); i++) {
		int type = arr[i]["model"].asInt();
		if (type == idx) {
			string nostr = arr[i]["id"].asString();
			GetDlgItem(IDC_EDIT5)->SetWindowTextW(CString(nostr.c_str()));

			igroup = arr[i]["igroup"].asInt();
			igroup1 = arr[i]["igroup1"].asInt();
			idriftx = arr[i]["idriftx"].asInt();
			idrifty = arr[i]["idrifty"].asInt();
			igap = arr[i]["igap"].asInt();
			igap1 = arr[i]["igap1"].asInt();
			iheight = arr[i]["iheight"].asInt();

			sp_driftx.SetPos(0);
			sp_driftx.SetPos(idriftx);
			sp_drifty.SetPos(idrifty);
			sp_group.SetPos(igroup);
			sp_group1.SetPos(igroup1);
			sp_gap.SetPos(igap);
			sp_gap1.SetPos(igap1);
			sp_height.SetPos(iheight);

			break;
		}
	}


	if (idx == 0) {
		GetDlgItem(IDC_EDIT6)->EnableWindow(false);
		GetDlgItem(IDC_SPIN5)->EnableWindow(false);
		//GetDlgItem(IDC_EDIT7)->EnableWindow(false);
		GetDlgItem(IDC_SPIN6)->EnableWindow(false);
		m_checkAddon.SetCheck(false);
		m_checkAddon.ShowWindow(false);
		m_gap_idx.ShowWindow(false);
		m_gap_val.ShowWindow(false);

		/*m_testlist.clear();
		CPoint point(dx(idriftx), rt(dy(idrifty)));
		for (int i = 0; i < igroup; i++) {
			point.y -= igap;
			m_testlist.push_back(point);
			point.x+=iheight;
			m_testlist.push_back(point);
			point.y -= igap;
			m_testlist.push_back(point);
			point.x += -1*iheight;
			m_testlist.push_back(point);
		
		}
		for (int i = 0; i < m_testlist.size(); i++) {
			DebugLog::writeLogF("idx=%d x=%d y=%d",i, m_testlist[i].x, m_testlist[i].y);
		}
		KillTimer(1);
		SetTimer(1, 2000, NULL);*/


		initAera();
		drawLed();
		drawTrackX();
	}
	else if (idx == 1) {
		GetDlgItem(IDC_EDIT6)->EnableWindow(false);
		GetDlgItem(IDC_SPIN5)->EnableWindow(false);
		//GetDlgItem(IDC_EDIT7)->EnableWindow(false);
		GetDlgItem(IDC_SPIN6)->EnableWindow(false);
		m_checkAddon.ShowWindow(true);
		initAera();
		drawLed();
		drawTrackY();

	}
	else if (idx == 2) {
		GetDlgItem(IDC_EDIT6)->EnableWindow(true);
		GetDlgItem(IDC_SPIN5)->EnableWindow(true);
		GetDlgItem(IDC_EDIT7)->EnableWindow(true);
		GetDlgItem(IDC_SPIN6)->EnableWindow(true);
		//m_checkAddon.SetCheck(false);
		bool sflag = false;
		m_checkAddon.ShowWindow(sflag);//true
		m_gap_idx.ShowWindow(sflag);
		m_gap_val.ShowWindow(sflag);
		initAera();
		drawLed();
		drawTrack3();

	}
}


void PageTrack2::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	updateTrackSel();

}

void PageTrack2::onSampleSize()
{

}

void PageTrack2::adjustTrack()
{
	int idx = m_models.GetCurSel();

	initAera();
	drawLed();
	if (idx == 0) {
		drawTrackX();
		drawRealPos();
	}
	else if (idx == 1) {
		drawTrackY();
		drawRealPos();
	}
	else if (idx == 2) {
		drawTrack3();
		drawRealPoint();
	}
}



void PageTrack2::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	int curval = sp_group.GetPos() + pNMUpDown->iDelta;
	if (igroup != curval) {
		igroup = curval;
		adjustTrack();
	}

}


void PageTrack2::OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	int curval = sp_gap.GetPos()+pNMUpDown->iDelta;
	if (igap != curval) {
		igap = curval;
		adjustTrack();
	}
}


void PageTrack2::OnDeltaposSpin3(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	int curval = sp_height.GetPos()+ pNMUpDown->iDelta;
	if (iheight != curval) {
		iheight = curval;
		adjustTrack();
	}
}


void PageTrack2::OnDeltaposSpin4(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void PageTrack2::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString strValue;
	GetDlgItem(IDC_EDIT1)->GetWindowText(strValue);
	int intValue = _ttoi(strValue);
}


void PageTrack2::OnEnChangeEdit2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString strValue;
	GetDlgItem(IDC_EDIT2)->GetWindowText(strValue);
	int intValue = _ttoi(strValue);
}


void PageTrack2::OnEnChangeEdit3()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString strValue;
	GetDlgItem(IDC_EDIT3)->GetWindowText(strValue);
	int intValue = _ttoi(strValue);
}

void PageTrack2::checkEditInput() {
	int updated = 0;
	CString strValue;
	int intValue;
	int curval;
	{
		int low, up;
		GetDlgItem(IDC_EDIT3)->GetWindowText(strValue);
		intValue = _ttoi(strValue);
		sp_height.GetRange32(low, up);
		if (intValue >= low && intValue <= up) {
			if (iheight != intValue) {
				//sp_height.SetPos(intValue);
				iheight = intValue;
				updated = 1;
			}
		}
	}
	{
		int low, up;
		GetDlgItem(IDC_EDIT2)->GetWindowText(strValue);
		intValue = _ttoi(strValue);
		sp_gap.GetRange32(low, up);
		if (intValue >= low && intValue <= up) {
			if (igap != intValue) {
				//sp_gap.SetPos(intValue);
				igap = intValue;
				updated = 1;
			}
		}
	}
	{
		int low, up;
		GetDlgItem(IDC_EDIT7)->GetWindowText(strValue);
		intValue = _ttoi(strValue);
		sp_gap1.GetRange32(low, up);
		if (intValue >= low && intValue <= up) {
			if (igap1 != intValue) {
				//sp_gap.SetPos(intValue);
				igap1 = intValue;
				updated = 1;
			}
		}
	}
	{
		int low, up;
		GetDlgItem(IDC_EDIT1)->GetWindowText(strValue);
		intValue = _ttoi(strValue);
		sp_group.GetRange32(low, up);
		if (intValue >= low && intValue <= up) {
			if (igroup != intValue) {
				//sp_group.SetPos(intValue);
				igroup = intValue;
				updated = 1;
			}
		}
	}
	{
		int low, up;
		GetDlgItem(IDC_EDIT6)->GetWindowText(strValue);
		intValue = _ttoi(strValue);
		sp_group1.GetRange32(low, up);
		if (intValue >= low && intValue <= up) {
			if (igroup1 != intValue) {
				//sp_group.SetPos(intValue);
				igroup1 = intValue;
				updated = 1;
			}
		}
	}
	{
		int low, up;
		GetDlgItem(IDC_EDIT8)->GetWindowText(strValue);
		intValue = _ttoi(strValue);
		sp_driftx.GetRange32(low, up);
		if (intValue >= low && intValue <= up) {
			if (idriftx != intValue) {
				//sp_group.SetPos(intValue);
				idriftx = intValue;
				updated = 1;
			}
		}
	} {
		int low, up;
		GetDlgItem(IDC_EDIT9)->GetWindowText(strValue);
		intValue = _ttoi(strValue);
		sp_drifty.GetRange32(low, up);
		if (intValue >= low && intValue <= up) {
			if (idrifty != intValue) {
				//sp_group.SetPos(intValue);
				idrifty = intValue;
				updated = 1;
			}
		}
	}

	if (updated)
	{
		adjustTrack();
	}

}


BOOL PageTrack2::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case   VK_RETURN://屏蔽回车  
				//   OnKeyDown(VK_SPACE,   LOWORD(pMsg   ->lParam),   HIWORD(pMsg->lParam)); 
			checkEditInput();
			return   TRUE;
		case   VK_ESCAPE://屏蔽Esc  
			return   TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void PageTrack2::OnDeltaposSpin5(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	int curval = sp_group1.GetPos()+ pNMUpDown->iDelta;
	if (igroup1 != curval) {
		igroup1 = curval;
		adjustTrack();
	}
}


void PageTrack2::OnDeltaposSpin6(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	int curval = sp_gap1.GetPos()+ pNMUpDown->iDelta;
	if (igap1 != curval) {
		igap1 = curval;
		adjustTrack();
	}
}


void PageTrack2::OnDeltaposSpin7(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	
	int curval = sp_driftx.GetPos()+ pNMUpDown->iDelta;

	CString strValue;
	GetDlgItem(IDC_EDIT8)->GetWindowText(strValue);
	int intValue = _ttoi(strValue);
	if (idriftx != intValue) {
		idriftx = intValue;
		adjustTrack();
	}
}


void PageTrack2::OnDeltaposSpin8(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	/*int curval = sp_drifty.GetPos()+ pNMUpDown->iDelta;
	if (idrifty != curval) {
		idrifty = curval;
		adjustTrack();
		return;
	}*/

	CString strValue;
	GetDlgItem(IDC_EDIT9)->GetWindowText(strValue);
	int intValue = _ttoi(strValue);
	if (idrifty != intValue) {
		idrifty = intValue;
		adjustTrack();
	}
}

void PageTrack2::writeTracklist(string& id)
{
	char path[512];
	getcwd(path, 511);
	string confPath = path;
	confPath += "/TrackList.json";

	int flen = 0;
	char buf[10240] = { 0 };

	FILE* pFile = fopen(confPath.c_str(), "r");
	if (pFile != NULL)
	{
		flen = fread(buf, sizeof(char), 10240, pFile);
		fclose(pFile);
	}
	Json::Value mgaps;

	if (m_models.GetCurSel() == 1|| m_models.GetCurSel() == 2) {
		for (int j = 1; j < igap; j++) {
			Json::Value tmpitem;
			tmpitem["id"] = j;
			tmpitem["val"] = m_gapAddon[j];
			mgaps.append(tmpitem);
		}
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
				//string nostr = arr[i]["id"].asString();
				//if (nostr == id)
				int mtype = arr[i]["model"].asInt();
				if (mtype == m_models.GetCurSel())
				{
					arr[i]["id"] = id;
					arr[i]["model"] = m_models.GetCurSel();
					arr[i]["idriftx"] = idriftx;
					arr[i]["idrifty"] = idrifty;
					arr[i]["igroup"] = igroup;
					arr[i]["igroup1"] = igroup1;
					arr[i]["igap"] = igap;
					arr[i]["igap1"] = igap1;
					arr[i]["iheight"] = iheight;
					if (m_models.GetCurSel() == 1 || m_models.GetCurSel() == 2) {
						arr[i]["addons"] = mgaps;
					}
					found = true;
					break;
				}

			}

			if (!found)
			{
				Json::Value item;
				item["id"] = id;
				item["model"] = m_models.GetCurSel();
				item["idriftx"] = idriftx;
				item["idrifty"] = idrifty;
				item["igroup"] = igroup;
				item["igroup1"] = igroup1;
				item["igap"] = igap;
				item["igap1"] = igap1;
				item["iheight"] = iheight;
				if (m_models.GetCurSel() == 1 || m_models.GetCurSel() == 2) {
					item["addons"] = mgaps;
				}
				arr.append(item);
			}
			Json::Value root;
			Json::FastWriter writer;

			root["list"] = arr;
			string configStr = writer.write(root);
			pFile = fopen(confPath.c_str(), "w");
			fwrite(configStr.c_str(), 1, configStr.length(), pFile);
		}
	}
	else
	{
		Json::Value root;
		Json::Value arr;
		Json::Value item;

		item["id"] = id;
		item["model"] = m_models.GetCurSel();
		item["idriftx"] = idriftx;
		item["idrifty"] = idrifty;
		item["igroup"] = igroup;
		item["igroup1"] = igroup1;
		item["igap"] = igap;
		item["igap1"] = igap1;
		item["iheight"] = iheight;
		if (m_models.GetCurSel() == 1 || m_models.GetCurSel() == 2) {
			item["addons"] = mgaps;
		}
		arr.append(item);
		root["list"] = arr;


		Json::FastWriter writer;
		string configStr = writer.write(root);
		pFile = fopen(confPath.c_str(), "w");
		fwrite(configStr.c_str(), 1, configStr.length(), pFile);
	}
	fclose(pFile);
	return;

}

void PageTrack2::readTracklist(Json::Value& list)
{
	char path[512];
	getcwd(path, 511);
	string confPath = path;
	confPath += "/TrackList.json";

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
			Json::Value arr = jmessage["businessList"];
			int total = arr.size();
			for (int i = 0; i < total; i++)
			{
				Json::Value contitem = arr[i];
			}

			list = jmessage;
		}

	}

}

int PageTrack2::delTracklist(string& id)
{
	char path[512];
	getcwd(path, 511);
	string confPath = path;
	confPath += "/TrackList.json";

	char* buf = (char*)malloc(1024 * 64);
	int res = 0;
	FILE* pFile = fopen(confPath.c_str(), "r");
	if (!pFile) {
		free(buf);
		return -2;
	}
	int flen = fread(buf, sizeof(char), 1024 * 64, pFile);
	fclose(pFile);
	if (flen > 0) {

		Json::Value root;
		Json::Value newarr;

		Json::Reader reader;
		Json::Value jmessage;
		if (reader.parse(buf, jmessage)) {
			Json::Value arr = jmessage["list"];
			int total = arr.size();
			for (int i = 0; i < total; i++) {

				Json::Value contitem = arr[i];
				string idstr = contitem["id"].asString();
				if (idstr == id) {
					continue;
				}
				Json::Value item = contitem;
				newarr.append(item);
			}
		}
		root["list"] = newarr;
		Json::FastWriter writer;
		string configStr = writer.write(root);
		pFile = fopen(confPath.c_str(), "w");
		fwrite(configStr.c_str(), 1, configStr.length(), pFile);
		fclose(pFile);

	}
	free(buf);


}


void PageTrack2::OnBnClickedCheck1()
{
	int idx = m_models.GetCurSel();
	if (!(idx == 1 || idx == 2)) return;
	// TODO: 在此添加控件通知处理程序代码
	if (IsDlgButtonChecked(IDC_CHECK1)) {
		
		Json::Value list;
		readTracklist(list);
		Json::Value arr = list["list"];
		for (int i = 0; i < arr.size(); i++) {
			int type = arr[i]["model"].asInt();
			if (type == idx) {
				Json::Value addons = arr[i]["addons"];

				for (int j = 0; j < addons.size(); j++) {
					int gapNo = addons[j]["id"].asInt();
					float val = addons[j]["val"].asDouble();
					m_gapAddon[gapNo] = val;
				}
			}
		}
		m_gap_idx.ResetContent();
		m_gap_idx.InsertString(0, _T("UniVal"));
		for (int i = 1; i < igroup;i++) {
			CString txt;
			txt.Format(L"Mod %d",i);
			m_gap_idx.InsertString(i, txt);
		}
		m_gap_idx.SetCurSel(0);
		CString valtxt;
		valtxt.Format(L"%.1f", m_gapAddon[0]);
		m_gap_val.SetWindowTextW(valtxt);

		m_gap_idx.ShowWindow(true);
		m_gap_val.ShowWindow(true);
		m_use_addons = true;
	}
	else {
		m_gap_idx.ShowWindow(false);
		m_gap_val.ShowWindow(false);
		m_use_addons = false;
	}
}


void PageTrack2::OnEnChangeEdit10()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString strValue;
	m_gap_val.GetWindowText(strValue);
	float fVal = _ttof(strValue);

	int idx = m_gap_idx.GetCurSel();
	m_gapAddon[idx] = fVal;
	/*if (idx == 0) {
		for (int i = 0; i < igroup; i++) {
			m_gapAddon[i] = fVal;
		}
	}*/


}


void PageTrack2::OnCbnSelchangeCombo2()
{
	// TODO: 在此添加控件通知处理程序代码
	int idx = m_gap_idx.GetCurSel();
	CString valtxt;
	valtxt.Format(L"%.1f", m_gapAddon[idx]);
	m_gap_val.SetWindowTextW(valtxt);
}
