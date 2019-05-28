/*	TestItem.h
 */
#ifndef __TESTITEM_H__
#define	__TESTITEM_H__
#include "CommHead.h"
#include "tset.h"
class CTestItem;
class ConfigFile;
class CPD218Dlg;
class ofstream;
class CSortListCtrl;
//class CDutDevice;
class CBsDevice;
struct bt_address_s;

class testSet;

#define NO_ERR                                  1
#define ERR_MIN                                 0x0100
#define ERR_MAX                                 0x0200
#define ERR_NO_RUN                              ERR_MIN
#define ERR_TERMINAL                            (ERR_NO_RUN+1)
#define ERR_NOMAL                               (ERR_TERMINAL+1)
#define ERR_STOP		                        (ERR_NOMAL+1)
#define ERR_MT8852_OP		                    (ERR_STOP+1)
#define ERR_MT8852_DATA		                    (ERR_MT8852_OP+1)
#define ERR_TMODE                               (ERR_MT8852_DATA+1)
#define ERR_AUDIO                               (ERR_TMODE+1)
#define ERR_TMODE_END                           ERR_AUDIO

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

//LED_DEBUG
#define LED_xDEBUG_TO_TEST
#define LED_xDEBUG_TESTING
#define LED_xDEBUG_TESTED

//MCU U5 pio<-->dut
#define MCU_PIO_DUT1_5V              'E'
#define MCU_PIO_NUM_DUT1_5V           7

#define MCU_PIO_DUT2_BATTERY         'E'
#define MCU_PIO_NUM_DUT2_BATTERY     8

#define MCU_PIO_LED                  'E'
#define MCU_PIO_NUM_LED              9

#define MCU_PIO_DUT1_VREG            'E'
#define MCU_PIO_NUM_DUT1_VREG        10

#define MCU_PIO_SPI                  'E'
#define MCU_PIO_NUM_SPI               11

#define MCU_PIO_DUT1_CURRENT          'E'
#define MCU_PIO_NUM_DUT1_CURRENT      12

#define MCU_PIO_DUT1_BATTERY          'E'
#define MCU_PIO_NUM_DUT1_BATTERY      13

#define MCU_PIO_DUT_PWR               'E'
#define MCU_PIO_NUM_DUT_PWR           14

#define MCU_PIO_DUT2_METER            'E'
#define MCU_PIO_NUM_DUT2_METER        15

//MCU U18 pio<-->dut
#define MCU_PIO_DUT2_5V                'E'
#define MCU_PIO_NUM_DUT2_5V             8

#define MCU_PIO_DUT2_CURRENT           'E'
#define MCU_PIO_NUM_DUT2_CURRENT        9

#define MCU_PIO_DUT2_VREG              'E'
#define MCU_PIO_NUM_DUT2_VREG          10

#define MCU_PIO_DUT_1V8               'D'
#define MCU_PIO_NUM_DUT_1V8            9

#define MCU_PIO_DUT_SPK_AMP            'E'
#define MCU_PIO_NUM_DUT_SPK_AMP        11

#define MCU_PIO_DUT2_RF_ROUTE          'E'
#define MCU_PIO_NUM_DUT2_RF_ROUTE      12

#define MCU_PIO_DUT_4010_METER         'E'
#define MCU_PIO_NUM_DUT_4010_METER     13

#define MCU_PIO_DUT2_AUDIO             'E'
#define MCU_PIO_NUM_DUT2_AUDIO         14


//led indication
#define MCU_PIO_RED_LED                'B'
#define MCU_PIO_NUM_RED_LED            13

#define MCU_PIO_GREEN_LED              'B'
#define MCU_PIO_NUM_GREEN_LED          14

#define MCU_PIO_BLUE_LED               'B'
#define MCU_PIO_NUM_BLUE_LED           15

//audio chk
#define MCU_PIO_AUDIO_CHK              'E'
#define MCU_PIO_NUM_AUDIO_CHK          15

//DUT1 led color chk
#define MCU_PIO_DUT1_RED_0               'A'
#define MCU_PIO_NUM_DUT1_RED_0            6

#define MCU_PIO_DUT1_GREEN_0             'A'
#define MCU_PIO_NUM_DUT1_GREEN_0          5

#define MCU_PIO_DUT1_BLUE_0              'A'
#define MCU_PIO_NUM_DUT1_BLUE_0           4


#define MCU_PIO_DUT1_RED_1               'A'
#define MCU_PIO_NUM_DUT1_RED_1            3

#define MCU_PIO_DUT1_GREEN_1             'A'
#define MCU_PIO_NUM_DUT1_GREEN_1          2

#define MCU_PIO_DUT1_BLUE_1              'A'
#define MCU_PIO_NUM_DUT1_BLUE_1           1

#define MCU_PIO_DUT1_RED_2               'C'
#define MCU_PIO_NUM_DUT1_RED_2            3

#define MCU_PIO_DUT1_GREEN_2             'C'
#define MCU_PIO_NUM_DUT1_GREEN_2          2

#define MCU_PIO_DUT1_BLUE_2              'C'
#define MCU_PIO_NUM_DUT1_BLUE_2           1


//DUT2 led color chk
#define MCU_PIO_DUT2_RED_0               'A'
#define MCU_PIO_NUM_DUT2_RED_0            6

#define MCU_PIO_DUT2_GREEN_0             'A'
#define MCU_PIO_NUM_DUT2_GREEN_0          5

#define MCU_PIO_DUT2_BLUE_0              'A'
#define MCU_PIO_NUM_DUT2_BLUE_0           4


#define MCU_PIO_DUT2_RED_1               'A'
#define MCU_PIO_NUM_DUT2_RED_1            3

#define MCU_PIO_DUT2_GREEN_1             'A'
#define MCU_PIO_NUM_DUT2_GREEN_1          2

#define MCU_PIO_DUT2_BLUE_1              'A'
#define MCU_PIO_NUM_DUT2_BLUE_1           1

#define MCU_PIO_DUT2_RED_2               'C'
#define MCU_PIO_NUM_DUT2_RED_2            3

#define MCU_PIO_DUT2_GREEN_2             'C'
#define MCU_PIO_NUM_DUT2_GREEN_2          2

#define MCU_PIO_DUT2_BLUE_2              'C'
#define MCU_PIO_NUM_DUT2_BLUE_2           1

//btn check
#define MCU_PIO_DUT1_BTN_1              'A'
#define MCU_PIO_NUM_DUT1_BTN_1           7

#define MCU_PIO_DUT1_BTN_2              'B'
#define MCU_PIO_NUM_DUT1_BTN_2           0

#define MCU_PIO_DUT1_BTN_3              'B'
#define MCU_PIO_NUM_DUT1_BTN_3           1

#define MCU_PIO_DUT2_BTN_1              'A'
#define MCU_PIO_NUM_DUT2_BTN_1           7

#define MCU_PIO_DUT2_BTN_2              'B'
#define MCU_PIO_NUM_DUT2_BTN_2           0

#define MCU_PIO_DUT2_BTN_3              'B'
#define MCU_PIO_NUM_DUT2_BTN_3           1



///////////CSR//////////////////

