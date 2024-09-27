
// MeasureBot.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "MeasureBot.h"
#include "MeasureBotDlg.h"
#include "CSplashWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <afxdisp.h> 

// CMeasureBotApp

BEGIN_MESSAGE_MAP(CMeasureBotApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CString service_ini_path;
// CMeasureBotApp 构造

CMeasureBotApp::CMeasureBotApp()
{
	
	// 支持重新启动管理器
	//m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CMeasureBotApp 对象

CMeasureBotApp theApp;

const GUID CDECL BASED_CODE _tlid =
		{0xfd4810f0,0x8835,0x4d7e,{0xac,0xa4,0x05,0x9d,0x00,0xa7,0x07,0xfd}};
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


// CMeasureBotApp 初始化

BOOL CMeasureBotApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	{
		char path[512];
		getcwd(path, 511);

		string m_tRoot = path;
		m_tRoot += "/MeasureBot.ini";
		service_ini_path.Format(L"%s", CString(m_tRoot.c_str()));
	}
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);



	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	// 分析自动化开关或注册/注销开关的命令行。
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 应用程序是用 /Embedding 或 /Automation 开关启动的。
	//使应用程序作为自动化服务器运行。
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// 通过 CoRegisterClassObject() 注册类工厂。
		COleTemplateServer::RegisterAll();
	}
	// 应用程序是用 /Unregserver 或 /Unregister 开关启动的。  移除
	// 注册表中的项。
	else if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
	{
		COleObjectFactory::UpdateRegistryAll(FALSE);
		AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor);
		return FALSE;
	}
	// 应用程序是以独立方式或用其他开关(如 /Register
	// 或 /Regserver)启动的。  更新注册表项，包括类型库。
	else
	{
		COleObjectFactory::UpdateRegistryAll();
		AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid);
		if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppRegister)
			return FALSE;
	}

	if(1){
		CSplashWnd* m_pSplashWnd;
		m_pSplashWnd = new CSplashWnd();
		m_pSplashWnd->Create(IDB_BITMAP2);
		m_pSplashWnd->ShowWindow(SW_SHOW);
		m_pSplashWnd->UpdateWindow();

		// 在这里进行其他初始化操作...

		// 假设1000毫秒后移除启动页
	//	::Sleep(3000); // 实际应用中应使用计时器或其他异步机制
		pts_time::wait(3000);
		m_pSplashWnd->ShowWindow(SW_HIDE);
		m_pSplashWnd->DestroyWindow();
		delete m_pSplashWnd;
		m_pSplashWnd = nullptr;
	}

	DebugLog::init();
	CMeasureBotDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
void CMeasureBotApp::onMessage2RunBoard(const string& message, int msg_id) {
	lock_msg.lock();

	HWND handle = ((CMeasureBotDlg*)m_pMainWnd)->m_ctrl_stat.GetSafeHwnd();
	if (handle == NULL) {
		lock_msg.unlock();
		return;
	}
	string* pMsgStr = new string(message);
	PostMessage(handle, WM_MYMESSAGE_SET_RUN, static_cast<WPARAM>(msg_id), reinterpret_cast<LPARAM>(pMsgStr));
	lock_msg.unlock();
}

void CMeasureBotApp::onMessage2SettingMC(const string& message, int msg_id)
{
	lock_msg.lock();
	
	HWND handle = ((CMeasureBotDlg*)m_pMainWnd)->m_ctrl_mc.GetSafeHwnd();
	if (handle == NULL) {
		lock_msg.unlock();
		return;
	}
	string* pMsgStr = new string(message);
	PostMessage(handle, WM_MYMESSAGE_SET_MC, static_cast<WPARAM>(msg_id), reinterpret_cast<LPARAM>(pMsgStr));
	lock_msg.unlock();
}


void CMeasureBotApp::onMessageCallback(const string& message, int msg_id)
{
	lock_msg.lock();
	HWND handle = m_pMainWnd->GetSafeHwnd();
	if (handle == NULL) {
		lock_msg.unlock();
		return;
	}
	string *pMsgStr = new string(message);
	PostMessage(handle, WM_MYMESSAGE_SIGNAL, static_cast<WPARAM>(msg_id), reinterpret_cast<LPARAM>(pMsgStr));
	lock_msg.unlock();
}

int CMeasureBotApp::ExitInstance()
{
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

CString CMeasureBotApp::tm2CStr(int tm) {
	time_t timestamp = tm; // 获取当前时间戳
	struct tm* tm_local = localtime(&timestamp); // 转换为本地时间

	char date_time[80];
	strftime(date_time, sizeof(date_time), "%Y-%m-%d %H:%M:%S", tm_local);

	CString tmp(date_time);

	return tmp;
}

bool CMeasureBotApp::getBool(LPCWSTR sec, LPCWSTR name)
{
	int intItem = ::GetPrivateProfileInt((sec), (name), 0, service_ini_path);

	return intItem == 1 ? true : false;
}

string CMeasureBotApp::tostr(CString& str) {
	string tmp((CT2A)str);
	return tmp;
}

CString CMeasureBotApp::getString(LPCWSTR sec, LPCWSTR name) {

	CString tmpIP;
	TCHAR tmp1[MAX_PATH] = _T("");
	::GetPrivateProfileString(sec, name, _T(""), tmp1, MAX_PATH, service_ini_path);
	tmpIP = tmp1;

	return tmpIP;
}
int CMeasureBotApp::getInt(LPCWSTR sec, LPCWSTR name) {
	return ::GetPrivateProfileInt((sec), (name), 0, service_ini_path);
}

string CMeasureBotApp::makeFname(char* prefix, int dur) {
	int year, month, day, hour, minute, second;
	time_t current;
	time(&current);
	struct tm* l = localtime(&current);
	string sampleName = theApp.tostr(theApp.getString(_T("sample"), _T("model")));
	string sampleSn = theApp.tostr(theApp.getString(_T("sample"), _T("sn")));
	string waveLen = theApp.tostr(theApp.getString(_T("sample"), _T("wavelength")));
	string suffix = theApp.tostr(theApp.getString(_T("File"), _T("suffix")));

	year = l->tm_year + 1900;
	month = l->tm_mon + 1;
	day = l->tm_mday;
	hour = l->tm_hour;
	minute = l->tm_min;
	second = l->tm_sec;
	char buf[128] = { 0 };
	if (string(prefix) == "Raw") {
		sprintf(buf, "\\%s_%d%02d%02d_%02d%02d%02d.xlsx", prefix,  year, month, day, hour, minute, second);
	}
	else {
		sprintf(buf, "%s_%s_%s_%s_%d%02d%02d_%s.xlsx", prefix, sampleName.c_str(), sampleSn.c_str(), waveLen.c_str(), year, month, day, suffix.c_str());

	}
	string report = buf; //theApp.tostr(theApp.getString(_T("File"), _T("report_path")));
	return report;

}


void CMeasureBotApp::split(const std::string& s, const std::string& delim, std::vector<std::string>* ret)
{

	size_t last = 0;
	size_t index = s.find_first_of(delim, last);

	while (index != std::string::npos)
	{
		ret->push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		ret->push_back(s.substr(last, index - last));
	}
}