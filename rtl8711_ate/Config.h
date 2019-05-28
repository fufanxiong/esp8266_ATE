#ifndef CONFIG_H
#define CONFIG_H
#include "CommHead.h"

class ofstream;
//-----------------------------------------------------------------------
#define CRF_PWR_TBL_LEN         16
#define CRF_PWR_STEP_LEN        8
#define CRF_PWR_STEP_CENTER     5

//-----------------------------------------------------------------------
class Cconfig_item 
{
private:
	static int itemSum;
	int index;
	int ParIndex;
	CString name;
	CString value;

	Cconfig_item& operator = (const Cconfig_item &x);
	Cconfig_item(Cconfig_item &x);
public:
	CdNode<Cconfig_item> dNode;
	
	Cconfig_item(int PIdex,const char *Pname,const char *Pvalue);
	virtual ~Cconfig_item();
	friend class ConfigFile;
};

class ConfigFile
{
private:
	bool dirty;
	CString fileName;
	 CRITICAL_SECTION update_cfg_file_cs;

	ConfigFile& operator = (const ConfigFile &x);
	ConfigFile(ConfigFile &x);

	const char* cmp_key(const char *pKey,const char *pdes);
	bool findItemFromFile(const CString &Key,CString &val,const CString &file);
	Cconfig_item *search_item(int parindex);
	bool is_always_update_item(Cconfig_item *pContent);
	bool is_skip_item(Cconfig_item *pContent, int level);
	bool copy_item(Cconfig_item *pContent);

    int nomal_readfile(const CString &fName,int level);
	int nomal_writefile(const CString &fName,int level);

    double rf_pwr_lv_tbl[CRF_PWR_TBL_LEN];
	int rf_pwr_index_tbl[CRF_PWR_TBL_LEN];
	double rf_pwr_step_tbl[CRF_PWR_STEP_LEN];
	void ini_rf_pwr_tbl(void);

public:
	CdNode<Cconfig_item> *cfglist;
        volatile bool writing_file;

	ConfigFile();
	virtual ~ConfigFile();
	bool ItemSet(int parindex,const CString &val);
	bool ItemSet(int parindex,double &val);
	bool ItemSet(int parindex,int &val);
	bool ItemSet(int parindex,unsigned long &val,bool hex_mode=false);
	bool ItemSet(int parindex,long &val,bool hex_mode=false);

	void MarkDirty(void) { dirty = true; };
	int readfile(const CString &fName,bool bin_format, int level);
	int writefile(const CString &fName,bool bin_format, int level);
	void update_sn_value(int sn_value,int old_sn_value);
	void update_config_to_disk(void);

        bool ItemIsValue(int parindex,const CString &val);
	bool ItemSelect(int parindex);
	const CString &VCString(int parindex);
	int Vint(int parindex);
	unsigned long Vulong(int parindex,bool hex_source = false);
	UINT64 Vuint64(int parindex,bool hex_source = false); 
	double VDouble(int parindex);

	bool get_tx_rand_uid(bool is_new,int &nap,int &uap,int &lap);
	bool get_tx_nap_uap(unsigned long nap_uap,int &nap,int &uap);
	bool get_tx_nap_uap(int &nap,int &uap);
	bool is_same_default_address(struct bt_address_s &addr);
	bool is_same_default_addr(struct bt_address_s &addr);

	bool wflash_crc32_is_ok(unsigned long &xpv_crc32,unsigned long &xdv_crc32);

	////////////////////CSR//////////////////////////////////////////////////////
	bool wloader_crc32_is_ok(unsigned long &xpv_crc32,unsigned long &xdv_crc32);


	void get_bt_address(CString &address);
	void get_end_address(CString &address);
	void get_default_address(CString &address);
	bool set_bt_address(CString &address);
	bool set_end_address(CString &address);
	bool set_default_address(CString &address);
	
	bool get_bt_address(bool is_new,int &nap,int &uap,int &lap);
	bool get_bt_nap_uap(unsigned long nap_uap,int &nap,int &uap);
	bool get_bt_nap_uap(int &nap,int &uap);

	
	unsigned char get_spk_ch_mode(void);
    unsigned char get_mic_ch_mode(void);

	int record_config(CString &file);


