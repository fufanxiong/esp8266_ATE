#if !defined(AFX_SELDIR_H__392D4B5B_F505_4DD7_BA0F_3081574162E8__INCLUDED_)
#define AFX_SELDIR_H__392D4B5B_F505_4DD7_BA0F_3081574162E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// selDir.h : header file
//

/////////////////////////////////////////////////////////////////////////////
class corder_wknum
{
private:
	corder_wknum& operator = (const corder_wknum &x);
	corder_wknum(corder_wknum &x);
protected:
public:
	corder_wknum(void);
	virtual ~corder_wknum();
	
	CString order;
	
	int wknum_len;
	CStringList* wknum;
	void add_wknum(CString& _wknum);
};
#define ORDER_WKNUM_LEN		256


/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// selDir dialog

class selDir : public CDialog
{
// Construction
public:
	selDir(CWnd* pParent = NULL);   // standard constructor
	virtual ~selDir();
	
	CString str_order;
	CString str_wknum;

protected:
	CStringList* all_dir_list;
	CStringList* GetDirectorys(void);
	
	int order_wknum_num;
	corder_wknum *order_wknum[ORDER_WKNUM_LEN];
	bool add_order_wknum(CString& order,CString& wknum);
	void build_order_wknum(void);
	
	int cur_order;
	int cur_wknum;
	
public:
// Dialog Data
	//{{AFX_DATA(selDir)
	enum { IDD = IDD_SelDir };
	CComboBox	m_sel_wknum;
	CComboBox	m_sel_order;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(selDir)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(selDir)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeComboOrder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELDIR_H__392D4B5B_F505_4DD7_BA0F_3081574162E8__INCLUDED_)
