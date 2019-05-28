#if !defined(AFX_SELDUTCOM_H__0E892E08_E73C_4030_8EE3_692F998920CB__INCLUDED_)
#define AFX_SELDUTCOM_H__0E892E08_E73C_4030_8EE3_692F998920CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// selDutCom.h : header file
//



class SPISourceContaner;



/////////////////////////////////////////////////////////////////////////////
// selDutCom dialog

class selDutCom : public CDialog
{

#if 0

private:
        bool __OnDeviceChange(UINT nEventType,DWORD dwData);
        bool need_rerun;
        unsigned int need_rerun_ev;
        bool runing;
        int gchange_remove_times;
        int gchange_plugin_times;

public:

	selDutCom(CWnd* pParent = NULL);   // standard constructor
	

	    int com[MAX_DUT_COM_OBJ];
        int com_map[COM_SOURCEMAX];
        int com_map_new[COM_SOURCEMAX];
        bool com_source_mapped;
	bool initialed;
        bool ADevEnable;
        bool BDevEnable;
        
        bool OnDeviceChange(UINT nEventType,DWORD dwData);
        bool adjust_other_dut_com(int obj_idx);
        void update_com_statu(void);
	int try_allocation_all_com(bool for_test);

#endif


// Construction  
public:
    selDutCom(ConfigFile *cfg, bool include_usb = false,CWnd* pParent = NULL);
	virtual ~selDutCom();
	void creat_port_list(void);
	void add_usb_port_list(void);

	//int gchange_remove_times;
    //int gchange_plugin_times;
	//bool OnDeviceChange(UINT nEventType,DWORD dwData);


    //SPISource *port;
	SPISource *spi_portA;
    SPISource *spi_portB;

	SPISourceContaner *SPIPorts;
	bool enable_usb;
    bool initialed;
	bool ADevEnable;
    bool BDevEnable;
	int  spi_count;

public:
// Dialog Data
	//{{AFX_DATA(selDutCom)
	enum { IDD = IDD_SelDutCom };
	CComboBox	m_BGCom;
	CComboBox	m_BCom;
	CComboBox	m_AGCom;
	CComboBox	m_ACom;
	int		m_dev_num;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(selDutCom)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(selDutCom)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetfocusComboA();
	afx_msg void OnSetfocusComboB();
	afx_msg void OnSelendokComboA();
	afx_msg void OnSelendokComboB();
	afx_msg void OnRadioA();
	afx_msg void OnRadioB();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeComboA();
	afx_msg void OnSelchangeComboB();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELDUTCOM_H__0E892E08_E73C_4030_8EE3_692F998920CB__INCLUDED_)
