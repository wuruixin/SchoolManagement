
// UserDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "User.h"
#include "UserDlg.h"
#include "afxdialogex.h"


#include "../../Common/CUMT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUserDlg 对话框



CUserDlg::CUserDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_USER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
}

BEGIN_MESSAGE_MAP(CUserDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ADD_STUDENT, &CUserDlg::OnBnClickedButtonAddStudent)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_STUDENT, &CUserDlg::OnBnClickedButtonModifyStudent)
	ON_BN_CLICKED(IDC_BUTTON_DEL_STUDENT, &CUserDlg::OnBnClickedButtonDelStudent)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_STUDENT, &CUserDlg::OnBnClickedButtonCheckStudent)
	ON_BN_CLICKED(IDC_BUTTON_ADD_COURSE, &CUserDlg::OnBnClickedButtonAddCourse)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_COURSE, &CUserDlg::OnBnClickedButtonModifyCourse)
	ON_BN_CLICKED(IDC_BUTTON_DEL_COURSE, &CUserDlg::OnBnClickedButtonDelCourse)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_COURSE, &CUserDlg::OnBnClickedButtonCheckCourse)
	ON_BN_CLICKED(IDC_BUTTON_ADD_CLASS, &CUserDlg::OnBnClickedButtonAddClass)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_CLASS, &CUserDlg::OnBnClickedButtonModifyClass)
	ON_BN_CLICKED(IDC_BUTTON_DEL_CLASS, &CUserDlg::OnBnClickedButtonDelClass)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_CLASS, &CUserDlg::OnBnClickedButtonCheckClass)
	//ON_BN_CLICKED(IDC_BUTTON1, &CUserDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_CONNECT, &CUserDlg::OnBnClickedButtonCreateConnect)
	ON_BN_CLICKED(IDC_BUTTON_GIVE_CLASS, &CUserDlg::OnBnClickedButtonGiveClass)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_GIVECLASS, &CUserDlg::OnBnClickedButtonCheckGiveclass)
	ON_BN_CLICKED(IDC_BUTTON_GIVE_COURSE, &CUserDlg::OnBnClickedButtonGiveCourse)
	ON_BN_CLICKED(IDC_DEL_GIVECOURSE, &CUserDlg::OnBnClickedDelGivecourse)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_STU_COURSE, &CUserDlg::OnBnClickedButtonCheckStuCourse)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_COURSE_STU, &CUserDlg::OnBnClickedButtonCheckCourseStu)
END_MESSAGE_MAP()


// CUserDlg 消息处理程序

BOOL CUserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUserDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUserDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


CUMT umt{};



VOID CUserDlg::DealMessage(CString SendSqlInfo)
{

	m_List.DeleteAllItems();
	int nNumber = m_List.GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nNumber; i++) {
		m_List.DeleteColumn(0);
	}

	umt.Send((LPBYTE)(LPCSTR)SendSqlInfo, SendSqlInfo.GetLength());

	//获取消息
	char buff[0x10000] = { 0 };
	DWORD dwNeedRead = umt.Recv((BYTE*)buff, 4);
	DWORD dwHaveRead = 0;
	if (dwNeedRead > dwHaveRead)
	{
		DWORD t_Read = umt.Recv((BYTE*)buff, 0x10000);
		dwHaveRead += t_Read;
	}
	CString szMsg = buff;

	//AfxMessageBox(szMsg);

	//处理消息，获取信息
	if (szMsg.GetLength() == 1) 
	{
		AfxMessageBox("操作成功");
		return;
	}

	int ncol = 0;

	CString sInfo1,sInfo2;
	sInfo1 = szMsg.Left(szMsg.Find("/"));
	sInfo2 = szMsg.Right(szMsg.GetLength() - szMsg.Find("/") - 1);

	//处理消息头
	while (TRUE) {
		if (sInfo1.Find("|") == -1) {
			m_List.InsertColumn(ncol++, sInfo1, LVCFMT_LEFT, 200);
			break;
		}

		CString sName;
		sName = sInfo1.Left(sInfo1.Find("|"));
		sInfo1 = sInfo1.Right(sInfo1.GetLength() - sInfo1.Find("|") - 1);

		m_List.InsertColumn(ncol++, sName, LVCFMT_LEFT, 200);
	}

	int nRow = 0, nCol = 0;
	//处理消息体
	while (sInfo2.Find("}") != -1) {
		nCol = 0;

		//插入行
		m_List.InsertItem(nRow, "");

		CString sMessage;
		sMessage = sInfo2.Mid(1, sInfo2.Find("}") - 1);
		sInfo2 = sInfo2.Right(sInfo2.GetLength() - sInfo2.Find("}") - 1);

		while (TRUE) {
			if (sMessage.Find("|") == -1) {
				m_List.SetItemText(nRow++, nCol++, sMessage);
				break;
			}

			CString sTemp = sMessage.Left(sMessage.Find("|"));
			sMessage = sMessage.Right(sMessage.GetLength() - sMessage.Find("|") - 1);

			//设置某一行
			m_List.SetItemText(nRow, nCol++, sTemp);

		}
	}

	return;
}


