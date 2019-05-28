#include "stdafx.h"
#include "PD218.h"
#include "PD218Dlg.h"
#include "err.h"
#include "config_deal.h"
#include "TestItem.h"
#include "cderr.h "   		//for definition of FNERR_BUFFERTOOSMALL
#include "selDutCom.h"
#include "soundIOlib.h"
#include "cpu.h"
#include "audioSacle.h"
#include "audioRouting.h"

#include <fstream.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h>
#include <Dbt.h>

#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <math.h>

#define AUDIO_441K              44100
#define AUDIO_32K               32000



CString audio_prompt;

#if 0
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

//构造函数初始化
CCFJAudioRouting::CCFJAudioRouting(testSet *Tset,ConfigFile &cfg)
{
        tset = Tset;
        //pwr_on_delay = cfg.Vulong(parPwrOnDelayTime);
        //pwr_off_delay = 2000;
        pc_rec_time = 3000;
        pc_no_man_rec_time = 5000;

        if(tset->dev_id == TEST_DEV_ID_B)
        {
            par_id = IDC_CurTestObj;
            par_limit_id = IDC_CurTestItem;
            prompt_id = IDC_CurTestObj;
        }
        else
        {
            par_id = IDC_CurTestObj2;
            par_limit_id = IDC_CurTestItem2;
            prompt_id = IDC_CurTestObj;
        }
       
		need_box = pCfgf.ItemSelect(parTestBoxNeed);	
        min_amp = cfg.VDouble(s_dut_audio_min_amp);
        max_amp = cfg.VDouble(s_dut_audio_max_amp);
        min_lv = cfg.VDouble(s_audio_pc_AD_min);
        max_thd = cfg.VDouble(s_audio_pc_THD_max);

        //BT audio io config(1:left 2:right 3:stereo)
		//cn_mic_ch = cfg.get_mic_ch_mode();
        //cn_spk_ch = cfg.get_spk_ch_mode();
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
void CCFJAudioRouting::prompt(int id,const char *str)
{
        audio_prompt = str;
        UPDATE_STATIC(id,audio_prompt);
}


//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
void CCFJAudioRouting::delay(unsigned long time)
{
	int i = 0;
	CString temp;
	
	while(time)
	{
		if(pMainWnd->terminal_test) break;
		if(!(i % 5))
		{
			temp.Format(" delay %d",time);
			prompt(prompt_id,temp);
		}
		Sleep(100);
		time -= 100;
		i += 1;
	}
}




void CCFJAudioRouting::close_dut_routing(void)
{
    if(pMainWnd->terminal_test) return;
	if(tset->dev_id ==  TEST_DEV_ID_A)
	{   
       pMCU2_Com->set_dut_current_off(1, MCU_PIO_DUT1_CURRENT,MCU_PIO_NUM_DUT1_CURRENT);
       pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY);
	   pMCU2_Com->set_dut_vreg_off(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG);	   
	}
	else
	{
       pMCU1_Com->set_dut_current_off(0, MCU_PIO_DUT2_CURRENT,MCU_PIO_NUM_DUT2_CURRENT);
       pMCU1_Com->set_dut_bat_off(0, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY);
	   pMCU1_Com->set_dut_vreg_off(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG); 
	}

}


//#endif

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
void CCFJAudioRouting::show_audio(void)
{
        CAudio_par &audio_obj = pMainWnd->audio_obj;
        CString temp;

        if(audio_obj.RShowID && audio_obj.LShowID) 
                temp.Format("L(%.2lf%% %.0lf) R(%.2lf%% %.0lf)",audio_obj.Lthd,audio_obj.Llv,audio_obj.Rthd,audio_obj.Rlv);
        else if(audio_obj.RShowID) temp.Format("R(%.2lf%% %.0lf)",audio_obj.Rthd,audio_obj.Rlv);
        else if(audio_obj.LShowID) temp.Format("L(%.2lf%% %.0lf)",audio_obj.Lthd,audio_obj.Llv);
        prompt(par_id,temp);
}


