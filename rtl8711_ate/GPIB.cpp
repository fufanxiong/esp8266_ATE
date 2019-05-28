//---------------------------------------------------------------------------
#include "stdafx.h"
#include "GPIB.h"
#include "err.h"
#include "PD218.h"
#include "PD218Dlg.h"

#if 0

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <class T> com_list<T>::com_list()
{
    next = NULL;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool GPIB_DEV::DevRMCreated = false;
#ifdef	GPIB_EXIT
ViSession GPIB_DEV::DEVRM;
#endif
com_list<CString>* GPIB_DEV::list_head = NULL;

GPIB_DEV::GPIB_DEV(const CString &Addr,const CString &name)
{
#ifdef	GPIB_EXIT
    Name = name;
    Address = Addr;
    Created = false;
    Session = VI_SUCCESS+1;
    
	if((false == DevRMCreated) &&
       (viOpenDefaultRM(&DEVRM) < VI_SUCCESS)) {
		ThrowErrMessage(OpenDefaultRM_ERR);
		return;
	}
	DevRMCreated = true;

    if(GPIB_DEV::CreatSession(Name) == 1) {
		Created = true;
        com.key = Address;
        addToList();
	}else Address = "";
#endif
}

GPIB_DEV::~GPIB_DEV()
{
#ifdef	GPIB_EXIT
    dependFromList();
	if(true == DevRMCreated) 
		DevRMCreated = true;
#endif
}

void GPIB_DEV::addToList(void)
{
    if(!list_head) {
        list_head = &com;
        return;
    }
    
    com.next = list_head;
    list_head = &com;
}

int GPIB_DEV::dependFromList(void)
{
    com_list<CString> *lhead = list_head;

    if(lhead == &com) {
        list_head = com.next;
        return(1);
    }
    
    while(lhead != NULL) {
        if(lhead->next == &com) {
            lhead->next = com.next;
            return(1);
        }
        lhead = lhead->next;
    }
    return(0);
}

int GPIB_DEV::DevUsed(CString devAddr)
{
    com_list<CString> *lhead = list_head;

    while(lhead != NULL) {
        if(lhead->key == devAddr) return(1);
        lhead = lhead->next;
    }
    return(0);
}

int GPIB_DEV::CreatSession(const CString &Name)
{
	bool flag = false;
#ifdef	GPIB_EXIT
	ViChar buffer[VI_FIND_BUFLEN];
	ViRsrc matches = buffer;
	ViUInt32 nmatches;
	ViFindList list;
	char char_name[1024] = {0};
	CString Addr = Address;

	if(DevRMCreated == false) return(false);
	viFindRsrc(DEVRM,"GPIB[0-1]*::?*INSTR",&list,&nmatches,matches);
	ViSession TempSession;

	while(nmatches--) {
		if(((Name == "NULL")&&(Addr != matches))||(DevUsed(matches))) {
			if(nmatches) viFindNext(list,matches);
			continue;
		}
		ViStatus Status = viOpen(DEVRM,matches,VI_NULL,VI_NULL,&TempSession);

		if(Status != VI_SUCCESS) {
			viClose(TempSession);
			if(nmatches) viFindNext(list,matches);
			continue;
		}
		
		for(int i=0;i<2;i++) {
			if(viPrintf(TempSession,"*RST\n") != VI_SUCCESS)
				goto __try_agian;
			if(viPrintf(TempSession,"*CLS\n") != VI_SUCCESS)
				goto __try_agian;

			char_name[0] = 0;
			if(Name != "NULL") {
				if(viQueryf(TempSession,"*IDN?\n","%t",char_name) != VI_SUCCESS)
				goto __try_agian;
			}

			flag = true;
			break;
		__try_agian:
			viClear(TempSession);
		}
		viClose(TempSession);
		
		CString DevName = char_name;
		if((Name == "NULL") || ((true == flag)&&(DevName.Find(Name) >= 0))) break;
		flag = false;
		if(nmatches) viFindNext(list,matches);
	}
	viClose(list);
	
	if(true == flag){
		if(viOpen(DEVRM,matches,VI_NULL,VI_NULL,&Session) == VI_SUCCESS) {
			Address = matches;
		}else {
			flag = false;
			viClose(Session);
		}
	}
#endif
	return(flag);
}

//-----------------------------------------------------------------------
// SpecAnaly
//-----------------------------------------------------------------------
VSpecAnaly::VSpecAnaly(const CString &Addr,const CString& name):GPIB_DEV(Addr,name)
{
#ifdef	GPIB_EXIT
	MkTime = pCfgf.Vulong(ParSpecMkCT);
	SweepTime = pCfgf.VDouble(ParSpecSweepTime);
	Rband = pCfgf.VDouble(ParSpecRband);
	Span = pCfgf.VDouble(ParSpecSpan);
	RLv = pCfgf.VDouble(ParSpecRLv);
	centerF = 2400000;
#endif
}

VSpecAnaly::~VSpecAnaly()
{
#ifdef	GPIB_EXIT
	if(Created == true) viClose(Session);
#endif
}


//---------------------------------------------------------------------------
// HP53181A SpecAnaly
//---------------------------------------------------------------------------
HP53181A::HP53181A(const CString &Addr,const CString& name):VSpecAnaly(Addr,name)
{
#ifdef	GPIB_EXIT
    if(Created == true) {   // initial device
	    viPrintf(Session,"IP\n");
	    Sleep(3000);
	    SetCenterF();
	    SetSpan(Span);
	    SetLv(RLv);

	    CString Cmd;
	    Cmd.Format("RB %.0lfKHZ\n",Rband);
	    viPrintf(Session,Cmd.GetBuffer(0));
	    Cmd.Format("ST %.0lfMS\n",SweepTime);
	    viPrintf(Session,Cmd.GetBuffer(0));
    }
#endif
}

bool HP53181A::SetSpan(double Span,char SUnit)
{
#ifdef	GPIB_EXIT
	CString Cmd;
	Cmd.Format("SP %.0lf%cZ\n",Span,SUnit);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
#endif
	return(true);
}

bool HP53181A::SetLv(double Lv)
{
#ifdef	GPIB_EXIT
	CString Cmd;
	Cmd.Format("RL %.0lfDB\n",Lv);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
#endif
	return(true);
}

bool HP53181A::tryMarkPK2Center(void)
{
#ifdef	GPIB_EXIT
	for(int i=0;i<3;i++) 
		if(viPrintf(Session,"MKPK HI;MKCF\n") == VI_SUCCESS) return(true);
#endif
	return(false);
}

bool HP53181A::SetCenterF(double freq,char fUnit)
{
#ifdef	GPIB_EXIT
	CString Cmd;
	Cmd.Format("CF %.0lf%cZ\n",freq,fUnit);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
#endif
	return(true);
}

bool HP53181A::ReadRFPower(double &f,double &lv,int freqSampleNum)
{
	bool rValue = true;
#ifdef	GPIB_EXIT    
	if(viPrintf(Session,"MXMH TRA\n") != VI_SUCCESS) return(false);
	if(MkTime) Sleep(MkTime);

	if(viPrintf(Session,"MKPK HI\n") != VI_SUCCESS) return(false);
	if(viQueryf(Session,"MKA?\n","%lf",&lv) != VI_SUCCESS) {
		lv = 0.0;
		rValue = false;
	}
	if(viQueryf(Session,"MKF?\n","%lf",&f) != VI_SUCCESS) {
		f = 0.0;
		rValue = false;
	}
	viPrintf(Session,"CLRW TRA\n");
#endif
	return(rValue);
}

//---------------------------------------------------------------------------
// MT8810C SpecAnaly
//---------------------------------------------------------------------------
MT8810C::MT8810C(const CString &Addr,const CString& name):VSpecAnaly(Addr,name)
{
#ifdef	GPIB_EXIT 
    if(Created == true) {   // initial device
	    viPrintf(Session,"INI\n");
	    Sleep(3000);
	    viPrintf(Session,"PNLMD SPECT\n");

	    CString Cmd;
	    Cmd.Format("RB %.0lfKHZ\n",Rband);
	    viPrintf(Session,Cmd.GetBuffer(0));
	    Cmd.Format("ST %.0lfMS\n",SweepTime);
	    viPrintf(Session,Cmd.GetBuffer(0));

	    SetCenterF();
	    SetSpan(Span);
	    SetLv(RLv);
    }
#endif 
}

bool MT8810C::SetSpan(double Span,char SUnit)
{
#ifdef	GPIB_EXIT 
	CString Cmd;
	Cmd.Format("SP %lf%cHZ\n",Span,SUnit);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
#endif
	return(true);
}

bool MT8810C::SetLv(double Lv)
{
#ifdef	GPIB_EXIT 
	CString Cmd;
	Cmd.Format("RL %lfDBm\n",Lv);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
#endif
	return(true);
}

bool MT8810C::tryMarkPK2Center(void)
{
#ifdef	GPIB_EXIT 
	for(int i=0;i<3;i++) {
		if(viPrintf(Session,"MKPK HI\n") != VI_SUCCESS) continue;
		if(viPrintf(Session,"MKCF\n") != VI_SUCCESS) continue;
		return(true);
	}
#endif
	return(false);
}

bool MT8810C::SetCenterF(double freq,char fUnit)
{
#ifdef	GPIB_EXIT 
	CString Cmd;
	Cmd.Format("CF %lf%cHZ\n",freq,fUnit);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
#endif
	return(true);
}

bool MT8810C::ReadRFPower(double &f,double &lv,int freqSampleNum)
{
	bool rValue = true;
#ifdef	GPIB_EXIT
	SetSpan(100);
	if(viPrintf(Session,"AMD 1\n") != VI_SUCCESS) return(false);
	if(MkTime) Sleep(MkTime);
	if(viPrintf(Session,"MKPK HI\n") != VI_SUCCESS) return(false);
	Sleep(500);

	viPrintf(Session,"BIN 0\n");
	char lvs[100];
	if(viQueryf(Session,"MKL?\n","%s",&lvs) != VI_SUCCESS) {
		lvs[0] = '0';
		lvs[1] = 0;
		rValue = false;
	}
	lv = atof(lvs);

	viPrintf(Session,"BIN 0\n");
	char fs[100];
	if(viQueryf(Session,"MKF?\n","%s",&fs) != VI_SUCCESS) {
		fs[0] = '0';
		fs[1] = 0;
		rValue = false;
	}
	f = atof(fs);

	viPrintf(Session,"AMD 0\n");
#endif
	return(rValue);
}

//---------------------------------------------------------------------------
// R4131 SpecAnaly
//---------------------------------------------------------------------------
R4131::R4131(const CString &Addr,const CString& name):VSpecAnaly(Addr,name)
{
#ifdef	GPIB_EXIT
    if(Created == true) {   // initial device
	    viPrintf(Session,"IP\n");
	    Sleep(3000);
	    viPrintf(Session,"HD0\n");
	    viPrintf(Session,"DL0\n");

	    CString Cmd;
	    Cmd.Format("RB %.0lfKZ\n",Rband);
	    viPrintf(Session,Cmd.GetBuffer(0));
	    Sleep(1000);
	    Cmd.Format("ST %.0lfMS\n",SweepTime);
	    viPrintf(Session,Cmd.GetBuffer(0));

	    SetCenterF();
	    SetSpan(Span);
	    SetLv(RLv);
    }
#endif
}

bool R4131::SetSpan(double Span,char SUnit)
{
#ifdef	GPIB_EXIT
	CString Cmd;
	Cmd.Format("SP %.0lf%cZ\n",Span,SUnit);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
#endif
	return(true);
}

bool R4131::SetLv(double Lv)
{
#ifdef	GPIB_EXIT
	CString Cmd;
	Cmd.Format("RL %.0lfDM\n",Lv);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
#endif
	return(true);
}

bool R4131::SetCenterF(double freq,char fUnit)
{
#ifdef	GPIB_EXIT
	CString CMD;
	CMD.Format("CF %.0lf%cZ\n",freq,fUnit);
	if(viPrintf(Session,CMD.GetBuffer(0)) != VI_SUCCESS) return(false);
#endif
	return(true);
}

bool R4131::tryMarkPK2Center(void)
{
#ifdef	GPIB_EXIT
	for(int i=0;i<3;i++) 
		if(viPrintf(Session,"MKPK HI;MKCF\n") == VI_SUCCESS) return(true);
#endif
	return(false);
}

bool R4131::ReadRFPower(double &f,double &lv,int freqSampleNum)
{
	bool rValue = true;
#ifdef	GPIB_EXIT    
	if(viPrintf(Session,"MA\n") != VI_SUCCESS) return(false);
	if(MkTime) Sleep(MkTime);

	if(viPrintf(Session,"M4\n") != VI_SUCCESS) return(false);
	if(viQueryf(Session,"OPML\n","%lf",&lv) != VI_SUCCESS) {
		lv = 0.0;
		rValue = false;
	}
	if(viQueryf(Session,"OPMF\n","%lf",&f) != VI_SUCCESS) {
		f = 0.0;
		rValue = false;
	}
	viPrintf(Session,"WR\n");
	Sleep(500);
	viPrintf(Session,"MA\n");
	Sleep(1000);
#endif
	return(rValue);
}

//---------------------------------------------------------------------------
// R3131 SpecAnaly
//---------------------------------------------------------------------------
R3131::R3131(const CString &Addr,const CString& name):VSpecAnaly(Addr,name)
{
#ifdef	GPIB_EXIT
    if(Created == true) {   // initial device
	    viPrintf(Session,"IP\n");
	    Sleep(3000);
	    viPrintf(Session,"HD0\n");
	    viPrintf(Session,"DL0\n");

	    CString Cmd;
	    Cmd.Format("RB %.0lfKZ\n",Rband);
	    viPrintf(Session,Cmd.GetBuffer(0));
	    Sleep(1000);
	    Cmd.Format("SW %.0lfMS\n",SweepTime);
	    viPrintf(Session,Cmd.GetBuffer(0));

	    SetCenterF();
	    SetSpan(Span);
	    SetLv(RLv);
    }
#endif
}

bool R3131::SetSpan(double Span,char SUnit)
{
#ifdef	GPIB_EXIT
	CString Temp;
	Temp.Format("SP %.0lf%cZ\n",Span,SUnit);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) return(false);
#endif
	return(true);
}

