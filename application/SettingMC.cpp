// SettingMC.cpp: 实现文件
//

#include "pch.h"
#include "MeasureBot.h"
#include "SettingMC.h"
#include "afxdialogex.h"
#include "MeasureBotDlg.h"

// SettingMC 对话框

IMPLEMENT_DYNAMIC(SettingMC, CDialogEx)

SettingMC::SettingMC(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MC, pParent)
	, m_group1(FALSE)
	, m_group2(FALSE)
{

}

SettingMC::~SettingMC()
{
}

void SettingMC::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_group1);
	DDX_Radio(pDX, IDC_RADIO3, m_group2);
	DDX_Control(pDX, IDC_LIST1, m_tracklist);
	DDX_Control(pDX, IDC_COMBO2, m_models);
	DDX_Control(pDX, IDC_COMBO1, m_height_list);
	DDX_Control(pDX, IDC_SPIN1, sp_ledx);
	DDX_Control(pDX, IDC_SPIN2, sp_ledy);
	DDX_Control(pDX, IDC_CHECK1, m_chk_nopower);
}


BEGIN_MESSAGE_MAP(SettingMC, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &SettingMC::OnBnClickedButton1)
	ON_MESSAGE(WM_MYMESSAGE_SET_MC, &SettingMC::OnMyMessageSignal)
	ON_CBN_SELCHANGE(IDC_COMBO2, &SettingMC::OnCbnSelchangeCombo2)
	ON_BN_CLICKED(IDC_RADIO1, &SettingMC::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &SettingMC::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &SettingMC::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &SettingMC::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_BUTTON2, &SettingMC::OnBnClickedBTNStop)
	ON_BN_CLICKED(IDC_BUTTON3, &SettingMC::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &SettingMC::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &SettingMC::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &SettingMC::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON8, &SettingMC::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON10, &SettingMC::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_CHECK1, &SettingMC::OnBnClickedCheck1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &SettingMC::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, &SettingMC::OnDeltaposSpin2)
END_MESSAGE_MAP()


// SettingMC 消息处理程序


