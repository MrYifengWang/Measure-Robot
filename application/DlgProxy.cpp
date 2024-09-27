
// DlgProxy.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MeasureBot.h"
#include "DlgProxy.h"
#include "MeasureBotDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMeasureBotDlgAutoProxy

IMPLEMENT_DYNCREATE(CMeasureBotDlgAutoProxy, CCmdTarget)

CMeasureBotDlgAutoProxy::CMeasureBotDlgAutoProxy()
{
	EnableAutomation();

	// 为使应用程序在自动化对象处于活动状态时一直保持
	//	运行，构造函数调用 AfxOleLockApp。
	AfxOleLockApp();

	// 通过应用程序的主窗口指针
	//  来访问对话框。  设置代理的内部指针
	//  指向对话框，并设置对话框的后向指针指向
	//  该代理。
	ASSERT_VALID(AfxGetApp()->m_pMainWnd);
	if (AfxGetApp()->m_pMainWnd)
	{
		ASSERT_KINDOF(CMeasureBotDlg, AfxGetApp()->m_pMainWnd);
		if (AfxGetApp()->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CMeasureBotDlg)))
		{
			m_pDialog = reinterpret_cast<CMeasureBotDlg*>(AfxGetApp()->m_pMainWnd);
			m_pDialog->m_pAutoProxy = this;
		}
	}
}

CMeasureBotDlgAutoProxy::~CMeasureBotDlgAutoProxy()
{
	// 为了在用 OLE 自动化创建所有对象后终止应用程序，
	//	析构函数调用 AfxOleUnlockApp。
	//  除了做其他事情外，这还将销毁主对话框
	if (m_pDialog != nullptr)
		m_pDialog->m_pAutoProxy = nullptr;
	AfxOleUnlockApp();
}

void CMeasureBotDlgAutoProxy::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。  基类将自动
	// 删除该对象。  在调用该基类之前，请添加您的
	// 对象所需的附加清理代码。

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CMeasureBotDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CMeasureBotDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// 注意: 我们添加了对 IID_IMeasureBot 的支持来支持类型安全绑定
//  以支持来自 VBA 的类型安全绑定。  此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {36aee4be-c5f0-43eb-a8f6-26d1eb8d1e93}
static const IID IID_IMeasureBot =
{0x36aee4be,0xc5f0,0x43eb,{0xa8,0xf6,0x26,0xd1,0xeb,0x8d,0x1e,0x93}};

BEGIN_INTERFACE_MAP(CMeasureBotDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CMeasureBotDlgAutoProxy, IID_IMeasureBot, Dispatch)
END_INTERFACE_MAP()

// IMPLEMENT_OLECREATE2 宏是在此项目的 pch.h 中定义的
// {77d5230e-aba6-4d0b-b2d7-79e1d540d18a}
IMPLEMENT_OLECREATE2(CMeasureBotDlgAutoProxy, "MeasureBot.Application", 0x77d5230e,0xaba6,0x4d0b,0xb2,0xd7,0x79,0xe1,0xd5,0x40,0xd1,0x8a)


// CMeasureBotDlgAutoProxy 消息处理程序