	double* get_pwr_step_tbl(void) { return(rf_pwr_step_tbl); };
	double* get_pwr_tbl(void) { return(rf_pwr_lv_tbl); };
	int* get_pwr_idx_tbl(void) { return(rf_pwr_index_tbl); };
	void update_pwr_tbl(CString &pwr_cfj,CString &pwr_step);
};


unsigned long a16ToUint(const char *a16);
UINT64 a16ToUint64(const char *a16);

///////////CSR///////////

#define	enum_io_ps_item(Key)		s_io_ps_##Key##_ready_sel,	\
					s_io_ps_##Key##_ready_key,	\
					s_io_ps_##Key##_ready_data,	\
					s_io_ps_##Key##_real_data


#define	enum_pio_item(IO)		s_pio_check##IO##_sel
#define	enum_aio_item(IO)		s_aio_check##IO##_sel
#define	enum_mbias_item(IO)		s_mbias_check##IO##_sel

#define	enum_pskey_item(Key)		s_psKey##Key##_sel,	\
					s_psKey##Key##_index,	\
					s_psKey##Key##_string,	\
					s_psKey##Key##_value
#define	enum_wpskey_item(Key)		s_wpsKey##Key##_sel,	\
					s_wpsKey##Key##_index,	\
					s_wpsKey##Key##_value



#define	enum_golden_par_item(Key)	s_##Key##_address,              \
	s_##Key##_version,              \
	s_##Key##_name,                 \
	s_##Key##_Curent_min,           \
	s_##Key##_Curent_max,           \
	s_##Key##_OPAv1,           \
	s_##Key##_OPAv2,          \
	s_##Key##_OPAv3,          \
	s_##Key##_INPUTPWR,               \
	s_##Key##_INPUTEVM,          \
	s_##Key##_evm1,			     \
	s_##Key##_evm2,		      \
	s_##Key##_evm3,		     \
	s_##Key##_ICFOFFSET,             \
    s_##Key##_SSTxPwr
	
#define	enum_sensor_par_item(Key)	s_##Key##_sel,  \
	s_##Key##_r0_ad_min,             \
	s_##Key##_r0_ad_max,             \
	s_##Key##_g0_ad_min,             \
	s_##Key##_g0_ad_max,             \
	s_##Key##_b0_ad_min,             \
	s_##Key##_b0_ad_max,             \
	s_##Key##_r0_ad_adj,             \
	s_##Key##_g0_ad_adj,             \
	s_##Key##_b0_ad_adj,             \
	s_##Key##_r0_offset,             \
	s_##Key##_g0_offset,             \
	s_##Key##_b0_offset,             \
	s_##Key##_r1_ad_min,             \
	s_##Key##_r1_ad_max,             \
	s_##Key##_g1_ad_min,             \
	s_##Key##_g1_ad_max,             \
	s_##Key##_b1_ad_min,             \
	s_##Key##_b1_ad_max,             \
	s_##Key##_r1_ad_adj,             \
	s_##Key##_g1_ad_adj,             \
	s_##Key##_b1_ad_adj,             \
    s_##Key##_r1_offset,             \
    s_##Key##_g1_offset,             \
    s_##Key##_b1_offset  



enum
{
    ParPrj = 0,

	//cfg_burn_software_sel,
	cfg_pwr_cvr_sel,
	cfg_pio_chk_sel,
	cfg_adc_chk_sel,
	cfg_w_mac_addr_sel,
	cfg_auto_write_mac_sel,	
	cfg_rf_frequency_test_sel,
	cfg_rf_power_test_sel,
	cfg_rf_send_test_sel,
	cfg_rf_recv_test_sel,	
	cfg_show_mac_addr_sel,
	
	
	ParSuperPassword,
	ParDateValid,
	ParNewYear,
	ParNewMonth,
	ParNewDay,
	ParCfjCreatDate_temp,
	ParATEVersion,
	ParCfjCreatDate,
	ParSetParPassword,
	ParCfgFilePassword,
	ParModifyDatePassword,
	ParAuthorizePassword,
	ParLanguage,
	
	ParErrCmdRecord,
	ParErrTerminal,
	slaveSaveCfjRealTime,
	//parPwrOnDelayTime,
	sWaitContinueRun,
	slaveParAutoRun,
	s_BModle_Sel,
	
