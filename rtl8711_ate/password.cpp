// password.cpp : implementation file
//

#include "stdafx.h"
#include "PD218.h"
#include "password.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// password dialog


password::password(const char* show,CWnd* pParent /*=NULL*/)
	: CDialog(password::IDD, pParent)
{
	//{{AFX_DATA_INIT(password)
	m_EData = _T("");
	m_PwordInf = _T("ÇëÊäÈëÃÜÂë");
	//}}AFX_DATA_INIT
//	SetFocus(GetDlgItem(IDC_EPassword)->m_hWnd);
	if(show && show[0]) m_PwordInf = show;
}


void password::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(password)
	DDX_Text(pDX, IDC_EPassword, m_EData);
	DDV_MaxChars(pDX, m_EData, 20);
	DDX_Text(pDX, IDC_enterPassword, m_PwordInf);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(password, CDialog)
	//{{AFX_MSG_MAP(password)
	ON_EN_CHANGE(IDC_EPassword, OnChangeEPassword)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// password message handlers

void password::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void password::OnChangeEPassword() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void password::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
