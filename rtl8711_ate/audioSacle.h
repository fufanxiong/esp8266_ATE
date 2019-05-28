#if !defined(AFX_AUDIOSACLE_H__4BBCB347_0CCE_478A_B0AD_796A081B370E__INCLUDED_)
#define AFX_AUDIOSACLE_H__4BBCB347_0CCE_478A_B0AD_796A081B370E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// audioSacle.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// audioSacle dialog


// audioScale dialog
struct audioScale_par_t
{
    bool need_box;
    double minAmp;
    double maxAmp;
    bool bm_test;

    double ini_ADutAmp;
    double ini_BDutAmp;
};



class audioSacle : public CDialog
{
// Construction
public:
	//audioSacle(struct audioScale_par_t &in_par,CWnd* pParent = NULL);   // standard constructor
    audioSacle(struct audioScale_par_t *in_par,CWnd* pParent = NULL);
	struct audioScale_par_t *par;

	CBrush m_Prompbrush;
	CFont m_Prompfont;
    CBrush m_Ampbrush;
	CFont m_Ampfont;
    CBrush m_VAmpbrush;
	CFont m_VAmpfont;
// Dialog Data
	//{{AFX_DATA(audioSacle)
	enum { IDD = IDD_AudioScale };
	double	m_AG_vamplitude;
	double	m_BG_vamplitude;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(audioSacle)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(audioSacle)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOSACLE_H__4BBCB347_0CCE_478A_B0AD_796A081B370E__INCLUDED_)
