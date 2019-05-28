#if !defined(AFX_NEW_PASSWORD_H__F9A95333_F6C5_47C2_B8F3_8C7426E29801__INCLUDED_)
#define AFX_NEW_PASSWORD_H__F9A95333_F6C5_47C2_B8F3_8C7426E29801__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// new_password.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// new_password dialog

class new_password : public CDialog
{
// Construction
public:
	new_password(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(new_password)
	enum { IDD = IDD_NEWPASSWORD };
	CString	m_confirm_password;
	CString	m_new_password;
	CString	m_old_password;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(new_password)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(new_password)
	afx_msg void OnChangeOldpassword();
	afx_msg void OnStatic1();
	afx_msg void OnChangeNewpassword();
	afx_msg void OnChangeConfirm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEW_PASSWORD_H__F9A95333_F6C5_47C2_B8F3_8C7426E29801__INCLUDED_)
