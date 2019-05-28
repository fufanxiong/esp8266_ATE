#ifndef __AUDIO_ROUTING_H__
#define	__AUDIO_ROUTING_H__
#include "CommHead.h"
#include "tset.h"
class ConfigFile;
class CPD218Dlg;
class ofstream;
//class CDutDevice;
class CBsDevice;
class testSet;

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
class CCFJAudioRouting
{
private:
	CCFJAudioRouting& operator = (const CCFJAudioRouting &x);
	CCFJAudioRouting(CCFJAudioRouting &x);
protected:
        //unsigned long pwr_on_delay;
        //unsigned long pwr_off_delay;
        unsigned long pc_rec_time;
        unsigned long pc_no_man_rec_time;
        testSet *tset;

        int par_id;
        int par_limit_id;
        int prompt_id;
        bool need_box;
        double min_amp;
        double max_amp;
        double min_lv;
        double max_thd;
        int cn_mic_ch;
        int cn_spk_ch;

        CString Text[7];
protected:
        void prompt(int id,const char *str);
        void delay(unsigned long time);
		//bool repower(int iHanlde);

        void show_audio(void);
        bool open_dut_routing(void);
        void close_dut_routing(void);
        bool wait_wave_out(CAudio_par &ok_par);
        friend class CCFJAudioRouting;
        friend bool audio_routing_cfg(CString &param,bool &rValue);
public:
        struct bt_address_s addr;
        CString err_str;
        CAudio_par audio_res;

        CCFJAudioRouting(testSet *Tset,ConfigFile &cfg);
        virtual ~CCFJAudioRouting() { };

        bool cfg_routing(bool man_adj_enable,bool wait_out = false,bool pc_sel_samples = false);
        bool cfg_dut_routing(bool man_adj_enable);
};

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
double get_wave_data_avg(double *data,int len);
double get_pos_offset(double *data,int len);


#endif