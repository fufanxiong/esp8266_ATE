/*	device.h
 */
#ifndef __NENDEVICE_H__
#define	__NENDEVICE_H__
#include "CommHead.h"
#include "TestEngine.h"
#include <visa.h>


class CTestItem;
class ConfigFile;
class CPD218Dlg;
class CSortListCtrl;
class ofstream;
class ifstream;
class CBsDevice;
class SPISource;
class VSpecAnaly;
class CDutGlodenPar;

#define VM_SUCCESS 0




//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
template <class data_t>
class port_list
{
private:
	port_list& operator = (const port_list &x);
	port_list(port_list &x);
	port_list();
public:
	port_list *next;
	data_t key;
    
	port_list(const CString &par1,const CString &par2,const CString &par3);
	virtual ~port_list(){};

	void addToList(port_list *&head);
	void dependFromList(port_list *&head);
	static bool DevUsed(const CString &par1,port_list *head);
};

template <class T> 
port_list<T>::port_list(const CString &par1,const CString &par2,const CString &par3)
		:key(par1,par2,par3)
{
    next = NULL;
}

template <class T> 
void port_list<T>::addToList(port_list *&head)
{
	port_list *com = this;

	if(!head) {
		head = com;
		return;
	}
	com->next = head;
	head = com;
}

template <class T> 
void port_list<T>::dependFromList(port_list *&head)
{
	port_list *lhead = head;
	port_list *com = this;

	if(lhead == com) {
		head = com->next;
		return;
	}
    
	while(lhead != NULL) {
		if(lhead->next == com) {
			lhead->next = com->next;
			return;
		}
		lhead = lhead->next;
	}
}

template <class PortSource> 
bool port_list<PortSource>::DevUsed(const CString &devAddr,port_list *head)
{
	port_list *lhead = head;

	while(lhead != NULL) {
		if(lhead->key.maping && (lhead->key.PortName == devAddr)) return(1);
		lhead = lhead->next;
	}
	return(0);
}

bool is_name_null(CString name);


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#define PORT_TYPE_SPI  	0
#define PORT_TYPE_USB 	1
class SPISource {
private:
	SPISource& operator = (const SPISource &x);
	SPISource(SPISource &x);
public:
	bool maping;
	CString PortNum;
	CString PortName;
	///////////CSR///////////////////////////////////////////////////////////////
	CString trans;
	int PortType;
	class SPISource* next;

	SPISource(bool maped = 0,const CString &name = "",const CString &NUM = "",int Type = PORT_TYPE_SPI)
	{
		maping = maped;
		PortNum = NUM;
		PortName = name;
		PortType = Type;
		next = NULL;
	};
	virtual ~SPISource(){};