BOOL SettingMC::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//pdlg= (CMeasureBotDlg*)GetParent()->GetParent();
	pdlg= (CMeasureBotDlg*)AfxGetMainWnd();
	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(true);
	((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(true);

	m_models.InsertString(0, _T("横向扫描"));
	m_models.InsertString(1, _T("纵向扫描"));
	m_models.InsertString(2, _T("网格定点"));
	m_models.SetWindowTextW(_T("选择模式"));

	m_height_list.InsertString(0, _T("0"));
	m_height_list.InsertString(1, _T("5"));
	m_height_list.InsertString(2, _T("10"));
	m_height_list.InsertString(3, _T("20"));
	m_height_list.InsertString(4, _T("30"));
	m_height_list.InsertString(5, _T("40"));
	m_height_list.InsertString(6, _T("50"));
	m_height_list.InsertString(7, _T("60"));
	m_height_list.InsertString(8, _T("70"));
	m_height_list.InsertString(9, _T("80"));
	m_height_list.InsertString(10, _T("90"));
	m_height_list.InsertString(11, _T("100"));
	m_height_list.SetCurSel(0);

	//m_models.SetWindowTextW(_T("选择模式"));

	sp_ledx.SetRange32(-30, 30);    //调节的范围
	sp_ledx.SetBase(0); //十进制
	sp_ledx.SetBuddy(GetDlgItem(IDC_EDIT9));    //设置关联的编辑框
	sp_ledy.SetRange32(-30, 30);    //调节的范围
	sp_ledy.SetBase(0); //十进制
	sp_ledy.SetBuddy(GetDlgItem(IDC_EDIT10));    //设置关联的编辑框


	switchPan(1);
	//GetDlgItem(IDC_COMBO1)->SetWindowTextW(_T("100"));
	GetDlgItem(IDC_EDIT5)->SetWindowTextW(_T("20"));
	GetDlgItem(IDC_EDIT6)->SetWindowTextW(_T("1"));

	GetDlgItem(IDC_EDIT4)->SetWindowTextW(L"50");
	GetDlgItem(IDC_EDIT3)->SetWindowTextW(L"20");

	GetDlgItem(IDC_EDIT7)->SetWindowTextW(L"--");
	GetDlgItem(IDC_EDIT8)->SetWindowTextW(L"--");

	curAxis = 1;
	m_connected = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
int SettingMC::getZHeight(int idx) {
	if (idx == 0)
		return 0;
	else if (idx == 1)
		return 5;
	else
		return (idx - 1) * 10;
}
int SettingMC::getCurHeight() {
	int idx = m_height_list.GetCurSel();
	if (idx == 0)
		return 0;
	else if (idx == 1)
		return 5;
	else
		return (idx - 1) * 10;
}


void SettingMC::refreshList(int itp)
{
	m_tracklist.ResetContent();
	Json::Value list;
	pdlg->m_page_track.readTracklist(list);
	Json::Value arr = list["list"];
	for (int i = 0; i < arr.size(); i++) {
		int type = arr[i]["model"].asInt();
		if (type == itp) {
			string nostr = arr[i]["id"].asString();
			m_tracklist.InsertString(i,CString(nostr.c_str()));
		}
	}
}

LRESULT SettingMC::OnMyMessageSignal(WPARAM wParam, LPARAM data) {

	int msg_id = wParam;
	std::string* msg = reinterpret_cast<std::string*>(data);
	if (msg_id == MC_MESSAGE) {
		
		Json::Reader reader;
		Json::Value jmessage;
		if (!reader.parse(*msg, jmessage)) {
			return 0;
		}
		if (jmessage["cmd"].asString() == "Connect") {
			int code = jmessage["code"].asInt();
			if (code == 0) {
				m_connected = 1;
				pdlg->m_ctrl_stat.ed_mc.SetWindowText(_T("已连接"));
			}
			else {
				m_connected = 0;
				if (code == -1001) {
					pdlg->m_ctrl_stat.ed_mc.SetWindowText(_T("连接卡失败"));
				}
			}
		}
		else if (jmessage["cmd"].asString() == "Pos") {
			if (jmessage["sub"] == "point") {
				CString tmpstr;
				tmpstr.Format(L"%d", jmessage["xpulse"].asInt());
				//pdlg->m_ctrl_stat.GetDlgItem(IDC_EDIT7)->SetWindowTextW(tmpstr);
				GetDlgItem(IDC_EDIT7)->SetWindowTextW(tmpstr);

				tmpstr.Format(L"%d", jmessage["ypulse"].asInt() * (1));
				//pdlg->m_ctrl_stat.GetDlgItem(IDC_EDIT8)->SetWindowTextW(tmpstr);
				GetDlgItem(IDC_EDIT8)->SetWindowTextW(tmpstr);

			}
		}
		else if (jmessage["cmd"].asString() == "Track") {
			if (jmessage["sub"] == "step") {
				if (jmessage["code"].asInt() == 0) {
					int tid = jmessage["tid"].asInt();
					((CButton*)GetDlgItem(IDC_BUTTON1))->EnableWindow(true);
					((CButton*)GetDlgItem(IDC_BUTTON2))->EnableWindow(false);
					m_task_stat = 0;
					pdlg->m_ctrl_lx.powerOnOff(0);
					pdlg->m_ctrl_x20.turnOnOff(0);
					CString tipstr;
					int idx = m_height_list.GetCurSel();
					if (idx < 11) {
						if (tid != 3) {
							tipstr.Format(L"高度%dMM已经扫描完成，请调整到%dMM，点击确定按钮继续扫描！", getZHeight(idx), getZHeight(idx + 1));
							int ret = ::MessageBox(NULL, tipstr, L"提示", MB_YESNO);
						//	int ret = IDYES;
							if (ret == IDYES) {
								isRepeatTrack = 1;
								m_height_list.SetCurSel(idx + 1);
								pdlg->m_page_list.curIdx = -1;
								OnBnClickedButton1();
							}
							else if (ret == IDNO) {
								isRepeatTrack = 0;
								tipstr.Format(L"所有高度已经扫描完成，测试结束！");
								int ret = ::MessageBox(NULL, tipstr, L"提示", MB_OK);
								pdlg->m_ctrl_lx.powerOnOff(0);
								pdlg->m_ctrl_x20.turnOnOff(0);
								pdlg->m_page_list.curIdx = -1;

							}
						}
						else {
							tipstr.Format(L"高度%dMM已经扫描完成，请先导出数据，然后调整高度进行后续测试！", getZHeight(idx));
							int ret = ::MessageBox(NULL, tipstr, L"提示", MB_OK);
						}
					}
					else {
						pdlg->m_page_list2.isDataready = 1;
						isRepeatTrack = 0;
					//	pdlg->m_page_list.isDataready = 1;
					}
				}
				else if (jmessage["code"].asInt() == 1) {
					pdlg->m_ctrl_lx.powerOnOff(1);
					pdlg->m_ctrl_x20.turnOnOff(1);

					m_isMeasure = 1;
				}
				else if (jmessage["code"].asInt() == 2) {
					pdlg->m_ctrl_lx.powerOnOff(0);
					pdlg->m_ctrl_x20.turnOnOff(0);

					m_isMeasure = 0;
				}else if (jmessage["code"].asInt() == 3) {
					int xstep = jmessage["x"].asInt();
					int ystep = jmessage["y"].asInt();

					if (m_task_stat == 1) {
						pdlg->m_page_list3.onPosVal(xstep, ystep);
						pdlg->m_page_track.onPosVal(xstep, ystep);
					}
				}

			}
			else if (jmessage["sub"] == "pos") {
				CString tmpstr;
				tmpstr.Format(L"%d", showX(jmessage["xpulse"].asInt()));
				pdlg->m_ctrl_stat.GetDlgItem(IDC_EDIT5)->SetWindowTextW(tmpstr);

				m_track_curx = showX(jmessage["xpulse"].asInt());
				tmpstr.Format(L"%d", showY(jmessage["ypulse"].asInt() * (-1)));
				pdlg->m_ctrl_stat.GetDlgItem(IDC_EDIT6)->SetWindowTextW(tmpstr);
				m_track_cury = showY(jmessage["ypulse"].asInt() * (-1));
				CString velstr;
				velstr.Format(L"%.2f, %.2f", jmessage["xvel"].asDouble() * 1000.0 / 300, jmessage["yvel"].asDouble() * 1000.0 / 300);
				pdlg->m_ctrl_stat.ed_mc_speed.SetWindowTextW(velstr);

				string axis = jmessage["axis"].asString();
				float dir = jmessage["dir"].asDouble();
				int gidx = jmessage["gidx"].asInt();
				double dbtm = jmessage["tm"].asDouble();

				if (m_task_stat == 1) {
					if (m_models.GetCurSel() == 0) {
						pdlg->m_page_list2.onPosVal(m_track_curx, m_track_cury, axis, dir, gidx, unsigned long(dbtm));
					}
					else if (m_models.GetCurSel() == 1) {
						pdlg->m_page_list.onPosVal(m_track_curx, m_track_cury,axis,dir,gidx);
					}

					pdlg->m_page_track.onPosVal(m_track_curx, m_track_cury, axis, dir, gidx);
				}

			}
			else if (jmessage["sub"] == "point") {
				CString velstr;
				velstr.Format(L"%.2f, %.2f", jmessage["xvel"].asDouble() * 1000.0/300, jmessage["yvel"].asDouble() * 1000.0/300);
				pdlg->m_ctrl_stat.ed_mc_speed.SetWindowTextW(velstr);

				if (m_ctrl_mod == 2) {
					CString tmpstr;
					tmpstr.Format(L"%d", showX(jmessage["xpulse"].asInt()));
					pdlg->m_ctrl_stat.GetDlgItem(IDC_EDIT5)->SetWindowTextW(tmpstr);

					tmpstr.Format(L"%d", showY(jmessage["ypulse"].asInt() * (-1)));
					pdlg->m_ctrl_stat.GetDlgItem(IDC_EDIT6)->SetWindowTextW(tmpstr);
					//m_isMeasure = 1;
					m_track3_curx = showX(jmessage["xpulse"].asInt());
					m_track3_cury = showY(jmessage["ypulse"].asInt() * (-1));
					//pdlg->m_page_track.onPosVal(m_track_curx, m_track_cury);

					/*if (m_task_stat == 1) {
						pdlg->m_page_list3.onPosVal(m_track_curx, m_track_cury);
					}*/
				}else if (m_ctrl_mod == 1) {
					CString tmpstr;
					tmpstr.Format(L"%d", jmessage["xpulse"].asInt());
					GetDlgItem(IDC_EDIT7)->SetWindowTextW(tmpstr);

					tmpstr.Format(L"%d", jmessage["ypulse"].asInt() * (-1));
					GetDlgItem(IDC_EDIT8)->SetWindowTextW(tmpstr);
				}
			}
		}

	}
	delete msg;

	return 0;
}

 int SettingMC::showX(int x) {
	 int ret = x - (pdlg->m_page_track.m_led_width / 2 - pdlg->m_page_track.m_led_driftx - pdlg->m_page_track.idriftx);
//	int ret2 = x - pdlg->m_page_track.m_led_width / 2 + pdlg->m_page_track.idriftx- pdlg->m_page_track.m_led_driftx;
	return ret;
};
 int SettingMC::showY(int y) {
	 int ret = y - (pdlg->m_page_track.m_led_height / 2 - pdlg->m_page_track.m_led_drifty - pdlg->m_page_track.idrifty);
		//y - pdlg->m_page_track.m_led_height / 2 + pdlg->m_page_track.idrifty- pdlg->m_page_track.m_led_drifty;
	return ret;
};

bool SettingMC::precheck(int tips) {
	if (!theApp.getBool(_T("system"), _T("thread_mc"))) {
		if(tips)
		::MessageBox(NULL, _T("运动卡未启用!"), _T("ERROR"), MB_OK);
		return false;
	}
	if (m_connected == 0) {
		if (tips)
		::MessageBox(NULL, _T("运动卡未连接!"), _T("ERROR"), MB_OK);
		return false;
	}

	return true;
}

void SettingMC::sendCommand(string& cmd,  char* peek) {
	if (cmd != "Connect") {
		if (m_connected == 0) {
			return;
		}
	}

	Json::Value root;
	Json::FastWriter writer;
	root["cmd"] = cmd;
	string peekcmd = "";
	if (peek != NULL) {
		peekcmd = peek;
	}

	if (cmd == "Connect")
	{
		root["ip"] = "192.168.0.201";
		root["devip"] = "192.168.0.1";
		root["port"] = 60000;
	}
	else if (cmd == "Close") {

	}
	else if (cmd == "Joghome") {
		root["dir"] = 1;
		root["axis"] = 1;
	}
	else if (cmd == "Stop") {
		peekcmd = "jog_stop";
	}
	else if (cmd == "Pos") {
		if (curAxis == 1) {
			root["ydist"] = 0;

			CString tempstr;
			GetDlgItem(IDC_EDIT4)->GetWindowText(tempstr);
			root["xdist"] = _ttoi(tempstr);


		}
		else if (curAxis == 2) {
			root["xdist"] = 0;
			CString tempstr;
			GetDlgItem(IDC_EDIT4)->GetWindowText(tempstr);
			root["ydist"] = _ttoi(tempstr);

		}
		CString tempstr;
		GetDlgItem(IDC_EDIT4)->GetWindowText(tempstr);
		root["vel"] = _ttoi(tempstr);
		
	}
	else if (cmd == "Zero") {
		root["axis"] = curAxis;
	}
	else if (cmd == "Close") {

	}

	string msg = writer.write(root);
	pdlg->mcQueue_.push(msg, peekcmd);

	return;
}


BOOL SettingMC::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	int curaxis = 0;
	if (((CButton*)GetDlgItem(IDC_RADIO3))->GetCheck() == 1)
		curaxis = 1;
	else if (((CButton*)GetDlgItem(IDC_RADIO4))->GetCheck() == 1)
		curaxis = 2;

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		
		/*CString tempstr;
		GetDlgItem(IDC_EDIT5)->GetWindowText(tempstr);
		AfxMessageBox(tempstr);*/

		return TRUE;
	}

	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:

		//负向
		if (pMsg->hwnd == GetDlgItem(IDC_BUTTON3)->m_hWnd || pMsg->hwnd == GetDlgItem(IDC_BUTTON4)->m_hWnd)
		{
			if (jog_sent == 0) {

				Json::Value root;
				Json::FastWriter writer;

				root["cmd"] = "Jog";
				if (pMsg->hwnd == GetDlgItem(IDC_BUTTON4)->m_hWnd) {
					root["dir"] = -1;
				}
				else {
					root["dir"] = 1;

				}

			

				root["axis"] = curaxis;
				root["acc"] = 0.1;
				root["dec"] = 0.1;

				string msg = writer.write(root);
				pdlg->mcQueue_.push(msg);
				jog_sent = 1;
			}

		}
		break;
	case WM_LBUTTONUP:
		if (pMsg->hwnd == GetDlgItem(IDC_BUTTON3)->m_hWnd || pMsg->hwnd == GetDlgItem(IDC_BUTTON4)->m_hWnd) {

			Json::Value root;
			Json::FastWriter writer;

			root["cmd"] = "Stop";
			root["axis"] = curaxis;
			root["acc"] = 0.1;
			root["dec"] = 0.1;

			string msg = writer.write(root);
			pdlg->mcQueue_.push(msg, "jog_stop");
			jog_sent = 0;
		}
		break;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void SettingMC::OnCbnSelchangeCombo2()
{
	// TODO: 在此添加控件通知处理程序代码
	int idx = m_models.GetCurSel();
	Json::Value list;
	pdlg->m_data_pages.SetCurSel(0);
	pdlg->m_page_track.readTracklist(list);

	Json::Value arr = list["list"];
	for (int i = 0; i < arr.size(); i++) {
		int type = arr[i]["model"].asInt();
		if (type == idx) {
			string nostr = arr[i]["id"].asString();
			//m_tracklist.InsertString(i, CString(nostr.c_str()));
			GetDlgItem(IDC_EDIT1)->SetWindowTextW(CString(nostr.c_str()));

			break;
		}
	}
	pdlg->m_page_track.m_models.SetCurSel(idx);
	pdlg->m_page_track.updateTrackSel();
	//refreshList(idx);
}


void SettingMC::switchPan(int gid) {
	bool flag = false;
	if (gid == 1)
		flag = false;
	else if (gid == 2)
		flag = true;
	/*GetDlgItem(IDC_BUTTON1)->EnableWindow(flag);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(flag);
	GetDlgItem(IDC_COMBO1)->EnableWindow(flag);
	GetDlgItem(IDC_EDIT1)->EnableWindow(flag);
	GetDlgItem(IDC_EDIT5)->EnableWindow(flag);
	GetDlgItem(IDC_EDIT6)->EnableWindow(flag);
	m_models.EnableWindow(flag);*/

	GetDlgItem(IDC_BUTTON1)->ShowWindow(flag);
	GetDlgItem(IDC_BUTTON2)->ShowWindow(flag);
	GetDlgItem(IDC_COMBO1)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT1)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT5)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT6)->ShowWindow(flag);
	m_models.ShowWindow(flag);

	GetDlgItem(IDC_STATIC1)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC2)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC3)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC4)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC5)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC6)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC7)->ShowWindow(flag);
	m_chk_nopower.ShowWindow(flag);



	if (gid == 1)
		flag = true;
	else if (gid == 2)
		flag = false;
	/*GetDlgItem(IDC_BUTTON3)->EnableWindow(flag);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(flag);
	GetDlgItem(IDC_BUTTON5)->EnableWindow(flag);
	GetDlgItem(IDC_BUTTON6)->EnableWindow(flag);
	GetDlgItem(IDC_EDIT3)->EnableWindow(flag);
	GetDlgItem(IDC_EDIT4)->EnableWindow(flag);*/

	GetDlgItem(IDC_BUTTON3)->ShowWindow(flag);
	GetDlgItem(IDC_BUTTON4)->ShowWindow(flag);
	GetDlgItem(IDC_BUTTON5)->ShowWindow(flag);
	GetDlgItem(IDC_BUTTON6)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT3)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT4)->ShowWindow(flag);

	GetDlgItem(IDC_STATIC11)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC12)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC13)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC14)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC15)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC16)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC17)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC18)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC19)->ShowWindow(flag);
	GetDlgItem(IDC_RADIO3)->ShowWindow(flag);
	GetDlgItem(IDC_RADIO4)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT7)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT8)->ShowWindow(flag);
	GetDlgItem(IDC_BUTTON8)->ShowWindow(flag);
	GetDlgItem(IDC_BUTTON10)->ShowWindow(flag);



	

	m_ctrl_mod = gid;
}

