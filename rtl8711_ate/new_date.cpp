// new_date.cpp : implementation file
//

#include "stdafx.h"
#include "pd218.h"
#include "new_date.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

unsigned long day_tbl[12+1] = {31,31,28,31,30,31,30,31,31,30,31,30,31};


/////////////////////////////////////////////////////////////////////////////
// new_date dialog


new_date::new_date(CWnd* pParent /*=NULL*/)
	: CDialog(new_date::IDD, pParent)
{
	//{{AFX_DATA_INIT(new_date)
	m_year = 2012;
	m_month = 1;
	m_day = 31;
	//}}AFX_DATA_INIT
}

new_date::new_date(long year,long month,long day,CWnd* pParent)
	: CDialog(new_date::IDD, pParent)
{
	m_year = year;
	m_month = month;
	m_day = day;
}

void new_date::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(new_date)
	DDX_Text(pDX, IDC_year, m_year);
	DDV_MinMaxLong(pDX, m_year, 2012, 2050);
	DDX_Text(pDX, IDC_month, m_month);
	DDV_MinMaxLong(pDX, m_month, 1, 12);
	DDX_Text(pDX, IDC_day, m_day);
	DDV_MinMaxLong(pDX, m_day, 1, 31);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(new_date, CDialog)
	//{{AFX_MSG_MAP(new_date)
	ON_EN_CHANGE(IDC_day, OnChangeday)
	ON_EN_CHANGE(IDC_month, OnChangemonth)
	ON_EN_CHANGE(IDC_year, OnChangeyear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// new_date message handlers

void new_date::OnChangeday() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();

	long max_day = day_tbl[m_month];
	if((!(m_year%4) && (m_year%100)) || !(m_year%400)) {
		if(m_month == 2) max_day = 29;
	}
	if(m_day > max_day) m_day = max_day;

	UpdateData(false);
}

void new_date::OnChangemonth() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();

	long max_day = day_tbl[m_month];
	if((!(m_year%4) && (m_year%100)) || !(m_year%400)) {
		if(m_month == 2) max_day = 29;
	}
	if(m_day > max_day) m_day = max_day;

	UpdateData(false);
}

void new_date::OnChangeyear() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();

	long max_day = day_tbl[m_month];
	if((!(m_year%4) && (m_year%100)) || !(m_year%400)) {
		if(m_month == 2) max_day = 29;
	}
	if(m_day > max_day) m_day = max_day;

	UpdateData(false);
}