bool R3131::SetLv(double Lv)
{
#ifdef	GPIB_EXIT
	CString Temp;
	Temp.Format("RL %.0lfDBM\n",Lv);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) return(false);
#endif
	return(true);
}

bool R3131::SetCenterF(double freq,char fUnit)
{
#ifdef	GPIB_EXIT
	CString CMD;
	CMD.Format("CF %.0lf%cZ\n",freq,fUnit);
	if(viPrintf(Session,CMD.GetBuffer(0)) != VI_SUCCESS) return(false);
#endif
	return(true);
}

bool R3131::ReadRFPower(double &f,double &lv,int freqSampleNum)
{
	bool rValue = true;
#ifdef	GPIB_EXIT    
	if(viPrintf(Session,"AM\n") != VI_SUCCESS) return(false);
	if(MkTime) Sleep(MkTime);
	
	if(viPrintf(Session,"PS\n") != VI_SUCCESS) return(false);
	
	if(viQueryf(Session,"ML?\n","%lf",&lv) != VI_SUCCESS) {
		lv = 0.0;
		rValue = false;
	}
	if(viQueryf(Session,"MF?\n","%lf",&f) != VI_SUCCESS) {
		f = 0.0;
		rValue = false;
	}
	viPrintf(Session,"AB\n");
#endif
	return(rValue);
}