	const CString& get_trans(void);
	const CString& port_name(void) { return(PortName); };
	const CString& port_num(void) { return(PortNum); };
	const int port_type(void) { return(PortType); };
	bool is_port_busy(void) { return(maping); };
	void set_port_busy(void) { maping = 1; };
	void set_port_idle(void) { maping = 0; };
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class SPISourceContaner {
private:
	SPISourceContaner& operator = (const SPISourceContaner &x);
	SPISourceContaner(SPISourceContaner &x);
public:
	class SPISource* hlist;

	SPISourceContaner(void) { hlist = NULL; };
	virtual ~SPISourceContaner();

	class SPISource* add(bool maped = 0,const CString &name = "",const CString &NUM = "",int Type = PORT_TYPE_SPI);
	class SPISource* del(class SPISource* hlist);
	class SPISource* get_idle(void);
	class SPISource* get_nex(SPISource* cur);
	class SPISource* get_port(const CString &name);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
struct bk_pskey_item {
	struct bk_pskey_item *next;
	unsigned long pskey;
	int data_len;
	unsigned short *data;
};

class backup_pskey_contaner {
private:
	backup_pskey_contaner& operator = (const backup_pskey_contaner &x);
	backup_pskey_contaner(backup_pskey_contaner &x);
public:
	struct bk_pskey_item* head;

	backup_pskey_contaner(void) { head = NULL; };
	virtual ~backup_pskey_contaner();

	struct bk_pskey_item* add(unsigned long key);
	struct bk_pskey_item* set_data(struct bk_pskey_item* item,int len,const unsigned short *data);
};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class PortSource {
private:
	PortSource& operator = (const PortSource &x);
	PortSource(PortSource &x);
public:
	bool maping;
	CString DevName;
	CString PortName;
	CString connect_probe_name;
	int Baud;
	int Parity;
	
	PortSource(const CString &port_name = "",
		   const CString &dev_name = "",
		   const CString &con_probe_name = "")
	{
		maping = false;
		PortName = port_name;
		DevName = dev_name;
		connect_probe_name = con_probe_name;
		Baud = 9600;
		Parity = NOPARITY;
	};
	virtual ~PortSource(){};

	bool is_probe_name_null(void) { return(is_name_null(connect_probe_name)); };
	bool is_port_name_null(void) { return(is_name_null(PortName)); };

};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//#define	COM_SOURCEMAX	32
#define	COM_SOURCEMAX	64
#define ALLRXBUFFER	0xffff
#define ERRSTOP		0x55
#define RxBufSize	2048

class CComDevice
{
public:
	static bool com_source_mapped;
	static int com_map[COM_SOURCEMAX];
	static port_list<PortSource> *list_head;
	static bool is_com_index_valid(int index);
	static bool is_com_index_can_used(int index);
	CString using_port_name;
private:
	CComDevice& operator = (const CComDevice &x);
	CComDevice(CComDevice &x);
	bool ClearRxBuf(unsigned length = 10);
	bool recive_by_length(char *str,unsigned int &len,unsigned long time = 2000);
	virtual bool recive_by_mask(char *d_str,unsigned int d_len,const char *m_str,
				    unsigned int m_len,unsigned long time = 2000,
				    unsigned int d_len_max = 1024);
	virtual bool ___send_search_get(const char *s_str,unsigned int s_len,
				     char *d_str,unsigned int d_len,
				     const char *v_str=NULL,unsigned int v_len=0,
				     unsigned long time = 2000,unsigned PurgeRx = 100);
	void record_err_cmd_data(const char *s_str,unsigned int s_len,
				 const char *v_str=NULL,unsigned int v_len=0);
	void record_err_cmd_data(unsigned long s_data,unsigned long v_data = -1);
protected:
	bool openCom(const CString &Com,int Baud,int Parity = NOPARITY);
	virtual bool DevValid(int high_low) = 0;
	//bool DevValid(void);

	HANDLE 	hCom;
	//CString using_port_name;
	char rxBuf[RxBufSize+128];
	int rxBuf_len;
public:
	port_list<PortSource> port;

	virtual ~CComDevice();
	CComDevice(const CString &dev,const CString &Com = "",const CString &probe_name = "");

	static void DevSourceMap(void);
	const bool &port_maped(void){return(port.key.maping);};
	virtual const CString &dev_name(void){return(port.key.DevName);};
	virtual const CString &port_name(void){return(port.key.PortName);};
	virtual const int &port_baud(void){return(port.key.Baud);};
	virtual const int &port_parity(void){return(port.key.Parity);};
	bool dev_is_ready(void) const {return(port.key.maping);};

	virtual bool connect(const CString &dName,int high_low, int Parity = NOPARITY);
	virtual void disconnect(void);

	// for str rx and tx
	virtual bool send(const char *str,unsigned int len = 0,unsigned PurgeRx = 100);
	virtual bool recive(char *d_str,unsigned int d_len,
			    unsigned long time = 2000,unsigned int d_len_max = 1024);
	virtual bool send_search_get(const char *s_str,unsigned int s_len,
				     char *d_str,unsigned int d_len,
				     const char *v_str=NULL,unsigned int v_len=0,
				     unsigned long time = 2000,unsigned PurgeRx = 100);
	virtual bool send_and_search(const char *s_str,unsigned int s_len,
				     const char *d_str,unsigned int d_len,
				     unsigned long time = 2000,unsigned PurgeRx = 100);
	virtual bool send_and_get(const char *s_str,unsigned int s_len,
				  char *d_str,unsigned int d_len,
				  unsigned long time = 2000,unsigned PurgeRx = 100);
	virtual bool send_and_verify(const char *s_str,unsigned int s_len,
				     const char *d_str,unsigned int d_len,
				     unsigned long time = 2000,unsigned PurgeRx = 100);

	virtual bool send_search_get(const char *s_str,unsigned int s_len,
				     unsigned long *d_data,unsigned int d_len,
				     const char *v_str=NULL,unsigned int v_len=0,
				     unsigned long time = 2000,unsigned PurgeRx = 100);
};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

class CBsDevice
{
private:
	CBsDevice& operator = (const CBsDevice &x);
	CBsDevice(CBsDevice &x);

protected:

public:
	virtual ~CBsDevice();
	//CBsDevice(const CString &Com,unsigned long id = 0xaa55);
	CBsDevice();

	CsrHandle_t iHandle;
	//struct bt_address_s addr;
	//struct bt_address_s r_addr;

	//struct bt_address_s last_use_addr;
	//struct bt_address_s *plast_use_addr;
	//class backup_pskey_contaner *backup_pskey;


	SPISource *spi_port;
	//RTL87XX_COM *com_port;
	//usb_dongle    *usb_dongle;
	//VSpecAnaly *PSpecAnaly;

	unsigned long dev_id;
	int dev_maped;
	CString info;
	CString info_1st;
	unsigned long WPwOnTime;
	unsigned long old_eep_uid;

	//bool file_load;
	//bool file_crc32_cal;
	//bool file_crc32_err;
	//unsigned long xpv_crc32;
	//unsigned long xdv_crc32;
	
	bool connect(const CString &dName);
	void disconnect(void);

	bool spi_allocated;
	bool is_dut_spi_allocated(void) { return(spi_allocated); };

	//bool gloden_spi_allocation;
	//bool is_gloden_spi_allocated(void) { return(gloden_spi_allocation); };
    
    CString errInfo(void);   

};





//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class GPIB_DEV
{
private:
	static bool DevRMCreated;
	static port_list<PortSource> *list_head;

	GPIB_DEV& operator = (const GPIB_DEV &x);
	GPIB_DEV(GPIB_DEV &x);
protected:
	static ViSession DEVRM;
	ViSession Session;
	CString Name;
	CString Address;
	bool Created;
	port_list<PortSource> port;

	int DevUsed(CString devAddr) {return(port_list<PortSource>::DevUsed(devAddr,list_head));};
	virtual int CreatSession(void);
	virtual int ReleaseSession(void);
public:
	GPIB_DEV(const CString &name,const CString &Addr = "",const CString &probe_name = "");
	virtual ~GPIB_DEV();

	virtual const CString &dev_name(void){return(port.key.DevName);};
	virtual const CString &port_name(void){return(port.key.PortName);};
	virtual const int &port_baud(void){return(port.key.Baud);};
	virtual const int &port_parity(void){return(port.key.Parity);};

	bool dev_is_ready(void) const {return(port.key.maping);};;
	virtual bool connect(void);
	bool creat_success(void) const {return(Created);};	
};

//-------------------------------------------------------------------------------------------------
// frequncey counter
//-------------------------------------------------------------------------------------------------

class FCOUNTER_DEV
{
private:
	FCOUNTER_DEV& operator = (const FCOUNTER_DEV &x);
	FCOUNTER_DEV(FCOUNTER_DEV &x);
	
protected:
	virtual void dev_paramter_ini(void);
public:
	int dev_statu;
	bool test_obj_is_bs;
	int old_test_obj_is_bs;
	class CTestItem *Tobj;

	bool parameter_can_use;
	unsigned long reset_device_time;
	unsigned long wait_data_time;

	virtual ~FCOUNTER_DEV();
	FCOUNTER_DEV(const CString &name);
	static bool creat_dev(const CString &name,class FCOUNTER_DEV *&dev);
	virtual const CString &dev_name(void) = 0;
	virtual const CString &port_name(void) = 0;
	virtual const int &port_baud(void) = 0;
	virtual const int &port_parity(void) = 0;

	virtual bool dev_is_ready(void) = 0;
	virtual bool connect(void) = 0;
	virtual bool ReadFrequency(double &data,class CTestItem *Tobj) = 0;
	void prompt_message(const CString &message,class CTestItem *Tobj);
};

//-------------------------------------------------------------------------------------------------
// bluetooth test set
//-------------------------------------------------------------------------------------------------
struct top_res
{
	bool valid;
	double av_max;
	double av_min;
	double av_pk;
	double av;
	double evm;
	int Failed;
	int Tested;
	bool res;
	bool is_hoping;
};
struct tic_res
{
	bool valid;
	double av;
	double pos_max;
	double neg_max;
	int Failed;
	int Tested;
	bool res;
	bool is_hoping;
};
struct tss_res
{
	bool valid;
	double Ber;
	double Fer;
	bool res;
	int fercrc;
	int ferlength;
	int ferlost;
	int packrx;
	int biterr;
	int frameerr;
	int packsend;
	bool is_hoping;
};

struct tSetResult
{
	struct top_res op[3];
	struct tic_res ic[3];
	struct tss_res ss[3];
};


struct ledRGB
{
   int R;
   int G;
   int B;
};

struct ledAdcResult
{
	struct ledRGB led_ad;
};

struct tSetResult_mt8852
{
        bool op_data_valid;
        bool op_res;
        double op_av;
        double op_av_max;
        double op_av_min;
        double op_av_pk;

        bool ic_data_valid;
        bool ic_res;
        double ic_foffset;
        double ic_av_offset;
        double ic_pos_offset;
        double ic_neg_offset;

        bool ss_data_valid;
        bool ss_res;
        double ss_cur_ber;
        double ss_overall_ber;
        double ss_cur_fer;
        double ss_overall_fer;
};



class C53131_DEV:public FCOUNTER_DEV,public GPIB_DEV
{
private:
	C53131_DEV& operator = (const C53131_DEV &x);
	C53131_DEV(C53131_DEV &x);
protected:
	void dev_paramter_ini(void);

public:
	C53131_DEV(const CString &name);
	virtual ~C53131_DEV();

	const CString &dev_name(void) { return(GPIB_DEV::dev_name()); };
	const CString &port_name(void) { return(GPIB_DEV::port_name()); };
	const int &port_baud(void) { return(GPIB_DEV::port_baud()); };
	const int &port_parity(void) { return(GPIB_DEV::port_parity()); };
	
	bool dev_is_ready(void){return(GPIB_DEV::dev_is_ready());};
	bool connect(void);
	bool ReadFrequency(double &data,class CTestItem *Tobj);
};

  
//-------------------------------------------------------------------------------------------------
class C53131A_DEV:public C53131_DEV
{
private:
	C53131A_DEV& operator = (const C53131A_DEV &x);
	C53131A_DEV(C53131A_DEV &x);
protected:
public:
	C53131A_DEV(const CString &name);
	virtual ~C53131A_DEV(){};
};
 
//-------------------------------------------------------------------------------------------------
class C53131B_DEV:public C53131_DEV
{
private:
	C53131B_DEV& operator = (const C53131B_DEV &x);
	C53131B_DEV(C53131B_DEV &x);
protected:
public:
	C53131B_DEV(const CString &name);
	virtual ~C53131B_DEV(){};
};

//-------------------------------------------------------------------------------------------------
// SpecAnaly
// get_measure_value(double &f,double &lv,int freqSampleNum = 8)
// f 	Hz
// lv 	dBm
//-------------------------------------------------------------------------------------------------
class VSpecAnaly
{
private:
	VSpecAnaly& operator = (const VSpecAnaly &x);
	VSpecAnaly(VSpecAnaly &x);
protected:
	double Span;
	unsigned long Mk_Hi_Time;
	double SweepTime;
	double Rband;
	double RLv;
	double centerF;

	virtual void dev_paramter_ini(void);
public:
	int dev_statu;
	bool test_obj_is_bs;
	int old_test_obj_is_bs;
	class CTestItem *Tobj;
	unsigned long reset_device_time;

	VSpecAnaly(const CString &name);
	virtual ~VSpecAnaly();

	static bool creat_dev(const CString &name,class VSpecAnaly *&dev);

	virtual const CString &dev_name(void) = 0;
	virtual const CString &port_name(void) = 0;
	virtual const int &port_baud(void) = 0;
	virtual const int &port_parity(void) = 0;

	virtual bool dev_is_ready(void) = 0;
	virtual bool connect(void) = 0;
	void prompt_message(const CString &message,class CTestItem *Tobj);

	virtual bool UpdateSpan(void){ return(SetSpan(Span)); };
	virtual bool UpdateRB(void){ return(SetRB(Rband)); };

	virtual bool SetRB(double Rband) = 0;
	virtual bool SetSpan(double Span=10,char SUnit = 'M') { return(false); };
	virtual bool SetLv(double Lv = 26) { return(false); };
	virtual bool SetCenterF(double freq=2400,char fUnit='M') = 0;
	virtual bool get_measure_value(double &f,double &lv,int freqSampleNum = 8) = 0;
	virtual bool get_measure_value2(double &f,double &lv,int freqSampleNum = 8) { f = 0;lv = 0; return(false); };
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class HP8563:public VSpecAnaly,public GPIB_DEV
{
private:
	HP8563& operator = (const HP8563 &x);
	HP8563(HP8563 &x);
	
protected:
	int channel;
	int antenna;
	CString dev_verify_str;
	unsigned long mark_cf_timer;
        
	void dev_paramter_ini(void);
	bool tryMarkPK2Center(void);
public:
	virtual ~HP8563();
	HP8563(const CString &name);

	const CString &dev_name(void) { return(GPIB_DEV::dev_name()); };
	const CString &port_name(void) { return(GPIB_DEV::port_name()); };
	const int &port_baud(void) { return(GPIB_DEV::port_baud()); };
	const int &port_parity(void) { return(GPIB_DEV::port_parity()); };

	bool dev_is_ready(void){return(GPIB_DEV::dev_is_ready());};
	bool connect(void);

	bool SetRB(double Rband);
	bool SetSpan(double Span=10,char SUnit = 'M');
	bool SetLv(double Lv = 26);
	bool SetCenterF(double freq=2400,char fUnit='M');
	bool get_measure_value(double &f,double &lv,int freqSampleNum = 8);
	bool get_measure_value2(double &f,double &lv,int freqSampleNum = 8);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class HP8590E:public VSpecAnaly,public GPIB_DEV
{
private:
	HP8590E& operator = (const HP8590E &x);
	HP8590E(HP8590E &x);
	
protected:
	int channel;
	int antenna;
	CString dev_verify_str;
        
	void dev_paramter_ini(void);
	bool tryMarkPK2Center(void);
public:
	virtual ~HP8590E();
	HP8590E(const CString &name);

	const CString &dev_name(void) { return(GPIB_DEV::dev_name()); };
	const CString &port_name(void) { return(GPIB_DEV::port_name()); };
	const int &port_baud(void) { return(GPIB_DEV::port_baud()); };
	const int &port_parity(void) { return(GPIB_DEV::port_parity()); };

	bool dev_is_ready(void){return(GPIB_DEV::dev_is_ready());};
	bool connect(void);

	bool SetRB(double Rband);
	bool SetSpan(double Span=10,char SUnit = 'M');
	bool SetLv(double Lv = 26);
	bool SetCenterF(double freq=2400,char fUnit='M');
	bool get_measure_value(double &f,double &lv,int freqSampleNum = 8);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class MT8810C:public VSpecAnaly,public GPIB_DEV
{
private:
	MT8810C& operator = (const MT8810C &x);
	MT8810C(MT8810C &x);
	
protected:
	int channel;
	int antenna;
	CString dev_verify_str;
        
	void dev_paramter_ini(void);
	bool tryMarkPK2Center(void);
public:
	virtual ~MT8810C();
	MT8810C(const CString &name);

	const CString &dev_name(void) { return(GPIB_DEV::dev_name()); };
	const CString &port_name(void) { return(GPIB_DEV::port_name()); };
	const int &port_baud(void) { return(GPIB_DEV::port_baud()); };
	const int &port_parity(void) { return(GPIB_DEV::port_parity()); };

	bool dev_is_ready(void){return(GPIB_DEV::dev_is_ready());};
	bool connect(void);

	bool SetRB(double Rband);
	bool SetSpan(double Span=10,char SUnit = 'M');
	bool SetLv(double Lv = 26);
	bool SetCenterF(double freq=2400,char fUnit='M');
	bool get_measure_value(double &f,double &lv,int freqSampleNum = 8);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class R4131:public VSpecAnaly,public GPIB_DEV
{
private:
	R4131& operator = (const R4131 &x);
	R4131(R4131 &x);
	
protected:
	int channel;
	int antenna;
	CString dev_verify_str;
        
	void dev_paramter_ini(void);
	bool tryMarkPK2Center(void);
public:
	virtual ~R4131();
	R4131(const CString &name);

	const CString &dev_name(void) { return(GPIB_DEV::dev_name()); };
	const CString &port_name(void) { return(GPIB_DEV::port_name()); };
	const int &port_baud(void) { return(GPIB_DEV::port_baud()); };
	const int &port_parity(void) { return(GPIB_DEV::port_parity()); };

	bool dev_is_ready(void){return(GPIB_DEV::dev_is_ready());};
	bool connect(void);

	bool SetRB(double Rband);
	bool SetSpan(double Span=10,char SUnit = 'M');
	bool SetLv(double Lv = 26);
	bool SetCenterF(double freq=2400,char fUnit='M');
	bool get_measure_value(double &f,double &lv,int freqSampleNum = 8);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class R3131:public VSpecAnaly,public GPIB_DEV
{
private:
	R3131& operator = (const R3131 &x);
	R3131(R3131 &x);
	
protected:
	int channel;
	int antenna;
	CString dev_verify_str;
        
	void dev_paramter_ini(void);
	bool tryMarkPK2Center(void);
public:
	virtual ~R3131();
	R3131(const CString &name);

	const CString &dev_name(void) { return(GPIB_DEV::dev_name()); };
	const CString &port_name(void) { return(GPIB_DEV::port_name()); };
	const int &port_baud(void) { return(GPIB_DEV::port_baud()); };
	const int &port_parity(void) { return(GPIB_DEV::port_parity()); };

	bool dev_is_ready(void){return(GPIB_DEV::dev_is_ready());};
	bool connect(void);

	bool SetRB(double Rband);
	bool SetSpan(double Span=10,char SUnit = 'M');
	bool SetLv(double Lv = 26);
	bool SetCenterF(double freq=2400,char fUnit='M');
	bool get_measure_value(double &f,double &lv,int freqSampleNum = 8);
	bool get_measure_value2(double &f,double &lv,int freqSampleNum = 8);
};


//      creat_test_f("OPCFG",op_lf_sel,L,"L");
//--------------------------------------------------------------------------------------------------------------------------
#define	creat_test_f(cmd,item_sel,item,item_v)	do {					                \
        if(item_sel) {                                                                                  \
                Temp.Format(cmd" %d,"item_v"RXFREQ,FREQ,%.3lfe+009\n",script,item##RxF/1000);           \
                if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;              \
                Temp.Format(cmd" %d,"item_v"TXFREQ,FREQ,%.3lfe+009\n",script,item##TxF/1000);           \
                if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;              \
                Temp.Format(cmd" %d,"item_v"FREQSEL,ON\n",script);                                      \
                if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;              \
        } else {                                                                                        \
                Temp.Format(cmd" %d,"item_v"FREQSEL,OFF\n",script);                                     \
                if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;              \
        }                                                                                               \
}while(0)


#define chl_item_set(test_sel,op_sel,ic_sel,ss_sel) do {                                         	\
	if(test_sel && op_sel) {Temp.Format("OPCFG %d,LFREQSEL,ON\n",script); op_test = true;}		\
	else Temp.Format("OPCFG %d,LFREQSEL,OFF\n",script);						\
	viPrintf(Session,Temp.GetBuffer(0));								\
	if(test_sel && ic_sel) {Temp.Format("ICCFG %d,LFREQSEL,ON\n",script); ic_test = true;}		\
	else Temp.Format("ICCFG %d,LFREQSEL,OFF\n",script);						\
	viPrintf(Session,Temp.GetBuffer(0));								\
	if(test_sel && ss_sel) {Temp.Format("SSCFG %d,LFREQSEL,ON\n",script); ss_test = true;}		\
	else Temp.Format("SSCFG %d,LFREQSEL,OFF\n",script);						\
	viPrintf(Session,Temp.GetBuffer(0));								\
}while(0)


#define chm_item_set(test_sel,op_sel,ic_sel,ss_sel) do {                                         	\
	if(test_sel && op_sel) {Temp.Format("OPCFG %d,MFREQSEL,ON\n",script); op_test = true;}		\
	else Temp.Format("OPCFG %d,MFREQSEL,OFF\n",script);						\
	viPrintf(Session,Temp.GetBuffer(0));								\
	if(test_sel && ic_sel) {Temp.Format("ICCFG %d,MFREQSEL,ON\n",script); ic_test = true;}		\
	else Temp.Format("ICCFG %d,MFREQSEL,OFF\n",script);						\
	viPrintf(Session,Temp.GetBuffer(0));								\
	if(test_sel && ss_sel) {Temp.Format("SSCFG %d,MFREQSEL,ON\n",script); ss_test = true;}		\
	else Temp.Format("SSCFG %d,MFREQSEL,OFF\n",script);					\
	viPrintf(Session,Temp.GetBuffer(0));								\
}while(0)


#define chh_item_set(test_sel,op_sel,ic_sel,ss_sel) do {                                         	\
	if(test_sel && op_sel) {Temp.Format("OPCFG %d,HFREQSEL,ON\n",script); op_test = true;}		\
	else Temp.Format("OPCFG %d,HFREQSEL,OFF\n",script);						\
	viPrintf(Session,Temp.GetBuffer(0));								\
	if(test_sel && ic_sel) {Temp.Format("ICCFG %d,HFREQSEL,ON\n",script); ic_test = true;}		\
	else Temp.Format("ICCFG %d,HFREQSEL,OFF\n",script);						\
	viPrintf(Session,Temp.GetBuffer(0));								\
	if(test_sel && ss_sel) {Temp.Format("SSCFG %d,HFREQSEL,ON\n",script); ss_test = true;}		\
	else Temp.Format("SSCFG %d,HFREQSEL,OFF\n",script);						\
	viPrintf(Session,Temp.GetBuffer(0));								\
}while(0)


#define test_content_set() do {										\
	if(op_test) Temp.Format("SCPTCFG %d,OP,ON\n",script);						\
	else Temp.Format("SCPTCFG %d,OP,OFF\n",script);							\
	viPrintf(Session,Temp.GetBuffer(0));								\
	if(ic_test) Temp.Format("SCPTCFG %d,IC,ON\n",script);						\
	else Temp.Format("SCPTCFG %d,IC,OFF\n",script);							\
	viPrintf(Session,Temp.GetBuffer(0));								\
	if(ss_test) Temp.Format("SCPTCFG %d,SS,ON\n",script);						\
	else Temp.Format("SCPTCFG %d,SS,OFF\n",script);							\
	viPrintf(Session,Temp.GetBuffer(0));								\
}while(0)

//--------------------------------------------------------------------------------------------------------------------------
class bt_TSET
{
private:
	bt_TSET& operator = (const bt_TSET &x);
	bt_TSET(bt_TSET &x);
protected:
	int dev_statu;
	int query(ViSession Session,CString cmd,char *buf,int len);
	virtual void dev_paramter_ini(void);
public:
	unsigned long idle_timer;
public:
	bt_TSET(const CString &name);
	virtual ~bt_TSET();
	
	static bool creat_dev(const CString &name,class bt_TSET *&dev);
	void prompt_message(const CString &message,class CTestItem *Tobj);
		
	virtual const CString &dev_name(void) = 0;
	virtual const CString &port_name(void) = 0;
	
	//add rf_pwr_cal
	virtual void hopping_st(bool &op_hopping,bool &ic_hopping,bool &ss_hopping) = 0;
	virtual bool dev_is_ready(void) = 0;
	virtual bool connect(void) = 0;
	
	virtual bool setPathOffset(double pathoffset) = 0;
	//virtual bool setDutPar(CDutGlodenPar *par) = 0;
	//virtual bool run(const CString& addr,class CTestItem *Tobj,struct tSetResult_& tresult) = 0;
	//virtual bool run(const CString& addr,class CTestItem *Tobj) = 0;
	virtual bool run2(const CString& addr,struct tSetResult &mt8852_res,class CTestItem *Tobj) = 0;
	virtual bool run(const CString& addr,struct tSetResult &mt8852_res,class CTestItem *Tobj) = 0;
	//virtual bool run3(const CString& addr,class CTestItem *Tobj,struct tSetResult_mt8852 &mt8852_res) = 0;
};



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class MT8852A_DEV:public bt_TSET,public GPIB_DEV
{
private:
	MT8852A_DEV& operator = (const MT8852A_DEV &x);
	MT8852A_DEV(MT8852A_DEV &x);
	
protected:
        unsigned long reset_device_time;
        unsigned long max_test_time;
		unsigned long discn_dut_time;

        unsigned long script;
        double pathOffset;
        double txPower;
        double freq_offset;


        double LRxF;
        double LTxF;
        double MRxF;
        double MTxF;
        double HRxF;
        double HTxF;

        bool op_lf_sel;
        bool op_mf_sel;
        bool op_hf_sel;
	bool op_is_hopping;
        CString op_test_type;
        CString op_hopping;
        unsigned long op_num_packets;
        double pwr_avmin;
        double pwr_avmax;
        double pwr_peak;

        bool ic_lf_sel;
        bool ic_mf_sel;
        bool ic_hf_sel;
		bool ic_is_hopping;
        CString ic_test_type;
        CString ic_hopping;
        unsigned long ic_num_packets;
        double foffset_pmax;
        double foffset_nmax;

        bool ss_lf_sel;
        bool ss_mf_sel;
        bool ss_hf_sel;
		bool ss_is_hopping;
        CString ss_hopping;
        unsigned long ss_num_packets;
        double ss_tx_pwr;
        double ss_ber_max;
        double ss_fer_max;

		CDutGlodenPar *dut_par;
	
		//bool get_result_lch(struct tSetResult& tresult);
        //bool get_result_mch(struct tSetResult& tresult);
	    //bool get_result_hch(struct tSetResult& tresult);
	bool get_result_xch(struct tSetResult& tresult,int ch);
	bool do_test(class CTestItem *Tobj,unsigned long delay = 1500);

     
	void dev_paramter_ini(void);
public:
	virtual ~MT8852A_DEV();
	MT8852A_DEV(const CString &name);

	const CString &dev_name(void) { return(GPIB_DEV::dev_name()); };
	const CString &port_name(void) { return(GPIB_DEV::port_name()); };
	
    //add rf_pwr_cal
	void hopping_st(bool &op_hopping,bool &ic_hopping,bool &ss_hopping) 
	{
		op_hopping = op_is_hopping;
		ic_hopping = ic_is_hopping;
		ss_hopping = ss_is_hopping;
	};


	bool dev_is_ready(void){return(GPIB_DEV::dev_is_ready());};
	bool connect(void);

    bool setPathOffset(double pathoffset);
	bool setDutPar(CDutGlodenPar *par);
	//bool run(const CString& addr,class CTestItem *Tobj,struct tSetResult_& tresult);
	//bool run(const CString& addr,class CTestItem *Tobj);
	bool run(const CString& addr,struct tSetResult &mt8852_res,class CTestItem *Tobj);
	bool run2(const CString& addr,struct tSetResult &mt8852_res,class CTestItem *Tobj);
	//bool run3(const CString& addr,class CTestItem *Tobj,struct tSetResult_mt8852& tresult);
	void disconnect_dut(void);
};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

#if 0
class MT8852B_DEV:public bt_TSET,public GPIB_DEV
{
private:
	MT8852B_DEV& operator = (const MT8852B_DEV &x);
	MT8852B_DEV(MT8852B_DEV &x);
	
protected:
        unsigned long reset_device_time;
        unsigned long max_test_time;

        unsigned long script;
        double pathOffset;
        double txPower;
        double LRxF;
        double LTxF;
        double MRxF;
        double MTxF;
        double HRxF;
        double HTxF;

        bool op_lf_sel;
        bool op_mf_sel;
        bool op_hf_sel;
        CString op_test_type;
        CString op_hopping;
        unsigned long op_num_packets;
        double pwr_avmin;
        double pwr_avmax;
        double pwr_peak;

        bool ic_lf_sel;
        bool ic_mf_sel;
        bool ic_hf_sel;
        CString ic_test_type;
        CString ic_hopping;
        unsigned long ic_num_packets;
        double foffset_pmax;
        double foffset_nmax;

        bool ss_lf_sel;
        bool ss_mf_sel;
        bool ss_hf_sel;
        CString ss_hopping;
        unsigned long ss_num_packets;
        double ss_tx_pwr;
        double ss_ber_max;
        double ss_fer_max;
     
		CDutGlodenPar *dut_par;

    bool get_result_xch(struct tSetResult& tresult,int ch);
	//bool get_result_lch(struct tSetResult& tresult);
	//bool get_result_mch(struct tSetResult& tresult);
	//bool get_result_hch(struct tSetResult& tresult);
	bool do_test(class CTestItem *Tobj,unsigned long delay = 1500);

	void dev_paramter_ini(void);
public:
	virtual ~MT8852B_DEV();
	MT8852B_DEV(const CString &name);

	const CString &dev_name(void) { return(GPIB_DEV::dev_name()); };
	const CString &port_name(void) { return(GPIB_DEV::port_name()); };
	
	bool dev_is_ready(void){return(GPIB_DEV::dev_is_ready()); };
	bool connect(void);

        bool setPathOffset(double pathoffset);
	bool setDutPar(CDutGlodenPar *par);
	//bool run(const CString& addr,class CTestItem *Tobj,struct tSetResult_& tresult);
	bool run(const CString& addr,class CTestItem *Tobj);
};

#endif



#if 0

class MT8852B_DEV:public MT8852A_DEV
{
private:
	MT8852B_DEV& operator = (const MT8852B_DEV &x);
	MT8852B_DEV(MT8852B_DEV &x);

	void dev_paramter_ini(void);
public:
	virtual ~MT8852B_DEV();
	MT8852B_DEV(const CString &name);

	bool connect(void);
};

#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class N4010A_DEV:public bt_TSET,public GPIB_DEV
{
private:
	N4010A_DEV& operator = (const N4010A_DEV &x);
	N4010A_DEV(N4010A_DEV &x);
	
protected:
	unsigned long reset_device_time;
	unsigned long max_test_time;
	unsigned long discn_dut_time;
	
	unsigned long script;
	double pathOffset;
	double txPower;
	double freq_offset;
	double CentFreq;//20171219
	
	double LRxF;
	double LTxF;
	double MRxF;
	double MTxF;
	double HRxF;
	double HTxF;
	
	bool op_lf_sel;
	bool op_mf_sel;
	bool op_hf_sel;
	bool op_is_hopping;
	CString op_test_type;
	CString op_hopping;
	unsigned long op_num_packets;
	double pwr_avmin;
	double pwr_avmax;
	double pwr_peak;
	
	bool ic_lf_sel;
	bool ic_mf_sel;
	bool ic_hf_sel;
	bool ic_is_hopping;
	CString ic_test_type;
	CString ic_hopping;
	unsigned long ic_num_packets;
	double foffset_pmax;
	double foffset_nmax;
	
	bool ss_lf_sel;
	bool ss_mf_sel;
	bool ss_hf_sel;
	bool ss_is_hopping;
	CString ss_hopping;
	unsigned long ss_num_packets;
	double ss_tx_pwr;
	double ss_ber_max;
	double ss_fer_max;
	
	CDutGlodenPar *dut_par;
	
	//bool get_result_lch(struct tSetResult& tresult);
	//bool get_result_mch(struct tSetResult& tresult);
	//bool get_result_hch(struct tSetResult& tresult);
	bool get_result_xch(struct tSetResult& tresult,int ch);
	bool do_test(class CTestItem *Tobj,unsigned long delay = 1500);
	
	
	void dev_paramter_ini(void);
public:
	
	//double CentFreq;//20171219
	virtual ~N4010A_DEV();
	N4010A_DEV(const CString &name);
	
	const CString &dev_name(void) { return(GPIB_DEV::dev_name()); };
	const CString &port_name(void) { return(GPIB_DEV::port_name()); };
	
	//add rf_pwr_cal
	void hopping_st(bool &op_hopping,bool &ic_hopping,bool &ss_hopping) 
	{
		op_hopping = op_is_hopping;
		ic_hopping = ic_is_hopping;
		ss_hopping = ss_is_hopping;
	};
	
	
	bool dev_is_ready(void){return(GPIB_DEV::dev_is_ready());};
	bool connect(void);
	
    bool setPathOffset(double pathoffset);
	
	int N4010A_DEV::set_rang(int rang,int &flag);
	int N4010A_DEV::set_rang2(int rang);
	int N4010A_DEV::set_rang3(double level,int rang);
	int N4010A_DEV::run_11a_g(double freq,int &range);
	bool setDutPar(CDutGlodenPar *par);
	//bool run(const CString& addr,class CTestItem *Tobj,struct tSetResult_& tresult);
	//bool run(const CString& addr,class CTestItem *Tobj);
	bool run(const CString& addr,struct tSetResult &mt8852_res,class CTestItem *Tobj);
	bool run2(const CString& addr,struct tSetResult &mt8852_res,class CTestItem *Tobj);
	//bool run3(const CString& addr,class CTestItem *Tobj,struct tSetResult_mt8852& tresult);
	void disconnect_dut(void);
	
	bool run3();
	bool stop();


	int setCentFre(double freq);
	bool setBand(double band);
	bool setInputPow(double pow);
	bool setAcqTime(double time);	//unit	us
	bool setInstMode(unsigned char mode);// 0-Link	1-RF Analyzer	2-RF Generator
	bool ReadRFFre(double &freqoffset);
	bool ReadRFAPower(double &pow);
	bool StartRFTX(int ch,double power);
};
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class RTL87XX_COM:public CComDevice
{
private:
	RTL87XX_COM& operator = (const RTL87XX_COM &x);
	RTL87XX_COM(RTL87XX_COM &x);
protected:
	bool DevValid(int high_low);
	unsigned long max_time;
public:
	virtual ~RTL87XX_COM(){};
	RTL87XX_COM(const CString &Com);
	//UM0155_page9
	//测试完成后更新的数据，需写入rtl8711
	//uint8 state;
	uint8 x_tal;
	//0x2B地址值
	uint8 tx_pwr_index_difference;
	
	uint8 tx_pwr_l;
	uint8 tx_pwr_m;
	uint8 tx_pwr_h;
	
	uint8 channel_plan;
	uint8 thermal_meter;
	//bool connect(const CString &dName, int port_baud, int Parity);
	//bool connect(const CString &dName, int high_low, int port_baud, int Parity);
};
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTBOX:public CComDevice
{
private:
	CTBOX& operator = (const CTBOX &x);
	CTBOX(CTBOX &x);
protected:
	//bool DevValid(void);
     bool DevValid(int);
        unsigned long max_time;
        bool get_st(bool &is_open);
public:
	virtual ~CTBOX(){};
	CTBOX(const CString &Com);

	bool connect(void);

        bool open_box(void);
        bool close_box(void);
};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CMultimeter
{
private:
	CMultimeter& operator = (const CMultimeter &x);
	CMultimeter(CMultimeter &x);
	
	bool openCom(void);
public:
	bool com_allocation;
	int com;
	port_list<PortSource> port;
	
	HANDLE 	hCom;
	bool thread_running;
	CWinThread *pWinThread;
	
	bool id_is_ok;
	CRITICAL_SECTION g_cs;
	volatile int measure_type;
	volatile double tvalue;
public:
	virtual ~CMultimeter();
	CMultimeter(int Com);
	
	bool recive_by_length(char *str,DWORD &len);
	int analyze(char *srt);
	
	//bool get_voltage(double &data);
	bool get_voltage(double *data);
	bool get_current(double &data);
	
	bool connect(void);
	void disconnect(void);
};


CMultimeter *getMultimeterDev(void);
enum
{
	Multimeter_voltage = 0,
        Multimeter_current,
        Multimeter_invalid
};
#define MULMETER_OVERFLOW       (9000*1000)



//-------------------------------------------------------------------------------------------------
//mcu serial com
//-------------------------------------------------------------------------------------------------
class CMCU_COM:public CComDevice
{
private:
	CMCU_COM& operator = (const CMCU_COM &x);
	CMCU_COM(CMCU_COM &x);
protected:
	bool DevValid(int high_low);
	bool connect_(const CString &dName,int high_low, int Parity = NOPARITY);


public:
	virtual ~CMCU_COM(){};
	CMCU_COM(const CString &Com);

	//bool Dev_Valid(int high_low);
	//bool connect_(const CString &dName,int high_low, int Parity = NOPARITY);

    bool mcu_deal_cmd(int mcu_id,char pin_grp, int pin_num, int high_low);
	
	//bool mcu_gpio_set_wr_cmd(const CString cmd);
	//bool mcu_gpio_rd_cmd(const CString cmd, char *rx_buf);
	//bool mcu_set_route_mt8852(int dev_id,bool &rout_sw);     /*mt8852 test*/
	//bool mcu_set_route_freqoffset(const CString cmd);        /*freq offset test*/

    


	bool set_test_board_short_pc_on(int mcu_id,char pin_grp, int pin_num, int high_low=1);
    bool set_test_board_short_pc_off(int mcu_id,char pin_grp, int pin_num, int high_low=0); 

    //dut1 && dut2
	bool set_dut_5v_on(int mcu_id,char pin_grp, int pin_num, int high_low=1);
	bool set_dut_5v_off(int mcu_id,char pin_grp, int pin_num, int high_low=0);

	bool set_dut_bat_on(int mcu_id,char pin_grp, int pin_num, int high_low=1);
	bool set_dut_bat_off(int mcu_id,char pin_grp, int pin_num, int high_low=0);
	
	bool set_dut_led_on(int mcu_id,char pin_grp, int pin_num, int high_low=1);
	bool set_dut_led_off(int mcu_id,char pin_grp, int pin_num, int high_low=0);

	bool set_dut_vreg_on(int mcu_id,char pin_grp, int pin_num, int high_low=1);
	bool set_dut_vreg_off(int mcu_id,char pin_grp, int pin_num, int high_low=0);

	bool set_dut_spi_on(int mcu_id,char pin_grp, int pin_num, int high_low=0);
	bool set_dut_spi_off(int mcu_id,char pin_grp, int pin_num, int high_low=1);

	bool set_dut_current_on(int mcu_id,char pin_grp, int pin_num, int high_low=1);
	bool set_dut_current_off(int mcu_id,char pin_grp, int pin_num, int high_low=0);

	bool set_dut_pwr_on(int mcu_id,char pin_grp, int pin_num, int high_low=1);
	bool set_dut_pwr_off(int mcu_id,char pin_grp, int pin_num, int high_low=0);

	bool set_dut2_rout_meter_on(int mcu_id,char pin_grp, int pin_num, int high_low=1);
	bool set_dut1_rout_meter_on(int mcu_id,char pin_grp, int pin_num, int high_low=0);

    bool set_dut_spk_amp_on(int mcu_id,char pin_grp, int pin_num, int high_low=0);
	bool set_dut_spk_amp_off(int mcu_id,char pin_grp, int pin_num, int high_low=1);

	bool set_dut2_rout_audio_on(int mcu_id,char pin_grp, int pin_num, int high_low=1);
	bool set_dut1_rout_audio_on(int mcu_id,char pin_grp, int pin_num, int high_low=0);

    //8852 test and current test cmd
    bool set_rout_8852_on(int mcu_id,char pin_grp, int pin_num, int high_low=0);
	bool set_rout_freq_on(int mcu_id,char pin_grp, int pin_num, int high_low=1);
	
	bool set_rout_4010_on(int mcu_id,char pin_grp, int pin_num, int high_low=0);

	bool set_4010_route_dut2(int mcu_id,char pin_grp, int pin_num, int high_low=0);
	bool set_4010_route_dut1(int mcu_id,char pin_grp, int pin_num, int high_low=1);

    bool set_8852_route_dut2(int mcu_id,char pin_grp, int pin_num, int high_low=0);
	bool set_8852_route_dut1(int mcu_id,char pin_grp, int pin_num, int high_low=1);
	

	bool set_freq_route_dut2(int mcu_id,char pin_grp, int pin_num, int high_low=0);
	bool set_freq_route_dut1(int mcu_id,char pin_grp, int pin_num, int high_low=1);
   
    //read led sense cmd
	bool get_dut_rgb(int mcu_id,char pin_grp, int pin_num, char *rx_buf);

	//motor cmd
	bool set_motor_down(int mcu_id,char pin_grp, int pin_num, int high_low=1);
	bool set_motor_up(int mcu_id,char pin_grp, int pin_num, int high_low=0);


	//led indication cmd
    bool set_LED_flash(int mcu_id,  char pin_grp, int pin_num, unsigned long delay);
    bool set_LED_on(int mcu_id, char pin_grp, int pin_num, int high_low=1);
	bool set_LED_off(int mcu_id, char pin_grp, int pin_num, int high_low=0);
};



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

#if 0

class CIOCARD
{
private:
	CIOCARD& operator = (const CIOCARD &x);
	CIOCARD(CIOCARD &x);
protected:
	HANDLE hd6652;
	int DO_L0_7;
	int DO_H8_15;
	
	bool pwr_is_vbus;
	bool pwr_source_is_vbus;
	
	bool set_route_vbus(void);
	bool set_route_bat(void);
public:
	virtual ~CIOCARD();
	CIOCARD(void);
	
	int connect(void);
	
	bool get_value(long &data);
	bool open_dut_pwr(void);
	bool shutdown_dut_pwr(void);
	bool set_route_mt8852(int dev_id);
	bool set_route_foffset(int dev_id);
	bool set_route_audio(int dev_id);
	
	bool set_route_current(int dev_id);
	bool set_route_ag_pwr(int dev_id,bool on_off);
	
	bool reset_golden;
};

#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#define	DEV_MAP_I2C_BUS1		0
#define	DEV_MAP_I2C_BUS2		1
#define	DEV_NO_MAP_I2C_BUS		2
#define	DEV_USE_I2C_BUS			DEV_MAP_I2C_BUS2


#define	FILE_NAME_IS_NULL		2
#define	FILE_OPEN_ERR			3












#endif //__NENDEVICE_H__
