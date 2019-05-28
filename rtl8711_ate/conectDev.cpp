// PD218Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "PD218.h"
#include "PD218Dlg.h"
#include "err.h"
#include "config_deal.h"
#include "TestItem.h"
#include "cderr.h "   		//for   definition   of   FNERR_BUFFERTOOSMALL
#include "selDutCom.h"
#include "soundIOlib.h"
#include "cpu.h"
#include "audioSacle.h"
#include "audioRouting.h"
#include "TestItem.h"

#include <fstream.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h>
#include <Dbt.h>

#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <math.h>


//=================================================================================================================
//=================================================================================================================
bool is_thd_less(CAudio_par &dec,CAudio_par &ref)
{
        double dlt_thd = 0;//pCfgf.VDouble(s_dut_max_distortion);
  
        if(dec.LShowID && (dec.Lthd > (ref.Lthd + dlt_thd))) return(false);
        if(dec.RShowID && (dec.Rthd > (ref.Rthd + dlt_thd))) return(false);
        return(true);
}
bool is_thd_less(CAudio_par &dec,double max_thd)
{

	  if(dec.LShowID && (dec.Lthd > max_thd)) return(false);
      if(dec.RShowID && (dec.Rthd > max_thd)) return(false);

        return(true);
}

//=================================================================================================================
//=================================================================================================================
#define DEV_OP_PROMP(str) do{ if(!pMainWnd->terminal_test) UPDATE_STATIC(IDC_CurTestObj,str); } while(0)


#define	CREAT_AND_CONNECT_DEVICE(DEV,pdev,DEVNAME)						    \
do{     DEV::creat_dev(Name,pdev);							                \
	if(!pdev) {										                        \
		ShowMessage += DEVNAME##":"+Name+" is not supporting!!\n";			\
		rValue = false;									                    \
	}else {											                        \
	if(!pdev->dev_is_ready()) {							                    \
                        temp = "Connecting "##DEVNAME##" ...";              \
                        DEV_OP_PROMP(temp);                                 \
			pdev->connect();							                    \
	}										                                \
	if(!pdev->dev_is_ready()) {							                    \
			if(is_name_null(pdev->port_name())) 				            \
				ShowMessage += "can not connect:"+Name;			      	    \
			else ShowMessage += "on port:" + 					            \
						pdev->port_name() + 				                \
					" can not connect:"+Name;				                \
			ShowMessage += "!!\n";							                \
			rValue = false;								                    \
	}										                                \
}											                                \
}while (0)



//extern int audio_gain;

