// SettingPower.cpp: 实现文件
//

#include "pch.h"
#include "MeasureBot.h"
#include "SettingPower.h"
#include "afxdialogex.h"


// SettingPower 对话框

IMPLEMENT_DYNAMIC(SettingPower, CDialogEx)

SettingPower::SettingPower(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_POWER, pParent)
{

}

SettingPower::~SettingPower()
{
}

void SettingPower::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SettingPower, CDialogEx)
	ON_MESSAGE(WM_MYMESSAGE_SET_POWER, &SettingPower::OnMyMessageSignal)
	ON_BN_CLICKED(IDC_CHECK1, &SettingPower::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &SettingPower::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, &SettingPower::OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_CHECK4, &SettingPower::OnBnClickedCheck4)
	ON_BN_CLICKED(IDC_CHECK5, &SettingPower::OnBnClickedCheck5)
	ON_BN_CLICKED(IDC_CHECK6, &SettingPower::OnBnClickedCheck6)
	ON_BN_CLICKED(IDC_CHECK7, &SettingPower::OnBnClickedCheck7)
	ON_BN_CLICKED(IDC_CHECK8, &SettingPower::OnBnClickedCheck8)
	ON_BN_CLICKED(IDC_CHECK9, &SettingPower::OnBnClickedCheck9)
	ON_BN_CLICKED(IDC_CHECK10, &SettingPower::OnBnClickedCheck10)
	ON_BN_CLICKED(IDC_CHECK11, &SettingPower::OnBnClickedCheck11)
	ON_BN_CLICKED(IDC_CHECK12, &SettingPower::OnBnClickedCheck12)
	ON_BN_CLICKED(IDC_CHECK13, &SettingPower::OnBnClickedCheck13)
	ON_BN_CLICKED(IDC_CHECK14, &SettingPower::OnBnClickedCheck14)
	ON_BN_CLICKED(IDC_CHECK15, &SettingPower::OnBnClickedCheck15)
	ON_BN_CLICKED(IDC_CHECK16, &SettingPower::OnBnClickedCheck16)
END_MESSAGE_MAP()


// SettingPower 消息处理程序


BOOL SettingPower::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	pdlg= (CMeasureBotDlg*)AfxGetMainWnd();


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
LRESULT SettingPower::OnMyMessageSignal(WPARAM wParam, LPARAM data) {
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
	}
	else if (msg_id == MC_MESSAGE) {


	}

	delete msg;
	return 0;
}

bool SettingPower::precheck(int tips) {
	if (!theApp.getBool(_T("system"), _T("thread_Lx"))) {
		if (tips)
			::MessageBox(NULL,_T("电源未启用!"), _T("ERROR"), MB_OK);
		return false;
	}
	if (m_connected==0) {
		if (tips)
			::MessageBox(NULL,_T("电源通讯端口未连接!"), _T("ERROR"), MB_OK);
		return false;
	}

	return true;
}

void SettingPower::powerOnOff(int flag) {

	if (!precheck())return;

	if (pdlg->m_ctrl_mc.m_onlyTrack == 1 && flag == 1) {
		return;
	}

	Json::Value root;
	Json::FastWriter writer;
	root["cmd"] = "Switch";
	string peekcmd = "";

	if (flag == 1) {
		root["enable"] = "on"; //0ff
	}
	else {
		root["enable"] = "off"; //0ff
	}
	string msg = writer.write(root);
	pdlg->lxQueie_.push(msg, peekcmd);
}

void SettingPower::sendCommand(string& cmd) {
	if (cmd != "Connect") {
		if (!precheck())return;
	}
	else {
		if (!theApp.getBool(_T("system"), _T("thread_Lx"))) {
			return ;
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
		int tst = theApp.getInt(_T("L5000"), _T("powerscale"));
		root["val"] = theApp.getInt(_T("L5000"), _T("powerscale"));

	}
	else if (cmd == "Connect") {
		root["port"] = theApp.tostr(theApp.getString(_T("Communication"), _T("lxport")));		
		root["baudrate"] = atoi(theApp.tostr(theApp.getString(_T("Communication"), _T("baudrate"))).c_str());
		root["databit"] = atoi(theApp.tostr(theApp.getString(_T("Communication"), _T("data"))).c_str());
		root["stopbit"] = atoi(theApp.tostr(theApp.getString(_T("Communication"), _T("stop"))).c_str());
		root["check"] = theApp.tostr(theApp.getString(_T("Communication"), _T("check"))).c_str();
		root["samplecycle"] = theApp.getInt(_T("L5000"), _T("sample"));

	}
	else if (cmd == "Measure") {
	}
	else if (cmd == "AutoMeasure") {

	}
	else if (cmd == "Stop") {
		peekcmd = "auto_stop";
	}


	string msg = writer.write(root);
	pdlg->lxQueie_.push(msg, peekcmd);

	return;
}

void SettingPower::initChannel() {

	if (!precheck())return;

	Json::Value root;
	Json::Value arr;
	Json::FastWriter writer;

	root["cmd"] = "setChannel";
	for (int i = 0; i < 16; i++) {

		CString chidx;
		chidx.Format(L"ch%d", i + 1);
		if (theApp.getBool(_T("L5000"), chidx)) {
			arr.append(i+1);
		}
	}
	root["channels"] = arr;
	root["enable"] = "on";

	string msg = writer.write(root);
	pdlg->lxQueie_.push(msg);

}

void SettingPower::selectChannel(int chn, int stat)
{
	if (!precheck())return;

	Json::Value root;
	Json::FastWriter writer;

	root["cmd"] = "setChannel";
	root["channels"].append(chn);


	if (stat == 1)
	{
		root["enable"] = "on";
	}
	else
	{
		root["enable"] = "off";
	}

	string msg = writer.write(root);
	pdlg->lxQueie_.push(msg);
}


void SettingPower::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码

	int stat = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck() ? 1 : 0;
	selectChannel(1, stat);
}


