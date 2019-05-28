// selDutCom.cpp : implementation file
//

#include "stdafx.h"
#include "PD218.h"
#include "PD218Dlg.h"
#include "Newdevice.h"
#include "tset.h"
#include "selDutCom.h"
#include "TestEngine.h"
#include "Config.h"
#include <fstream.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h>
#include <Dbt.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#pragma comment (lib,"TestEngine.lib")
//using namespace std;


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
extern char COM_Port_Name[][6];
const char str_window_title[] = "select dut spi";


const char *dut_com_promt[] =
{
	    "请插拔 A模块 spi口对应的 SPI 线",
        "请插拔 B模块 spi口对应的 SPI 线"
};


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------

bool SplitStr(CString &ports,CString &portsStr)
{
	portsStr = "";
	int i;
	for(i=0;i<ports.GetLength();i++) {
		if(ports[i] == ',') {
			i += 1;
			break;
		}
		portsStr += ports[i];
	}
	if(i == ports.GetLength()) ports = "";
	else
	{
		CString temp;
		for(;i<ports.GetLength();i++) {
			temp += ports[i];
		}
		ports = temp;
	}
	return(portsStr != "");
}

//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// selDutCom dialog

selDutCom::selDutCom(ConfigFile *cfg, bool include_usb, CWnd* pParent)
	: CDialog(selDutCom::IDD, pParent)
{
	//{{AFX_DATA_INIT(selDutCom)
	m_dev_num = -1;
	//}}AFX_DATA_INIT

		//gchange_plugin_times = 0;
        //gchange_remove_times = 0;
		enable_usb = include_usb;
		//port = NULL;
		spi_portA = NULL;
		spi_portB = NULL;
	    SPIPorts = new SPISourceContaner();
        spi_count = 0;   //spi count
        
		BDevEnable = true;
		if(!cfg->ItemSelect(s_BModle_Sel))
		{
		   BDevEnable = false;
		}

        ADevEnable = true;

//#endif

}


void selDutCom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(selDutCom)
	DDX_Control(pDX, IDC_COMBO_BG, m_BGCom);
	DDX_Control(pDX, IDC_COMBO_B, m_BCom);
	DDX_Control(pDX, IDC_COMBO_AG, m_AGCom);
	DDX_Control(pDX, IDC_COMBO_A, m_ACom);
	DDX_Radio(pDX, IDC_RADIO_A, m_dev_num);
	//}}AFX_DATA_MAP
}


selDutCom::~selDutCom()
{
	if(SPIPorts) delete SPIPorts;
	SPIPorts = NULL;

}


BEGIN_MESSAGE_MAP(selDutCom, CDialog)
	//{{AFX_MSG_MAP(selDutCom)
	ON_CBN_SETFOCUS(IDC_COMBO_A, OnSetfocusComboA)
	ON_CBN_SETFOCUS(IDC_COMBO_B, OnSetfocusComboB)
	ON_CBN_SELENDOK(IDC_COMBO_A, OnSelendokComboA)
	ON_CBN_SELENDOK(IDC_COMBO_B, OnSelendokComboB)
	ON_BN_CLICKED(IDC_RADIO_A, OnRadioA)
	ON_BN_CLICKED(IDC_RADIO_B, OnRadioB)
	ON_CBN_SELCHANGE(IDC_COMBO_A, OnSelchangeComboA)
	ON_CBN_SELCHANGE(IDC_COMBO_B, OnSelchangeComboB)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// selDutCom message handlers
