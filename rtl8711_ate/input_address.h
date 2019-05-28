#if !defined(AFX_INPUT_ADDRESS_H__8A3C50CD_9B58_45E4_9847_4B964A9E7DDA__INCLUDED_)
#define AFX_INPUT_ADDRESS_H__8A3C50CD_9B58_45E4_9847_4B964A9E7DDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// input_address.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// input_address dialog

class input_address : public CDialog
{
// Construction
public:
	input_address(CWnd* pParent = NULL);   // standard constructor

	CBrush m_Addrbrush;
	CFont m_Addrfont;
	CBrush m_Infobrush;
	CFont m_Infofont;
	

public:
// Dialog Data
	//{{AFX_DATA(input_address)
	enum { IDD = IDD_RegCode };
	CString	m_address;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(input_address)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(input_address)
	afx_msg void OnChangeEAddress();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUT_ADDRESS_H__8A3C50CD_9B58_45E4_9847_4B964A9E7DDA__INCLUDED_)
