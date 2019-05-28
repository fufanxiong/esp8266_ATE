#if !defined(AFX_NEW_DATE_H__57A7CDE8_B025_4962_99BB_5CE779CD2766__INCLUDED_)
#define AFX_NEW_DATE_H__57A7CDE8_B025_4962_99BB_5CE779CD2766__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// new_date.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// new_date dialog

class new_date : public CDialog
{
// Construction
public:
	new_date(CWnd* pParent = NULL);   // standard constructor
	new_date(long year,long month,long day,CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(new_date)
	enum { IDD = IDD_NEWDATE };
	long	m_year;
	long	m_month;
	long	m_day;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(new_date)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(new_date)
	afx_msg void OnChangeday();
	afx_msg void OnChangemonth();
	afx_msg void OnChangeyear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEW_DATE_H__57A7CDE8_B025_4962_99BB_5CE779CD2766__INCLUDED_)
