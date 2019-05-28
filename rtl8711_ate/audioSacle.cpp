// audioSacle.cpp : implementation file
//

#include "stdafx.h"
#include "pd218.h"
#include "audioSacle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// audioSacle dialog


//audioSacle::audioSacle(struct audioScale_par_t &in_par,CWnd* pParent /*=NULL*/)
audioSacle::audioSacle(struct audioScale_par_t *in_par,CWnd* pParent /*=NULL*/)
	: CDialog(audioSacle::IDD, pParent)
{
	//{{AFX_DATA_INIT(audioSacle)
	m_AG_vamplitude = 0.0;
	m_BG_vamplitude = 0.0;
	//}}AFX_DATA_INIT

    par = in_par;
    //m_AG_vamplitude = in_par.ini_ADutAmp;
    //m_BG_vamplitude = in_par.ini_BDutAmp;
	m_AG_vamplitude = in_par->ini_ADutAmp;
    m_BG_vamplitude = in_par->ini_BDutAmp;

    m_Prompbrush.CreateSolidBrush(RGB(255,0,0));
	m_Prompfont.CreatePointFont(200,"宋体");
    m_Ampbrush.CreateSolidBrush(RGB(0,0,0));
	m_Ampfont.CreatePointFont(150,"宋体");
    m_VAmpbrush.CreateSolidBrush(RGB(0,0,255));
	m_VAmpfont.CreatePointFont(150,"宋体");

}


void audioSacle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(audioSacle)
	DDX_Text(pDX, IDC_AG_VAmplitude, m_AG_vamplitude);
	DDV_MinMaxDouble(pDX, m_AG_vamplitude, 0., 1000.);
	DDX_Text(pDX, IDC_BG_VAmplitude, m_BG_vamplitude);
	DDV_MinMaxDouble(pDX, m_BG_vamplitude, 0., 1000.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(audioSacle, CDialog)
	//{{AFX_MSG_MAP(audioSacle)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// audioSacle message handlers

HBRUSH audioSacle::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	CString text;
    int id = pWnd->GetDlgCtrlID();

	switch(id) 
    {
        case IDC_PROMPT:
        {
                pDC->SelectObject(&m_Prompfont);
                pDC->SetTextColor(RGB(255,0,0));
                pDC->SetBkMode(TRANSPARENT);
        }
        break;
        case IDC_AG_Amplitude:
        case IDC_BG_Amplitude:
        {
                pDC->SelectObject(&m_Ampfont);
                pDC->SetTextColor(RGB(0,0,0));
                pDC->SetBkMode(TRANSPARENT);
        }
        break;
        case IDC_AG_VAmplitude:
        case IDC_BG_VAmplitude:
        {
                pDC->SetTextColor(RGB(0,0,255));
                pDC->SetBkMode(TRANSPARENT);
        }
        break;
        default:break;
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL audioSacle::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString note;
    if(par->need_box) note = "请将模块音频通路与电脑连通\n\n现在开始调节音频测试参数\n请勿将手放入测试箱\n\n";
    else note = "请将模块音频通路与电脑连通\n\n\n";
    GetDlgItem(IDC_PROMPT)->SetWindowText(note);
    

    GetDlgItem(IDC_AG_VAmplitude)->SetFont(&m_VAmpfont);
    GetDlgItem(IDC_BG_VAmplitude)->SetFont(&m_VAmpfont);
    GetDlgItem(IDOK)->SetFont(&m_VAmpfont);
    GetDlgItem(IDCANCEL)->SetFont(&m_VAmpfont);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void audioSacle::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	par->ini_ADutAmp = m_AG_vamplitude;
    par->ini_BDutAmp = m_BG_vamplitude;

	CDialog::OnOK();
}
