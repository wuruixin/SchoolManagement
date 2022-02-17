
// UserDlg.h: 头文件
//

#pragma once


// CUserDlg 对话框
class CUserDlg : public CDialogEx
{
// 构造
public:
	CUserDlg(CWnd* pParent = nullptr);	// 标准构造函数

	VOID DealMessage(CString SendSqlInfo);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_USER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAddStudent();
	afx_msg void OnBnClickedButtonModifyStudent();
	afx_msg void OnBnClickedButtonDelStudent();
	afx_msg void OnBnClickedButtonCheckStudent();
	afx_msg void OnBnClickedButtonAddCourse();
	afx_msg void OnBnClickedButtonModifyCourse();
	afx_msg void OnBnClickedButtonDelCourse();
	afx_msg void OnBnClickedButtonCheckCourse();
	afx_msg void OnBnClickedButtonAddClass();
	afx_msg void OnBnClickedButtonModifyClass();
	afx_msg void OnBnClickedButtonDelClass();
	afx_msg void OnBnClickedButtonCheckClass();
	afx_msg void OnBnClickedButtonUnionCheck();
//	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonCreateConnect();
	afx_msg void OnBnClickedButtonGiveClass();
	afx_msg void OnBnClickedButtonCheckGiveclass();
	afx_msg void OnBnClickedButtonGiveCourse();
	afx_msg void OnBnClickedDelGivecourse();
	CListCtrl m_List;
	afx_msg void OnBnClickedButtonCheckStuCourse();
	afx_msg void OnBnClickedButtonCheckCourseStu();
	virtual BOOL DestroyWindow();
};
