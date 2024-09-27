// SettingSample.cpp: 实现文件
//

#include "pch.h"
#include "MeasureBot.h"
#include "SettingSample.h"
#include "afxdialogex.h"



// SettingSample 对话框

IMPLEMENT_DYNAMIC(SettingSample, CDialogEx)

SettingSample::SettingSample(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SAMPLE, pParent)
{

}

SettingSample::~SettingSample()
{
}

void SettingSample::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, ed_mc);
	DDX_Control(pDX, IDC_EDIT2, ed_power);
	DDX_Control(pDX, IDC_EDIT3, ed_x20);
	DDX_Control(pDX, IDC_EDIT4, ed_smacq);
	DDX_Control(pDX, IDC_EDIT5, ed_xpos);
	DDX_Control(pDX, IDC_EDIT6, ed_ypos);
	DDX_Control(pDX, IDC_EDIT10, ed_mc_speed);
	DDX_Control(pDX, IDC_EDIT7, ed_irra);
	DDX_Control(pDX, IDC_EDIT8, ed_temp0);
	DDX_Control(pDX, IDC_EDIT9, ed_temp1);
	DDX_Control(pDX, IDC_EDIT11, ed_temp2);
	DDX_Control(pDX, IDC_EDIT12, ed_temp3);
	DDX_Control(pDX, IDC_EDIT13, ed_flow1);
	DDX_Control(pDX, IDC_EDIT14, ed_flow2);
	DDX_Control(pDX, IDC_LIST1, ed_power_list);
	DDX_Control(pDX, IDC_BUTTON1, m_btnPause);
}


BEGIN_MESSAGE_MAP(SettingSample, CDialogEx)
	ON_MESSAGE(WM_MYMESSAGE_SET_RUN, &SettingSample::OnMyMessageSignal)
	ON_BN_CLICKED(IDC_BUTTON1, &SettingSample::OnBnClickedPauseContinue)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// SettingSample 消息处理程序


BOOL SettingSample::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	pdlg = (CMeasureBotDlg*)AfxGetMainWnd();

	ed_mc.SetWindowText(_T("待启动"));
	ed_power.SetWindowText(_T("待启动"));
	ed_x20.SetWindowText(_T("待启动"));
	ed_smacq.SetWindowText(_T("待启动"));
	if (!theApp.getBool(_T("system"), _T("thread_mc"))) {
		ed_mc.SetWindowText(_T("未启用"));
	}
	if (!theApp.getBool(_T("system"), _T("thread_Lx"))) {
		ed_power.SetWindowText(_T("未启用"));
	}
	if (!theApp.getBool(_T("system"), _T("thread_x20"))) {
		ed_x20.SetWindowText(_T("未启用"));
	}
	if (!theApp.getBool(_T("system"), _T("thread_smacq"))) {
		ed_smacq.SetWindowText(_T("未启用"));
	}

	ed_xpos.SetWindowText(_T("0"));
	ed_ypos.SetWindowText(_T("0"));
	ed_mc_speed.SetWindowText(_T("0"));
	ed_irra.SetWindowText(_T("0"));
	ed_temp0.SetWindowText(_T("0"));
	ed_temp1.SetWindowText(_T("0"));
	ed_temp2.SetWindowText(_T("0"));
	ed_temp3.SetWindowText(_T("0"));
	ed_flow1.SetWindowText(_T("0"));
	ed_flow2.SetWindowText(_T("0"));

	DWORD  style = ed_power_list.GetExtendedStyle();
	style |= LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;// | LVS_EX_CHECKBOXES;
	ed_power_list.SetExtendedStyle(style);

	// Create Columns