//-------------------------------------------------------------------
// @brief 点击后添加学生信息
// @input 学生学号 学生姓名
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonAddStudent()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Input1, Input2, SendSqlInfo;

	GetDlgItemText(IDC_EDIT1, Input1);
	GetDlgItemText(IDC_EDIT2, Input2);

	if (Input1.GetLength() == 0 || Input2.GetLength() == 0) {
		AfxMessageBox("请输入学生学号和学生姓名");
		return;
	}

	SendSqlInfo.Format("insert into t_stu(t_stu.stu_number,t_stu.stu_name) values(\"%s\", \"%s\")", Input1.GetString(), Input2.GetString());

	//AfxMessageBox(SendSqlInfo);

	DealMessage(SendSqlInfo);

}

//-------------------------------------------------------------------
// @brief 点击后修改学生信息
// @input 修改前学生学号 修改后学生姓名
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonModifyStudent()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Input1, Input2, SendSqlInfo;

	GetDlgItemText(IDC_EDIT1, Input1);
	GetDlgItemText(IDC_EDIT2, Input2);

	SendSqlInfo.Format("update t_stu set stu_name = \"%s\" WHERE stu_number = \"%s\"", 
		Input2.GetString(), Input1.GetString());

	//AfxMessageBox(SendSqlInfo);

	DealMessage(SendSqlInfo);
}


//-------------------------------------------------------------------
// @brief 点击后删除学生信息
// @input 学生学号
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonDelStudent()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Input1, SendSqlInfo;

	GetDlgItemText(IDC_EDIT1, Input1);

	SendSqlInfo.Format("delete from t_stu where stu_number=\"%s\"", Input1.GetString());

	//AfxMessageBox(SendSqlInfo);

	DealMessage(SendSqlInfo);
}


//-------------------------------------------------------------------
// @brief 点击后查询学生信息
// @input 学生学号或学生姓名 支持4种查询方法
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonCheckStudent()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Input1, Input2, SendSqlInfo;

	GetDlgItemText(IDC_EDIT1, Input1);
	GetDlgItemText(IDC_EDIT2, Input2);

	if (Input1.GetLength() == 0 && Input2.GetLength() == 0)
	{
		SendSqlInfo.Format("select t_stu.stu_number as 学号,t_stu.stu_name as 姓名 from t_stu");
	}
	else if (Input1.GetLength() != 0 && Input2.GetLength() == 0)
	{
		SendSqlInfo.Format("select t_stu.stu_number as 学号,t_stu.stu_name as 姓名 from t_stu where stu_number=\"%s\"", Input1.GetString());
	}
	if (Input1.GetLength() == 0 && Input2.GetLength() != 0)
	{
		SendSqlInfo.Format("select t_stu.stu_number as 学号,t_stu.stu_name as 姓名 from t_stu where stu_name=\"%s\"", Input2.GetString());
	}
	if (Input1.GetLength() != 0 && Input2.GetLength() != 0)
	{
		SendSqlInfo.Format("select t_stu.stu_number as 学号,t_stu.stu_name as 姓名 from t_stu where stu_number=\"%s\" and stu_name=\"%s\"",
			Input1.GetString(), Input2.GetString());
	}

	//AfxMessageBox(SendSqlInfo);

	DealMessage(SendSqlInfo);

}


