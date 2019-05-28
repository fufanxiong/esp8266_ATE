// Recorder.h: interface for the CRecorder class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _____SIGNAL_IN_H____
#define	_____SIGNAL_IN_H____
#include <string>
#include <mmsystem.h>
#include "CommHead.h"

#define MAX_REC_BUF             3
#define REC_SAMPLES             32000
#define REC_CHANNELS            2
#define REC_BITSPERSAMPLE       16

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
typedef bool (*FnDataProcess)(short* lpData,unsigned long len,void* use_par,int widx,int ridx);
class Recorder
{
private:
        friend UINT __stdcall RecorderThreadFunc(LPVOID pParam);
        friend void CALLBACK waveInProc(HWAVEIN hwi,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2);

        unsigned long total_pack;
        bool dev_err;
	HWAVEIN m_hWaveIn;
        WAVEFORMATEX m_WaveFormat;
        WAVEHDR* _waveBlocks;

	CRITICAL_SECTION pending_buf_cs;
	WAVEHDR	*pending_WaveHeader[MAX_REC_BUF];
	volatile int pending_widx;
	volatile int pending_ridx;
	volatile bool wait_record_buffer_ready;

	CRITICAL_SECTION g_cs;
	HANDLE m_hEvent;
	volatile bool thread_terminal;
	volatile bool wav_thread_finish;
	volatile HANDLE m_hThread;
	FnDataProcess fnProcess;
	CAudio_par *pfunProcess_par;

	bool is_all_whead_clearn(void);
	void clearn_all_whead(void);
	bool update_record_buf(LPWAVEHDR pwh);
public:
	Recorder(void);
	virtual ~Recorder();

        bool ready(void) { return(_waveBlocks != NULL); }
        bool finish(void) { return(wav_thread_finish); }
	bool start(FnDataProcess fn,CAudio_par *par,unsigned long Time = 0);
	void stop(void *par);
};


#endif // _____SIGNAL_IN_H____