//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
static bool audio_routing_cfg(CString &param,bool &rValue)
{
        ConfigFile &cfg = pCfgf;
        CAudio_par &audio_obj = pMainWnd->audio_obj;
        CCFJAudioRouting ARouting(pMainWnd->tSetA,cfg);
        CCFJAudioRouting BRouting(pMainWnd->tSetB,cfg);
        struct audioScale_par_t par;
        bool stander_amp_ok = false;
        bool return_val = true;
        CString temp;

        if(pMainWnd->terminal_test) return(false);
        par.need_box = pCfgf.ItemSelect(parTestBoxNeed);
        par.minAmp = 0;//cfg.VDouble(s_dut_audio_min_amp);
        par.maxAmp = 0;//cfg.VDouble(s_dut_audio_max_amp);
        par.bm_test = pCfgf.ItemSelect(s_BModle_Sel);
        par.ini_ADutAmp = 0;
        par.ini_BDutAmp = 0;

        //computer mic-->spk test 
        audio_obj = pMainWnd->AAudio_cfg;
        if(!audio_obj.LShowID) audio_obj.LShowID = IDC_CurTestItem;
        if(!audio_obj.RShowID) audio_obj.RShowID = IDC_CurTestObj;

		if(!pMCU2_Com || !pMCU1_Com)
		{
			param = "null com";
			return false;
		}

//#if 0	
		if(!pMCU1_Com->set_dut_spk_amp_off(0, MCU_PIO_DUT_SPK_AMP, MCU_PIO_NUM_DUT_SPK_AMP))
		{
		    param = "set amplifier sw off err";
            return(false);
		}
		//MCU set audio route pc to pc
		if(!pMCU1_Com->set_test_board_short_pc_on(0,MCU_PIO_AUDIO_CHK,MCU_PIO_NUM_AUDIO_CHK))
        {
            param = "set audio short err";
            return(false);
        }	

		AfxMessageBox("音频通路已短接到电脑\n现在测量电脑音频失真度");

		//audio_gain = 1;
		if(!ARouting.cfg_routing(true,true,true))
        {
                param = ARouting.err_str;
			    rValue = false;
				return_val = false;
                goto __audio_cfj_err;
        }


        if(!is_thd_less(ARouting.audio_res, ARouting.max_thd))
        {
                rValue = false;
                param.Format("PC 失真度太大 L:%.2lf%% R:%.2lf%%\n",
					ARouting.audio_res.Lthd,ARouting.audio_res.Rthd);
				return_val = false;
                goto __audio_cfj_err;
        }
        pMainWnd->pc_cfg = ARouting.audio_res; //调整好的音频参数
        pMainWnd->AAudio_cfg.samples = pMainWnd->pc_cfg.samples;
        pMainWnd->BAudio_cfg.samples = pMainWnd->pc_cfg.samples;


#if 0
        //input golden stander amp
		audioSacle *dlg; = new audioSacle(&par);
        int res = dlg->DoModal();
        delete(dlg);
        if(IDOK != res)
        {
                rValue = false;
                param.Format("放弃调整");
                return(false);
        }
#endif
        
        if(par.need_box && (!pTbox || !pTbox->close_box()))
        {
                rValue = false;
                param = "tbox close err";
                return(false);
        }
		//else
		//{
			//tbox_is_open = false;
		//}
//#endif

//#endif
        // A modoule audio par cfg
        audio_obj = pMainWnd->AAudio_cfg;

		//MCU set dut pwr on to connect spi
		if(!pMCU2_Com->set_dut_pwr_on(1, MCU_PIO_DUT_PWR, MCU_PIO_NUM_DUT_PWR))
		{
		     param = "set dut pwr err";
             return(false);
		}
        if(!pMCU2_Com->set_dut_spi_on(1, MCU_PIO_SPI,MCU_PIO_NUM_SPI))
		{
		     param = "set dut spi on err";
             return(false);
		}

		//pc to pc audio off
		if(!pMCU1_Com->set_test_board_short_pc_off(0,MCU_PIO_AUDIO_CHK,MCU_PIO_NUM_AUDIO_CHK))
		{
			param = "set audio unshort err";
             return(false);
		}

		if(!pMCU1_Com->set_dut1_rout_audio_on(0,MCU_PIO_DUT2_AUDIO,MCU_PIO_NUM_DUT2_AUDIO))
		{
               param = "set dut1 audio rout err";
		       return(false);
		}

        //audio_gain = 100;
		if(!ARouting.cfg_dut_routing(true))
        {
                rValue = false;
                param = ARouting.err_str;  //err_str
                if(param == "") param = "config A dut err\n";
                return_val = false;
                goto __audio_cfj_err;
        }

        if(!is_thd_less(ARouting.audio_res,pMainWnd->pc_cfg))
        {
                rValue = false;
                param.Format("DUT A 失真度太大(L:%.2lf%% R:%.2lf%%) PC(L:%.2lf%% R:%.2lf%%)\n",
                             ARouting.audio_res.Lthd,ARouting.audio_res.Rthd,
                             pMainWnd->pc_cfg.Lthd,pMainWnd->pc_cfg.Rthd); 
                return_val = false;
                goto __audio_cfj_err;
        }
        pMainWnd->AAudio_cfg = ARouting.audio_res;

        
		//是否需要对B进行音频通路配置
		// B modoule audio par cfg
        if(!par.bm_test) goto __audio_cfj_err;

		//pc to pc audio off

		if(!pMCU1_Com->set_test_board_short_pc_off(0,MCU_PIO_AUDIO_CHK,MCU_PIO_NUM_AUDIO_CHK))
		{
			param = "set audio unshort err";
             return(false);
		}
		
		if(!pMCU1_Com->set_dut2_rout_audio_on(0,MCU_PIO_DUT2_AUDIO,MCU_PIO_NUM_DUT2_AUDIO))
		{
               param = "set dut2 audio rout err";
		       return(false);
		}

        audio_obj = pMainWnd->BAudio_cfg;
        if(!BRouting.cfg_dut_routing(true))
        {
                rValue = false;
                param = BRouting.err_str;
                if(param == "") param = "config B dut err\n";
                return_val = false;
                goto __audio_cfj_err;
        }

        if(!is_thd_less(BRouting.audio_res,pMainWnd->pc_cfg))
        {
                rValue = false;
                param.Format("DUT B 失真度太大(L:%.2lf%% R:%.2lf%%) PC(L:%.2lf%% R:%.2lf%%)\n",
                             BRouting.audio_res.Lthd,BRouting.audio_res.Rthd,
                             pMainWnd->pc_cfg.Lthd,pMainWnd->pc_cfg.Rthd);
                return_val = false;
                goto __audio_cfj_err;
        }
        pMainWnd->BAudio_cfg = BRouting.audio_res;


__audio_cfj_err:
        pMCU1_Com->set_test_board_short_pc_off(0,MCU_PIO_AUDIO_CHK,MCU_PIO_NUM_AUDIO_CHK);
		pMCU2_Com->set_dut_pwr_off(1, MCU_PIO_DUT_PWR, MCU_PIO_NUM_DUT_PWR);
        pMCU2_Com->set_dut_spi_off(1, MCU_PIO_SPI,MCU_PIO_NUM_SPI);

        if(pCfgf.ItemSelect(parTestBoxNeed))
		{
			if(!pTbox->open_box())
			{
			   param = "tbox open err";
			}
			//else
			//{
			   //tbox_is_open = true;
			//}

		}
        return(return_val);

}
			