//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
bool CCFJAudioRouting::cfg_routing(bool man_adj_enable, bool wait_out, bool pc_sel_samples)
{
        CAudio_par &audio_obj = pMainWnd->audio_obj;
        bool show_value = true;
        CString temp;
        ConfigFile &cfg = pCfgf;
        bool res = true;

        audio_obj.left_mute = false;
        audio_obj.right_mute = false;
        audio_obj.valid = false;
        audio_obj.stop = false;
        audio_obj.record_num = 0;
        pMainWnd->stop_audio_cfg = false;
        	   
        
        if(man_adj_enable) AUDIO_CFG_MSG(AUDIO_CFG_MSG_0,Text);
        else AUDIO_CFG_MSG(AUDIO_CFG_MSG_2,Text);
        temp.Format("请调音量使：THD<%.2lf%% 且 AD>%.0lf",max_thd,min_lv);
        prompt(par_limit_id,temp);

        if(!pMainWnd->start_audio_deal(&audio_obj))
        {
                err_str = "音频通路调整错误!!\n";
                AUDIO_CFG_MSG(AUDIO_CFG_MSG_1,Text);
                return(false);
        }

        //man ax adjust 3 minutes
        unsigned long time = 0;
        unsigned long adj_time = 3*60*1000;
        if(!man_adj_enable)
        {
                adj_time = pc_no_man_rec_time;
                temp.Format("正在读取数据，请等待 %dms",adj_time);
                prompt(par_limit_id,temp);
        }

        for(int i = 0; time < adj_time; i++, time += 100)
        {
                if(!pMainWnd) return(false);
                if(pMainWnd->terminal_test) break;
                if(pMainWnd->stop_audio_cfg) break;

                if(!(i%5)) show_value = true;
                if(audio_obj.valid && show_value)
                {
                        show_value = false;
                        show_audio();
                }
                Sleep(100);
        }

        //timeout or terminal
        if(man_adj_enable && !pMainWnd->stop_audio_cfg)
        {
                pMainWnd->stop_audio_deal(&audio_obj);
                AUDIO_CFG_MSG(AUDIO_CFG_MSG_1,Text);
                return(false);
        }
		
		//cfg routing
        if(man_adj_enable)
        {
                temp.Format("正在读取数据，请等待 %dms", pc_rec_time);
                prompt(par_limit_id,temp);

                if((audio_obj.record_num > 0) && (audio_obj.record_num < 100)) audio_obj.record_num = 100;              
				pMainWnd->audio_tdata_len = 0;
                unsigned long time = 0;
                for(int i=0;time < pc_rec_time;i++,time += 100)
                {
                        if(!pMainWnd) return(false);
                        if(pMainWnd->terminal_test) break;

                        if(!(i%5)) show_value = true;
                        if(audio_obj.valid && show_value)
                        {
                            show_value = false;
                            show_audio();
                        }
                        Sleep(100);
                }
                if(pMainWnd->terminal_test) 
                {
                        pMainWnd->stop_audio_deal(&audio_obj);
                        AUDIO_CFG_MSG(AUDIO_CFG_MSG_1,Text);
                        return(false);
                }
        }

        bool rvalid = true;
        bool lvalid = true;
        audio_res = audio_obj;
        if(!audio_res.valid || (pMainWnd->audio_tdata_len < MAX_AUDIO_TDATA) || !pMainWnd->get_fine_audio_value(&audio_res))
        {
                rvalid = false;
                lvalid = false;
        }
        if(!audio_res.LShowID) lvalid = true;
        else if(!audio_res.valid || !(audio_res.Llv >= min_lv)) lvalid = false;
       
        if(!audio_res.RShowID) rvalid = true;
        else if(!audio_res.valid || !(audio_res.Rlv >= min_lv)) rvalid = false;
		if(!lvalid || !rvalid)
        { 
                err_str.Format("音频通路参数不合适(L:%.0lf %.2lf%% R:%.0lf %.2lf%%),请重调整!!\n",
                               audio_res.Llv,audio_res.Lthd,audio_res.Rlv,audio_res.Rthd);
                res = false;
        }

        // look other samples
        if(res && pc_sel_samples)
        { 
                temp.Format("正在读取数据2，请等待 %dms",pc_rec_time);
                prompt(par_limit_id,temp);

                pMainWnd->stop_audio_deal(&audio_obj);
                if(audio_obj.samples != AUDIO_32K) audio_obj.samples = AUDIO_32K;
                else audio_obj.samples = AUDIO_441K;
                if(!pMainWnd->start_audio_deal(&audio_obj))
                {
                        err_str = "启动音频错误!!\n";
                        AUDIO_CFG_MSG(AUDIO_CFG_MSG_1,Text);
                        return(false);
                }
                
                audio_obj.record_num = 0;
                pMainWnd->audio_tdata_len = 0;
                unsigned long time = 0;
                show_value = true;
                for(i=0;time < pc_rec_time;i++,time += 100)
                {
                        if(!pMainWnd) return(false);
                        if(pMainWnd->terminal_test) break;

                        if(!(i%5)) show_value = true;
                        if(audio_obj.valid && show_value)
                        {
                            show_value = false;
                            show_audio();
                        }
                        Sleep(100);
                }
                if(pMainWnd->terminal_test) 
                {
                        pMainWnd->stop_audio_deal(&audio_obj);
                        AUDIO_CFG_MSG(AUDIO_CFG_MSG_1,Text);
                        return(false);
                }

                bool s2_rvalid = true;
                bool s2_lvalid = true;
                CAudio_par s2_audio_res = audio_obj;
                if(!s2_audio_res.valid || (pMainWnd->audio_tdata_len < MAX_AUDIO_TDATA) || !pMainWnd->get_fine_audio_value(&s2_audio_res))
                {
                        s2_rvalid = false;
                        s2_lvalid = false; 
                }
				
				if(!s2_audio_res.LShowID) s2_lvalid = true;
                else if(!(s2_audio_res.Llv >= min_lv)) s2_lvalid = false;

                if(!s2_audio_res.RShowID) s2_rvalid = true;
                else if(!(s2_audio_res.Rlv >= min_lv)) s2_rvalid = false;
                if(s2_lvalid && s2_rvalid)
                { 
                        double thd = audio_res.Rthd + audio_res.Lthd;
                        double s2_thd = s2_audio_res.Rthd + s2_audio_res.Lthd;
						if(fabs(thd - s2_thd) >= 0.1) 
						{
							if(thd > s2_thd) audio_res = s2_audio_res;
						}
						else
						{
							if(audio_res.show_pos > s2_audio_res.show_pos) audio_res = s2_audio_res;
						}   
                }
        }

#if 0
		if(res && wait_out) 
        {
                if(!wait_wave_out(audio_res))
                {
                         err_str += "电脑短接音频通路未正常断开!!\n";
                         res = false; 
                }
        }

#endif
        pMainWnd->stop_audio_deal(&audio_obj);      
		AUDIO_CFG_MSG(AUDIO_CFG_MSG_1,Text);
        return(res);
}


