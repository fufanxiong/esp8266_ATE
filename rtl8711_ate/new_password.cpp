// new_password.cpp : implementation file
//

#include "stdafx.h"
#include "pd218.h"
#include "new_password.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// new_password dialog


new_password::new_password(CWnd* pParent /*=NULL*/)
	: CDialog(new_password::IDD, pParent)
{
	//{{AFX_DATA_INIT(new_password)
	m_confirm_password = _T("");
	m_new_password = _T("");
	m_old_password = _T("");
	//}}AFX_DATA_INIT
}


void new_password::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(new_password)
	DDX_Text(pDX, IDC_CONFIRM, m_confirm_password);
	DDV_MaxChars(pDX, m_confirm_password, 20);
	DDX_Text(pDX, IDC_NEWPASSWORD, m_new_password);
	DDV_MaxChars(pDX, m_new_password, 20);
	DDX_Text(pDX, IDC_OLDPASSWORD, m_old_password);
	DDV_MaxChars(pDX, m_old_password, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(new_password, CDialog)
	//{{AFX_MSG_MAP(new_password)
	ON_EN_CHANGE(IDC_OLDPASSWORD, OnChangeOldpassword)
	ON_BN_CLICKED(IDC_STATIC1, OnStatic1)
	ON_EN_CHANGE(IDC_NEWPASSWORD, OnChangeNewpassword)
	ON_EN_CHANGE(IDC_CONFIRM, OnChangeConfirm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// new_password message handlers

void new_password::OnChangeOldpassword() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void new_password::OnStatic1() 
{
	// TODO: Add your control notification handler code here
	
}

void new_password::OnChangeNewpassword() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void new_password::OnChangeConfirm() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}