//---------------------------------------------------------------------------
// E4404B SpecAnaly
//---------------------------------------------------------------------------
E4404B::E4404B(const CString &Addr,const CString& name):VSpecAnaly(Addr,name)
{
    if(Created == true) {   // initial device
	    viPrintf(Session,"*RST\n");
	    Sleep(4000);
	    SetCenterF();
	    SetSpan(Span);
	    SetLv(RLv);

	    CString Cmd;
	    Cmd.Format(":SENSe:BANDwidth:VIDeo %.0lfKHZ\n",Rband);
	    viPrintf(Session,Cmd.GetBuffer(0));
	    Cmd.Format(":SENSe:SWEep:TIME %.0lfMS\n",SweepTime);
	    viPrintf(Session,Cmd.GetBuffer(0));

	    Cmd = ":CALCulate:MARKer:MODE POS\n";
	    viPrintf(Session,Cmd.GetBuffer(0));
	    Cmd = ":TRACe:MATH:PEAK:SORT AMPLitude\n";
	    viPrintf(Session,Cmd.GetBuffer(0));
    }
}

bool E4404B::SetSpan(double Span,char SUnit)
{
	CString Cmd;
	Cmd.Format(":SENSe:FREQuency:SPAN %.0lf%cHZ\n",Span,SUnit);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool E4404B::SetLv(double Lv)
{
	CString Cmd;
	Cmd.Format(":DISPlay:WINDow:TRACe:Y:SCALe:RLEVel %.0lfdBm\n",Lv);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool E4404B::tryMarkPK2Center(void)
{
	return(false);
}

bool E4404B::SetCenterF(double freq,char fUnit)
{
	CString Cmd;
	Cmd.Format(":SENSe:FREQuency:CENTer %.0lf%cHZ\n",freq,fUnit);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool E4404B::ReadRFPower(double &f,double &lv,int freqSampleNum)
{
	bool rValue = true;
    
	if(viPrintf(Session,":TRACe1:MODE MAXHold\n") != VI_SUCCESS) return(false);
	if(MkTime) Sleep(MkTime);

	if(viPrintf(Session,":CALCulate:MARKer1:MAXimum\n") != VI_SUCCESS) return(false);
	if(viQueryf(Session,":CALCulate:MARKer1:Y?\n","%lf",&lv) != VI_SUCCESS) {
		lv = 0.0;
		rValue = false;
	}
	if(viQueryf(Session,":CALCulate:MARKer1:X?\n","%lf",&f) != VI_SUCCESS) {
		f = 0.0;
		rValue = false;
	}
	viPrintf(Session,":TRACe1:MODE WRITe\n");
	return(rValue);
}


//---------------------------------------------------------------------------
// ER3131A SpecAnaly
//---------------------------------------------------------------------------
ER3131A::ER3131A(const CString &Addr,const CString& name):VSpecAnaly(Addr,name)
{
	if(Created == true) {   // initial device
		viPrintf(Session,"IP\n");
		Sleep(4000);
		SetCenterF();
		SetSpan(Span);
		SetLv(RLv);
	}
}

bool ER3131A::SetSpan(double Span,char SUnit)
{
	if(viPrintf(Session,"SP %lf%cZ\n",Span,SUnit) != VI_SUCCESS) return(false);
	return(true);
}

bool ER3131A::SetLv(double Lv)
{
	if(viPrintf(Session,"RL %fDB\n",Lv) != VI_SUCCESS) return(false);
	return(true);
}


bool ER3131A::tryMarkPK2Center(void)
{
	return(false);
}

bool ER3131A::SetCenterF(double freq,char fUnit)
{
	if(viPrintf(Session,"CF %lf%cZ\n",freq,fUnit) != VI_SUCCESS) return(false);
	return(true);
}

bool ER3131A::ReadRFPower(double &f,double &lv,int freqSampleNum)
{
	bool rValue = true;
    
	if(viPrintf(Session,"AM\n") != VI_SUCCESS) return(false);
	if(MkTime) Sleep(MkTime);
	if(viPrintf(Session,"MKPK\n") != VI_SUCCESS) return(false);

	char buf[64] = {0};
	if(viQueryf(Session,"MFL?\n","%t",buf) != VI_SUCCESS) {
		lv = 0.0;
		f = 0.0;
		rValue = false;
	}else {
		buf[18] = 0;
		f = atof(&buf[1]);

		buf[19+18] = 0;
		lv = atof(&buf[20]);
	}
	viPrintf(Session,"AW\n");

	return(rValue);
}
#endif

