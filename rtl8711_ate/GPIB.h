//---------------------------------------------------------------------------
#if 0

#ifndef GPIBDEVICE_H
#define GPIBDEVICE_H
#include <visa.h>
#define	GPIB_EXIT


//---------------------------------------------------------------------------
template <class data_t>
class com_list
{
private:
	com_list& operator = (const com_list &x);
	com_list(com_list &x);
public:
    com_list *next;
	data_t key;
    
	com_list();
	virtual ~com_list(){};
};


//---------------------------------------------------------------------------
class GPIB_DEV
{
private:
	static bool DevRMCreated;
	static com_list<CString> *list_head;

    com_list<CString> com;
    void addToList(void);
	int dependFromList(void);
    int DevUsed(CString devAddr);
    virtual int CreatSession(const CString &name);

    GPIB_DEV& operator = (const GPIB_DEV &x);
	GPIB_DEV(GPIB_DEV &x);
protected:
#ifdef	GPIB_EXIT
	static ViSession DEVRM;
	 ViSession Session;
#endif
    CString Name;
    CString Address;
   
	bool Created;
    
public:
	GPIB_DEV(const CString &Addr,const CString &name);
	virtual ~GPIB_DEV();

	bool creat_success(void) const {return(Created);};	
};

//-----------------------------------------------------------------------
// SpecAnaly
//-----------------------------------------------------------------------
class VSpecAnaly:public GPIB_DEV
{
private:
	VSpecAnaly& operator = (const VSpecAnaly &x);
	VSpecAnaly(VSpecAnaly &x);
protected:
	double Span;
	unsigned long MkTime;
	double SweepTime;
	double Rband;
	double RLv;
	double centerF;

public:
	VSpecAnaly(const CString &Addr,const CString &name);
	virtual ~VSpecAnaly();
    
	virtual bool SetCenterF(double freq=2400,char fUnit='M') = 0;
	virtual bool ReadRFPower(double &f,double &lv,int freqSampleNum = 8) = 0;
};

//-----------------------------------------------------------------------
// HP53181A SpecAnaly
//-----------------------------------------------------------------------
class HP53181A:public VSpecAnaly
{
private:
	HP53181A& operator = (const HP53181A &x);
	HP53181A(HP53181A &x);

	bool SetSpan(double Span=10,char SUnit = 'M');
	bool SetLv(double Lv=26);
	bool tryMarkPK2Center(void);
public:
	HP53181A(const CString &Addr,const CString &name);
	virtual ~HP53181A(){};
    
	bool SetCenterF(double freq=2400,char fUnit='M');
	bool ReadRFPower(double &f,double &lv,int freqSampleNum = 8);
};

//-----------------------------------------------------------------------
// MT8810C SpecAnaly
//-----------------------------------------------------------------------
class MT8810C:public VSpecAnaly
{
private:
	MT8810C& operator = (const MT8810C &x);
	MT8810C(MT8810C &x);

	bool SetSpan(double Span=10,char SUnit = 'M');
	bool SetLv(double Lv=26);
	bool tryMarkPK2Center(void);
public:
	MT8810C(const CString &Addr,const CString &name);
	virtual ~MT8810C(){};
    
	bool SetCenterF(double freq=2400,char fUnit='M');
	bool ReadRFPower(double &f,double &lv,int freqSampleNum = 8);
};

//-----------------------------------------------------------------------
// R4131 SpecAnaly
//-----------------------------------------------------------------------
class R4131:public VSpecAnaly
{
private:
	R4131& operator = (const R4131 &x);
	R4131(R4131 &x);

	bool SetSpan(double Span=10,char SUnit = 'M');
	bool SetLv(double Lv=26);
	bool tryMarkPK2Center(void);
public:
	R4131(const CString &Addr,const CString &name);
	virtual ~R4131(){};
    
	bool SetCenterF(double freq=2400,char fUnit='M');
	bool ReadRFPower(double &f,double &lv,int freqSampleNum = 8);
};

//-----------------------------------------------------------------------
// R3131 SpecAnaly
//-----------------------------------------------------------------------
class R3131:public VSpecAnaly
{
private:
	R3131& operator = (const R3131 &x);
	R3131(R3131 &x);

	bool SetSpan(double Span=10,char SUnit = 'M');
	bool SetLv(double Lv=26);
	bool tryMarkPK2Center(void){return(false);};
public:
	R3131(const CString &Addr,const CString &name);
	virtual ~R3131(){};
    
	bool SetCenterF(double freq=2400,char fUnit='M');
	bool ReadRFPower(double &f,double &lv,int freqSampleNum = 8);
};

//-----------------------------------------------------------------------
// E4404B SpecAnaly
//-----------------------------------------------------------------------
class E4404B:public VSpecAnaly
{
private:
	E4404B& operator = (const E4404B &x);
	E4404B(E4404B &x);

	bool SetSpan(double Span=10,char SUnit = 'M');
	bool SetLv(double Lv=26);
	bool tryMarkPK2Center(void);
public:
	E4404B(const CString &Addr,const CString &name);
	virtual ~E4404B(){};
    
	bool SetCenterF(double freq=2400,char fUnit='M');
	bool ReadRFPower(double &f,double &lv,int freqSampleNum = 8);
};

//-----------------------------------------------------------------------
// E4404B SpecAnaly
//-----------------------------------------------------------------------
class ER3131A:public VSpecAnaly
{
private:
	ER3131A& operator = (const ER3131A &x);
	ER3131A(ER3131A &x);

	bool SetSpan(double Span=10,char SUnit = 'M');
	bool SetLv(double Lv=26);
	bool tryMarkPK2Center(void);
public:
	ER3131A(const CString &Addr,const CString &name);
	virtual ~ER3131A(){};
    
	bool SetCenterF(double freq=2400,char fUnit='M');
	bool ReadRFPower(double &f,double &lv,int freqSampleNum = 8);
};

#endif

#endif	//GPIBDEVICE_H







