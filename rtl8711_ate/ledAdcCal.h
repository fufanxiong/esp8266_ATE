#if !defined(AFX_LEDADCCAL_H__47074B17_B277_4F9B_888C_03276F3378AB__INCLUDED_)
#define AFX_LEDADCCAL_H__47074B17_B277_4F9B_888C_03276F3378AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ledAdcCal.h : header file
//

class testSet;

/////////////////////////////////////////////////////////////////////////////
// ledAdcCal dialog

class ledAdcCal : public CDialog
{
// Construction
public:
	ledAdcCal(CWnd* pParent = NULL);   // standard constructor

public:
	CFont m_prompt_font;
	CFont m_button_font;
	
	HANDLE          hthread;
	DWORD           hthreadId;

	int ApathLedAdc[12];
	int BpathLedAdc[12];

	testSet 	*tset;
	CString		err_str;
	int         iHandle;
	int         led_test_idx;
	int         golden_idx;
	int         dutA_snr_sel_id;
	int         dutB_snr_sel_id;
	char        dutA_mcu_r_grp;
	int         dutA_mcu_r_pio_num;
	char        dutA_mcu_g_grp;
	int         dutA_mcu_g_pio_num;
	char        dutA_mcu_b_grp;
	int         dutA_mcu_b_pio_num;

	char        dutB_mcu_r_grp;
	int         dutB_mcu_r_pio_num;
	char        dutB_mcu_g_grp;
	int         dutB_mcu_g_pio_num;
	char        dutB_mcu_b_grp;
	int         dutB_mcu_b_pio_num;
	
	//double APathOffset[3];
	//double BPathOffset[3];
	
	//void result_adjust(struct tSetResult &mt8852_res,double *pathOffset);
	bool do_LED_test(int test_golden_idx);
	//bool dut_enter_rfmode(int test_golden_idx);
	bool get_led_val(int *red, int *green, int *blue);

	void led_test_finish(void);
	void show_prompt(CString str);

// Dialog Data
	//{{AFX_DATA(ledAdcCal)
	enum { IDD = IDD_LED_ADJUST };
	CButton	m_ok_button;
	CButton	m_cancel_button;
	CButton	m_led_cal_btn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ledAdcCal)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ledAdcCal)
	virtual BOOL OnInitDialog();
	afx_msg void OnLedCal();
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnCancel();
	afx_msg void OnPrompt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEDADCCAL_H__47074B17_B277_4F9B_888C_03276F3378AB__INCLUDED_)