//-------------------------------------------------------------------
// @brief 点击后添加课程信息
// @input 课程号 课程名
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonAddCourse()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Input3, Input4, SendSqlInfo;

	GetDlgItemText(IDC_EDIT3, Input3);
	GetDlgItemText(IDC_EDIT4, Input4);

	if (Input3.GetLength() == 0 || Input4.GetLength() == 0) {
		AfxMessageBox("请输入课程号和课程名字");
		return;
	}

	SendSqlInfo.Format("insert into t_course(t_course.course_number, t_course.course_name) values (\"%s\", \"%s\")", Input3.GetString(), Input4.GetString());
	//AfxMessageBox(SendSqlInfo);

	DealMessage(SendSqlInfo);
}


//-------------------------------------------------------------------
// @brief 点击后修改学生信息
// @input 修改前课程号 修改后课程名
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonModifyCourse()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Input3, Input4, SendSqlInfo;

	GetDlgItemText(IDC_EDIT3, Input3);
	GetDlgItemText(IDC_EDIT4, Input4);

	SendSqlInfo.Format("update t_course set course_name = \"%s\" WHERE course_number = \"%s\"",
		Input4.GetString(), Input3.GetString());

	//AfxMessageBox(SendSqlInfo);

	DealMessage(SendSqlInfo);
}


//-------------------------------------------------------------------
// @brief 点击后删除课程信息
// @input 课程号
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonDelCourse()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Input3, SendSqlInfo;

	GetDlgItemText(IDC_EDIT3, Input3);

	SendSqlInfo.Format("delete from t_course where course_number=\"%s\"", Input3.GetString());

	//AfxMessageBox(SendSqlInfo);

	DealMessage(SendSqlInfo);
}


//-------------------------------------------------------------------
// @brief 点击后查询课程信息
// @input 课程号或课程名
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonCheckCourse()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Input3, Input4, SendSqlInfo;

	GetDlgItemText(IDC_EDIT3, Input3);
	GetDlgItemText(IDC_EDIT4, Input4);

	if (Input3.GetLength() == 0 && Input4.GetLength() == 0)
	{
		SendSqlInfo.Format("select t_course.course_number as 课程号,t_course.course_name as 课程名 from t_course");
	}
	else if (Input3.GetLength() != 0 && Input4.GetLength() == 0)
	{
		SendSqlInfo.Format("select t_course.course_number as 课程号,t_course.course_name as 课程名 from t_course where course_number=\"%s\"", Input3.GetString());
	}
	if (Input3.GetLength() == 0 && Input4.GetLength() != 0)
	{
		SendSqlInfo.Format("select t_course.course_number as 课程号,t_course.course_name as 课程名 from t_course where course_name=\"%s\"", Input4.GetString());
	}
	if (Input3.GetLength() != 0 && Input4.GetLength() != 0)
	{
		SendSqlInfo.Format("select t_course.course_number as 课程号,t_course.course_name as 课程名 from t_course where course_number=\"%s\" and course_name=\"%s\"",
			Input3.GetString(), Input4.GetString());
	}

	//AfxMessageBox(SendSqlInfo);

	DealMessage(SendSqlInfo);
}


//-------------------------------------------------------------------
// @brief 点击后添加班级信息
// @input 班级名
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonAddClass()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Input5, SendSqlInfo;

	GetDlgItemText(IDC_EDIT5, Input5);

	if (Input5.GetLength() == 0) {
		AfxMessageBox("请输入班级名字");
		return;
	}

	SendSqlInfo.Format("insert into t_class(t_class.class_name) values (\"%s\")", Input5.GetString());

	//AfxMessageBox(SendSqlInfo);

	DealMessage(SendSqlInfo);
}


