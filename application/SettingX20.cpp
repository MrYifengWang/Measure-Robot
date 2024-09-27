// SettingX20.cpp: 实现文件
//

#include "pch.h"
#include "MeasureBot.h"
#include "SettingX20.h"
#include "afxdialogex.h"


// SettingX20 对话框

IMPLEMENT_DYNAMIC(SettingX20, CDialogEx)

SettingX20::SettingX20(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_X20, pParent)
{

}

SettingX20::~SettingX20()
{
}

void SettingX20::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SettingX20, CDialogEx)
	ON_MESSAGE(WM_MYMESSAGE_SET_X20, &SettingX20::OnMyMessageSignal)
END_MESSAGE_MAP()


// SettingX20 消息处理程序


BOOL SettingX20::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	pdlg= (CMeasureBotDlg*)AfxGetMainWnd();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
LRESULT SettingX20::OnMyMessageSignal(WPARAM wParam, LPARAM data) {
	std::string* msg = reinterpret_cast<std::string*>(data);
	int msg_id = wParam;
	if (msg_id == X20_MESSAGE)
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
				if (code == -4001) {
					pdlg->m_ctrl_stat.ed_x20.SetWindowText(_T("光表连接失败"));
				}
			}
		}
	}
	else if (msg_id == MC_MESSAGE) {

		
	}

	delete msg;

	return 0;
}

bool SettingX20::precheck(int tips) {
	if (!theApp.getBool(_T("system"), _T("thread_x20"))) {
		if (tips)
			::MessageBox(NULL, _T("光表未启用!"), _T("ERROR"), MB_OK);
		return false;
	}
	if (m_connected == 0) {
		if (tips)
			::MessageBox(NULL, _T("光表通讯端口未连接!"), _T("ERROR"), MB_OK);
		return false;
	}

	return true;
}

void SettingX20::turnOnOff(int stat) {
	return;
	if (stat == 1) {
		sendCommand(string("AutoMeasure"));
	}
	else {
		sendCommand(string("Stop"));
	}
}


void SettingX20::sendCommand(string& cmd) {
	
	if (cmd != "Connect") {
		if (!precheck())return;
	}
	else {
		if (!theApp.getBool(_T("system"), _T("thread_x20"))) {
			return;
		}
	}
	Json::Value root;
	Json::FastWriter writer;
	root["cmd"] = cmd;
	string peekcmd = "";

	if (cmd == "DevInfo") {

	}
	else if (cmd == "Set") {
		root["wavelength"] = theApp.tostr(theApp.getString(_T("X20"), _T("wavelength")));
		root["range"]= theApp.tostr(theApp.getString(_T("X20"), _T("range")));
		root["intetime"] = theApp.getInt(_T("X20"), _T("samplerate"));
	}
	else if (cmd == "Measure") {
	}
	else if (cmd == "AutoMeasure") {

	}
	else if (cmd == "Stop") {
		peekcmd = "auto_stop";
	}
	

	string msg = writer.write(root);
	pdlg->x20Queue_.push(msg, peekcmd);

	return;
}
