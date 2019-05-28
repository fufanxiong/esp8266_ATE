// PD218.h : main header file for the PD218 application
//

#if !defined(AFX_PD218_H__2951F1BD_8086_45BF_8FC8_5D1386B13419__INCLUDED_)
#define AFX_PD218_H__2951F1BD_8086_45BF_8FC8_5D1386B13419__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include "agilent_n4010_wlantest.h"

#include "resource.h"		// main symbols
#define	MUTEX_TEST_OBJ_PRO      (PortMutexString.GetBuffer(0))


class ConfigFile;
class SPISource;

/////////////////////////////////////////////////////////////////////////////
// CPD218App:
// See PD218.cpp for the implementation of this class
//
class CPD218App : public CWinApp
{
public:
	CPD218App();
	~CPD218App();
	void ActiveMyInstance(void);
    bool selectSpiPort(void);
	bool ini_config_par(void);
	bool valid_date_verify(void);

	ConfigFile *pCfgfile;
	SPISource *Spi_portA;
	SPISource *Spi_portB;

	//bool BModeSelected;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPD218App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPD218App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

extern CPD218App theApp;
extern CString PortMutexString;
extern Instrument_Interface Inst; 

enum
{
	PASSWORD_ID_PAR = 0,
	PASSWORD_ID_SEL_TITEM,
	PASSWORD_ID_CFG,
	PASSWORD_ID_CFG_MLV,
	PASSWORD_ID_CFG_HLV,
	PASSWORD_ID_VALID_DATE,
	PASSWORD_ID_AUTH,
	PASSWORD_ID_CREAT,
	PASSWORD_ID_CLR_TCOUNT,

	PASSWORD_ID_MAX,
};


#endif // !defined(AFX_PD218_H__2951F1BD_8086_45BF_8FC8_5D1386B13419__INCLUDED_)