//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
bool CCFJAudioRouting::wait_wave_out(CAudio_par &ok_par)
{
        CAudio_par &audio_obj = pMainWnd->audio_obj;
        bool show_value = true;
        ConfigFile &cfg = pCfgf;
        CAudio_par tpar;
        CString temp;

        temp = "电脑短接音频通路断开";
        prompt(par_limit_id,temp);
		Sleep(100);
		
        //man ax adjust 3 minutes
        unsigned long time = 0;
        unsigned long adj_time = 3*60*1000;
        for(int i = 0; time < adj_time; i++, time += 100)
        {
                if(!pMainWnd) return(false);
                if(pMainWnd->terminal_test) return(false);
                if(!pMainWnd->soundio->is_running()) break;
                if(pMainWnd->get_fine_audio_value(&tpar))
                {
                        bool lvalid = false;
                        if(!audio_res.LShowID) lvalid = true;
                        else if(tpar.Llv <= (ok_par.Llv / 3)) lvalid = true;

                        bool rvalid = false;
                        if(!audio_res.RShowID) rvalid = true;
                        else if(tpar.Rlv <= (ok_par.Rlv / 3)) rvalid = true;
                        if(rvalid && lvalid) return(true);
                }

                if(!(i%5)) show_value = true;
                if(audio_obj.valid && show_value)
                {
                        show_value = false;
                        show_audio();
                }
                Sleep(100);
        }


		if(!pMCU1_Com->set_test_board_short_pc_off(0,MCU_PIO_AUDIO_CHK,MCU_PIO_NUM_AUDIO_CHK))
        {
                return(false);
        }

        return(false);
}


//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
bool CCFJAudioRouting::cfg_dut_routing(bool man_adj_enable)
{
       if(pMainWnd->terminal_test) return(false);
      
	   if(!open_dut_routing())
        {
		   return (false);
        }

	   int golden_idx = tset->gloden_idx();
       if(!golden_idx)
	   {
	      err_str = "is not golden\n";
          return (false);
	   }
        CDutGlodenPar *pgloden = pMainWnd->dutGloden[golden_idx];
        tset->gloden_vamplitude = 0;//pgloden->amplitude; 
        
        if(!cfg_routing(man_adj_enable)) 
        {
            close_dut_routing();  
			return(false);
        }
	   close_dut_routing();
	   return(true);
}