	//slave head cfj
	slaveParComment,
	slaveParSn,
	slaveParOld_Sn,
	slaveParRecord,
	slaveParRecordfile,
	slaveParRecordDir,
	//slaveCTInterval,
	
	//spi connect config
	//slaveUsbConnectEn,
	slaveConnectTimes,
    
	//current test config
	ParMultimeterPort,
	ParMulTryCOMMum,
	//pwr off current config
	s_PwrOff_cur_sel,
	s_PwrOff_cur_min,
    s_PwrOff_cur_max,
	s_PwrOff_cur_stable_time,
	s_PwrOff_cur_resistor,
    
	//pwr on current config
	s_PwrOn_cur_sel,
    s_PwrOn_cur_min,
    s_PwrOn_cur_max,
	s_PwrOn_cur_stable_time,
	s_PwrOn_cur_resistor,

	//flash write config
	s_write_file_sel,
	s_keep_cspar_sel,
	s_write_flash_filename,
	
	//bt address config
	s_addr_verify_sel,
    s_defaddr_sel,
	s_InputAddress_man,
	s_readAddress_sel,
	slaveWAddressSel,
	s_defaddr_include_jx,
	//slaveIncAddrOnlyOk,
	slaveDefAddrWOnly,
	slaveDefaultLap,
	slaveAddrNap_Uap,
	slaveAddrLap,
	slaveTerminalAddrLap,
	slaveDefaultNap_Uap,
	//mac_address
	BsParWriteMACAddrSel,
	BsParWriteMAC1StartE2pAddr,
	BsParWriteMAC2StartE2pAddr,
	BsParWriteMAC3StartE2pAddr,	
	BsWriteMacaddrNumber,
	BsMacaddr,
	BsMacaddnum,
	//tbox config
	parTestBoxNeed,
	ParTestBoxMaxTime,
	s_close_tbox_delay,

	//bt self-function config
	s_verRead_sel,
	s_readName_sel,
	
	//pio test confi
	s_io_test_sel,
	ParBtIOTest,
	s_sel_project_id,
	enum_pio_item(9),
	
	s_uart_loopback_test_sel,
	s_uart_dut1_portname,
	s_uart_dut1_baudrate,
	s_uart_dut2_portname,
	s_uart_dut2_baudrate,
	
	//rf test device config
	s_N4010A_TEST_Sel,
	ParBtTestSetsDevs,
	ParBtTestSetsDev,
	ParN4010A_Dly_time,	
	ParN4010A_Port,
	ParN4010A_Probedev,	
	
	//rf freq offset test
	ParN4010A_FreTestSel,
	ParN4010A_TxFre,	
	ParN4010A_freqOffset,
	ParFreqIndex,
	ParFreqMinIndex,
	ParFreqMaxIndex,
	ParN4010A_FBW,	
	ParN4010A_FAcqtime,	
	
	//rf pw test
	ParN4010A_PWLEL,
	ParN4010A_MinPWLEL,
	ParN4010A_MaxPWLEL,
	ParN4010A_MinPW,
	ParN4010A_MaxPW,
	ParN4010A_PWLChTestSel,
	ParN4010A_PWLTxCh,	
	ParN4010A_PWMChTestSel,
	ParN4010A_PWMTxCh,
    ParN4010A_PWHChTestSel,
	ParN4010A_PWHTxCh,
	ParN4010A_PWFBW,
	ParN4010A_Inputpower,
	ParN4010A_PWAcqtime,
	
	//rf single sens test
    ParN4010A_SSPacktes,
	ParN4010A_SSLChTestSel,
	ParN4010A_SSLRxCh,
	ParN4010A_SSMChTestSel,
	ParN4010A_SSMRxCh,	
    ParN4010A_SSHChTestSel, 
	ParN4010A_SSHRxCh,	
	ParN4010A_SSPERMax,  
	ParN4010A_SSTxPwr,   
		
//20171122
  	Par_a_evm_loss,
	Par_a_evm_lossh,
	Par_a_evm_lossm,
	Par_b_evm_loss,
	Par_b_evm_lossh,
	Par_b_evm_lossm,
	