//-------------------------------------------------------------------
// @brief 点击后修改班级信息
// @input 修改前的班级名和修改后的班级名
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonModifyClass()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Input5, Input6, SendSqlInfo;

	GetDlgItemText(IDC_EDIT5, Input5);
	GetDlgItemText(IDC_EDIT6, Input6);

	SendSqlInfo.Format("update t_class set class_name = \"%s\" where t_class.class_name =\"%s\"",
		Input6.GetString(), Input5.GetString());

	//AfxMessageBox(SendSqlInfo);

	DealMessage(SendSqlInfo);
}


//-------------------------------------------------------------------
// @brief 点击后删除班级信息
// @input 班级名称
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonDelClass()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Input5, SendSqlInfo;

	GetDlgItemText(IDC_EDIT5, Input5);

	SendSqlInfo.Format("delete from t_class where class_name=\"%s\"", Input5.GetString());

	//AfxMessageBox(SendSqlInfo);

	DealMessage(SendSqlInfo);
}


//-------------------------------------------------------------------
// @brief 点击后查询班级信息
// @input 班级名
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonCheckClass()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Input1, Input5, SendSqlInfo;

	GetDlgItemText(IDC_EDIT1, Input1);
	GetDlgItemText(IDC_EDIT5, Input5);

	if (Input5.GetLength() == 0)
	{
		SendSqlInfo.Format("select t_class.class_name as \"班级名称\" from t_class");
	}
	else
	{
		SendSqlInfo.Format("select t_class.class_name as \"班级名称\" from t_class where class_name=\"%s\"", Input5.GetString());
	}

	//AfxMessageBox(SendSqlInfo);

	DealMessage(SendSqlInfo);
}



//-------------------------------------------------------------------
// @brief 组合查询 给学生分配班级
// @input 学生学号  班级名称
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonGiveClass()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Input1, Input5, SendSqlInfo;

	GetDlgItemText(IDC_EDIT1, Input1);
	GetDlgItemText(IDC_EDIT5, Input5);

	if (Input1.GetLength() == 0 || Input5.GetLength() == 0)
	{
		AfxMessageBox("请输入学生学号和班级名称");
	}

	SendSqlInfo.Format("update t_stu set t_stu.stu_class = (select t_class.class_id from t_class where t_class.class_name = \"%s\") where t_stu.stu_number = \"%s\"", Input5.GetString(), Input1.GetString());
	
	//AfxMessageBox(SendSqlInfo);

	DealMessage(SendSqlInfo);
}


//-------------------------------------------------------------------
// @brief 组合查询 查询班级信息
// @input 
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonCheckGiveclass()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Input1, Input2, Input5, SendSqlInfo;

	GetDlgItemText(IDC_EDIT1, Input1);
	GetDlgItemText(IDC_EDIT2, Input2);
	GetDlgItemText(IDC_EDIT5, Input5);

	if (Input1.GetLength() != 0)
	{
		SendSqlInfo.Format("select t_stu.stu_number as 学生学号,t_stu.stu_name 学生姓名,t_class.class_name 班级名称 from t_class inner join t_stu on t_stu.stu_class = t_class.class_id and t_stu.stu_number = \"%s\"",Input1.GetString());
	}
	else if (Input2.GetLength() != 0)
	{
		SendSqlInfo.Format("select t_stu.stu_number as 学生学号,t_stu.stu_name 学生姓名,t_class.class_name 班级名称 from t_class inner join t_stu on t_stu.stu_class = t_class.class_id and t_stu.stu_name = \"%s\"",Input2.GetString());
	}
	else if(Input5.GetLength() != 0)
	{
		SendSqlInfo.Format("select t_stu.stu_number as 学生学号,t_stu.stu_name 学生姓名,t_class.class_name 班级名称 from t_class inner join t_stu on t_stu.stu_class = t_class.class_id and t_class.class_name = \"%s\"", Input5.GetString());
	}
	else 
	{
		AfxMessageBox("请输入学生学号或学生姓名或班级名称进行查询！");
		return;
	}

	//AfxMessageBox(SendSqlInfo);

	DealMessage(SendSqlInfo);
}