unsigned short pio = 0xffff;
bool CCFJAudioRouting::open_dut_routing(void)
{
    ConfigFile &cfg = pCfgf;
	CString temp;
	CBsDevice *pobj;
	CString Dev;
	uint16 ver_len = 0;
	uint16 io = 0;
	int i;
	int count = 3;
	int iHandle = 0;
	int MAX_TIMEOUT = 10*1000; //up to 5 times
	int iTimeout = 0;
	bool return_val = true;
	int status;
	int mode;
	
	tset->r_addr.is_valid = false;
	if(pMainWnd->terminal_test) 
	{
		err_str = "";
		return (false);
	}
    
	if(!tset || !tset->pSpiObj)
    {
         err_str = "invalid dut\n";
         return (false);
    }

    Dev = (tset->dev_id == TEST_DEV_ID_A)?"DUT A":"DUT B";
    temp = "音频通路：连接 ";
    temp += Dev;
    prompt(prompt_id,temp);
	
	if(pMainWnd->terminal_test) return(false);
	
	pobj = tset->pSpiObj;
    
    //AfxMessageBox(pobj->spi_port->PortName); 

	//connect
	//spi connect need pwr
	if(!pMCU1_Com || !pMCU2_Com)
	{
	    err_str = "null com";
		return (false);
	}

__dut_reBattery:
    if(tset->dev_id ==  TEST_DEV_ID_A)
	{   
	   pMCU2_Com->set_dut_current_off(1, MCU_PIO_DUT1_CURRENT,MCU_PIO_NUM_DUT1_CURRENT);  
	   pMCU2_Com->set_dut_vreg_off(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG);
	   pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY);	   
	   
	   Sleep(500);
	}
	else
	{
	   pMCU1_Com->set_dut_current_off(0, MCU_PIO_DUT2_CURRENT,MCU_PIO_NUM_DUT2_CURRENT);
       pMCU1_Com->set_dut_vreg_off(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG); 
	   pMCU1_Com->set_dut_bat_off(0, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY);
	      
	   Sleep(500);
	}
    
    if(tset->dev_id ==  TEST_DEV_ID_A)
	{  
	   pMCU2_Com->set_dut_current_on(1, MCU_PIO_DUT1_CURRENT,MCU_PIO_NUM_DUT1_CURRENT);
       pMCU2_Com->set_dut_bat_on(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY);
	   Sleep(500);
	   pMCU2_Com->set_dut_vreg_on(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG);	
	   pMCU1_Com->set_dut1_rout_audio_on(0,MCU_PIO_DUT2_AUDIO,MCU_PIO_NUM_DUT2_AUDIO);
	   
	}
	else
	{
	   pMCU1_Com->set_dut_current_on(0, MCU_PIO_DUT2_CURRENT,MCU_PIO_NUM_DUT2_CURRENT);
       pMCU1_Com->set_dut_bat_on(0, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY);
	   Sleep(500);
	   pMCU1_Com->set_dut_vreg_on(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG); 
	   pMCU1_Com->set_dut2_rout_audio_on(0,MCU_PIO_DUT2_AUDIO,MCU_PIO_NUM_DUT2_AUDIO);
	   
	}

    Sleep(1000); //等待按键开机
	do
	{
		if(pobj->spi_port->port_type() == PORT_TYPE_USB)
		{
	       iHandle = openTestEngine(USB, pobj->spi_port->port_num(), 0, 1000, 500);
		}
		else
		{
		   iHandle = openTestEngineSpiTrans(pobj->spi_port->port_num(),0);
		}
		Sleep(100);
		iTimeout += 1000;
	}while(iHandle == 0 && iTimeout <= MAX_TIMEOUT);

    if(!iHandle)
	{
	    if(count--) goto __dut_reBattery;
		err_str = "connect spi err";
		return_val = false;
		goto __cfg_dut_routing_err;
	}

	uint32 lap;
	uint8  uap;
	uint16 nap;
	if(psReadBdAddr(iHandle, &lap, &uap, &nap) != TE_OK)
	{
		err_str = "read bt address err";
		return_val = false;
		goto __cfg_dut_routing_err;
	}

	tset->r_addr.lap = lap;
	tset->r_addr.nap = nap;
	tset->r_addr.uap = uap;
	tset->r_addr.is_valid = true;

	if(!tset->gloden_idx())
	{ 
		err_str = "";
		err_str.Format("%.4X%.2X %.6X is not golden\n",tset->r_addr.nap,tset->r_addr.uap,tset->r_addr.lap);
		return_val = false;
		goto __cfg_dut_routing_err;
	}
	
	temp = Dev +"开启音频通路...";
	prompt(prompt_id, temp);