//添加列,0---序号，第二个参数显示内容，
//第三个参数显示对齐方式，第四个参数表头显示的宽度
	ed_power_list.InsertColumn(0, _T("通道"), LVCFMT_CENTER, 50);
	ed_power_list.InsertColumn(1, _T("状态"), LVCFMT_CENTER, 130);
	ed_power_list.InsertColumn(2, _T("电流"), LVCFMT_CENTER, 80);
	ed_power_list.InsertColumn(3, _T("电压"), LVCFMT_CENTER, 80);
	//ed_power_list.InsertColumn(4, _T("温度"), LVCFMT_CENTER, 80);

	//插入行内容
	//尾部添加行
	for (int i = 0; i < 16; i++) {
		ed_power_list.InsertItem(ed_power_list.GetItemCount(), _T("第"));
		//可以设置某一行某一列的内容，参数依次为行，列，文本内容
		CString val(L"--");
		CString chidx;
		chidx.Format(L"ch%d", i + 1);
		if (theApp.getBool(_T("L5000"), chidx)) {
			val = L"0";
		}

		ed_power_list.SetItemText(i, 0, chidx);
		ed_power_list.SetItemText(i, 1, val);
		ed_power_list.SetItemText(i, 2, val);
		ed_power_list.SetItemText(i, 3, val);
	//	ed_power_list.SetItemText(i, 4, val);
	}

	string ustr = theApp.tostr(theApp.getString(_T("X20"), _T("irraunit")));
	if (ustr == "W/cm2") {
		selUnit = 2;
	}
	else {
		selUnit = 1;
	}

	GetDlgItem(IDC_STATIC_IRRA)->SetWindowTextW(theApp.getString(_T("X20"), _T("irraunit")));
	// UINT nState = LVIS_CUT; // 要设置的状态
	 //ed_power_list.SetItemState(3, nState, LVIS_CUT);
	ed_power_list.SetCheck(3, true);
	// ed_power_list.SetItemState(4, 0, LVIS_SELECTED);
	// ed_power_list.SetItemData(3, RGB(128,128,128));

	string selflow = theApp.tostr(theApp.getString(_T("Acquisition"), _T("flower")));
	if (selflow == "5-40L") {
		selFlower = 1;
	}
	else {
		selFlower = 2;
	}

	alarmTemp = theApp.getInt(_T("X20"), _T("tempalarm"));
	coolType = theApp.getString(_T("sample"), _T("cooltype"));

	this->SetTimer(1, 1000,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
LRESULT SettingSample::OnMyMessageSignal(WPARAM wParam, LPARAM data) {
	std::string* msg = reinterpret_cast<std::string*>(data);
	int msg_id = wParam;
	if (msg_id == L5000_MESSAGE)
	{
		Json::Reader reader;
		Json::Value jmessage;
		if (!reader.parse(*msg, jmessage)) {
			return 0;
		}
		if (jmessage["cmd"].asString() == "Connect") {
			int code = jmessage["code"].asInt();
			if (code == 0) {
				m_connected = 1;
				pdlg->m_ctrl_stat.ed_x20.SetWindowText(_T("已连接"));
			}
			else {
				m_connected = 0;
				if (code == -2001) {
					pdlg->m_ctrl_stat.ed_x20.SetWindowText(_T("通讯连接失败"));
				}
			}
		}
		else if (jmessage["cmd"].asString() == "Measure") {
			Json::Value arr;
			Json::Value arr1;
			if (jmessage["temps"].isArray()) {
				arr = jmessage["temps"];
				for (int i = 0; i < 16; i++) {
					int chn = arr[i]["chn"].asInt();
					int temp = arr[i]["temp"].asInt();

					CString val;
					val.Format(L"%.2f", temp / 10.0);
					//ed_power_list.SetItemText(chn - 1, 4, val);

					curPow_.tm = time(NULL);
					curPow_.chn[chn - 1].ntc = temp/10.0;
				}
			}
			if (jmessage["pows"].isArray()) {
				arr1 = jmessage["pows"];
				int prepow = 0;
				for (int i = 0; i < 16; i++) {
					int chn = arr1[i]["chn"].asInt();
					int vol = arr1[i]["vol"].asInt();
					int cur = arr1[i]["cur"].asInt();
					int pow = arr1[i]["pow"].asInt();

					CString val(L"--");
					{
						{
							val.Format(L"%.2f", vol / 100.0); //电流
							ed_power_list.SetItemText(chn - 1, 2, val);
						} {

							val.Format(L"%.2f", pow / 10.0); //电压
							ed_power_list.SetItemText(chn - 1, 3, val);

						}
						prepow = pow;

						curPow_.tm = time(NULL);
						curPow_.chn[chn - 1].pow = pow / 10.0;
						curPow_.chn[chn - 1].cur = vol / 100.0;
					}

					if (chn > 10) {
						DebugLog::writeLogF("----------set real Pow:%d %.4f %.4f",chn, curPow_.chn[chn - 1].cur = vol / 100.0, curPow_.chn[chn - 1].pow);
					}
					
				}
			}


		}
	}
	else if (msg_id == SMACQ_MESSAGE) {
		Json::Reader reader;
		Json::Value jmessage;
		if (!reader.parse(*msg, jmessage)) {
			return 0;
		}
		if (jmessage["cmd"].asString() == "Measure") {
			if (jmessage["card"].asString() == "2101") {
				int temp = jmessage["temp"].asInt();
				int temp_root = jmessage["temp_room"].asInt();
				float tp1 = temp / 10.0;
				{
					CString tempstr;
					tempstr.Format(L"%.2f", temp / 10.0);
					ed_temp0.SetWindowText(tempstr);
				}
				{
					CString tempstr;
					tempstr.Format(L"%.2f", temp_root / 10.0);
					ed_temp3.SetWindowText(tempstr);
				}

				if ((tp1 >= alarmTemp && isInAlarm == 0)) {
					isInAlarm = 1;
					taskPause();
					
				}
				if ((tp1 < alarmTemp && isInAlarm == 1)) {
					isInAlarm = 0;
					taskContinue();
					
				}

				curTemp.tm = time(NULL);
				curTemp.temp1 = tp1;
				curTemp.temp2 = temp_root / 10.0;
			}
			else if (jmessage["card"].asString() == "2003") {

				float intemp, outtemp, flow;
				intemp = jmessage["chn1"].asDouble();
				outtemp = jmessage["chn2"].asDouble();
				CString tempstr;
				tempstr.Format(L"%.2f", intemp); //in water
				ed_temp1.SetWindowText(tempstr);
				tempstr.Format(L"%.2f", outtemp); //out water
				ed_temp2.SetWindowText(tempstr);

				if (selFlower == 1) {
					flow = jmessage["chn3"].asDouble();
					//5-40L
				}
				else {
					flow = jmessage["chn4"].asDouble();
					 //2-16L
				}
				if (coolType == L"water" && flow<0.0001) {
					pdlg->m_ctrl_lx.powerOnOff(0);
				}
				tempstr.Format(L"%.2f", flow);
				ed_flow1.SetWindowText(tempstr);

				curTemp.tm = time(NULL);
				curTemp.tempin = intemp;
				curTemp.tempout = outtemp;
				curTemp.flow = flow;


			}
		}

	}

	delete msg;
}

void SettingSample::onChannel(int channel, int sel) {
	int chn = channel - 1;
	if (sel == 1) {
		ed_power_list.SetItemText(chn, 1, _T("0"));
		ed_power_list.SetItemText(chn, 2, _T("0"));
		ed_power_list.SetItemText(chn, 3, _T("0"));
		//ed_power_list.SetItemText(chn, 4, _T("0"));
	}
	else {
		ed_power_list.SetItemText(chn, 1, _T("--"));
		ed_power_list.SetItemText(chn, 2, _T("--"));
		ed_power_list.SetItemText(chn, 3, _T("--"));
	//	ed_power_list.SetItemText(chn, 4, _T("--"));
	}
}

void SettingSample::onResponse(Json::Value& resp)
{
	if (resp["dev"].asString() == "mc") {

	}
	else if (resp["dev"].asString() == "x20") {

	}

	else if (resp["dev"].asString() == "Lx") {

	}
	else if (resp["dev"].asString() == "smacq") {

	}
}
void SettingSample::onStat(Json::Value& resp) {
	if (resp["dev"].asString() == "mc") {
		if (resp["stat"].asInt() == 1) {
			ed_mc.SetWindowText(_T("正常"));
		}

	}
	else if (resp["dev"].asString() == "x20") {

	}

	else if (resp["dev"].asString() == "Lx") {

	}
	else if (resp["dev"].asString() == "smacq") {

	}
}
void SettingSample::onData(Json::Value& resp) {
	if (resp["dev"].asString() == "mc") {

	}
	else if (resp["dev"].asString() == "x20") {

	}

	else if (resp["dev"].asString() == "Lx") {

	}
	else if (resp["dev"].asString() == "smacq") {

	}
}

bool SettingSample::precheck(int tips) {
	if (!theApp.getBool(_T("system"), _T("thread_smacq"))) {
		if (tips)
			::MessageBox(NULL, _T("流量温度未启用!"), _T("ERROR"), MB_OK);
		return false;
	}
	if (m_connected == 0) {
		if (tips)
			::MessageBox(NULL, _T("流量温度通讯端口未连接!"), _T("ERROR"), MB_OK);
		return false;
	}

	return true;
}

void SettingSample::sendCommand(string& cmd) {
	if (cmd != "Connect") {
		if (!precheck())return;
	}
	else {
		if (!theApp.getBool(_T("system"), _T("thread_smacq"))) {
			return;
		}
	}
	Json::Value root;
	Json::FastWriter writer;
	root["cmd"] = cmd;
	string peekcmd = "";

	if (cmd == "Switch") {
		root["enable"] = "on"; //0ff
	}
	else if (cmd == "setRatio") {
		root["val"] = 200;// atoi(ratiostr.c_str());

	}
	else if (cmd == "Connect") {
		root["port"] = theApp.tostr(theApp.getString(_T("Communication"), _T("smacqport")));
		root["baudrate"] = atoi(theApp.tostr(theApp.getString(_T("Communication"), _T("baudrate"))).c_str());
		root["databit"] = atoi(theApp.tostr(theApp.getString(_T("Communication"), _T("data"))).c_str());
		root["stopbit"] = atoi(theApp.tostr(theApp.getString(_T("Communication"), _T("stop"))).c_str());
		root["check"] = theApp.tostr(theApp.getString(_T("Communication"), _T("check"))).c_str();
		root["tempcycle"] = theApp.getInt(_T("Acquisition"), _T("temprate"));
		root["flowcycle"] = theApp.getInt(_T("Acquisition"), _T("flowrate"));

	}
	else if (cmd == "Measure") {
	}
	else if (cmd == "AutoMeasure") {

	}
	else if (cmd == "Stop") {
		peekcmd = "auto_stop";
	}


	string msg = writer.write(root);
	pdlg->smacqQueie_.push(msg, peekcmd);

	return;
}


void SettingSample::taskPause() {
	pdlg->m_ctrl_mc.sendCommand(string("Track"), "track_pause");
	pdlg->m_ctrl_lx.powerOnOff(0);
	pdlg->m_ctrl_mc.m_isMeasure = 0;
	pdlg->m_page_list2.preTm = 0;

}
void SettingSample::taskContinue() {
	pdlg->m_ctrl_mc.sendCommand(string("Track"), "track_continue");
	pdlg->m_ctrl_lx.powerOnOff(1);
	pdlg->m_ctrl_mc.m_isMeasure = 1;
}

void SettingSample::OnBnClickedPauseContinue()
{
	// TODO: 在此添加控件通知处理程序代码
	if (isInAlarm == 1) return;
	CString tempstr;
	m_btnPause.GetWindowText(tempstr);
	if (tempstr == L"暂停") {
		taskPause();
		m_btnPause.SetWindowTextW(L"继续");
	}
	else {

		taskContinue();
		m_btnPause.SetWindowTextW(L"暂停");
	}
}


void SettingSample::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	static int count = 0;
	count++;
	if (count % 10 == 0) {
		if (pdlg->m_ctrl_mc.m_task_stat == 1) 
		{
			curPow_.tm = time(NULL);
			powList.push_back(curPow_);
		}
	}
	if (count % 10 == 0) {
		if (pdlg->m_ctrl_mc.m_task_stat == 1) 
		{
			curTemp.tm = time(NULL);
			tmpFlowList.push_back(curTemp);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}


BOOL SettingSample::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
