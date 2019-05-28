#if !defined(AFX_PASSWORD_H__566153F3_AF41_4327_B43E_A769ED819797__INCLUDED_)
#define AFX_PASSWORD_H__566153F3_AF41_4327_B43E_A769ED819797__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// password.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// password dialog

class password : public CDialog
{
private:
	
// Construction
public:
	password(const char* show = NULL,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(password)
	enum { IDD = IDD_password };
	CString	m_EData;
	CString	m_PwordInf;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(password)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(password)
	virtual void OnOK();
	afx_msg void OnChangeEPassword();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSWORD_H__566153F3_AF41_4327_B43E_A769ED819797__INCLUDED_)