void SettingMC::OnBnClickedRadio1()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_task_stat == 1) {
		::MessageBox(NULL, _T("请先结束测试任务!"), _T("ERROR"), MB_OK);
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(false);
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(true);
		return;
	}
	switchPan(1);

}


void SettingMC::OnBnClickedRadio2()
{
	// TODO: 在此添加控件通知处理程序代码
	switchPan(2);
}


void SettingMC::OnBnClickedRadio3()
{
	// TODO: 在此添加控件通知处理程序代码
	curAxis = 1;
}


void SettingMC::OnBnClickedRadio4()
{
	// TODO: 在此添加控件通知处理程序代码
	curAxis = 2;
}


void SettingMC::OnBnClickedBTNStop()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_task_stat == 0)return;

	sendCommand(string("Track"),"track_stop" );

	m_task_stat = 0;
	((CButton*)GetDlgItem(IDC_BUTTON2))->EnableWindow(false);
	((CButton*)GetDlgItem(IDC_BUTTON1))->EnableWindow(true);

	stopTask();
}

void SettingMC::OnBnClickedButton1()
{

	if (!precheck(1))
		return;
	if (m_task_stat > 0)
		return;


	((CButton*)GetDlgItem(IDC_BUTTON1))->EnableWindow(false);
	((CButton*)GetDlgItem(IDC_BUTTON2))->EnableWindow(true);

	m_task_stat = 1;
	// TODO: 在此添加控件通知处理程序代码
	startTask();
}