BOOL selDutCom::OnInitDialog() 
{
	CDialog::OnInitDialog();

//#if 0
	// TODO: Add extra initialization here
        SetWindowText(str_window_title);
        if(!ADevEnable && !BDevEnable)
        {
			    CString show = "设备 SPI 口已分配或指定的设备不合适!!";
                AfxMessageBox(show);
                EndDialog(IDCANCEL);
                return TRUE;
        }
       
        //com_source_mapped = false;
        //DevSourceMap(com_map,com_source_mapped);
        //initialed = true;

        if(!ADevEnable)
        {
                m_ACom.EnableWindow(false);
                GetDlgItem(IDC_RADIO_A)->EnableWindow(false);
               //m_AGCom.EnableWindow(false);
               //GetDlgItem(IDC_RADIO_AG)->EnableWindow(false);
        }
        if(!BDevEnable)
        {
                m_BCom.EnableWindow(false);
                GetDlgItem(IDC_RADIO_B)->EnableWindow(false);
                //m_BGCom.EnableWindow(false);
                //GetDlgItem(IDC_RADIO_BG)->EnableWindow(false);
        }
        if(ADevEnable)
        {       
                m_dev_num = 0;
                m_ACom.SetFocus();
        }
        else 
        {
                m_dev_num = 1;
                m_BCom.SetFocus();
        }
        //update_com_statu();

		///////////CSR////////////
		
		creat_port_list();
		SPISource *port = SPIPorts->hlist;
		if(!port)
		{
            AfxMessageBox("no spi port found!");
			EndDialog(IDCANCEL);  //OnCancel();
            return TRUE;
		}
        if (BDevEnable) {
			if (spi_count <= 1) {
				CString show = "检测到的SPI口数量不足，请插入另外一根的SPI线!!";
                AfxMessageBox(show);
                EndDialog(IDCANCEL);
                return TRUE;			
			}
		}

		if(enable_usb) add_usb_port_list();
		
		
		if(ADevEnable) {
		     CComboBox* comboA= (CComboBox*)GetDlgItem(IDC_COMBO_A);

             do{
				 if(port->port_name().Find("USB")>=0)
				 {
				    comboA->AddString(port->port_name());
				 }
				
			 }while (port = SPIPorts->get_nex(port));
			// comboA->DeleteString(1);
	     	comboA->SetCurSel(0);
            
			if (BDevEnable){
				SPISource *port = SPIPorts->hlist;

				CComboBox* comboB= (CComboBox*)GetDlgItem(IDC_COMBO_B);
				do{
					if(port->port_name().Find("USB")>=0)
					{
					   comboB->AddString(port->port_name());
					}
				}while (port = SPIPorts->get_nex(port));
				//comboB->DeleteString(0);
			 comboB->SetCurSel(1);
			}

        } else if (BDevEnable){
             CComboBox* comboB= (CComboBox*)GetDlgItem(IDC_COMBO_B);
             do{
				 comboB->AddString(port->port_name());
			 }while (port = SPIPorts->get_nex(port));
			 
			 comboB->SetCurSel(1);   //A module SPI is allocated?
		}   
		
        CString show = "请手动分配或，操作以下步骤!!\n\n";
        show += dut_com_promt[m_dev_num];
        GetDlgItem(IDC_SELCOM_PROMPT)->SetWindowText(show);
        UpdateData(FALSE);
//#endif
        

	// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
        return TRUE;
}



void selDutCom::creat_port_list(void)
{
	uint32 iHandle(0);
	uint16 maxLen(256);
	uint16 count(0);
	char* portsStr = new char[maxLen];
	char* transStr = new char[maxLen];
	portsStr[255] = 0;
	transStr[255] = 0;
	
	CString param;
	int32 status = teGetAvailableSpiPorts(&maxLen, portsStr, transStr, &count);
	if((status != TE_OK) && (maxLen != 0)) 
	{
		// Not enough space - resize the storage
		delete[] portsStr;
		delete[] transStr;
		portsStr = new char[maxLen];
		transStr = new char[maxLen];
		status = teGetAvailableSpiPorts(&maxLen, portsStr, transStr, &count);
	}
	if((status != TE_OK) || (count == 0)) 
	{
		param =  "getting SPI ports err";
		delete[] portsStr;
		delete[] transStr;
		return;
	}
	
	// Split up the comma separated strings of ports / transport options
	CString ports_org = portsStr;
	CString ports;
	CString trans_org = transStr;
	CString trans;
	
	// Open the SPI port using the trans string,we're just using the first in the list
	while(SplitStr(ports_org,ports) && SplitStr(trans_org,trans))
	{
		SPIPorts->add(0,ports,trans);
		spi_count++;     //spi count计数
	}
	
	delete[] portsStr;
	delete[] transStr;
}