//#if 0

	//Ampifier sw on
#if 0
	if(pCfgf.ItemSelect(s_audio_amp_sel))
	{
	    if(!pMCU1_Com->set_dut_spk_amp_on(0, MCU_PIO_DUT_SPK_AMP, MCU_PIO_NUM_DUT_SPK_AMP))
		{
		   err_str = "dut amplifier sw on err";
		   return_val = false;
		   goto __cfg_dut_routing_err;
		}
	}
#endif
	//vm disable, 1
	status = psWriteVmDisable(iHandle, 1);
	if(status != TE_OK)
	{
      err_str =  "psWriteVmDisable error";
	  return_val = false;
	  goto __cfg_dut_routing_err;

	}
	else 
	{
		Sleep(1000);
		uint8 cur_st;
		status = psReadVmDisable(iHandle,&cur_st);
		if((status != TE_OK) || (cur_st != 1))
		{
            err_str = "vm disable not ok";
			return_val = false;
			goto __cfg_dut_routing_err;
		}
	}

#if 0	
	//Codec_io clr, 0x01b9 is codec io ps_key
	status = psRead(iHandle, 0x01b9, PS_STORES_IFR, 1, &io, &ver_len);
    if(status!= TE_OK)
	{
		pio = 0xffff;
	}
	else
	{
	    pio = io;
		status = psClear(iHandle,0x01b9, PS_STORES_IF);
		if(status != TE_OK)
		{
			err_str = "clr codec pio err";
			return_val = false;
		    goto __cfg_dut_routing_err;
		}
	}
#endif

	//bc reset
	for(i = 0; i < 5; i++)
	{
		status = bccmdSetWarmReset(iHandle,1000);
		if(status == TE_OK)
		{
			Sleep(3000);
			break;
		}
		else if(i == 4)
		{
		    err_str = "bc warm reset err";
			return_val = false;
		    goto __cfg_dut_routing_err;
		}

		Sleep(500);
	}

	//vm enable, 0
	status = psWriteVmDisable(iHandle, 0);
	if(status != TE_OK)
	{
      err_str =  "psWriteEnable error";
	  return_val = false;
	  goto __cfg_dut_routing_err;

	}
	else 
	{
		Sleep(1000);
		uint8 cur_st;
		status = psReadVmDisable(iHandle,&cur_st);
		if((status != TE_OK) || (cur_st != 0))
		{
            err_str = "vm enable not ok";
			return_val = false;
			goto __cfg_dut_routing_err;
		}
	}

#if 0	
	//Codec_io store, 0x01b9 is codec io ps_key
	if(pio != 0xffff)
	{
		status = psWriteVerify(iHandle, 0x01b9, PS_STORES_IF, 1, &pio);
		if(status != TE_OK)
		{
			err_str= "set back codec io err";
            return_val = false;
			goto __cfg_dut_routing_err;
		}
	}
#endif
#if 0
	mode = pCfgf.Vint(s_audio_mic_spk_mode);
#endif
	status = radiotestStereoCodecLB(iHandle,44100, mode);
	if(status != TE_OK)
	{
		err_str =  "dut audio loop back err\n";
		return_val = false;
		goto __cfg_dut_routing_err;
	}
	
	if(!iHandle) closeTestEngine(iHandle);
	return(true);

//#endif

__cfg_dut_routing_err:

	if(!iHandle) closeTestEngine(iHandle);
	if(tset->dev_id ==  TEST_DEV_ID_A)
	{   
       pMCU2_Com->set_dut_current_off(1, MCU_PIO_DUT1_CURRENT,MCU_PIO_NUM_DUT1_CURRENT);
       pMCU2_Com->set_dut_bat_off(1, MCU_PIO_DUT1_BATTERY, MCU_PIO_NUM_DUT1_BATTERY);
	   pMCU2_Com->set_dut_vreg_off(1, MCU_PIO_DUT1_VREG, MCU_PIO_NUM_DUT1_VREG);
	   pMCU1_Com->set_dut_spk_amp_off(0, MCU_PIO_DUT_SPK_AMP, MCU_PIO_NUM_DUT_SPK_AMP);
	}
	else
	{
       pMCU1_Com->set_dut_current_off(0, MCU_PIO_DUT2_CURRENT,MCU_PIO_NUM_DUT2_CURRENT);
       pMCU1_Com->set_dut_bat_off(0, MCU_PIO_DUT2_BATTERY, MCU_PIO_NUM_DUT2_BATTERY);
	   pMCU1_Com->set_dut_vreg_off(0, MCU_PIO_DUT2_VREG, MCU_PIO_NUM_DUT2_VREG); 
	   pMCU1_Com->set_dut_spk_amp_off(0, MCU_PIO_DUT_SPK_AMP, MCU_PIO_NUM_DUT_SPK_AMP);
	}

	return(return_val);
}


