/*	tset.h
 */
#ifndef __TSET_H__
#define	__TSET_H__
#include "CommHead.h"
#include <visa.h>



class CTestItem;
class CTestItem_CSR;
class ConfigFile;
class CPD218Dlg;
class CSortListCtrl;
class ofstream;
class ifstream;
class CBsDevice;
struct tSetResult;

#define VM_SUCCESS 0


struct test_sel_t
{
        bool image_write;
        bool fre_offset_test;
        bool rf_test_by_gloden;
        bool io_test;
        bool audio_test;
        bool wAddress_test;
        bool wName_test;

        unsigned long reset_delay;
        unsigned long cn_delay;
        int try_times;
};


//≤‚ ‘œÓƒø–Ú∫≈
enum
{
		TID_POWER_BOARD = 1,
		TID_BATTERY_ON,
		TID_BATTERY_OFF,
		TID_INTO_MP_MODE,
		TID_INTO_NORMAL_MODE,
		TID_CURENT_PWR_OFF,
		TID_POWER_VREG,
		TID_SPI_SW_ON,
		TID_SPI_SW_OFF,
		TID_CLOSE_BOX,
		//TID_CLOSE_BOX_DELAY,
		TID_SPI_UNLOCK,
		TID_CN_DUT,
		TID_CURENT_PWR_ON,
		TID_BK_PSKEY, 
		TID_BK_CS,
		TID_WFILE,
		TID_VERIFY_WFILE,
		TID_WB_PSKEY, 
		TID_WB_CS, 
		
		TID_EXTERN_BURN,
		TID_INPUT_ADDR,   	      
		
		TID_READ_ADDR,
		TID_LABEL_ADDR_VERIFY,
		TID_DEFADDR_VERIFY,
		TID_READ_VER,
		TID_READ_NAME,
		TID_WRITE_ADDR,
		TID_WTriad,
		TID_WRITE_XTAL,

		TID_WR_PSKEY1, 
		TID_WR_PSKEY2, 
		TID_WR_PSKEY3, 
		TID_WR_PSKEY4, 
		TID_WR_PSKEY5, 
		TID_WR_PSKEY6, 
		TID_WR_PSKEY7, 
		TID_WR_PSKEY8, 
		TID_WR_PSKEY9, 
		TID_WR_PSKEY10,
		TID_WR_PSKEY11,
		TID_WR_PSKEY12,
		TID_WR_PSKEY13,
		TID_WR_PSKEY14,
		TID_WR_PSKEY15,	

		TID_BT_RESET,		
		
		TID_RD_PSKEY1, 
		TID_RD_PSKEY2, 
		TID_RD_PSKEY3, 
		TID_RD_PSKEY4, 
		TID_RD_PSKEY5, 
		TID_RD_PSKEY6, 
		TID_RD_PSKEY7, 
		TID_RD_PSKEY8, 
		TID_RD_PSKEY9, 
		TID_RD_PSKEY10,
		TID_RD_PSKEY11,
		TID_RD_PSKEY12,
		TID_RD_PSKEY13,
		TID_RD_PSKEY14,
		TID_RD_PSKEY15,
		
		TID_SPI_LOCK,
        
		TID_CLR_XTAL,
		TID_VM_DISABLE,
		TID_CODEC_IO_CLR,
		TID_VM_ENABLE,
		TID_CODEC_IO_STORE,
		TID_PIO_SET_DIR,
		
		TID_PIO0,
        	TID_PIO1,
        	TID_PIO2,
		TID_PIO3,
		TID_PIO4, 
		TID_PIO5, 
		TID_PIO6, 
		TID_PIO7, 
		TID_PIO8, 
		TID_PIO9, 
		TID_PIO10,
		TID_PIO11,
		TID_PIO12,
		TID_PIO13,
		TID_PIO14,
		TID_PIO15,
		TID_UART_IO,
		TID_PCM_IO,
		TID_PIO19,
		TID_PIO20,
		TID_PIO21,
		TID_PIO22,
		TID_PIO23,
		TID_PIO24,
		TID_PIO25,
		TID_PIO26,
		TID_PIO27,
		TID_PIO28,
		TID_PIO29,
		TID_PIO30,
		TID_PIO31,