#define format_record_par(unformat_par) do {                            \
        unformat_par.Replace(","," ");                                  \
        unformat_par.Replace("\r","");                                  \
        unformat_par.Replace("\n","");                                  \
}while(0)

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTestItem
{
private:
	CTestItem& operator = (const CTestItem &x);
	CTestItem(CTestItem &x);
public:
	CdNode<CTestItem> dNode;
	static int ItemSum;
	static bool have_fail_item;
	static bool pwr_on_fail;
	static bool led0_port_update;
	static bool led1_port_update;
	static bool bt_address_used;
	
	/*fu add for save rf para*/
	static int fre_index[2];
	static int pwr_low_index[2];
	static int pwr_middle_index[2];
	static int pwr_high_index[2];
	static double freoffset_para[2];
	static double low_avpower[2];
	static double middle_avpower[2];
	static double high_avpower[2];
	static double low_average_evm[2];
	static double middle_average_evm[2];
	static double high_average_evm[2];
	/*fu add for save rf para*/
	
	int identifier;

	int err_min;
    int err_max;
	
	int index;
	CString name;
	bool result;
	CString param;
	CString NeedMode;
	unsigned long ItemTFMask;
	bool is_tested;
	bool terminate_test_if_err_accur;

	testSet *tset;
	CString item_range;

	CTestItem(const char* pname,testSet *Tset,int ID);
	virtual ~CTestItem() {--tset->tItemSum;};

    int Index(void) { return(identifier); }
	//virtual bool can_run_when_err(int err) { return ((err >= err_min) && (err <= err_max)); };
	virtual bool disable_run_when_err(void) {return (terminate_test_if_err_accur); };
    virtual void set_run_err_range(int ErrMin,int ErrMax) { err_min = ErrMin; err_max = ErrMax; };

	virtual void ClearResult(void);
	virtual int ItemTestReady(ConfigFile &cfg = pCfgf);
	virtual int ItemTestClean(ConfigFile &cfg = pCfgf);
    virtual int TestOp(ConfigFile &cfg = pCfgf);
	virtual void gen_prompt();
	virtual void show_message(void);
	virtual void update_range(void);

    virtual void record_header(ofstream &fs);
    virtual void record_result(ofstream &fs);
	virtual bool test_condition_verify(void) { return(!have_fail_item); };
	virtual void wait_continue(void);
};