//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
DWORD WINAPI doDeviceConnect(LPVOID ref_mts)
{
	CString ShowMessage;
	bool rValue = true;
	CString temp;
	CString Name;
    ConfigFile &cfg = pCfgf;
	///CString OldText;
	CString cmd_str;
	bool item_sel = false;
	//bool pwrOff_cur_sel = cfg.ItemSelect(s_PwrOff_cur_sel);
	//unsigned long pwr_on_delay = cfg.Vulong(parPwrOnDelayTime);
#if 1
	if(!pMainWnd->terminal_test && !pMainWnd->authorization)
	{       
		temp = "检测电脑测试环境...";
		DEV_OP_PROMP(temp);
	#if 0
		// close pc uuid---only this pc can use this soft
		if(verifyIdFile()) pMainWnd->authorization = true;
		else
		{
			ShowMessage = "此计算机环境配置失败!!";
			rValue = false;
			goto __err_out;
		}
	#else
		pMainWnd->authorization = true;
	#endif
	}
#endif

#if	0
	// use frequency counter connect
	if(!pMainWnd->terminal_test && cfg.ItemSelect(slaveXtalSel)) 
    {
		Name = cfg.VCString(ParFCountDev);
		CREAT_AND_CONNECT_DEVICE(FCOUNTER_DEV, pFCounter, "FCounter");
		
		if(pFCounter && !pFCounter->dev_is_ready()) 
		{
		    delete pFCounter;
		    pFCounter = NULL;
		}

		UPDATE_TITLE();
	}
#endif

#if 0
    else if(!pMainWnd->terminal_test && cfg.ItemSelect(slaveXtalSel))   //connect VSpec, HP8594
	{
	    Name = cfg.VCString(ParSpecDev);
		CREAT_AND_CONNECT_DEVICE(VSpecAnaly,pSpecAnaly,"SpecAnaly");
		unsigned long time = cfg.Vulong(ParSpecIniDlayTime);
		if(time) Sleep(time);
	}
#endif


// connect N4010A,需要判断probeName,ParN4010A_Probedev
#if 1
	if(!pMainWnd->terminal_test )
	//if(!pMainWnd->terminal_test && cfg.ItemSelect(s_N4010A_TEST_Sel))
	{
		Name = "N4010A";	//cfg.VCString(ParBtTestSetsDev);
		temp = "connecting ";
		temp += Name;
		DEV_OP_PROMP(temp);
		
		CREAT_AND_CONNECT_DEVICE(bt_TSET,pBtTestSet, "N4010A");
        
		if(pBtTestSet && !pBtTestSet->dev_is_ready()) 
		{
		    delete pBtTestSet;
		    pBtTestSet = NULL;
		}
        UPDATE_TITLE();
#if 1
		//if(Inst.state == true) 	
		{
			Inst.Dispose();
			Inst.ReleaseDispatch();
			AfxOleTerm();
			Inst.state = false;
		}

		if(pBtTestSet) 	
		{
			AfxOleInit();
			Inst.CreateDispatch("Agilent_N4010_WLanTest.Instrument");
			Inst.SetInstrumentVISAResourceString(cfg.VCString(ParN4010A_Port));
			Inst.SetShowSplashScreen(true);
			Inst.ActivateInstrument();
			//Inst.SetWlanMode();
			Inst.state = true;
			
		}
#endif		
     }
#endif
	
//万用表测电流
#if 1	
	// connect Multimeter
	UPDATE_MULTIMETER(); 
	if(!pMainWnd->terminal_test &&(
		  cfg.ItemSelect(s_PwrOff_cur_sel)
		||cfg.ItemSelect(s_PwrOn_cur_sel)))
	{ 
		if(!pMultimeter)
		{ 
			temp = "Connecting Multimeter ... ";
			DEV_OP_PROMP(temp);
			pMultimeter = getMultimeterDev();
			if(pMultimeter) UPDATE_TITLE();
			else
			{
				ShowMessage += "Multimeter not connect!!\n";
				rValue = false;
			}
		}
    }
#endif


#if 1
	// connect test box
	if(!pMainWnd->terminal_test && cfg.ItemSelect(parTestBoxNeed) && !pTbox) 
	{
		//temp = "正在连接屏蔽箱...";
		temp = "connecting TestBox ...";
		DEV_OP_PROMP(temp);
		
		pTbox = new CTBOX("");
		if(pTbox->connect()) UPDATE_TITLE();
		else
		{
			delete(pTbox);
			pTbox = NULL;
			//ShowMessage += "未连接屏蔽箱!!\n";
			ShowMessage += "TestBox 未连接!!\n";
			rValue = false;
		}
	}
 
#endif	
	

//mcu的串口控制与PC连接
#if 1
	//connect MCU COM
	if(!pMainWnd->terminal_test && !pMCU1_Com)
	{	    		
		//串口线连接到板上的COM口。
		temp = "U18 正在分配COM口...";
		DEV_OP_PROMP(temp);
		pMCU1_Com = new CMCU_COM("U18"); 
		if(pMCU1_Com->connect("", 0)) UPDATE_TITLE();
		else
		{
			delete(pMCU1_Com);
			pMCU1_Com = NULL;
			ShowMessage += "U18 分配COM口失败!!\n";
			rValue = false;
		}
		//AfxMessageBox(pMCU1_Com->port.key.PortName);
	}
	if(!pMainWnd->terminal_test && !pMCU2_Com)	
	{	
		temp = "U5 正在分配COM口...";
		DEV_OP_PROMP(temp);
		pMCU2_Com = new CMCU_COM("U5");
		if(pMCU2_Com->connect("", 1)) UPDATE_TITLE();
		else
		{
			delete(pMCU2_Com);
			pMCU2_Com = NULL;
			ShowMessage += "U5 分配COM口失败!!\n";
			rValue = false;
		}
		//AfxMessageBox(pMCU2_Com->port.key.PortName);
	}
#endif

//trl8711的串口控制与PC连接
//connect trl8711 COM
#if 0
	if(!pMCU2_Com->set_dut_pwr_on(1, MCU_PIO_DUT_PWR, MCU_PIO_NUM_DUT_PWR))
	{ 
		   ShowMessage +=  "set dut pwr err";
		   rValue = false;
	}	
	if(!pMCU2_Com->set_dut_spi_on(1, MCU_PIO_SPI,MCU_PIO_NUM_SPI))
	{
		ShowMessage += "set dut spi on err";
		rValue = false;
	}
	Sleep(1000);
	if(!pMCU2_Com->set_dut_bat_on(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY))
	{
		ShowMessage += "set dut1 bat on err";
		rValue = false;
	}
	/*Sleep(1000);
	if(!pMainWnd->terminal_test && !pRTL8711_1_Com)
	{	    		
		//串口线连接到板上的COM口。
		temp = "R8711_A 正在分配COM口...";
		DEV_OP_PROMP(temp);
		pRTL8711_1_Com = new RTL87XX_COM("R8711_A"); 
		if(pRTL8711_1_Com->connect("", 0)) UPDATE_TITLE();
		else
		{
			delete(pRTL8711_1_Com);
			pRTL8711_1_Com = NULL;
			ShowMessage += "R8711_A 分配COM口失败!!\n";
			rValue = false;
		}
		//AfxMessageBox(pRTL8711_1_Com->port.key.PortName);
	}*/
	Sleep(1000);
	if(!pMCU2_Com->set_dut_bat_on(1, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY))
	{
		ShowMessage += "set dut2 bat on err";
		rValue = false;
	}
	/*Sleep(1000);
	if(!pMainWnd->terminal_test &&!pRTL8711_2_Com)
	{
		temp = "R8711_B 正在分配COM口...";
		DEV_OP_PROMP(temp);
		pRTL8711_2_Com = new RTL87XX_COM("R8711_B"); 
		if(pRTL8711_2_Com->connect("", 0)) UPDATE_TITLE();
		else
		{
			delete(pRTL8711_2_Com);
			pRTL8711_2_Com = NULL;
			ShowMessage += "R8711_B 分配COM口失败!!\n";
			rValue = false;
		}
		//AfxMessageBox(pRTL8711_2_Com->port.key.PortName);
	}*/
	ShowMessage += "test失败!!\n";
	rValue = false;
#endif


	 if(pMainWnd->terminal_test) 
	 {
		 pMainWnd->cnrouting_running = false;
		 return(false);
	 }
	
__err_out:
        pMainWnd->all_device_connected = rValue;
        pMainWnd->PostMessage(WM_CNDEV_FINSH,(WPARAM)rValue,(LPARAM)NULL);
	if(rValue == true)
	{
        	AfxMessageBox("仪器连接完成,请先调整完RF功率补偿后,再使用金机测试一遍机架!");
		pMainWnd->PostMessage(WM_CNDEV_FINSH_SUCCESS,(WPARAM)NULL,(LPARAM)NULL);
		return(true);
	}
	AfxMessageBox(ShowMessage.GetBuffer(0));
	return(false);
}






