	Par_a_sens_loss,
	Par_a_sens_lossh,
	Par_a_sens_lossm,
	Par_b_sens_loss,
	Par_b_sens_lossh,
	Par_b_sens_lossm,
	
	Par_instrument_compensate,
	Par_evm_max,
	Par_bw20_bw40_df,
	Par_ofdm_bw40_df,
	Par_sens_err_per_max,

	Par_read_soft_ver,
	Par_set_soft_ver,	//fu add ver
	Par_instru_foff_loss,	//fu add instrument loss foffset
	Par_mcu_io_chk,
	Par_8711_io_chk,
	Par_mf_evm_chk,
	Par_lf_evm_chk,
	Par_hf_evm_chk,

    //audio path config
	s_audio_rec_max_time,
	s_audio_max_time,
	s_audio_watch_times,
	s_audio_pc_AD_min,
	s_audio_pc_THD_max,
	s_DevA_LCH_pathLoss,
        s_DevA_MCH_pathLoss,
        s_DevA_HCH_pathLoss,
        s_DevB_LCH_pathLoss,
        s_DevB_MCH_pathLoss,
        s_DevB_HCH_pathLoss,
	s_image_file,
	s_image_crc32,
	s_wimageBkXtal,
	s_wimageBkAddr,
	s_mic_ch_mode,
	s_spk_ch_mode,
	


	// for sys par
        enum_golden_par_item(dut),
        enum_golden_par_item(G11),
        enum_golden_par_item(G12),
        enum_golden_par_item(G13),
        enum_golden_par_item(G21),
        enum_golden_par_item(G22),
        enum_golden_par_item(G23),
        enum_golden_par_item(G31),
        enum_golden_par_item(G32),
        enum_golden_par_item(G33),
        enum_golden_par_item(G41),
        enum_golden_par_item(G42),
        enum_golden_par_item(G43),



	
	//set par and rf_pwr_cal
	slaveXtalAGoldenOffset,
	slaveXtalBGoldenOffset,
	slaveDefaultNapUapGroup,
    slaveGoldenNapUapGroup,
	s_RFPwrTbl,
	s_RFPwrStepTbl,
	s_RFPwrCenter,
	s_RFPwrFineMin,
	s_RFPwrFineMax,
	s_RFPwrAdjMin,
	s_RFPwrAdjMax,

};



enum {
	TPIO_0 = 0,   TPIO_1,   TPIO_2,   TPIO_3,   TPIO_4,   TPIO_5,    TPIO_6,   TPIO_7,
	TPIO_8,       TPIO_9,   TPIO_10,  TPIO_11,  TPIO_12,  TPIO_13,   TPIO_14,  TPIO_15,	
	
	TPIO_19 = 19, TPIO_20,  TPIO_21,  TPIO_22,  TPIO_23,  TPIO_24,   TPIO_25,  TPIO_26,
	TPIO_27,      TPIO_28,  TPIO_29,  TPIO_30,  TPIO_31,  TAIO_0,    TAIO_1,   MBIAS_A,
	MBIAS_B,      TAIO_2,   TAIO_3,
	TOIO_MAX,
};



#define	ENINPUT_IO_MASK		0x0030ffff
#define CFG_FILE_OP_LEVEL0      0
#define CFG_FILE_OP_LEVEL1      1
#define CFG_FILE_OP_LEVEL2      2
#define CFG_FILE_OP_LEVEL3      3

void add_item(CString &des,char *str,int len);
unsigned long hex_str_to_ulong(char *str,int num);
#define UINT64_BYTE(data,n)	(((((unsigned long*)&(data))[(n)/sizeof(long)])>>(((n)%sizeof(long))*8))&0xff)
int OpenRecordFile(const char* pfile,ofstream& fs,const char* url,bool new_file);
bool adjust_hexdigital(CString &address);
bool get_format_bt_addr(CString &address,struct bt_address_s &addr);


const char* in_pio_name[];
const char* out_pio_name[];
int find_inpio_by_name(const char* pio);
int find_outpio_by_name(const char* pio);
bool is_same_default_address(struct bt_address_s &addr);
bool cal_file_check_sum(const char* file,unsigned long &crc_32);
void round_1point(double &data);


#endif //CONFIG_H