//-------------------------------------------------------------------
// @brief 组合查询 给学生分配课程
// @input 
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonGiveCourse()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Input1, Input3, SendSqlInfo;

	GetDlgItemText(IDC_EDIT1, Input1);
	GetDlgItemText(IDC_EDIT3, Input3);

	if (Input1.GetLength() == 0 || Input3.GetLength() == 0) {
		AfxMessageBox("请输入学号和课程号");
		return;
	}
	
	SendSqlInfo.Format("insert into t_selcourse values((select t_stu.stu_id from t_stu where t_stu.stu_number = \"%s\"),(select t_course.course_id from t_course where t_course.course_number = \"%s\"))", Input1.GetString(), Input3.GetString());

	//AfxMessageBox(SendSqlInfo);

	DealMessage(SendSqlInfo);
}


//-------------------------------------------------------------------
// @brief 组合查询 删除学生选择的课程
// @input 
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedDelGivecourse()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Input1, Input3, SendSqlInfo;

	GetDlgItemText(IDC_EDIT1, Input1);
	GetDlgItemText(IDC_EDIT3, Input3);

	if (Input1.GetLength() == 0 || Input3.GetLength() == 0) {
		//AfxMessageBox("请输入学号和课程号");
		return;
	}

	SendSqlInfo.Format("delete from t_selcourse where t_selcourse.sel_stu_id=(select t_stu.stu_id from t_stu where t_stu.stu_number=\"%s\") and t_selcourse.sel_course_id = (select t_course.course_id from t_course where t_course.course_number = \"%s\")", Input1.GetString(), Input3.GetString());

	//AfxMessageBox(SendSqlInfo);

	DealMessage(SendSqlInfo);
}

//-------------------------------------------------------------------
// @brief 组合查询 查看学生课程
// @input 
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonCheckStuCourse()
{
	// TODO: 在此添加控件通知处理程序代码

	CString Input1, SendSqlInfo;

	GetDlgItemText(IDC_EDIT1, Input1);

	if (Input1.GetLength() != 0) 
	{
		SendSqlInfo.Format("select t_stu.stu_number as 学生学号,t_stu.stu_name as 学生姓名, t_course.course_number as 课程号, t_course.course_name as 课程名 from t_stu, t_course, t_selcourse where t_selcourse.sel_stu_id = t_stu.stu_id and t_selcourse.sel_course_id = t_course.course_id and t_selcourse.sel_stu_id = (select t_stu.stu_id from t_stu where t_stu.stu_number = %s)",Input1.GetString());
	}
	
	//AfxMessageBox(SendSqlInfo);

	DealMessage(SendSqlInfo);
}


//-------------------------------------------------------------------
// @brief 组合查询 查看课程学生
// @input 
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonCheckCourseStu()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Input3, SendSqlInfo;

	GetDlgItemText(IDC_EDIT3, Input3);

	if (Input3.GetLength() != 0)
	{
		SendSqlInfo.Format("select t_course.course_number as 课程号, t_course.course_name as 课程名,t_stu.stu_number as 学生学号,t_stu.stu_name as 学生姓名 from t_stu, t_course, t_selcourse where t_selcourse.sel_stu_id = t_stu.stu_id and t_selcourse.sel_course_id = t_course.course_id and t_course.course_id = (select t_course.course_id from t_course where t_course.course_number = %s)", Input3.GetString());
	}

	//AfxMessageBox(SendSqlInfo);
	DealMessage(SendSqlInfo);
}


//-------------------------------------------------------------------
// @brief 点击后建立数据库连接
//-------------------------------------------------------------------
void CUserDlg::OnBnClickedButtonCreateConnect()
{
	// TODO: 在此添加控件通知处理程序代码

	//BOOL bRet = umt.Connect("127.0.0.1", 5566);
	BOOL bRet = umt.Connect("192.168.10.1", 5566);

	if (bRet) {
		AfxMessageBox("连接成功");
		return;
	}
	AfxMessageBox("连接失败");
}


BOOL CUserDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	umt.Close();

	return CDialogEx::DestroyWindow();
}
