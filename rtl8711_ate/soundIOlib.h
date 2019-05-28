
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SOUNDIOLIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SOUNDIOLIB_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifndef ____SOUNDIOLIB____
#define ____SOUNDIOLIB____
#ifdef SOUNDIOLIB_EXPORTS
#define SOUNDIOLIB_API __declspec(dllexport)
#else
#define SOUNDIOLIB_API __declspec(dllimport)
#endif

#define REC_BLOCK_SIZE          8192


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class SOUNDIOLIB_API CAudio_par
{
public:
        CAudio_par(void);
	virtual ~CAudio_par() { };

        bool operator < (const CAudio_par &s) const;
public:
        volatile bool valid;
	volatile double Llv;
	volatile double Lthd;
	volatile double Rlv;
	volatile double Rthd;
        volatile int RShowID;
        volatile int LShowID;
        volatile unsigned short samples;
        volatile int show_pos;

        volatile bool left_mute; 
        volatile bool right_mute;

        volatile bool rec_data_start;
        volatile int record_num;
        volatile double valid_lv;
        volatile bool stop;
};

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
typedef bool (*da_deal_fun)(short* lpData,unsigned long len,void* use_par,int widx,int ridx);
#if 0
bool rec_data_deal(short* lpData,unsigned long len,void* use_par,int widx,int ridx)
{
    CAudio_par &par = *(CAudio_par*)use_par;
    CPD218Dlg* pdlg = pMainWnd;

    if(!pdlg) return(false);
    pdlg->rec_num += 1;
    if(!par.stop && TryEnterCriticalSection(&pdlg->rec_cs))
    {
            for(unsigned long i=0;i<len;i++) pdlg->wave_data[i] = lpData[i];
            pdlg->wave_data_ok = true;
            LeaveCriticalSection(&pdlg->rec_cs);
            pdlg->PostMessage(WM_WAVE_SHOW,(WPARAM)NULL,(LPARAM)use_par);
    }
    return(true);
}
#endif

class SOUNDIOLIB_API CSoundIO
{
private:
	CSoundIO& operator = (const CSoundIO &x);
	CSoundIO(CSoundIO &x);
public:
        CSoundIO(unsigned short play_samples = 44100);
	virtual ~CSoundIO();

public:
        bool is_initialed(void);
        bool set_play_samples(unsigned short sample);
        bool start_audio_deal(da_deal_fun fn,CAudio_par &par,unsigned long rec_time,unsigned long tone_time);
        bool stop_audio_deal(CAudio_par &par);
        bool is_running(void);
        bool audio_analyse(short *data,CAudio_par &par);
};



#endif  //____SOUNDIOLIB____