//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_Battery_on:public CTestItem
{
private:
	CTObj_Battery_on& operator = (const CTObj_Battery_on &x);
	CTObj_Battery_on(CTObj_Battery_on &x);


public:
   CTObj_Battery_on(const char *pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
   int ItemTestReady(ConfigFile &cfg);
   int ItemTestClean(ConfigFile &cfg);
   bool mutex_hodle1;
   bool mutex_hodle2;

   virtual ~CTObj_Battery_on() {};
   void gen_prompt();

   int TestOp(ConfigFile &cfg = pCfgf);

};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

#if 1

class CTObj_Battery_off:public CTestItem
{
private:
	CTObj_Battery_off& operator = (const CTObj_Battery_off &x);
	//CTObj_Battery_off(CTObj_Battery_on &x);
	CTObj_Battery_off(CTObj_Battery_off &x);


public:
   CTObj_Battery_off(const char *pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
   int ItemTestReady(ConfigFile &cfg);
   int ItemTestClean(ConfigFile &cfg);
   bool mutex_hodle1;
   bool mutex_hodle2;

   void gen_prompt();
   virtual ~CTObj_Battery_off() {};

   int TestOp(ConfigFile &cfg = pCfgf);

};

#endif


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

#if 1

class CTObj_dut_into_mp_mode:public CTestItem
{
private:
	CTObj_dut_into_mp_mode& operator = (const CTObj_dut_into_mp_mode &x);
	CTObj_dut_into_mp_mode(CTObj_dut_into_mp_mode &x);


public:
   CTObj_dut_into_mp_mode(const char *pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
   bool mutex_hodle1;
   bool mutex_hodle2;

   void gen_prompt();
   //virtual CTObj_dut_into_mp_mode() {};
    int ItemTestReady(ConfigFile &cfg);
   int ItemTestClean(ConfigFile &cfg);

   int TestOp(ConfigFile &cfg = pCfgf);

};

#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

#if 1

class CTObj_dut_into_normal_mode:public CTestItem
{
private:
	CTObj_dut_into_normal_mode& operator = (const CTObj_dut_into_normal_mode &x);
	CTObj_dut_into_normal_mode(CTObj_dut_into_normal_mode &x);


public:
   CTObj_dut_into_normal_mode(const char *pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
   bool mutex_hodle1;
   bool mutex_hodle2;

   void gen_prompt();

   int TestOp(ConfigFile &cfg = pCfgf);

};

#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_Vreg_on:public CTestItem
{
private:
	CTObj_Vreg_on& operator = (const CTObj_Vreg_on &x);
	CTObj_Vreg_on(CTObj_Vreg_on &x);


public:
   CTObj_Vreg_on(const char *pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
   int ItemTestReady(ConfigFile &cfg);
   int ItemTestClean(ConfigFile &cfg);
   bool mutex_hodle2;
   bool mutex_hodle1;


   virtual ~CTObj_Vreg_on() {};
   void gen_prompt();

   int TestOp(ConfigFile &cfg = pCfgf);

};



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#if 0

class CTObj_Vreg_off:public CTestItem
{
   private:
	CTObj_Vreg_off& operator = (const CTObj_Vreg_off &x);
	CTObj_Vreg_off(CTObj_Vreg_off &x);

public:
	CTObj_Vreg_off(const char* pname, testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_Vreg_off() { };
	void gen_prompt();

	 int ItemTestReady(ConfigFile &cfg);
   int ItemTestClean(ConfigFile &cfg);
   bool mutex_hodle2;
   bool mutex_hodle1;

	int TestOp(ConfigFile &cfg = pCfgf);

};

#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_lock:public CTestItem
{
private:
	CTObj_lock& operator = (const CTObj_lock &x);
	CTObj_lock(CTObj_lock &x);
protected:

public:
	CTObj_lock(const char *pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
    virtual ~CTObj_lock() {};

	int TestOp(ConfigFile &cfg = pCfgf);

};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_unlock:public CTestItem
{
private:
	CTObj_unlock &operator = (const CTObj_unlock &x);
	CTObj_unlock(CTObj_unlock &x);
protected:

public:
	CTObj_unlock(const char *pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_unlock() {  };

	int TestOp(ConfigFile &cfg = pCfgf);

};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_delay:public CTestItem
{
private:
	CTObj_delay& operator = (const CTObj_delay &x);
	CTObj_delay(CTObj_delay &x);

protected:
	unsigned long delay_time;
	//bool mutex_hodle;

public:
	CTObj_delay(const char* pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_delay() {};

	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_conn_spi:public CTestItem
{
private:
	CTObj_conn_spi& operator = (const CTObj_conn_spi &x);
	CTObj_conn_spi(CTObj_conn_spi &x);

protected:
	unsigned long connect_times;
	//bool mutex_hodle;

public:
	CTObj_conn_spi(const char* pname,  testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_conn_spi() {};


	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_discn_spi:public CTestItem
{
private:
	CTObj_discn_spi& operator = (const CTObj_discn_spi &x);
	CTObj_discn_spi(CTObj_discn_spi &x);

protected:
	//bool mutex_hodle;

public:
	CTObj_discn_spi(const char* pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_discn_spi() {};


	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
};



//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

class CTObj_reconnect:public CTestItem
{
private:
	CTObj_reconnect& operator = (const CTObj_reconnect &x);
	CTObj_reconnect(CTObj_reconnect &x);
protected:

        bool reconnect(void);
public:
	CTObj_reconnect(const char* pname,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_reconnect() { };
};



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#if 0
class CTObj_verCheck:public CTestItem
{
private:
	CTObj_verCheck& operator = (const CTObj_verCheck &x);
	CTObj_verCheck(CTObj_verCheck &x);
protected:
	CString version;
	unsigned long ps_key;
	//bool mutex_hodle;

public:
	CTObj_verCheck(const char* pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_verCheck() {};


	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
};

#endif
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_psKeyCheck:public CTestItem
{
private:
	CTObj_psKeyCheck& operator = (const CTObj_psKeyCheck &x);
	CTObj_psKeyCheck(CTObj_psKeyCheck &x);
protected:
	CString key;
	unsigned long ps_key;
	bool is_string;
	//bool mutex_hodle;
public:
	CTObj_psKeyCheck(const char* pname, testSet *Tset, int ID, unsigned long ps_key,
			 unsigned long key_value,unsigned long type_string,
			 ConfigFile &cfg = pCfgf);
	virtual ~CTObj_psKeyCheck() {};


	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_read_bkpskey:public CTestItem
{
private:
	CTObj_read_bkpskey& operator = (const CTObj_read_bkpskey &x);
	CTObj_read_bkpskey(CTObj_read_bkpskey &x);
protected:
    //bool mutex_hodle;

public:
	CTObj_read_bkpskey(const char* pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_read_bkpskey() {};

	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_write_bkpskey:public CTestItem
{
private:
	CTObj_write_bkpskey& operator = (const CTObj_write_bkpskey &x);
	CTObj_write_bkpskey(CTObj_write_bkpskey &x);
protected:
    //bool mutex_hodle;

public:
	CTObj_write_bkpskey(const char* pname, testSet *Tset, int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_write_bkpskey() {};

	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_write_pskey:public CTestItem
{
private:
	CTObj_write_pskey& operator = (const CTObj_write_pskey &x);
	CTObj_write_pskey(CTObj_write_pskey &x);
protected:
	CString key;
	unsigned long ps_key;
	bool is_string;

	int data_len;
	unsigned short *data;
    //bool mutex_hodle;

public:
	CTObj_write_pskey(const char* pname, testSet *Tset, int ID,int ps_key_base,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_write_pskey();

	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_current:public CTestItem
{
private:
	CTObj_current& operator = (const CTObj_current &x);
	CTObj_current(CTObj_current &x);

protected:
        unsigned long stable_time;
		double current_min;
        double current_max;
        double resistor;
		unsigned long delay_time;
		int on_off_cur;

		bool mutex_hodle1;
		bool mutex_hodle2;
public:
	CTObj_current(const char* pname, testSet *Tset, int ID, int cfj_item, int on_off, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_current() {};

	int ItemTestReady(ConfigFile &cfg = pCfgf);
	int ItemTestClean(ConfigFile &cfg = pCfgf);

	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_clr_XtalOffset:public CTestItem
{
private:
	CTObj_clr_XtalOffset& operator = (const CTObj_clr_XtalOffset &x);
	CTObj_clr_XtalOffset(CTObj_clr_XtalOffset &x);
	
protected:
    //bool mutex_hodle;


public:
	CTObj_clr_XtalOffset(const char* pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_clr_XtalOffset() {};

	int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#define	VM_ENABLE	0
#define	VM_DISABLE	1

class CTObj_vm_enable:public CTestItem
{
private:
	CTObj_vm_enable& operator = (const CTObj_vm_enable &x);
	CTObj_vm_enable(CTObj_vm_enable &x);
	
protected:
	bool vm_disable;
	unsigned long time;
    //bool mutex_hodle;


public:
	CTObj_vm_enable(const char* pname, testSet *Tset, int ID, bool disable,
		      ConfigFile &cfg = pCfgf);
	virtual ~CTObj_vm_enable() {};
;
	int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_codec_io_read:public CTestItem
{
private:
	CTObj_codec_io_read& operator = (const CTObj_codec_io_read &x);
	CTObj_codec_io_read(CTObj_codec_io_read &x);
	
protected:
	bool clr_or_store;
	unsigned int ps_key;


public:
	static unsigned short pio;

	CTObj_codec_io_read(const char* pname, testSet *Tset, int ID, bool clr,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_codec_io_read() {};

	int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#define	IS_IO_TEST(IO,pio)	(cfg.ItemSelect(s_pio_check##IO##_sel)?(1ul<<pio):0)
class CTObj_set_io_dir:public CTestItem
{
private:
	CTObj_set_io_dir& operator = (const CTObj_set_io_dir &x);
	CTObj_set_io_dir(CTObj_set_io_dir &x);
	
protected:
	unsigned long out_io_mask;
    //bool mutex_hodle;

public:
	CTObj_set_io_dir(const char* pname, testSet *Tset, int ID, bool is_bs = false,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_set_io_dir() {};

	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

#if 0
class CTObj_io_pskey_ready:public CTestItem
{
private:
	CTObj_io_pskey_ready& operator = (const CTObj_io_pskey_ready &x);
	CTObj_io_pskey_ready(CTObj_io_pskey_ready &x);
	
protected:
	unsigned short ps_key;
	unsigned short data_ref;
	unsigned short real_data;
	unsigned short data;
	int len;
    //bool mutex_hodle;

	
	bool data_is_same(void) { return(data_ref == data); };
public:
	CTObj_io_pskey_ready(const char* pname, testSet *Tset, int ID, unsigned short pskey,unsigned short data,
			     unsigned short rdata,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_io_pskey_ready() {};

	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
	friend class CTObj_io_pskey_back;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_io_pskey_back:public CTestItem
{
private:
	CTObj_io_pskey_back& operator = (const CTObj_io_pskey_back &x);
	CTObj_io_pskey_back(CTObj_io_pskey_back &x);
	
protected:
	CTObj_io_pskey_ready *pskey_obj;
    //bool mutex_hodle;

public:
	CTObj_io_pskey_back(const char* pname, testSet *Tset, int ID, CTObj_io_pskey_ready *obj,
			    ConfigFile &cfg = pCfgf);
	virtual ~CTObj_io_pskey_back() {};

	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
};
#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/* struct pio_test_s {
	int in_pio;
	int out_pio;
}; */
#define	SET_IN_IO_MASK(IO,in_io_mask)	do {						\
	if(cfg.ItemSelect(s_pio_check##IO##_sel)) {					\
          in_io_mask |= (1ul<<IO);				\
	}										\
}while(0)

class CTObj_io_port:public CTestItem
{
private:
	CTObj_io_port& operator = (const CTObj_io_port &x);
	CTObj_io_port(CTObj_io_port &x);
	
protected:
	//struct pio_test_s pio;
	unsigned long time;
	unsigned long in_io_mask;
	unsigned long low_data;
	unsigned long high_data;
	int pio_name;
    	//add
	uint32 map_mask;
	uint32 pio_mask;
	uint32 in_output_dir;
	uint32 value_low;
	CString pio_mask_string;
	CString mcu_gpio_rd_cmd;
	CString mcu_gpio_wr_cmd;
	CString mcu_gpio_set_output_cmd;
	CString mcu_gpio_set_input_cmd;
		
    	bool mutex_hodle1;
	bool mutex_hodle2;

	virtual bool set_pio(int out_pio,bool lv);
	virtual bool is_pio_lv(int pio_name,bool lv,int i);
public:
	CTObj_io_port(const char* pname, testSet *Tset, int ID, int pin_num,
		      ConfigFile &cfg = pCfgf);
	virtual ~CTObj_io_port() {};

	int ItemTestReady(ConfigFile &cfg);
	int ItemTestClean(ConfigFile &cfg);

	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_led_io:public CTObj_io_port
{
private:
	CTObj_led_io& operator = (const CTObj_led_io &x);
	CTObj_led_io(CTObj_led_io &x);
protected:
	uint16 led;
	int led_io_num;
    int pin_name;

public:
	CTObj_led_io(const char* pname, testSet *Tset, int ID, int pin_num,
		      ConfigFile &cfg = pCfgf);
	virtual ~CTObj_led_io() {};

	bool set_led_out(int lv);
	bool is_led_lv(int lv);

	void gen_prompt(void);

	int TestOp(ConfigFile &cfg = pCfgf);

};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_Aio_port:public CTestItem
{
private:
	CTObj_Aio_port& operator = (const CTObj_Aio_port &x);
	CTObj_Aio_port(CTObj_Aio_port &x);
	
protected:
	unsigned long low_lv_ad_data;
	unsigned long high_lv_ad_data;
	unsigned long low_data;
	unsigned long high_data;
	unsigned long time;
    CString mcu_gpio_set_output_cmd;
	CString mcu_gpio_wr_high_cmd;
	CString mcu_gpio_wr_low_cmd;
	bool mutex_hodle1;
	bool mutex_hodle2;
	int pio_name;
	int aio;

	bool is_pio_lv(int in_pio,bool lv);
public:
	CTObj_Aio_port(const char* pname, testSet *Tset, int ID, 
		      ConfigFile &cfg = pCfgf);
	virtual ~CTObj_Aio_port() {};

	void gen_prompt();

	int ItemTestReady(ConfigFile &cfg);
	int ItemTestClean(ConfigFile &cfg);
	int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_mbias:public CTestItem
{
private:
	CTObj_mbias& operator = (const CTObj_mbias &x);
	CTObj_mbias(CTObj_mbias &x);
protected:
    unsigned long delay_time;
	CString mcu_gpio_set_input_cmd;
	CString mcu_gpio_rd_cmd;
	int pin_num;
	int pin_name;
	int instance;
	int pio_name;
	bool mutex_hodle1;
	bool mutex_hodle2;

public:
	CTObj_mbias(const char* pname,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_mbias() { };

        void gen_prompt(void);

	int ItemTestReady(ConfigFile &cfg);
	int ItemTestClean(ConfigFile &cfg);
	signed long int set_mbias_out(int instance, bool lv);
	bool is_mbias_lv(bool lv);
	int TestOp(ConfigFile &cfg = pCfgf);

};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_uart_cts_rts:public CTestItem
{
private:
	CTObj_uart_cts_rts& operator = (const CTObj_uart_cts_rts &x);
	CTObj_uart_cts_rts(CTObj_uart_cts_rts &x);
	
protected:
    //bool mutex_hodle;
   

public:
	CTObj_uart_cts_rts(const char* pname, testSet *Tset, int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_uart_cts_rts() {};
	void gen_prompt();

	int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_uart_test:public CTestItem
{
private:
	CTObj_uart_test& operator = (const CTObj_uart_test &x);
	CTObj_uart_test(CTObj_uart_test &x);
	
protected:
    //bool mutex_hodle;
	CString port;
	int baudrate;
	
public:
	CTObj_uart_test(const char* pname, testSet *Tset, int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_uart_test() {};
	void gen_prompt();
	
	int TestOp(ConfigFile &cfg = pCfgf);
};
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_pcm_in_out:public CTestItem
{
private:
	CTObj_pcm_in_out& operator = (const CTObj_pcm_in_out &x);
	CTObj_pcm_in_out(CTObj_pcm_in_out &x);
	
protected:
    bool mutex_hodle;
   

public:
	CTObj_pcm_in_out(const char* pname, testSet *Tset, int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_pcm_in_out() {};
	void gen_prompt();

	int TestOp(ConfigFile &cfg = pCfgf);
};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_usb_np_io:public CTestItem
{
private:
	CTObj_usb_np_io& operator = (const CTObj_usb_np_io &x);
	CTObj_usb_np_io(CTObj_usb_np_io &x);
	USHORT USB_VID;
	USHORT USB_PID;

protected:
    //bool mutex_hodle;


public:
	CTObj_usb_np_io(const char* pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_usb_np_io() {};

	int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
enum 
	{
		CHAGE_TRICK = 0,
		CHAGE_HIGH_CUR,
		CHAGE_IDLE_ERR,
		CHAGE_STANDBY,
		CHAGE_NO_POWER,
	};


class CTObj_vbus_usb33_io:public CTestItem
{
private:
	CTObj_vbus_usb33_io& operator = (const CTObj_vbus_usb33_io &x);
	CTObj_vbus_usb33_io(CTObj_vbus_usb33_io &x);

protected:
    //bool mutex_hodle;
	CString mcu_gpio_rd_cmd;
	CString mcu_gpio_wr_cmd;
	CString mcu_gpio_set_output_cmd;
	CString mcu_gpio_set_input_cmd;
	

	bool is_vbus_lv(bool lv);
	bool set_vbus(bool lv);
public:
	CTObj_vbus_usb33_io(const char* pname, testSet *Tset, int ID,
			    ConfigFile &cfg = pCfgf);
	virtual ~CTObj_vbus_usb33_io() {};

	bool is_pio_lv(bool lv);
	int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_audio_io:public CTestItem
{

private:
	CTObj_audio_io& operator = (const CTObj_audio_io &x);
	CTObj_audio_io(CTObj_audio_io &x);
protected:
        int spk_ch_mode;
        int mic_ch_mode;
        double max_thd;
        double min_amp;
        double max_amp;
        int watch_times;
        int av_times;
        unsigned long rec_time;

        double audio_valid_coef;
        CAudio_par *pcfg;
        CAudio_par audio_par;
        CString CurTestObj;
        CString CurTestItem;
        unsigned long wait_time;

        bool data_valid;
        double Llv,Rlv,Lthd,Rthd;
        bool Lresult,Rresult;

        bool mutex_holde1;
		bool mutex_holde2;

        bool par_is_gloden;
public:
	CTObj_audio_io(const char* pname,testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_audio_io() { };

        int ItemTestReady(ConfigFile &cfg = pCfgf);
	int ItemTestClean(ConfigFile &cfg = pCfgf);
        void ClearResult(void);

        void gen_prompt(void);
        void update_range(void);
	int TestOp(ConfigFile &cfg = pCfgf);

        void record_header(ofstream &fs);
        void record_result(ofstream &fs);
	
};

struct static_obj_t
{
	CString cap;
	int id_idx;
};



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_crystal_adj:public CTestItem
{
private:
	CTObj_crystal_adj& operator = (const CTObj_crystal_adj &x);
	CTObj_crystal_adj(CTObj_crystal_adj &x);

protected:
	unsigned long use_ch;
	unsigned long offset;
	double fcounter_offset;

	int crystal_par_max;
	int crystal_par_min;
	double center_freq;
    unsigned long tx_open_delay;
	unsigned long change_set_delay;
	unsigned long stable_time;
	unsigned long try_times;
	double min_rf_pwr;
    bool mutex_hodle1;
	bool mutex_hodle2;

public:
	CTObj_crystal_adj(const char* pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_crystal_adj() {};

    int ItemTestReady(ConfigFile &cfg = pCfgf);
	int ItemTestClean(ConfigFile &cfg = pCfgf);
	bool getFreq_Fcounter(double &f);
	//bool getFreq_VSpec(double &f);

	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);

};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_read_addr:public CTestItem
{
private:
	CTObj_read_addr& operator = (const CTObj_read_addr &x);
	CTObj_read_addr(CTObj_read_addr &x);

protected:
    //bool mutex_hodle;


public:
	CTObj_read_addr(const char* pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_read_addr() {};

	int TestOp(ConfigFile &cfg = pCfgf);

};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_input_addr:public CTestItem
{
private:
	CTObj_input_addr& operator = (const CTObj_input_addr &x);
	CTObj_input_addr(CTObj_input_addr &x);
protected:
	struct bt_address_s* p_address;
	unsigned long man_input_type;
	bool default_address_write_only;

        int nap;
        int uap;
        int min_lap;
        int max_lap;

        bool auto_gen_addr(ConfigFile &cfg);
        bool is_jx_addr(void);
public:
	CTObj_input_addr(const char* pname,testSet *Tset,int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_input_addr() {};

	void gen_prompt(void);
	int TestOp(ConfigFile &cfg = pCfgf);
};



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_waddr:public CTestItem
{
private:
	CTObj_waddr& operator = (const CTObj_waddr &x);
	CTObj_waddr(CTObj_waddr &x);

protected:
	//unsigned char addrBuf[6];
	bool default_address_write_only;
	bool man_input_type;
    //bool wwhen_def_address;
    //bool mutex_hodle;

public:
	CTObj_waddr(const char* pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_waddr() {};

	bool is_dev_addr_equ_input(void);
	//void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
};



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_reset:public CTestItem
{
private:
	CTObj_reset& operator = (const CTObj_reset &x);
	CTObj_reset(CTObj_reset &x);

protected:
	unsigned long times;
	unsigned long time;
	unsigned long delay_time;

    bool need_vreg_on_s;

public:
	CTObj_reset(const char* pname, testSet *Tset, int ID, unsigned long rtime, bool need_vreg_on, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_reset() {};


	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
};



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_flash_write:public CTestItem
{
private:
	CTObj_flash_write& operator = (const CTObj_flash_write &x);
	CTObj_flash_write(CTObj_flash_write &x);

protected:
	int xtal;
	int delay;
	CString file;
	bool *condition;
	//unsigned long connect_times;
    
    bool mutex_holde;

public:
	CTObj_flash_write(const char* pname, testSet *Tset, int ID, bool *cd, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_flash_write() {};
    int ItemTestReady(ConfigFile &cfg = pCfgf);
	int ItemTestClean(ConfigFile &cfg = pCfgf);
	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
};
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_rback_cs:public CTestItem
{
private:
	CTObj_rback_cs& operator = (const CTObj_rback_cs &x);
	CTObj_rback_cs(CTObj_rback_cs &x);
	
protected:
	public:
		CTObj_rback_cs(const char* pname,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
		virtual ~CTObj_rback_cs() {};
		
		void gen_prompt(void);
		int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_wback_cs:public CTestItem
{
private:
	CTObj_wback_cs& operator = (const CTObj_wback_cs &x);
	CTObj_wback_cs(CTObj_wback_cs &x);
	
protected:
	public:
		CTObj_wback_cs(const char* pname,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
		virtual ~CTObj_wback_cs() {};
		
		void gen_prompt(void);
		int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_flash_verify:public CTestItem
{
private:
	CTObj_flash_verify& operator = (const CTObj_flash_verify &x);
	CTObj_flash_verify(CTObj_flash_verify &x);

protected:
	int xtal;
	int delay;
	CString file;
	bool for_write;
 
    //bool mutex_hodle;

public:
	bool verify_result;

	CTObj_flash_verify(const char* pname, testSet *Tset, int ID, bool for_write,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_flash_verify() {};

	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_rback_pskey:public CTestItem
{
private:
	CTObj_rback_pskey& operator = (const CTObj_rback_pskey &x);
	CTObj_rback_pskey(CTObj_rback_pskey &x);

protected:
	struct bk_pskey_item* ps_item;
public:
	CTObj_rback_pskey(const char* pname,struct bk_pskey_item* item,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_rback_pskey() {};

	void gen_prompt(void);
	int TestOp(ConfigFile &cfg = pCfgf);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_wback_pskey:public CTestItem
{
private:
	CTObj_wback_pskey& operator = (const CTObj_wback_pskey &x);
	CTObj_wback_pskey(CTObj_wback_pskey &x);

protected:
	struct bk_pskey_item* ps_item;
public:
	CTObj_wback_pskey(const char* pname,struct bk_pskey_item* item,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_wback_pskey() {};

	void gen_prompt(void);
	int TestOp(ConfigFile &cfg = pCfgf);
};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------


class CTObj_SpiLock:public CTestItem
{
private:
	CTObj_SpiLock& operator = (const CTObj_SpiLock &x);
	CTObj_SpiLock(CTObj_SpiLock &x);
	
protected:
	unsigned long xpv_crc32;
	unsigned long xdv_crc32;
	bool file_crc32_err;
	CString file;
	
	int xtal;
	int delay;
	bool reset_need;
public:
	CTObj_SpiLock(const char* pname, testSet *Tset, int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_SpiLock() {};
	
	bool CTObj_SpiLock::update_loader(void);
	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);
};



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_SpiUnlock:public CTestItem
{
private:
	CTObj_SpiUnlock& operator = (const CTObj_SpiUnlock &x);
	CTObj_SpiUnlock(CTObj_SpiUnlock &x);
	
protected:
	CString key_file;
public:
	CTObj_SpiUnlock(const char* pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_SpiUnlock() {};
	
	int TestOp(ConfigFile &cfg = pCfgf);
};



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_extern_flash:public CTestItem
{
private:
	CTObj_extern_flash& operator = (const CTObj_extern_flash &x);
	CTObj_extern_flash(CTObj_extern_flash &x);
	
protected:
	CString flash_file;
public:
	CTObj_extern_flash(const char* pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_extern_flash() {};
	void gen_prompt(void);
	int TestOp(ConfigFile &cfg = pCfgf);
};

//-----------------------------------------------------------------------
class CTObj_WriteMACAddr:public CTestItem
{
private:
	CTObj_WriteMACAddr& operator = (const CTObj_WriteMACAddr &x);
	CTObj_WriteMACAddr(CTObj_WriteMACAddr &x);
	
//protected:
	//unsigned long reconnect_poweroffTime;
	
	
public:
	CTObj_WriteMACAddr(const char* pname,testSet *Tset, int ID, 
		ConfigFile &cfg = pCfgf);
	virtual ~CTObj_WriteMACAddr() {};
	
	void gen_prompt(CSortListCtrl &promp = MainList);
	int TestOp(ConfigFile &cfg = pCfgf);
	int WriteMacResult(CBsDevice *Obj,int addr,CString mac_a,CString mac_b,CString mac_c,CString mac_d,CString mac_e,CString mac_f);
	
};

//-------------------------------------------------------------------------------------------------
class CTObj_AutoWriteMACAddr:public CTestItem
{
private:
	CTObj_AutoWriteMACAddr& operator = (const CTObj_AutoWriteMACAddr &x);
	CTObj_AutoWriteMACAddr(CTObj_AutoWriteMACAddr &x);
	
	//protected:
	//unsigned long reconnect_poweroffTime;
	
	
public:
	int testnumber;
	CString macaddr;
	int iE2pAddr;
	int macaddnum;
	CTObj_AutoWriteMACAddr(const char* pname,testSet *Tset,int ID, 
		ConfigFile &cfg = pCfgf);
	virtual ~CTObj_AutoWriteMACAddr() {};
	
	int WriteMacResult(CBsDevice *Obj,int addr,CString mac_a,CString mac_b,CString mac_c,CString mac_d,CString mac_e,CString mac_f);
	void gen_prompt(CSortListCtrl &promp = MainList);
	int MacTestOp(CString mac,ConfigFile &cfg = pCfgf);
	int TestOp(ConfigFile &cfg = pCfgf);
	CString shortMAC(CString &str);
	
};
//-------------------------------------------------------------------------------------------------
class CTObj_DisplayMAC:public CTestItem
{
private:
	CTObj_DisplayMAC& operator = (const CTObj_DisplayMAC &x);
	CTObj_DisplayMAC(CTObj_DisplayMAC &x);
	
	//protected:
	//unsigned long reconnect_poweroffTime;
	
	
public:
	int BrAddr;

	CTObj_DisplayMAC(const char* pname,testSet *Tset, int ID, 
		ConfigFile &cfg = pCfgf);
	virtual ~CTObj_DisplayMAC() {};
	#if 0
	void gen_prompt(CSortListCtrl &promp = MainList);
	#else
        void gen_prompt();
	#endif
	int TestOp(ConfigFile &cfg = pCfgf);	
};
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
class CTObj_ShowSoftVer:public CTestItem
{
private:
	CTObj_ShowSoftVer& operator = (const CTObj_ShowSoftVer &x);
	CTObj_ShowSoftVer(CTObj_ShowSoftVer &x);			
public:
	CString soft_ver;
	unsigned long time;
	CTObj_ShowSoftVer(const char* pname,testSet *Tset, int ID, 
		ConfigFile &cfg = pCfgf);
	virtual ~CTObj_ShowSoftVer() {};
	
	void gen_prompt();
	int TestOp(ConfigFile &cfg = pCfgf);	
};
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------

class CTObj_N4010A_fre:public CTestItem
{
private:
	CTObj_N4010A_fre& operator = (const CTObj_N4010A_fre &x);
	CTObj_N4010A_fre(CTObj_N4010A_fre &x);
protected:
	double center_fre;
	double fre_offset;
	double freq_compensate;//²¹³¥Öµ
	bool par_is_gloden;
	
	bool mutex_hodle1;
	bool mutex_hodle2;
	
public:
	CTObj_N4010A_fre(const char* pname,testSet *Tset,int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_N4010A_fre() { };
	
	void gen_prompt(void);
	void update_range(void);
	int TestOp(ConfigFile &cfg = pCfgf);
	
	int ItemTestReady(ConfigFile &cfg = pCfgf);
	int ItemTestClean(ConfigFile &cfg = pCfgf);
	
	void record_header(ofstream &fs);
	void record_result(ofstream &fs);
};
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

class CTObj_N4010A_pwr:public CTestItem
{
private:
	CTObj_N4010A_pwr& operator = (const CTObj_N4010A_pwr &x);
	CTObj_N4010A_pwr(CTObj_N4010A_pwr &x);
protected:
	//double input_power;
	double path_loss;
	uint8 index_difference;
	double pwr_min;
	double pwr_max;
	double input_pwr;
	int channel;
	bool par_is_gloden;
	
	//double pwr_compensate;//²¹³¥Öµ
	bool mutex_hodle1;
	bool mutex_hodle2;

	bool adjustflag;
	public:
		CTObj_N4010A_pwr(const char* pname,int ch,testSet *Tset,bool adjustpowerflag,int ID,ConfigFile &cfg = pCfgf);
		virtual ~CTObj_N4010A_pwr() { };
		
		void gen_prompt(void);
		void update_range(void);
		int TestOp(ConfigFile &cfg = pCfgf);
		
		int ItemTestReady(ConfigFile &cfg = pCfgf);
		int ItemTestClean(ConfigFile &cfg = pCfgf);

		void record_header(ofstream &fs);
		void record_result(ofstream &fs);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

class CTObj_N4010A_EVM:public CTestItem
{
private:
	CTObj_N4010A_EVM& operator = (const CTObj_N4010A_EVM &x);
	CTObj_N4010A_EVM(CTObj_N4010A_EVM &x);
protected:
	//double input_power;
	double path_loss;
	double evm_max;
	//double evm_compensate;//²¹³¥Öµ
	//double pwr_min;
	//double pwr_max;
	//double input_pwr;
	int channel;
	//bool par_is_gloden;
	
	bool mutex_hodle1;
	bool mutex_hodle2;

	bool adjustflag;
	public:
		CTObj_N4010A_EVM(const char* pname,int ch,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
		virtual ~CTObj_N4010A_EVM() { };
		
		void gen_prompt(void);
		void update_range(void);
		int TestOp(ConfigFile &cfg = pCfgf);
		
		int ItemTestReady(ConfigFile &cfg = pCfgf);
		int ItemTestClean(ConfigFile &cfg = pCfgf);

		void record_header(ofstream &fs);
		void record_result(ofstream &fs);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_N4010A_sens:public CTestItem
{
private:
	CTObj_N4010A_sens& operator = (const CTObj_N4010A_sens &x);
	CTObj_N4010A_sens(CTObj_N4010A_sens &x);
protected:
	double per_max;
	double tx_power;
    bool par_is_gloden;
	int channel;
	//int packtesnum;
	double path_loss;

	bool mutex_hodle1;
	bool mutex_hodle2;

	public:
		CTObj_N4010A_sens(const char* pname,int ch,testSet *Tset,int ID, ConfigFile &cfg = pCfgf);
		virtual ~CTObj_N4010A_sens() { };
		
		void gen_prompt(void);
		void update_range(void);
		int TestOp(ConfigFile &cfg = pCfgf);
		
		int ItemTestReady(ConfigFile &cfg = pCfgf);
		int ItemTestClean(ConfigFile &cfg = pCfgf);

		void record_header(ofstream &fs);
        void record_result(ofstream &fs);
};
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

class CTObj_RFPARASAVE:public CTestItem
{
private:
	CTObj_RFPARASAVE& operator = (const CTObj_RFPARASAVE &x);
	CTObj_RFPARASAVE(CTObj_RFPARASAVE &x);
	
protected:
	
public:
	CTObj_RFPARASAVE(const char* pname,testSet *Tset,int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_RFPARASAVE() {};
	
	void gen_prompt(void);
	void update_range(void);
	int TestOp(ConfigFile &cfg = pCfgf);
	void record_header(ofstream &fs);
	void record_result(ofstream &fs);
};
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

class CTObj_mt8852_test:public CTestItem
{
private:
	CTObj_mt8852_test& operator = (const CTObj_mt8852_test &x);
	CTObj_mt8852_test(CTObj_mt8852_test &x);
protected:
        bool mutex_holde;
        unsigned long delay_time;
		unsigned long rout_sw_time;
        unsigned long discn_dut_time;
        unsigned long retry_delay;

		bool rf_power_adjust_sel;

		bool rf_par_set_err;
        bool rf_rout_set_err;
        bool rf_power_adjust_op_valid;
        int rf_power_adjust_value;
		unsigned short rf_power_cur_par;
        unsigned short rf_power_adjust_par;

        double MT8852_DevA_pathoffset;
        double MT8852_DevB_pathoffset;
        bool par_is_gloden;


		bool dut_enter_test(void);
        bool do_8852_test(struct tSetResult *tresult);

		bool rf_power_dec(double &pwr,int &idx);
        bool rf_power_inc(double &pwr,int &idx);
        int rf_power_is_adjusted(void);

		bool save_pwr_par(void);

		bool mutex_hodle1;
	    bool mutex_hodle2;
		bool mutex_hodle3;

		struct tSetResult_mt8852 *tresult_mt8852;
public:
        double APathOffset[3];
        double BPathOffset[3];
protected:
        double RfPwrCenter;
        double RfPwrMin;
        double RfPwrMax;
        double RfPwrAdjMin;
        double RfPwrAdjMax;

public:
	CTObj_mt8852_test(const char* pname,testSet *Tset,int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_mt8852_test() { };

    int ItemTestReady(ConfigFile &cfg = pCfgf);
	int ItemTestClean(ConfigFile &cfg = pCfgf);
    void ClearResult(void);

	void gen_prompt(void);
	int TestOp(ConfigFile &cfg = pCfgf);
};


#define RF_PWR_ADJ_RES_NULL             0
#define RF_PWR_ADJ_RES_PAR_CHG          1
#define RF_PWR_ADJ_RES_PAR_OK           2
#define RF_PWR_ADJ_RES_NOT_504          3
#define RF_PWR_ADJ_RES_ERR              4
#define RF_PWR_ADJ_RES_PWR_ERR          5
#define RF_PWR_ADJ_RES_OVER_RANGE       6
#define RF_PWR_ADJ_RES_GOLDEN           7

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
struct top_res;
class CTObj_mt8852_pwr:public CTestItem
{
private:
	CTObj_mt8852_pwr& operator = (const CTObj_mt8852_pwr &x);
	CTObj_mt8852_pwr(CTObj_mt8852_pwr &x);
protected:
        double path_loss;
	    double pwr_min;
        double pwr_max;
        double pwr_pk;
	int channel;
        struct top_res *res;
		bool par_is_gloden;

public:
	CTObj_mt8852_pwr(const char* pname,int ch,testSet *Tset,int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_mt8852_pwr() { };

	void gen_prompt(void);
	 void update_range(void);
	int TestOp(ConfigFile &cfg = pCfgf);
	
	void record_header(ofstream &fs);
    void record_result(ofstream &fs);
};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
struct tic_res;
class CTObj_mt8852_offset:public CTestItem
{
private:
	CTObj_mt8852_offset& operator = (const CTObj_mt8852_offset &x);
	CTObj_mt8852_offset(CTObj_mt8852_offset &x);
protected:
	double neg_max;
	double pos_max;
	struct tic_res *res;
	bool par_is_gloden;


	public:
		CTObj_mt8852_offset(const char* pname,int ch,testSet *Tset,int ID, ConfigFile &cfg = pCfgf);
		virtual ~CTObj_mt8852_offset() { };
		
		void gen_prompt(void);
		void update_range(void);
		int TestOp(ConfigFile &cfg = pCfgf);

		void record_header(ofstream &fs);
        void record_result(ofstream &fs);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
struct tss_res;
class CTObj_mt8852_sens:public CTestItem
{
private:
	CTObj_mt8852_sens& operator = (const CTObj_mt8852_sens &x);
	CTObj_mt8852_sens(CTObj_mt8852_sens &x);
protected:
	double per_max;
	double fer_max;
	//double tx_power;
	struct tss_res *res;
    bool par_is_gloden;

	public:
		CTObj_mt8852_sens(const char* pname,int ch,testSet *Tset,int ID, ConfigFile &cfg = pCfgf);
		virtual ~CTObj_mt8852_sens() { };
		
		void gen_prompt(void);
		void update_range(void);
		int TestOp(ConfigFile &cfg = pCfgf);

		void record_header(ofstream &fs);
        void record_result(ofstream &fs);
};



//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
class CTObj_sync:public CTestItem
{
private:
	CTObj_sync& operator = (const CTObj_sync &x);
	CTObj_sync(CTObj_sync &x);
protected:
        int sync_id;
        unsigned long max_sync_time;
public:
	CTObj_sync(const char* pname,int Sync_id,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_sync() { };

	int TestOp(ConfigFile &cfg = pCfgf);
};



//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
class CTObj_close_tbox:public CTestItem
{
private:
	CTObj_close_tbox& operator = (const CTObj_close_tbox &x);
	CTObj_close_tbox(CTObj_close_tbox &x);

protected:
        CString com;
        bool mutex_holde;
        bool range_show;
		unsigned long max_delay_time;
public:
	CTObj_close_tbox(const char* pname,testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_close_tbox() { };

    int ItemTestReady(ConfigFile &cfg = pCfgf);
	int ItemTestClean(ConfigFile &cfg = pCfgf);

        void gen_prompt(void);
        void update_range(void);
	int TestOp(ConfigFile &cfg = pCfgf);
};

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
class CTObj_tbox_delay:public CTestItem
{
private:
	CTObj_tbox_delay& operator = (const CTObj_tbox_delay &x);
	CTObj_tbox_delay(CTObj_tbox_delay &x);
protected:
    bool mutex_holde;  
	unsigned long max_delay_time;
public:
	CTObj_tbox_delay(const char* pname,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_tbox_delay() { };

	void gen_prompt(void);
	int ItemTestReady(ConfigFile &cfg = pCfgf);
	int ItemTestClean(ConfigFile &cfg = pCfgf);
	int TestOp(ConfigFile &cfg = pCfgf);
};

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
class CTObj_open_tbox:public CTestItem
{
private:
	CTObj_open_tbox& operator = (const CTObj_open_tbox &x);
	CTObj_open_tbox(CTObj_open_tbox &x);

protected:
        CString com;
        bool mutex_holde;
        bool range_show;
public:
	CTObj_open_tbox(const char* pname,testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_open_tbox() { };

        int ItemTestReady(ConfigFile &cfg = pCfgf);
	int ItemTestClean(ConfigFile &cfg = pCfgf);

        void gen_prompt(void);
        void update_range(void);
        int TestOp(ConfigFile &cfg = pCfgf);
};




//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------


class CTObj_pwrDut:public CTestItem
{
private:
	CTObj_pwrDut& operator = (const CTObj_pwrDut &x);
	CTObj_pwrDut(CTObj_pwrDut &x);
protected:
    bool mutex_holde;

public:
	CTObj_pwrDut(const char* pname,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_pwrDut() { };

	 int ItemTestReady(ConfigFile &cfg = pCfgf);
	int ItemTestClean(ConfigFile &cfg = pCfgf);
        void gen_prompt(void);
	int TestOp(ConfigFile &cfg = pCfgf);
};



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#if 0

class CTObj_pwroffDut:public CTestItem
{
private:
	CTObj_pwroffDut& operator = (const CTObj_pwroffDut &x);
	CTObj_pwroffDut(CTObj_pwroffDut &x);
protected:
    bool mutex_holde;

public:
	CTObj_pwroffDut(const char* pname,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_pwroffDut() { };
	int ItemTestReady(ConfigFile &cfg = pCfgf);
	int ItemTestClean(ConfigFile &cfg = pCfgf);
         void gen_prompt(void);   
	int TestOp(ConfigFile &cfg = pCfgf);
};

#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_spi_switch_on:public CTestItem
{
private:
	CTObj_spi_switch_on& operator = (const CTObj_spi_switch_on &x);
	CTObj_spi_switch_on(CTObj_spi_switch_on &x);
protected:
    bool mutex_holde1;
	bool mutex_holde2;

public:
	CTObj_spi_switch_on(const char* pname,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_spi_switch_on() { };
	int ItemTestReady(ConfigFile &cfg = pCfgf);
	int ItemTestClean(ConfigFile &cfg = pCfgf);
         void gen_prompt(void);   
	int TestOp(ConfigFile &cfg = pCfgf);

};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_spi_switch_off:public CTestItem
{
private:
	CTObj_spi_switch_off& operator = (const CTObj_spi_switch_off &x);
	CTObj_spi_switch_off(CTObj_spi_switch_off &x);
protected:
    bool mutex_holde1;
	bool mutex_holde2;
	
public:
	CTObj_spi_switch_off(const char* pname,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_spi_switch_off() { };
	int ItemTestReady(ConfigFile &cfg = pCfgf);
	int ItemTestClean(ConfigFile &cfg = pCfgf);
	void gen_prompt(void);   
	int TestOp(ConfigFile &cfg = pCfgf);
	
};
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
class CTObj_addr_verify:public CTestItem
{
private:
	CTObj_addr_verify& operator = (const CTObj_addr_verify &x);
	CTObj_addr_verify(CTObj_addr_verify &x);
protected:
public:
	CTObj_addr_verify(const char* pname,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_addr_verify() { };

	int TestOp(ConfigFile &cfg = pCfgf);
};


//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
class CTObj_defaddr:public CTestItem
{
private:
	CTObj_defaddr& operator = (const CTObj_defaddr &x);
	CTObj_defaddr(CTObj_defaddr &x);
protected:
        CString def_group;
        CString def_addr;

        bool lable_addr_verify;
        bool is_lable_addr(void);
public:
	CTObj_defaddr(const char* pname,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_defaddr() { };

        void gen_prompt(void);
	int TestOp(ConfigFile &cfg = pCfgf);
};


//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
class CTObj_read_ver:public CTObj_reconnect
{
private:
	CTObj_read_ver& operator = (const CTObj_read_ver &x);
	CTObj_read_ver(CTObj_read_ver &x);
protected:
        CString bt_ver;
        bool verify;
        bool par_is_gloden;
public:
	CTObj_read_ver(const char* pname,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_read_ver() { };

        void gen_prompt(void);
        void update_range(void);
	int TestOp(ConfigFile &cfg = pCfgf);
};


//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
class CTObj_read_name:public CTObj_reconnect
{
private:
	CTObj_read_name& operator = (const CTObj_read_name &x);
	CTObj_read_name(CTObj_read_name &x);
protected:
        CString bt_name;
        bool verify;
        bool par_is_gloden;
public:
	CTObj_read_name(const char* pname,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_read_name() { };

        void gen_prompt(void);
        void update_range(void);
	int TestOp(ConfigFile &cfg = pCfgf);
};


//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
class CTObj_wxtal:public CTestItem
{
private:
	CTObj_wxtal& operator = (const CTObj_wxtal &x);
	CTObj_wxtal(CTObj_wxtal &x);
protected:
	unsigned long fTrim;

public:
	CTObj_wxtal(const char* pname,testSet *Tset,int ID,ConfigFile &cfg = pCfgf);
	virtual ~CTObj_wxtal() { };

        void gen_prompt(void);
	int TestOp(ConfigFile &cfg = pCfgf);
};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class CTObj_charge_adj:public CTestItem
{
private:
	CTObj_charge_adj& operator = (const CTObj_charge_adj &x);
	CTObj_charge_adj(CTObj_charge_adj &x);

protected:
	double max_full_current;
	uint16 chg_current_pskey;
	uint16 chg_current;

	uint16 chg_firmware_ctl_pskey;
	uint16 chg_firmware_ctl;

	uint16 trim_pskey;
	uint16 back_current;

	bool mutex_hodle1;
    bool mutex_hodle2;
	bool mutex_hodle3;

	unsigned long charge_reset_time;
	unsigned long stable_time;
public:
	CTObj_charge_adj(const char* pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_charge_adj() {};

	void gen_prompt(void);
	int ItemTestReady(ConfigFile &cfg = pCfgf);
	int ItemTestClean(ConfigFile &cfg = pCfgf);
	int TestOp(ConfigFile &cfg = pCfgf);
};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

class CTObj_rgb_chk:public CTestItem
{
private:
	CTObj_rgb_chk& operator = (const CTObj_rgb_chk &x);
	CTObj_rgb_chk(CTObj_rgb_chk &x);

protected:
	int led_id_s;
	char mcu_r_grp_s;
	char mcu_g_grp_s;
	char mcu_b_grp_s;
	int pin_r_num_s;
	int pin_g_num_s;
	int pin_b_num_s;


	unsigned long led_red;
	unsigned long led_green;
	unsigned long led_blue;
	int red_offset;
	int green_offset;
	int blue_offset;

    int cfg_item_start;
	ConfigFile cfgf;
	int led_red_min;
	int led_red_max;
	int led_green_min;
	int led_green_max;
	int led_blue_min;
	int led_blue_max;


	//double led1_red;
	//double led1_green;
	//double led1_blue;


public:
	CTObj_rgb_chk(const char* pname,testSet *Tset,int ID, int led_id,
							 char mcu_r_grp, char mcu_g_grp, char mcu_b_grp, 
							 int pin_r_num,  int pin_g_num, int pin_b_num, int cfj_item,
							 ConfigFile &cfg = pCfgf);
	virtual ~CTObj_rgb_chk() {};

	void gen_prompt(void);
    bool set_led_light(int led, int enable);
	bool get_led_value(int &red, int &green, int &blue);
	void update_range(void);
	int TestOp(ConfigFile &cfg = pCfgf);
};



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

class CTObj_button_chk:public CTestItem
{
private:
	CTObj_button_chk& operator = (const CTObj_button_chk &x);
	CTObj_button_chk(CTObj_button_chk &x);

protected:
	char mcu_grp_s;
	int pin_num_s;
	CString mcu_gpio_set_input_cmd;
    CString mcu_gpio_rd_cmd;
    

public:
	CTObj_button_chk(const char* pname,testSet *Tset,int ID, char mcu_grp, int pin_num, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_button_chk() {};

	//void gen_prompt(void);
	int TestOp(ConfigFile &cfg = pCfgf);
};


//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------

class RF_test_integration:public CTestItem
{
private:
	RF_test_integration& operator = (const RF_test_integration &x);
	RF_test_integration(RF_test_integration &x);
protected:
	//double center_fre;
	//double fre_offset;
	//double freq_compensate;//²¹³¥Öµ
	//double pathloss_power_low;
	//double pathloss_power_middle;
	//double pathloss_power_high;
	//double pathloss_evm_low;
	//double pathloss_evm_middle;
	//double pathloss_evm_high;
	uint8 index_difference;
	double pwr_min;
	double pwr_max;
	double fre_instrument_loss;
	
	bool par_is_gloden;
	
	bool mutex_hodle1;
	bool mutex_hodle2;
	
public:
	RF_test_integration(const char* pname,testSet *Tset,int ID, ConfigFile &cfg = pCfgf);
	virtual ~RF_test_integration() { };
	
	void gen_prompt(void);
	void update_range(void);
	int TestOp(ConfigFile &cfg = pCfgf);
	
	int ItemTestReady(ConfigFile &cfg = pCfgf);
	int ItemTestClean(ConfigFile &cfg = pCfgf);
	
	void record_header(ofstream &fs);
	void record_result(ofstream &fs);
	void save_adjust_freqoffset(int index,double freqoffset);
	void save_adjust_power(int index,double avpower_para,int flag);
	void save_read_evm(double evm_para,int flag);
	
};

class CTObj_wtriad:public CTestItem
{
private:
	CTObj_wtriad& operator = (const CTObj_wtriad &x);
	CTObj_wtriad(CTObj_wtriad &x);

protected:

	CString file;
	CStringList * TriadList;

public:
	CTObj_wtriad(const char* pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_wtriad() {};

	bool getline_info(CString file_path,const char* mac,char *content);
	int TestOp(ConfigFile &cfg = pCfgf);
	void gen_prompt();
};

extern int stop_rtl8711_tx( RTL87XX_COM* com_A_B, CString &param);	//fu add for PD218dlg for expection
//-------------------------------------------------------------------------------------------------
#endif //__TESTITEM_H__
