// PageConfig1.cpp: 实现文件
//

#include "pch.h"
#include "MeasureBot.h"
#include "PageConfig1.h"
#include "afxdialogex.h"


// PageConfig1 对话框

IMPLEMENT_DYNAMIC(PageConfig1, CDialogEx)

PageConfig1::PageConfig1(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CFG, pParent)
{

}

PageConfig1::~PageConfig1()
{
}

void PageConfig1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID1, m_Prop_conf);
}


BEGIN_MESSAGE_MAP(PageConfig1, CDialogEx)
	ON_NOTIFY(NM_THEMECHANGED, IDC_MFCPROPERTYGRID1, &PageConfig1::OnThemechangedMfcpropertygrid1)
	ON_STN_CLICKED(IDC_MFCPROPERTYGRID1, &PageConfig1::OnStnClickedMfcpropertygrid1)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_WM_SIZE()
END_MESSAGE_MAP()



// PageConfig1 消息处理程序
void PageConfig1::initProp() {

	m_Prop_conf.EnableHeaderCtrl(FALSE); //是否启用表头
	m_Prop_conf.EnableDescriptionArea(true); //是否启用描述功能
	m_Prop_conf.SetVSDotNetLook(true);
	m_Prop_conf.MarkModifiedProperties(); //是否着重显示修改

	//设置属性表格颜色
	m_Prop_conf.SetCustomColors(
		RGB(255, 255, 255),
		RGB(0, 0, 0),
		RGB(200, 200, 200),
		RGB(0, 0, 0),
		RGB(240, 240, 240),
		RGB(0, 0, 0),
		RGB(200, 200, 200));


	//更改表头宽度
	HDITEM item;
	item.cxy = 200;
	item.mask = HDI_WIDTH;
	m_Prop_conf.GetHeaderCtrl().SetItem(0, new HDITEM(item));
	//
	{
		CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("设备选项"));
		pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("启用XY轴"), (_variant_t)theApp.getBool(_T("system"), _T("thread_mc")), _T("Specifies that the window uses MS Shell Dlg font"), 1001));
		pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("启用X20光表"), (_variant_t)theApp.getBool(_T("system"), _T("thread_x20")), _T("Specifies that the window uses MS Shell Dlg font"), 1002));
		pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("启用L5000电源"), (_variant_t)theApp.getBool(_T("system"), _T("thread_Lx")), _T("Specifies that the window uses MS Shell Dlg font"), 1003));
		pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("启用流量采集"), (_variant_t)theApp.getBool(_T("system"), _T("thread_smacq")), _T("Specifies that the window uses MS Shell Dlg font"), 1004));
		m_Prop_conf.AddProperty(pGroup2);
	}

	{
		CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("测试要求"));

		{
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("工作距离（MM）"), theApp.getString(_T("requirement"), _T("distance")),
				_T("tips"), 4001);
			pGroup1->AddSubItem(pProp);
		}

		{
			pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("辐照度>=（W/cm2）"), (_variant_t)theApp.getInt(_T("requirement"), _T("irradiation")),
				_T("tips"), 4002));
		}

		{
			pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("均匀度>=（%）"), (_variant_t)theApp.getInt(_T("requirement"), _T("average")),
				_T("tips"), 4003));
		}

		m_Prop_conf.AddProperty(pGroup1);

	}

	{
		CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("电源"));

		{
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("采样频率(ms)"), (_variant_t)theApp.getInt(_T("L5000"), _T("sample")),
				_T(""), 3001);
			pGroup1->AddSubItem(pProp);
		}

		{
			pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("电流/功率(%)"), (_variant_t)theApp.getInt(_T("L5000"), _T("powerscale")),
				_T("tips"), 3002));
		}
		/*{
			pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("全部通道"), (_variant_t)theApp.getBool(_T("L5000"), _T("allchannel")), _T(""), 3003));
		}*/
		{
			CMFCPropertyGridProperty* pChannel = new CMFCPropertyGridProperty(_T("使能通道"), 0, TRUE);

			for (int i = 1; i <= 16; i++)
			{
				CString chidx;
				chidx.Format(L"ch%d", i);
				pChannel->AddSubItem(new CMFCPropertyGridProperty(chidx, (_variant_t)theApp.getBool(_T("L5000"), chidx), _T(""), 3010 + i));

			}

			pGroup1->AddSubItem(pChannel);

		}

		m_Prop_conf.AddProperty(pGroup1);

	}
	{
		CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("采集卡"));

		{
			pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("温度采样间隔（MS）"), (_variant_t)theApp.getInt(_T("Acquisition"), _T("temprate")),
				_T("tips"), 7001));
		}
		{
			pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("流量采样间隔（MS）"), (_variant_t)theApp.getInt(_T("Acquisition"), _T("flowrate")),
				_T("tips"), 7002));
		}
		{
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("流量计"), theApp.getString(_T("Acquisition"), _T("flower")),
				_T("One of:5-40L 2-16L"), 7003);
			pProp->AddOption(_T("5-40L"));
			pProp->AddOption(_T("2-16L"));
			pGroup1->AddSubItem(pProp);
		}

		m_Prop_conf.AddProperty(pGroup1);

	}
	{
		CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("通讯参数"));

		{
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("电源端口"), theApp.getString(_T("Communication"), _T("lxport")),
				_T("One of:com1,com2,com3,orcom4"), 6001);
			pProp->AddOption(_T("com1"));
			pProp->AddOption(_T("com2"));
			pProp->AddOption(_T("com3"));
			pProp->AddOption(_T("com4"));
			pProp->AddOption(_T("com5"));
			pProp->AddOption(_T("com6"));
			pProp->AddOption(_T("com7"));
			pProp->AddOption(_T("com8"));
			pProp->AddOption(_T("com9"));
			pProp->AddOption(_T("com10"));
			pProp->AddOption(_T("com11"));
			pProp->AddOption(_T("com12"));
			pGroup1->AddSubItem(pProp);
		}
		{
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("采集卡端口"), theApp.getString(_T("Communication"), _T("smacqport")),
				_T("One of:com1,com2,com3,orcom4"), 6002);
			pProp->AddOption(_T("com1"));
			pProp->AddOption(_T("com2"));
			pProp->AddOption(_T("com3"));
			pProp->AddOption(_T("com4"));
			pProp->AddOption(_T("com5"));
			pProp->AddOption(_T("com6"));
			pProp->AddOption(_T("com7"));
			pProp->AddOption(_T("com8"));
			pProp->AddOption(_T("com9"));
			pProp->AddOption(_T("com10"));
			pProp->AddOption(_T("com11"));
			pProp->AddOption(_T("com12"));
			pGroup1->AddSubItem(pProp);
		}
		/*{
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("光表端口"), theApp.getString(_T("Communication"), _T("x20port")),
				_T("One of:com1,com2,com3,orcom4"), 6003);
			pProp->AddOption(_T("com1"));
			pProp->AddOption(_T("com2"));
			pProp->AddOption(_T("com3"));
			pProp->AddOption(_T("com4"));
			pProp->AddOption(_T("com5"));
			pProp->AddOption(_T("com6"));
			pProp->AddOption(_T("com7"));
			pProp->AddOption(_T("com8"));
			pProp->AddOption(_T("com9"));

			pGroup1->AddSubItem(pProp);
		}*/

		{
			/*CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("RS485参数"), theApp.getString(_T("Communication"), _T("rs485")),
				_T("tips"), 6004);*/

			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("RS485参数"), 0, TRUE);
			{
				CMFCPropertyGridProperty* PItem = new CMFCPropertyGridProperty(_T("波特率"), theApp.getString(_T("Communication"), _T("baudrate")),
					_T(""), 6024);
				PItem->AddOption(_T("9600"));
				PItem->AddOption(_T("19200"));
				PItem->AddOption(_T("38400"));
				PItem->AddOption(_T("57600"));
				PItem->AddOption(_T("115200"));
				pProp->AddSubItem(PItem);
			}
			{
				CMFCPropertyGridProperty* PItem = new CMFCPropertyGridProperty(_T("校验位"), theApp.getString(_T("Communication"), _T("check")),
					_T(""), 6025);
				PItem->AddOption(_T("E"));
				PItem->AddOption(_T("O"));
				pProp->AddSubItem(PItem);
			}
			{
				CMFCPropertyGridProperty* PItem = new CMFCPropertyGridProperty(_T("数据位"), theApp.getString(_T("Communication"), _T("data")),
					_T(""), 6026);
				PItem->AddOption(_T("8"));
				PItem->AddOption(_T("7"));
				pProp->AddSubItem(PItem);
			}
			{
				CMFCPropertyGridProperty* PItem = new CMFCPropertyGridProperty(_T("停止位"), theApp.getString(_T("Communication"), _T("stop")),
					_T(""), 6027);
				PItem->AddOption(_T("0"));
				PItem->AddOption(_T("1"));
				pProp->AddSubItem(PItem);
			}


			pGroup1->AddSubItem(pProp);
		}
		{
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("运动卡IP"), theApp.getString(_T("Communication"), _T("mcIP")),
				_T("tips"), 6005);
			pGroup1->AddSubItem(pProp);
		}
		{
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("上位机IP"), theApp.getString(_T("Communication"), _T("hostIP")),
				_T("tips"), 6006);
			pGroup1->AddSubItem(pProp);
		}


		m_Prop_conf.AddProperty(pGroup1);

	}


	{
		CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("光表"));

		{
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("波长"), theApp.getString(_T("X20"), _T("wavelength")),
				_T("One of:365nm,375nm,385nm,395nm,or405nm"), 2001);
			/*CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("波长"), _T("365nm"),
				_T("One of:365nm,375nm,385nm,395nm,or405nm"),2001);*/
			pProp->AddOption(_T("365nm"));
			pProp->AddOption(_T("375nm"));
			pProp->AddOption(_T("385nm"));
			pProp->AddOption(_T("395nm"));
			pProp->AddOption(_T("405nm"));
			pProp->AllowEdit(FALSE);
			pGroup1->AddSubItem(pProp);
		}
		{
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("量程"), theApp.getString(_T("X20"), _T("range")),
				_T("One of:auto,0-6"), 2002);
			pProp->AddOption(_T("auto"));
			pProp->AddOption(_T("0"));
			pProp->AddOption(_T("1"));
			pProp->AddOption(_T("2"));
			pProp->AddOption(_T("3"));
			pProp->AddOption(_T("4"));
			pProp->AddOption(_T("5"));
			pProp->AddOption(_T("6"));
			pProp->AllowEdit(FALSE);
			pGroup1->AddSubItem(pProp);
		}

		{
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("测量模式"), theApp.getString(_T("X20"), _T("samplemode")),
				_T("One of:Continuous,orDiscrete"), 2003);
			pProp->AddOption(_T("Continuous"));
			pProp->AddOption(_T("Discrete"));

			pProp->AllowEdit(FALSE);
			pGroup1->AddSubItem(pProp);
		}
		{
			pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("报警温度（摄氏度）"), (_variant_t)theApp.getInt(_T("X20"), _T("tempalarm")),
				_T("tips"), 2004));
		}

		{
			pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("积分时间（毫秒）"), (_variant_t)theApp.getInt(_T("X20"), _T("samplerate")),
				_T("tips"), 2005));
		}
		{
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("测量单位"), theApp.getString(_T("X20"), _T("irraunit")),
				_T("tips"), 2006);
			pProp->AddOption(_T("W/cm2"));
			pProp->AddOption(_T("mW/cm2"));
			pProp->AllowEdit(FALSE);
			pGroup1->AddSubItem(pProp);
		}

		m_Prop_conf.AddProperty(pGroup1);

	}
	{
		CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("样品信息"));

		{
			pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("样品型号"), theApp.getString(_T("sample"), _T("model")),
				_T("tips"), 5001));
		}

		{
			pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("样品序列号"), theApp.getString(_T("sample"), _T("sn")),
				_T("tips"), 5002));
		}
		{
			pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("样品数量"), (_variant_t)theApp.getInt(_T("sample"), _T("count")),
				_T("tips"), 5003));
		}
		{
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("样品波长"), theApp.getString(_T("sample"), _T("wavelength")),
				_T("One of:365nm,375nm,385nm,395nm,or405nm"), 5004);
			pProp->AddOption(_T("365nm"));
			pProp->AddOption(_T("375nm"));
			pProp->AddOption(_T("385nm"));
			pProp->AddOption(_T("395nm"));
			pProp->AddOption(_T("405nm"));
			pProp->AllowEdit(FALSE);
			pGroup1->AddSubItem(pProp);
		}
		{
			pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("模组数量"), (_variant_t)theApp.getInt(_T("sample"), _T("modcount")),
				_T("tips"), 5006));
		}
		{
			pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("模组宽度"), (_variant_t)theApp.getInt(_T("sample"), _T("modwidth")),
				_T("tips"), 5016));
		}
		{
			pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("额定电流（A）"), (_variant_t)theApp.getInt(_T("sample"), _T("cur")),
				_T("tips"), 5007));
		}

		{
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("出光面宽度"), (_variant_t)theApp.getInt(_T("sample"), _T("height")), _T("Specifies the window's height"), 5008);
			pProp->EnableSpinControl(TRUE, 50, 300);
			pGroup1->AddSubItem(pProp);
		}
		{

			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("出光面长度"), (_variant_t)theApp.getInt(_T("sample"), _T("width")), _T("Specifies the window's width"), 5009);
			pProp->EnableSpinControl(TRUE, 50, 200);
			pGroup1->AddSubItem(pProp);
		} {
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("零点偏移X"), (_variant_t)theApp.getInt(_T("sample"), _T("driftx")), _T("Specifies the window's height"), 5018);
			pProp->EnableSpinControl(TRUE, -300, 300);
			pGroup1->AddSubItem(pProp);
		}
		{

			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("零点偏移Y"), (_variant_t)theApp.getInt(_T("sample"), _T("drifty")), _T("Specifies the window's width"), 5019);
			pProp->EnableSpinControl(TRUE, -200, 200);
			pGroup1->AddSubItem(pProp);
		}

		{
			CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("冷却方式"), theApp.getString(_T("sample"), _T("cooltype")),
				_T("One of:Water,orWind"), 5010);
			pProp->AddOption(_T("Water"));
			pProp->AddOption(_T("Wind"));

			pProp->AllowEdit(FALSE);
			pGroup1->AddSubItem(pProp);
		}
		{
			pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("电源通道数"), (_variant_t)theApp.getInt(_T("sample"), _T("powerchannel")),
				_T("tips"), 5011));
		}

		m_Prop_conf.AddProperty(pGroup1);

	}
	{
		CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("计算参数"));

		{
			pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("光强阈值（%）"), (_variant_t)theApp.getInt(_T("compute"), _T("minirra")),
				_T("tips"), 9001));
		}

		m_Prop_conf.AddProperty(pGroup1);

	}

	{
		CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("文件导出"));

		/*{
			static const TCHAR szFilter[] = _T("Icon Files(*.xls)|*.xls|All Files(*.*)|*.*||");
			pGroup1->AddSubItem(new CMFCPropertyGridFileProperty(_T("模板文件"), TRUE,
				_T(""), _T("ico"), 0, szFilter, _T("Specifies the window icon"),8001));
		}
		{
			static const TCHAR szFilter[] = _T("Icon Files(*.xls)|*.xls|All Files(*.*)|*.*||");
			pGroup1->AddSubItem(new CMFCPropertyGridFileProperty(_T("报表文件"), TRUE,
				_T(""), _T("ico"), 0, szFilter, _T("Specifies the window icon"),8002));
		}*/
		{
			pGroup1->AddSubItem(new CMFCPropertyGridFileProperty(_T("模板路径"), theApp.getString(_T("File"), _T("template_path")),
				8003));
		}
		{
			pGroup1->AddSubItem(new CMFCPropertyGridFileProperty(_T("报表路径"), theApp.getString(_T("File"), _T("report_path")),
				8004));
		}
		{
			pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("报告编号"), (_variant_t)theApp.getString(_T("File"), _T("suffix")),
				_T("tips"), 8005));
		}
		m_Prop_conf.AddProperty(pGroup1);

	}
	return;
	//==================================



	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Appearance"));
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D Look"), (_variant_t)false,
		_T("Specifies the window's font will be non-bold and controls will have a 3D border")));
	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Border"), _T("Dialog Frame"),
		_T("One of:None,Thin,Resizable,or Dialog Frame"));
	pProp->AddOption(_T("None"));
	pProp->AddOption(_T("Thin"));
	pProp->AddOption(_T("Resizable"));
	pProp->AddOption(_T("Dialog Frame"));
	pProp->AllowEdit(FALSE);
	pGroup1->AddSubItem(pProp);
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Caption"), (_variant_t)_T("About"),
		_T("tips")));
	m_Prop_conf.AddProperty(pGroup1);

	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("Window Size"), 0, TRUE);
	pProp = new CMFCPropertyGridProperty(_T("Height"), (_variant_t)250l, _T("Specifies the window's height"));
	pProp->EnableSpinControl(TRUE, 50, 300);
	pSize->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("Width"), (_variant_t)150l, _T("Specifies the window's width"));
	pProp->EnableSpinControl(TRUE, 50, 200);
	pSize->AddSubItem(pProp);
	m_Prop_conf.AddProperty(pSize);

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Font"));
	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);
	lstrcpy(lf.lfFaceName, _T("Arial"));

	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("Font"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("Specifies the default font for the window")));
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("Use System Font"), (_variant_t)true, _T("Specifies that the window uses MS Shell Dlg font")));
	m_Prop_conf.AddProperty(pGroup2);

	//********第四行*********
	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("Misc"));
	pProp = new CMFCPropertyGridProperty(_T("Name"), _T("Application"));
	pProp->Enable(FALSE);
	pGroup3->AddSubItem(pProp);

	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("Window Color"),
		RGB(210, 192, 254), NULL, _T("Specifies the default window color"));
	pColorProp->EnableOtherButton(_T("Other..."));
	pColorProp->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
	pGroup3->AddSubItem(pColorProp);

	static const TCHAR szFilter[] = _T("Icon Files(*.ico)|*.ico|All Files(*.*)|*.*||");
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Icon"), TRUE,
		_T(""), _T("ico"), 0, szFilter, _T("Specifies the window icon")));
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Folder"), _T("c:\\")));
	m_Prop_conf.AddProperty(pGroup3);

	//*********第五行*********
	//CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("Hierarchy"));
	//CMFCPropertyGridProperty* pGroup5 = new CMFCPropertyGridProperty(_T("First sub-level"));
	//pGroup4->AddSubItem(pGroup5);

	//CMFCPropertyGridProperty* pGroup6 = new CMFCPropertyGridProperty(_T("Second sub_level"));
	//pGroup5->AddSubItem(pGroup6);

	//pGroup6->AddSubItem(new CMFCPropertyGridProperty(_T("Item1"), (_variant_t)_T("Value 1"), _T("This is a description")));
	//pGroup6->AddSubItem(new CMFCPropertyGridProperty(_T("Item2"), (_variant_t)_T("Value 2"), _T("This is a description")));
	//pGroup6->AddSubItem(new CMFCPropertyGridProperty(_T("Item3"), (_variant_t)_T("Value 3"), _T("This is a description")));

	//pGroup6->Expand(FALSE);//不需全部展开
	//m_Prop_conf.AddProperty(pGroup4);