//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
double get_wave_data_avg(double *data,int len)
{
        int j;
        double temp;
        for(int i=1;i<len;i++)
        {
			j = i;
			temp = data[i];
			while(j)
			{
				if(data[j-1] <= temp) break;
				data[j] = data[j-1];
				data[j-1] = temp;
				j -= 1;
			}
        }

//avg
        if(len == 1) return(data[0]);
        if(len <= 3)
        { 
                temp = 0;
                for(i = 0;i<len;i++) temp += data[i];
                temp = temp/len;
                return(temp);
        }

//avg
        len = len - 2;
        temp = data[0];
        for(i =0;i<len;i++) data[i] = data[i+1];
        data[i] = temp;

        if(len <= 4)
        { 
                temp = 0;
                for(i = 0;i<len;i++) temp += data[i];
                temp = temp/len;
                return(temp);
        }

//mid avg
        double avg = 0;
        for(i =0;i<len;i++) avg += data[i];
        avg /= len;

        double data1,max1,data2,max2;
        data1 = data[0];
        max1 = fabs(data1 - avg);
        data2 = data[1];
        max2 = fabs(data2 - avg);
        for(i = 2;i<len;i++)
        {
                temp =  fabs(data[i] - avg);
                if(temp > max1)
                { 
                        data[i-2] = data1;
                        data1 = data[i];
                        max1 = temp;
                }
                else if(temp > max2)
                { 
                        data[i-2] = data2;
                        data2 = data[i];
                        max2 = temp;
                }
                else
                {
                        data[i-2] = data[i];
                }
        }
        data[i-2] = data1;
        data[i-1] = data2;

        len = len - 2;
        temp = 0;
        for(i = 0;i<len;i++) temp += data[i];
        temp = temp/len;
        return(temp);
}

double get_pos_offset(double *data,int len)
{
        int j;
        double temp;

        for(int i=1;i<len;i++)
        {
                j = i;
                temp = data[i];
                while(j)
                {
                        if(data[j-1] <= temp) break;
                        data[j] = data[j-1];
                        data[j-1] = temp;
                        j -= 1;
                }
        }

        if(len == 1) return(0);
        if(len <= 3)
        { 
                temp = 0;
                for(i = 0;i<len;i++) temp += data[i];
                temp = temp/len;

                double offset = 0;
                for(i = 0;i<len;i++) offset += fabs(temp - data[i]);
                offset = offset*200/len;
                return(offset);
        }

//avg
        len = len - 2;
        temp = data[0];
        for(i =0;i<len;i++) data[i] = data[i+1];
        data[i] = temp;

        if(len <= 4)
        { 
                temp = 0;
                for(i = 0;i<len;i++) temp += data[i];
                temp = temp/len;

                double offset = 0;
                for(i = 0;i<len;i++) offset += fabs(temp - data[i]);
                offset = offset*200/len;
                return(offset);
        }

//mid avg
        double avg = 0;
        for(i =0;i<len;i++) avg += data[i];
        avg /= len;

        double data1,max1,data2,max2;
        data1 = data[0];
        max1 = fabs(data1 - avg);
        data2 = data[1];
        max2 = fabs(data2 - avg);
        for(i = 2;i<len;i++)
        {
                temp =  fabs(data[i] - avg);
                if(temp > max1)
                { 
                        data[i-2] = data1;
                        data1 = data[i];
                        max1 = temp;
                }
                else if(temp > max2)
                { 
                        data[i-2] = data2;
                        data2 = data[i];
                        max2 = temp;
                }
                else
                {
                        data[i-2] = data[i];
                }
        }
        data[i-2] = data1;
        data[i-1] = data2;

        len = len - 2;
        temp = 0;
        for(i = 0;i<len;i++) temp += data[i];
        temp = temp/len;

        double offset = 0;
        for(i = 0;i<len;i++) offset += fabs(temp - data[i]);
        offset = offset*200/len;
        return(offset);
}

//#endif

#endif