void selDutCom::add_usb_port_list(void)
{
	static const unsigned long MAX_USB_DEVICES = 20;
#define CSR_USB_FMT "\\\\.\\csr%d"
	
	CString ports;
	CString trans;
	CString portname;
	HANDLE test;
	for (int n=0;n<MAX_USB_DEVICES;n++)
	{
		trans = "";
		trans.Format("\\\\.\\csr%d",n);
		
		test = ::CreateFile(trans,GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,0,0);
		if(INVALID_HANDLE_VALUE == test) break;
		
		::CloseHandle(test);
		ports = "";
		ports.Format("usb\\csr%d",n);
		SPIPorts->add(0,ports,trans,PORT_TYPE_USB);
	}
}


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
void selDutCom::OnRadioA() 
{
	m_dev_num = 0;
    m_ACom.SetFocus();
}


void selDutCom::OnRadioB() 
{
	m_dev_num = 1;
    m_BCom.SetFocus();
}


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
void selDutCom::OnSetfocusComboA() 
{
	m_dev_num = 0;
        UpdateData(FALSE);
}



void selDutCom::OnSetfocusComboB() 
{
	m_dev_num = 1;
	UpdateData(FALSE);
}





//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
void selDutCom::OnSelendokComboA() 
{
#if 0
	m_dev_num = 0;

	CString str_com;
        m_ACom.GetWindowText(str_com);
        com[m_dev_num] = ComNameToIdx(str_com);
        adjust_other_dut_com(m_dev_num);
        update_com_statu();
        UpdateData(FALSE);
#endif
}



void selDutCom::OnSelendokComboB()
{
#if 0
	m_dev_num = 2;

	CString str_com;
        m_BCom.GetWindowText(str_com);
        com[m_dev_num] = ComNameToIdx(str_com);
        adjust_other_dut_com(m_dev_num);
        update_com_statu();
        UpdateData(FALSE);
#endif	
}





void selDutCom::OnOK() 
{
   //try_allocation_all_com(false);
    
    CComboBox* comboA = ( CComboBox*)GetDlgItem(IDC_COMBO_A);
	
	int nIndex = comboA->GetCurSel();
	CString strItem;
	comboA->GetLBText(nIndex,strItem);

	//port = SPIPorts->get_port(strItem);
    spi_portA = SPIPorts->get_port(strItem); 

    //pMainWnd->tSetA->pSpiObj->dut_spi_allocation = true;


	//初始化SPI port
	//pMainWnd->tSetA->pSpiObj->spi_port = new SPISource(0,port->port_name(),
		                                    //port->port_num(),port->port_type());

	if (BDevEnable) {
	   CComboBox* comboB = ( CComboBox*)GetDlgItem(IDC_COMBO_B);
	   nIndex = comboB->GetCurSel();
	   comboB->GetLBText(nIndex,strItem);
       spi_portB = SPIPorts->get_port(strItem); 

	}

	//this->PostMessage(WM_REFRESH_TEST_ITEM_LIST,(WPARAM)NULL,(LPARAM)NULL);

	CDialog::OnOK();
}



void selDutCom::OnCancel() 
{
	// TODO: Add extra cleanup here
	//pMainWnd->tSetA->pSpiObj->spi_port = NULL;
	//pMainWnd->tSetB->pSpiObj->spi_port = NULL;
	CDialog::OnCancel();
}



void selDutCom::OnSelchangeComboA() 
{
	// TODO: Add your control notification handler code here
	CComboBox* combo= ( CComboBox*)GetDlgItem(IDC_COMBO_A);
	int nIndex = combo->GetCurSel();
	
	CString strItem;
	combo->GetLBText(nIndex,strItem);
	
}



void selDutCom::OnSelchangeComboB() 
{
	// TODO: Add your control notification handler code here
	CComboBox* combo= ( CComboBox*)GetDlgItem(IDC_COMBO_B);
	int nIndex = combo->GetCurSel();
	
	CString strItem;
	combo->GetLBText(nIndex,strItem);
	
}