		TID_AIO0,
		TID_AIO1,
		TID_MIBAS_IO0,
		TID_MIBAS_IO1,
		TID_VBUS_3V3,
		TID_USB_NP_IO,
		TID_AUDIO_IO,
		//TID_AUDIO_SYNC,
		TID_ADJ_XTAL,
		TID_ADJ_APOWER,
		TID_SAVE_RFPARA,
		
		//TID_MT8852_LOCK,
		TID_MT8852_TEST,
		//TID_MT8852_UNLOCK,
		TID_N4010A_PWR1,
		TID_N4010A_PWR2,
		TID_N4010A_PWR3,
		TID_N4010A_FREQ1,
		TID_N4010A_FREQ2,
		TID_N4010A_FREQ3,

		TID_N4010A_EVM1,
		TID_N4010A_EVM2,
		TID_N4010A_EVM3,
		
		TID_N4010A_SENS1,
		TID_N4010A_SENS2,
		TID_N4010A_SENS3,
		TID_N4010A_SENSANY,
		
		TID_CHARGE_ADJ,
		//TID_SENSE_COLOR1,
		//TID_SENSE_COLOR2,
		TID_BTN,

		TID_OPEN_TBOX,
        
		TID_DISCN_DUT,
};



class CDutGlodenPar
{
private:
	CDutGlodenPar(CDutGlodenPar &x);
protected:
	
public:
	int id;
	CString addr;
	CString ver;
	CString name;
	double current_min;
	double current_max;
	double ch1_rf_avpwr;
	double ch2_rf_avpwr;
	double ch3_rf_avpwr;
	double rf_inputpwr;
	double rf_inputevm;
	double ch1_rf_evm;
	double ch2_rf_evm;
	double ch3_rf_evm;
	int rf_fre_offset;
	double sens_pwr;
	
public:
	CDutGlodenPar(void){};
	CDutGlodenPar(int cfj_key_idx,ConfigFile &cfg);
	virtual ~CDutGlodenPar() { };
	
	void update_to_cfg(ConfigFile &cfg);
	void update_from_cfg(ConfigFile &cfg);
};


class CDutSensorPar
{
private:
	CDutSensorPar(CDutSensorPar &x);
protected:
	
public:
	int id;
	int led0_red_adc_min;
	int led0_red_adc_max;
	int led0_green_adc_min;
	int led0_green_adc_max;
	int led0_blue_adc_min;
	int led0_blue_adc_max;
	int led0_red_adc_adj;
	int led0_green_adc_adj;
	int led0_blue_adc_adj;
	int led0_red_offset;
	int led0_green_offset;
	int led0_blue_offset;
	
	int led1_red_adc_min;
	int led1_red_adc_max;
	int led1_green_adc_min;
	int led1_green_adc_max;
	int led1_blue_adc_min;
	int led1_blue_adc_max;
	int led1_red_adc_adj;
	int led1_green_adc_adj;
	int led1_blue_adc_adj;
	int led1_red_offset;
	int led1_green_offset;
	int led1_blue_offset;
	
	
public:
	CDutSensorPar(void){};
	CDutSensorPar(int cfj_key_idx,ConfigFile &cfg);
	virtual ~CDutSensorPar() { };
	
	void update_to_cfg(ConfigFile &cfg);
	void update_from_cfg(ConfigFile &cfg);
};

#define MAX_DUT_GLODEN_NUM      13
#define DUT_PAR_IDX             0

#define MAX_DUT_COLOR_SNR_NUM   6