//	m_Prop_conf.AdjustLayout();
	//**********************

	//CRect rt;
	//GetClientRect(&rt);
//	m_Prop_conf.SetWindowPos(NULL, rt.left, rt.top, rt.Width(), rt.Height(), SWP_NOACTIVATE | SWP_NOZORDER);


}


BOOL PageConfig1::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	pdlg = (CMeasureBotDlg*)AfxGetMainWnd();
	initProp();
	isInited = true;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void PageConfig1::OnThemechangedMfcpropertygrid1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// 该功能要求使用 Windows XP 或更高版本。
	// 符号 _WIN32_WINNT 必须 >= 0x0501。
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void PageConfig1::OnStnClickedMfcpropertygrid1()
{
	// TODO: 在此添加控件通知处理程序代码
	return;

}

void PageConfig1::handleChannel() {

}

LRESULT PageConfig1::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*)lParam;
	BOOL bResetMDIChild = FALSE;

	int pid = (int)pProp->GetData();
	CString name = pProp->GetName();
	COleVariant cur = pProp->GetValue();
	COleVariant pre = pProp->GetOriginalValue();

	if (pid == 1001) {
		bool pretf = pre.boolVal;
		bool curtf = cur.boolVal;
		if (pretf != curtf) {
			CString strPort;
			strPort.Format(L"%d", curtf ? 1 : 0);
			::WritePrivateProfileString(_T("system"), _T("thread_mc"), strPort, service_ini_path);
		}
	}
	else if (pid == 1002) {
		bool pretf = pre.boolVal;
		bool curtf = cur.boolVal;
		if (pretf != curtf) {
			CString strPort;
			strPort.Format(L"%d", curtf ? 1 : 0);
			::WritePrivateProfileString(_T("system"), _T("thread_x20"), strPort, service_ini_path);
		}

	}
	else if (pid == 1003) {
		bool pretf = pre.boolVal;
		bool curtf = cur.boolVal;
		if (pretf != curtf) {
			CString strPort;
			strPort.Format(L"%d", curtf ? 1 : 0);
			::WritePrivateProfileString(_T("system"), _T("thread_Lx"), strPort, service_ini_path);
		}

	}
	else if (pid == 1004) {
		bool pretf = pre.boolVal;
		bool curtf = cur.boolVal;
		if (pretf != curtf) {
			CString strPort;
			strPort.Format(L"%d", curtf ? 1 : 0);
			::WritePrivateProfileString(_T("system"), _T("thread_smacq"), strPort, service_ini_path);
		}

	}
	else if (pid == 2001) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("X20"), _T("wavelength"), curstr, service_ini_path);
		}

	}
	else if (pid == 2002) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("X20"), _T("range"), curstr, service_ini_path);
		}

	}
	else if (pid == 2003) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("X20"), _T("samplemode"), curstr, service_ini_path);
		}

	}
	else if (pid == 2004) {
		int prei = pre.intVal;
		int curi = cur.intVal;
		if (prei != curi) {
			CString strPort;
			strPort.Format(L"%d", curi);
			::WritePrivateProfileString(_T("X20"), _T("tempalarm"), strPort, service_ini_path);
			pdlg->m_ctrl_stat.alarmTemp = curi;
		}

	}
	else if (pid == 2005) {
		int prei = pre.intVal;
		int curi = cur.intVal;
		if (prei != curi) {
			CString strPort;
			strPort.Format(L"%d", curi);
			::WritePrivateProfileString(_T("X20"), _T("samplerate"), strPort, service_ini_path);
		}

	}
	else if (pid == 2006) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("X20"), _T("irraunit"), curstr, service_ini_path);
			pdlg->m_ctrl_stat.GetDlgItem(IDC_STATIC_IRRA)->SetWindowTextW(curstr);
			string ustr = theApp.tostr(theApp.getString(_T("X20"), _T("irraunit")));
			if (ustr == "W/cm2") {
				pdlg->m_ctrl_stat.selUnit = 2;
			}
			else {
				pdlg->m_ctrl_stat.selUnit = 1;
			}
		}

	}
	else if (pid == 3001) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("L5000"), _T("samplerate"), curstr, service_ini_path);
		}

	}
	else if (pid == 3002) {
		int prei = pre.intVal;
		int curi = cur.intVal;
		if (prei != curi) {
			CString strPort;
			strPort.Format(L"%d", curi);
			::WritePrivateProfileString(_T("L5000"), _T("powerscale"), strPort, service_ini_path);
			pdlg->m_ctrl_lx.sendCommand(string("setRatio"));
		}

	}
	else if (pid >= 3011 && pid <= 3026) {
		int chn = pid - 3010;
		bool pretf = pre.boolVal;
		bool curtf = cur.boolVal;
		if (pretf != curtf) {
			CString strPort;
			strPort.Format(L"%d", curtf ? 1 : 0);
			CString chnstr;
			chnstr.Format(L"ch%d", chn);
			::WritePrivateProfileString(_T("L5000"), chnstr, strPort, service_ini_path);

			pdlg->m_ctrl_stat.onChannel(chn, curtf ? 1 : 0);
		}
	}
	else if (pid == 4001) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("requirement"), _T("distance"), curstr, service_ini_path);
		}

	}
	else if (pid == 4002) {
		int prei = pre.intVal;
		int curi = cur.intVal;
		if (prei != curi) {
			CString strPort;
			strPort.Format(L"%d", curi);
			::WritePrivateProfileString(_T("requirement"), _T("irradiation"), strPort, service_ini_path);
		}

	}
	else if (pid == 4003) {
		int prei = pre.intVal;
		int curi = cur.intVal;
		if (prei != curi) {
			CString strPort;
			strPort.Format(L"%d", curi);
			::WritePrivateProfileString(_T("requirement"), _T("average"), strPort, service_ini_path);
		}
	}
	else if (pid == 5001) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("sample"), _T("model"), curstr, service_ini_path);
		}

	}
	else if (pid == 5002) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("sample"), _T("sn"), curstr, service_ini_path);
		}

	}
	else if (pid == 5003) {
		int prei = pre.intVal;
		int curi = cur.intVal;
		if (prei != curi) {
			CString strPort;
			strPort.Format(L"%d", curi);
			::WritePrivateProfileString(_T("sample"), _T("count"), strPort, service_ini_path);
		}
	}
	else if (pid == 5004) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("sample"), _T("wavelength"), curstr, service_ini_path);
		}

	}
	else if (pid == 5006) {
		int prei = pre.intVal;
		int curi = cur.intVal;
		if (prei != curi) {
			CString strPort;
			strPort.Format(L"%d", curi);
			::WritePrivateProfileString(_T("sample"), _T("modcount"), strPort, service_ini_path);
			pdlg->m_page_list2.resetList();
		}
	}
	else if (pid == 5016) {
		int prei = pre.intVal;
		int curi = cur.intVal;
		if (prei != curi) {
			CString strPort;
			strPort.Format(L"%d", curi);
			::WritePrivateProfileString(_T("sample"), _T("modwidth"), strPort, service_ini_path);
			pdlg->m_page_track.m_mod_width = curi;
		}
	}
	else if (pid == 5007) {
		int prei = pre.intVal;
		int curi = cur.intVal;
		if (prei != curi) {
			CString strPort;
			strPort.Format(L"%d", curi);
			::WritePrivateProfileString(_T("sample"), _T("cur"), strPort, service_ini_path);
		}
	}
	else if (pid == 5008) {
		int prei = pre.intVal;
		int curi = cur.intVal;
		if (prei != curi) {
			CString strPort;
			strPort.Format(L"%d", curi);
			::WritePrivateProfileString(_T("sample"), _T("height"), strPort, service_ini_path);
			pdlg->m_page_track.m_led_height = curi;
			pdlg->m_page_track.onLedChanged();
		}
	}
	else if (pid == 5009) {
		int prei = pre.intVal;
		int curi = cur.intVal;
		if (prei != curi) {
			CString strPort;
			strPort.Format(L"%d", curi);
			::WritePrivateProfileString(_T("sample"), _T("width"), strPort, service_ini_path);
			pdlg->m_page_track.m_led_width = curi;
			pdlg->m_page_track.onLedChanged();
		}
	}
	else if (pid == 5018) {
		int prei = pre.intVal;
		int curi = cur.intVal;
		if (prei != curi) {
			CString strPort;
			strPort.Format(L"%d", curi);
			::WritePrivateProfileString(_T("sample"), _T("driftx"), strPort, service_ini_path);
			pdlg->m_page_track.m_led_driftx = curi;
			pdlg->m_page_track.onLedChanged();
		}
	}
	else if (pid == 5019) {
		int prei = pre.intVal;
		int curi = cur.intVal;
		if (prei != curi) {
			CString strPort;
			strPort.Format(L"%d", curi);
			::WritePrivateProfileString(_T("sample"), _T("drifty"), strPort, service_ini_path);
			pdlg->m_page_track.m_led_drifty = curi;
			pdlg->m_page_track.onLedChanged();
		}
	}
	else if (pid == 5010) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("sample"), _T("cooltype"), curstr, service_ini_path);
		}

	}
	else if (pid == 5011) {
		int prei = pre.intVal;
		int curi = cur.intVal;
		if (prei != curi) {
			CString strPort;
			strPort.Format(L"%d", curi);
			::WritePrivateProfileString(_T("sample"), _T("powerchannel"), strPort, service_ini_path);
		}
	}
	else if (pid == 6001) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("Communication"), _T("lxport"), curstr, service_ini_path);
		}

	}
	else if (pid == 6002) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("Communication"), _T("smacqport"), curstr, service_ini_path);
		}

	}
	else if (pid == 6003) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("Communication"), _T("x20port"), curstr, service_ini_path);
		}

	}
	else if (pid == 6004) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("Communication"), _T("rs485"), curstr, service_ini_path);
		}

	}
	else if (pid == 6024) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("Communication"), _T("baudrate"), curstr, service_ini_path);
		}
	}
	else if (pid == 6025) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("Communication"), _T("check"), curstr, service_ini_path);
		}
	}
	else if (pid == 6026) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("Communication"), _T("data"), curstr, service_ini_path);
		}
	}
	else if (pid == 6027) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("Communication"), _T("stop"), curstr, service_ini_path);
		}
	}
	else if (pid == 6005) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("Communication"), _T("mcIP"), curstr, service_ini_path);
		}

	}
	else if (pid == 6006) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("Communication"), _T("hostIP"), curstr, service_ini_path);
		}

	}
	else if (pid == 7001) {
		int prei = pre.intVal;
		int curi = cur.intVal;
		if (prei != curi) {
			CString strPort;
			strPort.Format(L"%d", curi);
			::WritePrivateProfileString(_T("Acquisition"), _T("temprate"), strPort, service_ini_path);
		}
	}
	else if (pid == 7002) {
		int prei = pre.intVal;
		int curi = cur.intVal;
		if (prei != curi) {
			CString strPort;
			strPort.Format(L"%d", curi);
			::WritePrivateProfileString(_T("Acquisition"), _T("flowrate"), strPort, service_ini_path);
		}
	}
	else if (pid == 7003) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("Acquisition"), _T("flower"), curstr, service_ini_path);
		}
		string selflow = theApp.tostr(curstr);
		if (selflow == "5-40L") {
			pdlg->m_ctrl_stat.selFlower = 1;
		}
		else {
			pdlg->m_ctrl_stat.selFlower = 2;
		}

	}
	else if (pid == 8003) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("File"), _T("template_path"), curstr, service_ini_path);
		}

	}
	else if (pid == 8004) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("File"), _T("report_path"), curstr, service_ini_path);
		}

	}
	else if (pid == 8005) {
		CString prestr = pre.bstrVal;
		CString curstr = cur.bstrVal;
		if (prestr != curstr) {
			::WritePrivateProfileString(_T("File"), _T("suffix"), curstr, service_ini_path);
		}
	}
	else if (pid == 9001) {
		int prei = pre.intVal;
		int curi = cur.intVal;
		if (prei != curi) {
			CString strPort;
			strPort.Format(L"%d", curi);
			::WritePrivateProfileString(_T("compute"), _T("minirra"), strPort, service_ini_path);
		}
	}


	return 0;
}

void PageConfig1::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (isInited) {
		CRect rect;
		this->GetClientRect(&rect);
		rect.top += 5;// 30;
		rect.bottom -= 5;
		rect.left += 5;
		rect.right -= 5;
		m_Prop_conf.MoveWindow(&rect);
	}
	// TODO: 在此处添加消息处理程序代码
}