void SettingPower::OnBnClickedCheck2()
{
	// TODO: 在此添加控件通知处理程序代码
	int stat = ((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck() ? 1 : 0;
	selectChannel(2, stat);
}


void SettingPower::OnBnClickedCheck3()
{
	// TODO: 在此添加控件通知处理程序代码
	int stat = ((CButton*)GetDlgItem(IDC_CHECK3))->GetCheck() ? 1 : 0;
	selectChannel(3, stat);
}


void SettingPower::OnBnClickedCheck4()
{
	// TODO: 在此添加控件通知处理程序代码
	int stat = ((CButton*)GetDlgItem(IDC_CHECK4))->GetCheck() ? 1 : 0;
	selectChannel(4, stat);
}


void SettingPower::OnBnClickedCheck5()
{
	int stat = ((CButton*)GetDlgItem(IDC_CHECK5))->GetCheck() ? 1 : 0;
	selectChannel(5, stat);
	// TODO: 在此添加控件通知处理程序代码
}


void SettingPower::OnBnClickedCheck6()
{
	// TODO: 在此添加控件通知处理程序代码
	int stat = ((CButton*)GetDlgItem(IDC_CHECK6))->GetCheck() ? 1 : 0;
	selectChannel(6, stat);
}


void SettingPower::OnBnClickedCheck7()
{
	// TODO: 在此添加控件通知处理程序代码
	int stat = ((CButton*)GetDlgItem(IDC_CHECK7))->GetCheck() ? 1 : 0;
	selectChannel(7, stat);
}


void SettingPower::OnBnClickedCheck8()
{
	// TODO: 在此添加控件通知处理程序代码
	int stat = ((CButton*)GetDlgItem(IDC_CHECK8))->GetCheck() ? 1 : 0;
	selectChannel(8, stat);
}


void SettingPower::OnBnClickedCheck9()
{
	// TODO: 在此添加控件通知处理程序代码
	int stat = ((CButton*)GetDlgItem(IDC_CHECK9))->GetCheck() ? 1 : 0;
	selectChannel(9, stat);
}


void SettingPower::OnBnClickedCheck10()
{
	// TODO: 在此添加控件通知处理程序代码
	int stat = ((CButton*)GetDlgItem(IDC_CHECK10))->GetCheck() ? 1 : 0;
	selectChannel(10, stat);
}


void SettingPower::OnBnClickedCheck11()
{
	// TODO: 在此添加控件通知处理程序代码
	int stat = ((CButton*)GetDlgItem(IDC_CHECK11))->GetCheck() ? 1 : 0;
	selectChannel(11, stat);
}


void SettingPower::OnBnClickedCheck12()
{
	// TODO: 在此添加控件通知处理程序代码
	int stat = ((CButton*)GetDlgItem(IDC_CHECK12))->GetCheck() ? 1 : 0;
	selectChannel(12, stat);
}


void SettingPower::OnBnClickedCheck13()
{
	// TODO: 在此添加控件通知处理程序代码
	int stat = ((CButton*)GetDlgItem(IDC_CHECK13))->GetCheck() ? 1 : 0;
	selectChannel(13, stat);
}


void SettingPower::OnBnClickedCheck14()
{
	// TODO: 在此添加控件通知处理程序代码
	int stat = ((CButton*)GetDlgItem(IDC_CHECK14))->GetCheck() ? 1 : 0;
	selectChannel(14, stat);
}


void SettingPower::OnBnClickedCheck15()
{
	// TODO: 在此添加控件通知处理程序代码
	int stat = ((CButton*)GetDlgItem(IDC_CHECK15))->GetCheck() ? 1 : 0;
	selectChannel(15, stat);
}


void SettingPower::OnBnClickedCheck16()
{
	// TODO: 在此添加控件通知处理程序代码
	int stat = ((CButton*)GetDlgItem(IDC_CHECK16))->GetCheck() ? 1 : 0;
	selectChannel(16, stat);
}