struct bk_cs_info {
	struct bt_address_s bk_addr;
	int crystal_ftrim;
	int tx_power_level;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class testSet
{
public:
        int dev_id;
		CBsDevice       *pSpiObj;
        CSortListCtrl	*tlist;
        int idc_CurTestObj;
        int idc_CurTestItem;
        CString prompt_test_name;

        int fail_titem_id;
		unsigned long pass_num;
        unsigned long fail_num;
		
        int             tItemSum;
        CdNode<CTestItem> *TItemList;
        CTestItem *cur_test_item;

        bool own_op_lock;
		bool pwr_opened;
		bool tmode_pwr_opened;
		bool file_crc32_cal;
		bool file_crc32_err;
		unsigned long xpv_crc32;
		unsigned long xdv_crc32;
		//add
		bool rf_is_tested;
		bool xtal_adj_is_tested;
		
		bool            *testRunning;
        HANDLE          hthread;
	    DWORD           hthreadId;
        bool            test_result;
		int   reset_times;

		//HANDLE      myProc_Handle;
		//DWORD       myProc_HandleId;

    struct bt_address_s addr;
	struct bt_address_s r_addr;
	struct bt_address_s *pdev_addr;

	struct bk_cs_info tset_cfgset;
	
	    char bt_name[64];

        volatile bool* ptbox_is_open;
        volatile bool* pdut_pwr_is_on;
        volatile bool* pdut_spi_is_on;

        struct tSetResult *mt8852_res;
		class backup_pskey_contaner *backup_pskey;


        //bool image_loaded;
        //CString image_ver;
        //CString image_name;

        struct test_sel_t tsel;
        double gloden_vamplitude;

		//add in rf_pew_cal
		CString golden_group;
		bool need_input_addr_first;
		//bool ic_is_v504;
        bool is_golden;

	    bool is_b_Test;

private:
        int __DoItemTest(CTestItem *item);
        void sle_item(CTestItem *item);
        void unsle_item(CTestItem *item);
        void show_item_result(CTestItem *item);

		//void CreatTestItemXtal(ConfigFile &cfg,bool &need_reset);
		//void CreatTestItem8852(ConfigFile &cfg,bool &need_reset);
		//void CreatTestItemAudioIO(ConfigFile &cfg);
public:
        testSet(void);
	virtual ~testSet();
 
	//add in rf_pwr_cal
	int golden_idx_in_group(int idx);
	const char* gloden_idx_name(int idx);
	const char* gloden_idx_name(struct bt_address_s *addr = NULL);
	int  gloden_idx(struct bt_address_s *addr = NULL);
	bool is_default_addr(void);

        void show_range(CTestItem *item);
        bool ShowTestItemList(void);
		bool CreatTestItemList(testSet *Tset, ConfigFile &cfg);
		bool CreatTestItem_write_file(testSet *Tset, ConfigFile &cfg,bool *cd);
        void DeleteTestItemList(void);
        
        void ToReadyState(void);
        void ToFinishState(void);
        void show_item_par(CTestItem *item);
        

        void DoSelfMsg(WPARAM wParam);     
        int DoItemTest(CTestItem *item,int err_code);    
        bool AllTestItemOk(void);
};
#define TEST_DEV_ID_A           0
#define TEST_DEV_ID_B           1

#define TSET_MSG(msg,tset)      pMainWnd->SendMessage(WM_ITEM_MSG,(WPARAM)msg,(LPARAM)(tset))
#define MSG_SEL_ITEM            0
#define MSG_SHOW_RESULT         (MSG_SEL_ITEM+1)
#define MSG_SHOW_PAR            (MSG_SHOW_RESULT+1)
#define MSG_SHOW_RANGE          (MSG_SHOW_PAR+1)
#define MSG_READY               (MSG_SHOW_RANGE+1)
#define MSG_FINISH              (MSG_READY+1)



void set_title_show(CString *title);
//bool is_bt_com_allocate(void);
bool is_bt_spi_allocate(void);




//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#if 0

#define pos_idx         0
#define pos_item_name   1
#define pos_P_F         2
#define pos_range       3
#define pos_param       4

#endif

////////////////////////CSR//////////////////////////////////////////////////

//#if 0

#define pos_idx         0
#define pos_item_name   1
#define pos_range       2
#define pos_param       3
#define pos_P_F         4

//#endif

#endif //__NENDEVICE_H__