int SettingMC::startTask() {
	//this->sendCommand(string("TrackTask"));
	{
		Json::Value root;
		Json::FastWriter writer;
		root["cmd"] = "TrackTask";
		string peekcmd = "";
		CString tempstr;
		GetDlgItem(IDC_EDIT5)->GetWindowText(tempstr);
		m_curSpeed = _ttoi(tempstr);
		root["vel"] = m_curSpeed;
		root["repeat"] = isRepeatTrack;
		root["model"] = m_models.GetCurSel() + 1;
		root["tracknum"] = "1111111";
		string msg = writer.write(root);
		pdlg->mcQueue_.push(msg, peekcmd);
	}
	if (m_models.GetCurSel() + 1 == 1) {
		pdlg->m_page_list2.onNewLine();
	}
	if (m_models.GetCurSel() + 1 == 2) {
		pdlg->m_page_list.onMeasureStart();
	}
	if (m_onlyTrack == 1) {
		pdlg->m_ctrl_lx.powerOnOff(0);
		pdlg->m_ctrl_x20.turnOnOff(0);

	}
	else {
		//pdlg->m_ctrl_lx.powerOnOff(1);
		//pdlg->m_ctrl_x20.turnOnOff(1);

	}
	return 0;
}
int SettingMC::stopTask() { 
	isRepeatTrack = 0;
	CString tipstr;
	tipstr.Format(L"测量结束！");
	int ret = ::MessageBox(NULL, tipstr, L"提示", MB_OK);
	pdlg->m_ctrl_lx.powerOnOff(0);
	pdlg->m_ctrl_x20.turnOnOff(0);
	pdlg->m_page_list.curIdx = -1;
	return 0;
}
int SettingMC::pauseTask() { return 0; }
int SettingMC::continueTask() { return 0; }

void SettingMC::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
}


void SettingMC::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
}


void SettingMC::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	sendCommand(string("Home"));
}


void SettingMC::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	sendCommand(string("Stop"));
}


void SettingMC::OnBnClickedButton8()
{
	// TODO: 在此添加控件通知处理程序代码
	sendCommand(string("Pos"));

}


void SettingMC::OnBnClickedButton10()
{
	// TODO: 在此添加控件通知处理程序代码
	sendCommand(string("Zero"));

}


void SettingMC::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	if (IsDlgButtonChecked(IDC_CHECK1)) {
		m_onlyTrack = 1;
	}
	else {
		m_onlyTrack = 0;
	}
}


void SettingMC::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int curval = sp_ledx.GetPos() + pNMUpDown->iDelta;
	if (iLedx != curval) {
		iLedx = curval;
	}
	*pResult = 0;
}


void SettingMC::OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int curval = sp_ledx.GetPos() + pNMUpDown->iDelta;
	if (iLedy != curval) {
		iLedy = curval;
	}
	*pResult = 0;
}
