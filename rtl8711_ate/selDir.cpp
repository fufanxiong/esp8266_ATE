// selDir.cpp : implementation file
//

#include "stdafx.h"
#include "pd218.h"
#include "selDir.h"
#include "PD218Dlg.h"
#include "err.h"
#include "password.h"
#include "TestItem.h"
#include "config_deal.h"
#include "cpu.h"

#include <fstream.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h>
#include <Dbt.h>

#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
corder_wknum::corder_wknum(void)
{
	wknum = new CStringList();
	wknum_len = 0;
}

corder_wknum::~corder_wknum()
{
	wknum->RemoveAll();
	delete wknum;
	wknum = NULL;
	wknum_len = 0;
}

void corder_wknum::add_wknum(CString& _wknum)
{
	wknum->AddTail(_wknum);
	wknum_len += 1;
}


/////////////////////////////////////////////////////////////////////////////
// selDir dialog


selDir::selDir(CWnd* pParent /*=NULL*/)
	: CDialog(selDir::IDD, pParent)
{
	//{{AFX_DATA_INIT(selDir)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	all_dir_list = new CStringList();
	
	order_wknum_num = 0;
	cur_order = -1;
        for(int i=0;i<ORDER_WKNUM_LEN;i++) order_wknum[i] = NULL;
}


selDir::~selDir()
{
	all_dir_list->RemoveAll();
	delete all_dir_list;
	all_dir_list = NULL;
	
	for(int i = 0; i < ORDER_WKNUM_LEN; i++)
	{
		if(order_wknum[i] != NULL) delete order_wknum[i];
		order_wknum[i] = NULL;
	}
	order_wknum_num = 0;
}


void selDir::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(selDir)
	DDX_Control(pDX, IDC_COMBO_WK_NUM, m_sel_wknum);
	DDX_Control(pDX, IDC_COMBO_ORDER, m_sel_order);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(selDir, CDialog)
	//{{AFX_MSG_MAP(selDir)
	ON_CBN_SELCHANGE(IDC_COMBO_ORDER, OnSelchangeComboOrder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// selDir message handlers

BOOL selDir::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    GetDirectorys();
	build_order_wknum();
	
	for(int i=0;i<order_wknum_num;i++)
	{
		m_sel_order.AddString(order_wknum[i]->order);
	}
	if(order_wknum_num) m_sel_order.SetCurSel(0);
        OnSelchangeComboOrder();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
CStringList* selDir::GetDirectorys(void)
{
        CStringList* list = all_dir_list;
        CFileFind fileFind;
        BOOL re;

        CString Dir = pCfgf.VCString(slaveParRecordDir);
	TCHAR szFilePath[MAX_PATH + 1];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
        CString str_url =  szFilePath;
	str_url += Dir;
        str_url += "\\";

        re = fileFind.FindFile(str_url + "*.*");
        if (re)
        {        
                while (re)
                {
                    re = fileFind.FindNextFile();
                    if (fileFind.IsDots()) continue;
                    if (fileFind.IsDirectory())
                    {
                        list->AddTail(fileFind.GetFileName());
                    }
                }
        }
		
        return list;
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
bool selDir::add_order_wknum(CString& order,CString& wknum)
{
        for(int i = 0; i < ORDER_WKNUM_LEN; i++)
        {
                if(!order_wknum[i]) break;
                if(order_wknum[i]->order == order)
                { 
                        order_wknum[i]->add_wknum(wknum);
                        return(true);
                }
        }
        if(i >= ORDER_WKNUM_LEN) return(false);

        order_wknum_num += 1;
        order_wknum[i] = new corder_wknum();
        order_wknum[i]->order = order;
        order_wknum[i]->add_wknum(wknum);
        return(true);
}

void selDir::build_order_wknum(void)
{
        int num = all_dir_list->GetCount();
        CString temp,order,wknum;
        int pos;

        if(!num) return;
        for(int i = 0; i < ORDER_WKNUM_LEN; i++)
        {
                if(order_wknum[i])
                { 
                        delete order_wknum[i];
                        order_wknum[i] = NULL;
                }
        }
        order_wknum_num = 0;

        for(i=0;i<num;i++)
        {
			temp = all_dir_list->GetAt(all_dir_list->FindIndex(i));
			if((temp[0] != 'p') && (temp[0] != 'P')) continue;
			if((temp[1] != 'o') && (temp[1] != 'O')) continue;
			pos = temp.Find(" ");
			if(pos < 0) continue;
			
			order = temp.Left(pos);
			wknum = temp.Right(temp.GetLength() - pos - 1);
			if((order == "") || (wknum == "")) continue;
			add_order_wknum(order,wknum);
        }
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
void selDir::OnSelchangeComboOrder() 
{
	UpdateData(TRUE);
	
        int order_idx = m_sel_order.GetCurSel();
        if((order_idx < 0) || (order_idx >= order_wknum_num)) return;
        if(order_idx == cur_order) return;
        cur_order = order_idx;

        CStringList* wknumlist = order_wknum[cur_order]->wknum;
        m_sel_wknum.ResetContent();
        for(int i=0;i<order_wknum[cur_order]->wknum_len;i++)
        {
                m_sel_wknum.AddString(wknumlist->GetAt(wknumlist->FindIndex(i)));
        }
        m_sel_wknum.SetCurSel(0);
        UpdateData(FALSE);
}



void selDir::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	
	int order_idx = m_sel_order.GetCurSel();
	m_sel_order.GetWindowText(str_order);
        m_sel_wknum.GetWindowText(str_wknum);

	CDialog::OnOK();
}
