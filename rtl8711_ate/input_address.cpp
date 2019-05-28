// input_address.cpp : implementation file
//

#include "stdafx.h"
#include "pd218.h"
#include "input_address.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// input_address dialog


input_address::input_address(CWnd* pParent /*=NULL*/)
	: CDialog(input_address::IDD, pParent)
{
	//{{AFX_DATA_INIT(input_address)
	m_address = _T("");
	//}}AFX_DATA_INIT

	m_Addrbrush.CreateSolidBrush(RGB(0,255,0));
	m_Addrfont.CreatePointFont(400,"ËÎÌו");
	m_Infobrush.CreateSolidBrush(RGB(0,255,0));
	m_Infofont.CreatePointFont(250,"ËÎÌו");
}


void input_address::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(input_address)
	DDX_Text(pDX, IDC_EAddress, m_address);
	DDV_MaxChars(pDX, m_address, 12);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(input_address, CDialog)
	//{{AFX_MSG_MAP(input_address)
	ON_EN_CHANGE(IDC_EAddress, OnChangeEAddress)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// input_address message handlers

void input_address::OnChangeEAddress() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();

	CString temp;
	for(int i=0;i<m_address.GetLength();i++) {
		if(((m_address[i] >= '0') && (m_address[i] <= '9')) ||
		   ((m_address[i] >= 'a') && (m_address[i] <= 'f')) ||
		   ((m_address[i] >= 'A') && (m_address[i] <= 'F'))
		   ) temp += m_address[i];
	}
	m_address = temp;
	UpdateData(false);
	((CEdit*)GetDlgItem(IDC_EAddress))->SetSel(temp.GetLength(),temp.GetLength(),TRUE); 
}

void input_address::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void input_address::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

HBRUSH input_address::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	CString text;
	CFont *pfont = &m_Addrfont;
	switch(pWnd->GetDlgCtrlID()) {
	case IDC_EAddress:
		pDC->SetTextColor(RGB(0,255,0));
		pDC->SetBkMode(TRANSPARENT);
	break;
	case IDC_enterRegisterCode:
		pDC->SetTextColor(RGB(0,0,255));
		pfont = &m_Infofont;
		pDC->SetBkMode(TRANSPARENT);
		pDC->SelectObject(pfont);
	break;

	default:break;
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL input_address::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	GetDlgItem(IDC_EAddress)->SetFont(&m_Addrfont);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
