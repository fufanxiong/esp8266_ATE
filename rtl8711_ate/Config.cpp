// HsConfig.cpp : implementation file
//

#include "stdafx.h"
#include "Config.h"
#include "PD218.h"
#include "Newdevice.h"

#include <math.h>
#include <time.h>
#include <fstream.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h>


//------------------------------------------------------------------------
//CSR ATE V1.2.0.0
//------------------------------------------------------------------------

extern CString Cfj_file_ini;
extern CString Cfj_file_bin;

extern CString valid_date_pin;
extern CString authorization_pin;
extern CString setpar_password;
extern CString configfile_password;

const CString super_password_name = "super password";
//const CString super_password = "12345+54321";
const CString super_password = "123456";
const CString valid_date_f = "ATE date valid";
const CString valid_date_y = "ATE valid year";
const CString valid_date_m = "ATE valid month";
const CString valid_date_d = "ATE valid day";

const CString prj_name_item = "Prj";
const CString bt_ate_version_item = "RTL8711 ATE version";
CString bt_ate_version = " V2.0.0_ONLY_READ";

const CString support_counter_item =     "Support frequency counter";
const CString support_SpecAnaly_item =   "Support SpecAnaly device";
const CString support_btTestSets_item =  "Support BtTestSets device";
const CString support_counter_dev =      "53131A/53131B/SP3386/SP312B";
const CString support_SpecAnaly_dev =    "HP8563/R3131";
const CString support_BtTesetSets_dev =  "N4010A";

const CString support_btTestPro_item =  "IO Test Project ID";
const CString btIOTest_ProjectId =      "101-01/101-02/101-03";



const double RFPwrTblAry[CRF_PWR_TBL_LEN][2] = {
        {-2,     2},              //+1.0
        {-1.5,   2},              //+1.0
        {-1.0,   2},              //+1.0
        {-0.5,   2},              //+1.0
        {0,      2},              //+1.0
        {0.5,    2},              //+1.0
        {1.0,    0},              //+0
        {1.5,    0},              //+0
        {2.0,    0},              //+0
        {2.5,    0},              //+0
        {3.0,    0},              //+0
        {3.5,   -3},              //-1.2
        {4.0,   -4},              //-1.8
        {4.5,   -5},              //-2.45
        {5.0,   -5},              //-2.45
        {5.5,   -5}               //-2.45
};
const double RfPwrStepTblAry[8] = {
        0.5,                    // 2
        0.5,                    // 1
        0.0,                    // 0 default
       -0.3,                    //-1
       -0.4,                    //-2
       -0.5,                    //-3
       -0.6,                    //-4
       -0.65,                   //-5
};



const int always_update_item_arry[] = {
	ParPrj,
	ParATEVersion,
	ParCfjCreatDate,
//	ParFCountDevs,
	//ParSpecDevs,
};
const int copy_item_arry[][2] = {

	{ParCfjCreatDate_temp,ParCfjCreatDate},
};

const int skip_item_arry[] = {
	    ParCfjCreatDate_temp,
		ParSuperPassword,
		ParSetParPassword,
		ParCfgFilePassword,
		ParModifyDatePassword,
		ParAuthorizePassword,
		
		ParDateValid,
		ParNewYear,
		ParNewMonth,
		ParNewDay,
		ParCfjCreatDate_temp,
};

const int skip_item_arry2[] = 
{
	    ParErrCmdRecord,
		ParErrTerminal,
		slaveSaveCfjRealTime,
        ParLanguage,
		
        slaveParSn,
        //slaveParOldSn,
        //sWaitContinueRun,	
        s_RFPwrTbl,
};



const char* in_pio_name[] = {
	"PIO_00","PIO_01","PIO_02","PIO_03","PIO_04","PIO_05","PIO_06","PIO_07",
	"PIO_08","PIO_09","PIO_10","PIO_11","PIO_12","PIO_13","PIO_14","PIO_15",
	"PCM_DATA",		//PIO_16
	"PCM_SYNC",		//PIO_17
	"UART_OUT",		//PIO_18
	"PCM_CLK",		//PIO_19
	"AIO_0",		//PIO_20
	"AIO_1",		//PIO_21
	"LED_0",		//PIO_22
	"LED_1",		//PIO_23
	"PIO_ERR","PIO_ERR","PIO_ERR","PIO_ERR","PIO_ERR","PIO_ERR","PIO_ERR","PIO_ERR",
	"PIO_ERR","PIO_ERR","PIO_ERR","PIO_ERR","PIO_ERR",
	0
};
const char* out_pio_name[] = {
	"PIO_00",  "PIO_01",   "PIO_02",  "PIO_03",   "PIO_04",   "PIO_05",  "PIO_06",  "PIO_07",
	"PIO_08",  "PIO_09",   "PIO_10",  "PIO_11",   "PIO_12",   "PIO_13",  "PIO_14",  "PIO_15",
	"PIO_16",  "PIO_17",   "PIO_18",  "PCM_SYNC",   "PCM_CLK",   "PIO_21",  "PIO_22",  "PIO_23",
	"PIO_24",  "PIO_25",   "PIO_26",  "PIO_27",   "PIO_28",   "LED_0",  "LED_1",  "LED_2",
	"AIO_0",   "AIO_1",    "MBIAS_A", "MBIAS_B",  "AIO_2",    "AIO_3",   "PIO_ERR",
	0
};


#define NEW_BLANK_LINE()        NewItem(slaveParComment,	"//",	        "")

#define	NEW_GLODEN_PAR(Key,KNAME,ADDR)	do {					                                   \
	NewItem(s_##Key##_address,	 KNAME##" bt address",	     	          ADDR		        );     \
	NewItem(s_##Key##_version,	 KNAME##" bt version",		              "0x28"		    );     \
	NewItem(s_##Key##_name,	         KNAME##" bt name",		              "Onkyo W800BTL"	);     \
	NewItem(s_##Key##_Curent_min,	 KNAME##" current min",	              "7"		);             \
	NewItem(s_##Key##_Curent_max,	 KNAME##" current max",	              "13"		);      \
	NewItem(s_##Key##_OPAv1,			KNAME##" RF power LCH avg",   "13.0"	    );      \
	NewItem(s_##Key##_OPAv2,		KNAME##" RF power MCH avg",   "13.0"	    );      \
	NewItem(s_##Key##_OPAv3,		KNAME##" RF power HCH avg",   "13.0"	    );      \
	NewItem(s_##Key##_INPUTPWR,       KNAME##" RF input evm ",          "13.0"    );      \
	NewItem(s_##Key##_INPUTEVM,       KNAME##" RF input power ",          "-32.0"    );      \
	NewItem(s_##Key##_evm1,			KNAME##" RF evm LCH avg",   "-32.0"	    );      \
	NewItem(s_##Key##_evm2,		KNAME##" RF evm MCH avg",   "-32.0"	    );      \
	NewItem(s_##Key##_evm3,		KNAME##" RF evm HCH avg",   "-32.0"	    );      \
	NewItem(s_##Key##_ICFOFFSET,    KNAME##" freq offset(Hz)",			"2000"      );      \
	NewItem(s_##Key##_SSTxPwr,      KNAME##" sigle_sens tx power",       "-89.0"   );      \
}while(0)


#define	NEW_SENSOR_PAR(Key, KNAME)	do {					                                        \
	NewItem(s_##Key##_sel,	         KNAME##" color chk select",	            "no"	    );      \
	NewItem(s_##Key##_r0_ad_min,	 KNAME##" led0 red color AD min",	     	"0"		    );      \
	NewItem(s_##Key##_r0_ad_max,	 KNAME##" led0 red color AD max",	     	"1000"		);      \
	NewItem(s_##Key##_g0_ad_min,	 KNAME##" led0 green color AD min",	        "0"	        );      \
	NewItem(s_##Key##_g0_ad_max,	 KNAME##" led0 green color AD max",	        "1000"	    );      \
	NewItem(s_##Key##_b0_ad_min,	 KNAME##" led0 blue color AD min",	        "500"		);      \
	NewItem(s_##Key##_b0_ad_max,	 KNAME##" led0 blue color AD max",	        "4095"		);      \
	NewItem(s_##Key##_r0_ad_adj,     KNAME##" led0 red color AD adjusted",	    "500"		);      \
	NewItem(s_##Key##_g0_ad_adj, 	 KNAME##" led0 green color AD adjusted",	"500"		);      \
	NewItem(s_##Key##_b0_ad_adj,     KNAME##" led0 blue color AD adjusted",     "2750"	    );      \
	NewItem(s_##Key##_r0_offset,     KNAME##" led0 red color offset",           "500"	    );      \
	NewItem(s_##Key##_g0_offset,     KNAME##" led0 green color offset",         "500"	    );      \
	NewItem(s_##Key##_b0_offset,     KNAME##" led0 blue color offset",          "1000"	    );      \
	NewItem(s_##Key##_r1_ad_min,	 KNAME##" led1 red color AD min",	        "500"		);      \
	NewItem(s_##Key##_r1_ad_max,	 KNAME##" led1 red color AD max",	        "4095"		);      \
	NewItem(s_##Key##_g1_ad_min,     KNAME##" led1 green color AD min",	        "500"		);      \
	NewItem(s_##Key##_g1_ad_max,     KNAME##" led1 green color AD max",	        "4095"		);      \
	NewItem(s_##Key##_b1_ad_min,     KNAME##" led1 blue color AD min",	        "500"       );      \
	NewItem(s_##Key##_b1_ad_max,     KNAME##" led1 blue color AD max",	        "4095"      );      \
	NewItem(s_##Key##_r1_ad_adj,     KNAME##" led1 red color AD adjusted",      "2750"	    );      \
	NewItem(s_##Key##_g1_ad_adj,     KNAME##" led1 green color AD adjusted",    "2750"	    );      \
	NewItem(s_##Key##_b1_ad_adj,     KNAME##" led1 blue color AD adjusted",     "2750"	    );      \
	NewItem(s_##Key##_r1_offset,     KNAME##" led1 red color offset",           "1000"	    );      \
	NewItem(s_##Key##_g1_offset,     KNAME##" led1 green color offset",         "1000"	    );      \
	NewItem(s_##Key##_b1_offset,     KNAME##" led1 blue color offset",          "1000"	    );      \
}while(0)


#define NewItem(ParIndex,name,value)                       				\
	do{	pItem = new Cconfig_item(ParIndex,name,value);				    \
	        pItem->dNode.add_tail(cfglist);						        \
	}while(0)

////BT ps key check config
#define	PSKEY_TEST(Key,KNAME,INDEX,VALUE)	do {					    \
	NewItem(s_psKey##Key##_sel,	KNAME##"_ck test sel",	"no");			\
	NewItem(s_psKey##Key##_index,	KNAME##"_ck index",	INDEX);			\
	NewItem(s_psKey##Key##_string,	KNAME##"_ck type string","yes");	\
	NewItem(s_psKey##Key##_value,	KNAME##"_ck value",	VALUE);			\
}while(0)
#define	WPSKEY_TEST(Key,KNAME,INDEX,VALUE)	do {					    \
	NewItem(s_wpsKey##Key##_sel,	KNAME##"_write test sel","no");	    \
	NewItem(s_wpsKey##Key##_index,	KNAME##"_write index",	INDEX);		\
	NewItem(s_wpsKey##Key##_value,	KNAME##"_write value",	VALUE);		\
}while(0)


#define	PIO_TEST(IO,IO_NAME)	do {					                \
	NewItem(s_pio_check##IO##_sel,	IO_NAME##" test sel",	"no");	    \
}while(0)


#define	AIO_TEST(IO,IO_NAME)	do {					                \
	NewItem(s_aio_check##IO##_sel,	IO_NAME##" test sel",	"no");	    \
}while(0)

#define	MBIAS_TEST(IO,IO_NAME)	do {					                \
	NewItem(s_mbias_check##IO##_sel,	IO_NAME##" test sel",	"no");  \
}while(0)



ConfigFile::ConfigFile()
{
	cfglist = NULL;
	dirty = false;
	 writing_file = false;
        InitializeCriticalSection(&update_cfg_file_cs);

	CString Sn;
	unsigned long Int_Sn = (unsigned)time(NULL);
	Sn.Format("%d",Int_Sn);
	CString OldSn = Sn+"_old";

	CTime Time = CTime::GetCurrentTime();
	CString creat_date = Time.Format("%Y-%m-%d %H:%M:%S");

	Cconfig_item *pItem;

    CString RFPwrTable,RFPwrStepTable,pwr_temp;
        for(int i=0;i<CRF_PWR_TBL_LEN;i++)
        {
                if(i) pwr_temp.Format(" %.2lf %.0lf", RFPwrTblAry[i][0], RFPwrTblAry[i][1]);
                else pwr_temp.Format("%.2lf %.0lf", RFPwrTblAry[i][0], RFPwrTblAry[i][1]);
                RFPwrTable += pwr_temp;

                if(i) pwr_temp.Format(" %.2lf", RfPwrStepTblAry[i]);
                else pwr_temp.Format("%.2lf", RfPwrStepTblAry[i]);
                RFPwrStepTable += pwr_temp;
        }

	NewItem(slaveParComment,	    "//config file info",	    ""		        );
	NewItem(ParPrj,			        prj_name_item,			    "RTL87XX ATE config"	);
	

	//NewItem(cfg_burn_software_sel,		    "burn software sel",			    "no"			);
	NewItem(cfg_pwr_cvr_sel,		    "electric current test sel",			    "no"			);
	NewItem(cfg_pio_chk_sel,		    "IO test sel",			    "no"			);
	NewItem(cfg_adc_chk_sel,		    "ADC test sel",			    "no"			);
	NewItem(cfg_w_mac_addr_sel,		    "write MAC addr sel",			    "no"			);
	NewItem(cfg_auto_write_mac_sel,		    "auto write MAC sel",			    "no"			);
	NewItem(cfg_rf_frequency_test_sel,		    "RF fre test sel",			    "no"			);
	NewItem(cfg_rf_power_test_sel,		    "RF power test sel",			    "no"			);
	NewItem(cfg_rf_send_test_sel,		    "RF send test sel",			    "no"			);
	NewItem(cfg_rf_recv_test_sel,		    "RF recv test sel",			    "no"			);
	NewItem(cfg_show_mac_addr_sel,		    "show MAC addr sel",			    "no"			);

	
//ini文件不显示	
	NewItem(ParSuperPassword,	    super_password_name,		super_password  );
	NewItem(ParDateValid,		    valid_date_f,			    "no"			);
	NewItem(ParNewYear,		        valid_date_y,			    "2020"			);
	NewItem(ParNewMonth,		    valid_date_m,			    "12"			);
	NewItem(ParNewDay,		        valid_date_d,			    "31"			);
	NewItem(ParCfjCreatDate_temp,	"config creat date temp",	creat_date		);
//ini文件不显示

	
	NewItem(ParATEVersion,		    bt_ate_version_item,		bt_ate_version	);
	NewItem(ParCfjCreatDate,	    "config creat date",		creat_date		);


//ini文件不显示	
	NewItem(ParSetParPassword,	    "setpar password",		    setpar_password	);
	NewItem(ParCfgFilePassword,	    "config file password",		configfile_password	);
	NewItem(ParModifyDatePassword,	"modify date password",		valid_date_pin	);
    	NewItem(ParAuthorizePassword,	"authorize PC password",	authorization_pin );
	NewItem(ParLanguage,	        "display language",	        "en"			);	
	NewItem(ParErrCmdRecord,	    "Record err cmd",		          "yes"		);
	NewItem(ParErrTerminal,		    "terminal when err",		      "yes"		);
	NewItem(slaveSaveCfjRealTime,	"save config file every test",	  "yes"		);
	//NewItem(parPwrOnDelayTime,	    "test pwr on delay",		  "2500"    );
	NewItem(slaveParSn,		        "BT SN",			             Sn			);
//ini文件不显示

	
	NewItem(slaveParOld_Sn,		    "BT Old SN",			         OldSn		);
	NewItem(slaveParRecord,		    "BT record test",		         "yes"		);
	NewItem(slaveParRecordfile,	    "BT record file",		         "bt_log"   );
	NewItem(slaveParRecordDir,	    "BT record directory",		     "log"		);
	//NewItem(slaveCTInterval,	    "BT continue interval",		     "2000"		);
	NewItem(slaveParComment,	    "//test special character config",""		);
	NewItem(sWaitContinueRun,	    "BT wait continue run when err",  "no"	    );
	NewItem(slaveParAutoRun,	    "BT auto run enable",		      "yes"		);
	NewItem(s_BModle_Sel,		    "BT B model test select",	      "yes"		);	
	NewItem(slaveParComment,	    "//spi connect config",		      ""		);
	//NewItem(slaveUsbConnectEn,	"BT can use usb connect",		  "no"		);
	NewItem(slaveConnectTimes,	    "BT try connect times(*100ms)",	  "20"		);	
	NewItem(slaveParComment,	    "//Multimeter config",	        ""		    );
	NewItem(ParMultimeterPort,	    "Multimeter com port",		    "COM1"	    );
	NewItem(ParMulTryCOMMum,	    "Multimeter try com num",	    "5"	        );	
	NewItem(slaveParComment,	      "//pwr off current config",		          ""	 );
	NewItem(s_PwrOff_cur_sel,         "BT power off current sel",	              "no"	 );
	NewItem(s_PwrOff_cur_min,	      "BT power off current min(uA)",	          "0.0"  );
       NewItem(s_PwrOff_cur_max,	      "BT power off current max(uA)",	          "3.0"  );
	NewItem(s_PwrOff_cur_stable_time, "BT power off current stable time(ms)",	  "1000" );
       NewItem(s_PwrOff_cur_resistor,	  "BT power off current use resistor(ohm)",   "1000" );
 
 	NewItem(slaveParComment,	      "//pwr on current config",		           ""	  );
	NewItem(s_PwrOn_cur_sel,          "BT power on current sel",	               "no"   );
    	NewItem(s_PwrOn_cur_min,          "BT power on current min(mA)",	           "0"	  );
    	NewItem(s_PwrOn_cur_max,          "BT power on current max(mA)",	           "30"  );
	NewItem(s_PwrOn_cur_stable_time,  "BT power on current stable time",	       "2000" );
	NewItem(s_PwrOn_cur_resistor,     "BT power on current use resistor(ohm)",     "0.1"  );
	
	NewItem(slaveParComment,	      "//flash write config",		          ""	      );
	NewItem(s_write_file_sel,	      "BT write flash select",	              "no"		  );
	NewItem(s_keep_cspar_sel,	      "BT keep cs par select",	              "no"		  );
	NewItem(s_write_flash_filename,	  "write flash filename",				 "./loader/flash.xuv"  );
	
	NewItem(slaveParComment,	      "//bt address config",		          ""	      );
	NewItem(s_readAddress_sel,	      "BT read addr sel",	                  "yes"		  );
	NewItem(s_addr_verify_sel,	      "BT label addr verify sel",	          "no"		  );
    	NewItem(s_defaddr_sel,	          "BT default addr verify sel",	          "no"		  );
	NewItem(s_InputAddress_man,	      "BT input addr manually",	              "no"		  );	
	NewItem(slaveWAddressSel,	      "BT write addr sel",	                  "no"		  );
	NewItem(s_defaddr_include_jx,	  "BT default addr included tested",      "no"	      );
	//NewItem(slaveIncAddrOnlyOk,	      "BT increase addr only all ok",	      "yes"		  );
	NewItem(slaveDefAddrWOnly,	      "BT write default address only",        "no"		  );
	NewItem(slaveDefaultNap_Uap,	  "BT default nap&uap",		              "784405"	  );
	NewItem(slaveDefaultLap,	      "BT default lap",		                  "000000"	  );
	NewItem(slaveAddrNap_Uap,	      "BT address nap&uap",		              "784405"	  );
	NewItem(slaveAddrLap,		      "BT address current lap",		          "000000"	  );
	NewItem(slaveTerminalAddrLap,	  "BT address terminal lap",	          "ffffff"	  );	
	
	NewItem(slaveParComment,	      "//tbox config",		                  ""	      );
	NewItem(parTestBoxNeed,		      "test box must need",		              "no"	      );
	NewItem(ParTestBoxMaxTime,	      "test box comm read total timeout",	  "5000"	  );
	NewItem(s_close_tbox_delay,	      "test box connect to dut wait time",	  "5000"	  );//yhl 2018/11/20
	
	NewItem(slaveParComment,	      "//bt self-attributes config",		  ""	      );
	NewItem(s_verRead_sel,		      "BT read major version select",	      "yes"		  );
	NewItem(s_readName_sel,		      "BT read name select",		          "yes"		  );
	
	NewItem(slaveParComment,	      "//pio test config",	             ""	            );
//	NewItem(s_io_test_sel,			  "BT io test sel",					"no"	);
//	NewItem(ParBtIOTest,			  support_btTestPro_item,		     btIOTest_ProjectId	);
//	NewItem(s_sel_project_id,		  "BT selcet project id",		     "101-02"		);
	PIO_TEST( 9,"BT PIO_09" );  
	
	NewItem(slaveParComment,	     "//uart loopback test config",	    ""          );
	NewItem(s_uart_loopback_test_sel, "BT uart loopback test sel",	        "no"		);
	NewItem(s_uart_dut1_portname,	     "BT uart test dut1's portname",	"COM13"		);
	NewItem(s_uart_dut1_baudrate,	     "BT uart test dut1's baudrate",	 "9600"		);
	NewItem(s_uart_dut2_portname,		 "BT uart test dut2's portname",    "COM14"		);
	NewItem(s_uart_dut2_baudrate,		 "BT uart test dut2's baudrate",    "9600"		);

	NewItem(slaveParComment,	    "//rf test device config",	    ""		                        );
	NewItem(s_N4010A_TEST_Sel,	     "BT rf test sel",		             "no"		                );
	NewItem(ParBtTestSetsDevs,	     support_btTestSets_item,		     support_BtTesetSets_dev	);
	NewItem(ParBtTestSetsDev,	     "rf test use device",		         "N4010A"	                );
	NewItem(ParN4010A_Dly_time,		"N4010A delay time",	             "500"		                );
	NewItem(ParN4010A_Port,			"N4010A port",						 "GPIB0::10::INSTR"	     );
	NewItem(ParN4010A_Probedev,		"N4010A probe name",				  "N4010A"		         );

	NewItem(slaveParComment,	     "//rf freq offset test",	              ""            );
	NewItem(ParN4010A_FreTestSel,	"N4010A  low freq test sel",			 "no"	        );
	NewItem(ParN4010A_TxFre,		"N4010A  tx freq (MHz)",			"2412"	        );
	NewItem(ParN4010A_freqOffset,   "N4010A  freq offset (Hz)",					  "10000"	    );
	NewItem(ParFreqIndex,			"N4010A  freq par index",				"32"			);
	NewItem(ParFreqMinIndex,		"N4010A  freq par min index",				"0"			);
	NewItem(ParFreqMaxIndex,		"N4010A  freq par max index",				"63"			);
	NewItem(ParN4010A_FBW,			"N4010A  freq bandwidth (MHz)",			"1.3"		);
	NewItem(ParN4010A_FAcqtime,		"N4010A  freq acquisition Time (us)",		"625"		);
	
	NewItem(slaveParComment,	     "//rf pw test",	              ""            );
	NewItem(ParN4010A_PWLEL,		"N4010A  default power level",			"45"	    );
	NewItem(ParN4010A_MinPWLEL,		"N4010A  min power level",				 "0"	    );
	NewItem(ParN4010A_MaxPWLEL,		"N4010A  max power level",				"63"	    );
	NewItem(ParN4010A_MinPW,		"N4010A  min power",					  "12"	    );
	NewItem(ParN4010A_MaxPW,		"N4010A  max power",					  "14"	    );
	NewItem(ParN4010A_PWLChTestSel,	"N4010A  low channel test sel",			 "no"	        );
	NewItem(ParN4010A_PWLTxCh,		"N4010A  low tx channel ",					 "1"	        );
	NewItem(ParN4010A_PWMChTestSel,	"N4010A  mid channel test sel",			 "yes"	        );
	NewItem(ParN4010A_PWMTxCh,		"N4010A  mid tx channel ",				 "7"			);
    NewItem(ParN4010A_PWHChTestSel,	"N4010A  high channel test sel",			 "no"	        );
	NewItem(ParN4010A_PWHTxCh,		"N4010A  high tx channel ",			  "13"		);
	NewItem(ParN4010A_PWFBW,		"N4010A  pw bandwidth (MHz)",				"1.3"		);
	NewItem(ParN4010A_Inputpower,	"N4010A  pw Input power (dBm)",				"10"		);
	NewItem(ParN4010A_PWAcqtime,	"N4010A  pw Acquisition Time (us)",			"625"		);
	
	NewItem(slaveParComment,	     "//rf single sens test",	                     ""             );
    NewItem(ParN4010A_SSPacktes,    "N4010A sigle_sens num of packets",            "100"	        );
	NewItem(ParN4010A_SSLChTestSel,  "N4010A sigle_sens low channel test sel",         "no"	        );
	NewItem(ParN4010A_SSLRxCh,		"N4010A sigle_sens low rx channel ",			"0"	        );
	NewItem(ParN4010A_SSMChTestSel,  "N4010A sigle_sens mid channel test sel",         "yes"	        );
	NewItem(ParN4010A_SSMRxCh,		"N4010A sigle_sens low rx channel ",			"20"	        );
    NewItem(ParN4010A_SSHChTestSel,  "N4010A sigle_sens high channel test sel",        "no"	        );	
	NewItem(ParN4010A_SSHRxCh,		"N4010A sigle_sens low rx channel ",			"39"	        );
	NewItem(ParN4010A_SSPERMax,     "N4010A sigle_sens max per",                   "0.1"	        );
	NewItem(ParN4010A_SSTxPwr,      "N4010A sigle_sens tx power (dBm)",                  "-89"        );


		
//20171122
	NewItem(Par_a_evm_loss,			"Par_a_evm_loss",			"0.0"	    );
	NewItem(Par_a_evm_lossh,		"Par_a_evm_lossh",				 "0.0"	    );
	NewItem(Par_a_evm_lossm,		"Par_a_evm_lossm",				"0.0"	    );
	NewItem(Par_b_evm_loss,			"Par_b_evm_loss",					  "0.0"	    );
	NewItem(Par_b_evm_lossh,		"Par_b_evm_lossh",					  "0.0"	    );
	NewItem(Par_b_evm_lossm,		"Par_b_evm_lossm",			"0.0"	    );
	
	NewItem(Par_a_sens_loss,			"Par_a_sens_loss",			"0.0"	    );
	NewItem(Par_a_sens_lossh,		"Par_a_sens_lossh",				 "0.0"	    );
	NewItem(Par_a_sens_lossm,		"Par_a_sens_lossm",				"0.0"	    );
	NewItem(Par_b_sens_loss,			"Par_b_sens_loss",					  "0.0"	    );
	NewItem(Par_b_sens_lossh,		"Par_b_sens_lossh",					  "0.0"	    );
	NewItem(Par_b_sens_lossm,		"Par_b_sens_lossm",			"0.0"	    );
	
	NewItem(Par_instrument_compensate,			"Par_instrument_compensate",				 "15.0"		);
	NewItem(Par_evm_max,			"Par_evm_max",				 "0.0"	    );
	NewItem(Par_bw20_bw40_df,		"Par_bw20_bw40_df",				"0"	    );
	NewItem(Par_ofdm_bw40_df,		"Par_ofdm_bw40_df",				"0"	    );
	NewItem(Par_sens_err_per_max,	"sens_err_per_max",					  "10"	    );


	NewItem(Par_read_soft_ver, "Par_read_soft_ver", 		"yes"			);
	
	NewItem(Par_set_soft_ver,		"Par_set_soft_verion",			"1.0.0"	    );		//fu add set ver
	NewItem(Par_instru_foff_loss,			"Par_instru_foff_loss",			"0.0"	    );	//fu add foffset instrument loss
	
	NewItem(Par_mcu_io_chk, "Par_mcu_io_chk", 		 "yes"			);
	NewItem(Par_8711_io_chk, "Par_8711_io_chk", 		 "yes"			);
	NewItem(Par_mf_evm_chk, "Par_mf_evm_chk", 		 "yes"			);
	NewItem(Par_lf_evm_chk, "Par_lf_evm_chk", 		 "yes"			);
	NewItem(Par_hf_evm_chk, "Par_hf_evm_chk", 		 "yes"			);
	


	
	NewItem(slaveParComment,	             "//rf test golden par config",	            ""          );
	NewItem(s_DevA_LCH_pathLoss,	        "BT RF MT8852 Dev A LCH pathLoss",          "0.0"		);
    NewItem(s_DevA_MCH_pathLoss,	        "BT RF MT8852 Dev A MCH pathLoss",          "0.0"		);
    NewItem(s_DevA_HCH_pathLoss,	        "BT RF MT8852 Dev A HCH pathLoss",          "0.0"		);
    NewItem(s_DevB_LCH_pathLoss,	        "BT RF MT8852 Dev B LCH pathLoss",          "0.0"		);
    NewItem(s_DevB_MCH_pathLoss,	        "BT RF MT8852 Dev B MCH pathLoss",          "0.0"		);
    NewItem(s_DevB_HCH_pathLoss,	        "BT RF MT8852 Dev B HCH pathLoss",          "0.0"		);
#if 0
	NewItem(s_image_file,		            "BT image file name",		               "V503-JX239-02-794B2A1F.vfi");
	NewItem(s_image_crc32,		            "BT image crc32",		                   "E8C135FF"	);
	NewItem(s_wimageBkXtal,		            "BT image write back xtal",	               "yes"		);
	NewItem(s_wimageBkAddr,		            "BT image write back address",	           "yes"		);
	NewItem(s_mic_ch_mode,	                "BT audio mic ch out mode",	               "3"			);
	NewItem(s_spk_ch_mode,	                "BT audio spk ch out mode",	               "3"			);
#endif
	
	NewItem(slaveParComment,	    "//gloden sys par config",	""		                    );
    NEW_GLODEN_PAR(dut,             "dut",                          "78440500A509"          );
    NEW_BLANK_LINE();
    NEW_GLODEN_PAR(G11,             "gloden11",                     "7844053454fe"          );
    NEW_BLANK_LINE();
    NEW_GLODEN_PAR(G12,             "gloden12",                     "7844053454ff"          );
    NEW_BLANK_LINE();
    NEW_GLODEN_PAR(G13,             "gloden13",                     "784405345500"          );
    NEW_BLANK_LINE();
    NEW_GLODEN_PAR(G21,             "gloden21",                     "784405019423"          );
    NEW_BLANK_LINE();
    NEW_GLODEN_PAR(G22,             "gloden22",                     "78440501BB25"          );
    NEW_BLANK_LINE();
    NEW_GLODEN_PAR(G23,             "gloden23",                     "784405019420"          );
    NEW_BLANK_LINE();
    NEW_GLODEN_PAR(G31,             "gloden31",                     "256020048031"          );
    NEW_BLANK_LINE();
    NEW_GLODEN_PAR(G32,             "gloden32",                     "256020048032"          );
    NEW_BLANK_LINE();
    NEW_GLODEN_PAR(G33,             "gloden33",                     "256020048033"          );
    NEW_BLANK_LINE();
    NEW_GLODEN_PAR(G41,             "gloden41",                     "256020048041"          );
    NEW_BLANK_LINE();
    NEW_GLODEN_PAR(G42,             "gloden42",                     "256020048042"          );
    NEW_BLANK_LINE();
    NEW_GLODEN_PAR(G43,             "gloden43",                     "256020048043"          );

#if 0
	NewItem(slaveParComment,	        "//gloden set par and rf_pwr_cal",	   ""		        );
	NewItem(slaveXtalAGoldenOffset,	    "BT xtal adj A golden freq offset",    "0.0"			);
	NewItem(slaveXtalBGoldenOffset,	    "BT xtal adj B golden freq offset",    "0.0"		    );
	NewItem(slaveDefaultNapUapGroup,    "BT default nap&uap group",	           "A5A5A5"		    );
  	NewItem(slaveGoldenNapUapGroup,     "BT golden  nap&uap group",	           "15801A"		    );
	NewItem(s_RFPwrTbl,                 "BT RF power table",                   RFPwrTable	    );
	NewItem(s_RFPwrStepTbl,             "BT RF power step table",              RFPwrStepTable	);
	ini_rf_pwr_tbl();
	NewItem(s_RFPwrCenter,              "BT RF power center",                  "2.0"		    );
	NewItem(s_RFPwrFineMin,             "BT RF power fine min",                "1.0"		    );
	NewItem(s_RFPwrFineMax,             "BT RF power fine max",                "3.0"		    );
	NewItem(s_RFPwrAdjMin,              "BT RF power adjust min",              "-0.5"		    ); 
	NewItem(s_RFPwrAdjMax,              "BT RF power adjust max",              "4.5"		    );
#endif

}

	
ConfigFile::~ConfigFile()
{
#if 0
	if(dirty) writefile(Cfj_file_bin);

	CdNode<Cconfig_item> *pItem;
	Cconfig_item *pTItem;
	do{
		if(!(pItem = cfglist)) break;
		pTItem = pItem->cur;
		pItem->del(cfglist);
		delete(pTItem);
	}while (true);
#endif
	update_config_to_disk();
	CdNode<Cconfig_item> *pItem;
	Cconfig_item *pTItem;
	do{
		if(!(pItem = cfglist)) break;
		pTItem = pItem->cur;
		pItem->del(cfglist);
		delete(pTItem);
	}while (true);
        DeleteCriticalSection(&update_cfg_file_cs);
}

void ConfigFile::update_config_to_disk(void)
{
#if 0
    if(dirty) writefile(Cfj_file_bin);
    dirty = 0;
#endif

	if(!dirty || writing_file) return;
        if(TryEnterCriticalSection(&update_cfg_file_cs))
        {
                writing_file = true;

                CString ftemp = Cfj_file_bin + ".temp";
                CString fback = Cfj_file_bin + ".back";
                writefile(ftemp,true,CFG_FILE_OP_LEVEL0);
                rename(Cfj_file_bin,fback);
                rename(ftemp,Cfj_file_bin);
                DeleteFile(fback);

                writing_file = false;
                dirty = false;
                LeaveCriticalSection(&update_cfg_file_cs);
        }


}

///record cfg file
int ConfigFile::record_config(CString &file)
{
    //return(nomal_writefile(file, true));
	 return(nomal_writefile(file,CFG_FILE_OP_LEVEL2));
}

//////////////////////////////////////////////////////////////////////////


bool ConfigFile::wloader_crc32_is_ok(unsigned long &xpv_crc32,unsigned long &xdv_crc32)
{
	xpv_crc32 = 0;
	xdv_crc32 = 0;
	unsigned long crc32 = 0;
	
	CString write_file ;
	if(write_file == "") return(false);
	if(!cal_file_check_sum(write_file,crc32)) return(false);
	xpv_crc32 = crc32;
	
//	write_file = VCString(s_wload_xdv_name);
	if(write_file == "") return(false);
	if(!cal_file_check_sum(write_file,crc32)) return(false);
	xdv_crc32 = crc32;
	
//	if((crc32 != Vulong(s_wload_xdv_crc32,true)) || 
//		(xpv_crc32 != Vulong(s_wload_xpv_crc32,true))) return(false);
	return(true);
}



//---------------------------------------------------------------------------
// config item constructor
//---------------------------------------------------------------------------
int Cconfig_item::itemSum = 0;
Cconfig_item::Cconfig_item(int PIdex,const char *Pname,const char *Pvalue)
{
	ParIndex = PIdex;
	if(Pname && ((Pname[0]&&('/' == Pname[0])) && (Pname[1]&&('/' == Pname[1])))) {
		name = Pname;
	}else {
		itemSum++;
		name.Format("%.3d  ",itemSum);
		name += Pname;
	}
	value = Pvalue;
	dNode.cur = this;
}

Cconfig_item::~Cconfig_item()
{
	if((name.GetLength() >= 1)&&('/' != name[0]))  --itemSum;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const char* ConfigFile::cmp_key(const char *pKey,const char *pdes)
{
	if(!*pKey++ || !*pKey++ || !*pKey++ || 
	   !*pdes++ || !*pdes++ || !*pdes++) return(NULL);
	
	while(*pKey) {
		if(!*pdes) return(NULL);
		if(*pKey++ != *pdes++) return(NULL);
	}

	while(*pdes == ' ') ++pdes;			// fined '='
	if(*pdes++ != '=') return(NULL);
	
	while(*pdes && (*pdes == ' ')) ++pdes;		// delete ' '
	if(!*pdes) return(NULL);
	return(pdes);
}

bool ConfigFile::findItemFromFile(const CString &Key,CString &val,const CString &file)
{
	const int DALEN = 1024;
	bool RValue = false;

	if((Key == "") || (file == "")) return(RValue);
	ifstream ifs(file);
	if(!ifs.is_open()) return(RValue);
   
	const char *p;
	char *p2;
	char buf[DALEN];
	while(!ifs.eof()) {				// find key line
		ifs.getline(buf,DALEN);
		if(ifs.eof() || ifs.fail()) break;
		if(buf[0] == '\0') continue;

		p = buf;                            	// delete ' '
		while(*p && (*p == ' ')) ++p;
		if(!*p) continue;

		if(!(p = cmp_key(Key,p))) continue;
		RValue = true;
		break;
	}
	if(false == RValue) goto __find_err;
	
	p2 = (char*)p;
	while(*p2 && (p2<&buf[DALEN])) ++p2;      	// delete tail ' '  
	if(*p2--) goto __find_err;
	while(p2 > p) {
		if(*p2 != ' ') break;
		*p2 = 0;
		--p2;
	}

	val = "";
	while(*p) val += *p++;				// value Item
	RValue = true;

__find_err:
	ifs.close();
	return(RValue);
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
int ConfigFile::nomal_readfile(const CString &file,int level)
{
	if(file == "") return(false);

	ifstream ifs(file,ios::nocreate);
	if(!ifs.is_open()) return(false);
	ifs.close();

    bool should_input_item = false;
	bool have_new_item = false;
	CdNode<Cconfig_item> *pItem = cfglist;
	Cconfig_item *pContent;
	CString value;
	while(pItem) 
	{
		pContent = pItem->cur;
		
		should_input_item = false;
		if((CFG_FILE_OP_LEVEL0 == level) && ('/' == pContent->name.GetAt(0))) 
			should_input_item = true;
		else if(!is_skip_item(pContent,level)) 
			should_input_item = true;
		
		if(should_input_item) 
		{
			value = "";
			if(false == findItemFromFile(pContent->name,value,file)) have_new_item = true;
			else if(!is_always_update_item(pContent)) pContent->value = value;
			copy_item(pContent);
		}
		pItem = pItem->next;
		if(pItem == cfglist) break;
	}

	if(have_new_item) dirty = true;

	fileName = file;
	return(true);

}

bool ConfigFile::is_always_update_item(Cconfig_item *pContent)
{
	int item_num = sizeof(always_update_item_arry)/sizeof(int);

	for(int i=0;i<item_num;i++) {
		if(pContent->ParIndex == always_update_item_arry[i]) return(true);
	}
	return(false);
}

bool ConfigFile::copy_item(Cconfig_item *pContent)
{
	int item_num = sizeof(copy_item_arry)/(sizeof(int)*2);

	for(int i=0;i<item_num;i++) {
		if(pContent->ParIndex == copy_item_arry[i][0]) {
			ItemSet(copy_item_arry[i][1],pContent->value);
			return(true);
		}
	}
	return(false);
}


// less of skip_array2[]
bool ConfigFile::is_skip_item(Cconfig_item *pContent,int level)
{
	int item_num,i;
	
	if(CFG_FILE_OP_LEVEL0 == level) return(false);
	
	item_num = sizeof(skip_item_arry)/sizeof(int);
	for(i = 0; i < item_num; i++)
	{
		if(pContent->ParIndex == skip_item_arry[i]) return(true);
	}
	if(level <= CFG_FILE_OP_LEVEL1) return(false);
	
	item_num = sizeof(skip_item_arry2)/sizeof(int);
	for(i = 0; i < item_num; i++)
	{
		if(pContent->ParIndex == skip_item_arry2[i]) return(true);
	}
	return(false);
}

void ConfigFile::update_sn_value(int sn_value,int old_sn_value)
{
	CString Sn;
	unsigned long Int_Sn = (unsigned)time(NULL);
	Sn.Format("%d",Int_Sn);
	ItemSet(old_sn_value,VCString(sn_value));
	ItemSet(sn_value,Sn);
}

int ConfigFile::nomal_writefile(const CString &file,int level)
{
	if(file == "") return(false);
        
	ofstream fs;
	fs.open(file);
	if(!fs.is_open()) return(false);

	bool should_output_item;
	CdNode<Cconfig_item> *pItem = cfglist;
	Cconfig_item *pContent;
	CString w_content;
	bool is_first_comment = true;
	while(pItem) 
    {
		pContent = pItem->cur;
		
		should_output_item = false;
		if((CFG_FILE_OP_LEVEL0 == level) && (pContent->name != "")) 
			should_output_item = true;
		else if(!is_skip_item(pContent,level)) 
			should_output_item = true;
		
		if(should_output_item) 
		{
			if('/' == pContent->name.GetAt(0)) 
			{
				if(pContent->name == "//") w_content = "";
				else
				{
					if(!is_first_comment) fs << endl;
					else is_first_comment = false;
					w_content = pContent->name;
				}
			}
			else 
			{
				w_content = pContent->name + " = ";
				if(pContent->value != "") w_content += pContent->value;
			}
			fs<<w_content;
			fs<<endl;
		}
		pItem = pItem->next;
		if(pItem == cfglist) break;
	}

	fs.close();

	dirty = false;
	return(true);
}



int rand_sed = 0x1245;
int ConfigFile::writefile(const CString &file,bool bin_format,int level)
{
#if 0
	if(file == "") return(false);
	if(!bin_format) return(nomal_writefile(file,true));

	CString temp_file = file + ".temp";
	int res = nomal_writefile(temp_file);
	if(res != 1) return(res);

	ifstream ifs(temp_file,ios::nocreate);
	if(!ifs.is_open()) return(false);

	ofstream ofs(file,ios::binary);
	if(!ofs.is_open()) {
		ifs.close();
		return(false);
	}

	srand(rand_sed);
	char temp_char;
	while(!ifs.eof()) {
		ifs.get(temp_char);
		temp_char = (temp_char + (rand()%0xff))&0xff;
		ofs.put(temp_char);
	}

	ofs.close();
	ifs.close();
	remove(temp_file);

	dirty = false;
	return(true);
#endif


	if(file == "") 
		return(false);
	if(!bin_format) 
		return(nomal_writefile(file,level));

	CString temp_file = file + ".temp";
	int res = nomal_writefile(temp_file,CFG_FILE_OP_LEVEL0);
	if(res != 1) 
        {
                remove(temp_file);
                return(res);
        }

	ifstream ifs(temp_file,ios::nocreate);
	if(!ifs.is_open()) return(false);

	ofstream ofs(file,ios::binary);
	if(!ofs.is_open()) {
		ifs.close();
		return(false);
	}

	srand(rand_sed);
	char temp_char;
	while(!ifs.eof()) {
		ifs.get(temp_char);
		temp_char = (temp_char + (rand()%0xff))&0xff;
		ofs.put(temp_char);
	}

	ofs.close();
	ifs.close();
	remove(temp_file);

	dirty = false;
	return(true);



}

int ConfigFile::readfile(const CString &file,bool bin_format,int level)
{
#if 0
	if(file == "") return(false);
	if(!bin_format) 
	{
		int res = nomal_readfile(file,level);
		ini_rf_pwr_tbl();
		return(res);
	}

	ifstream ifs(file,ios::nocreate|ios::in|ios::binary);
	if(!ifs.is_open()) return(false);
        	
	CString temp_file = file + ".temp";
	ofstream ofs(temp_file);
	if(!ofs.is_open()) {
		ifs.close();
		return(false);
	}
	
	srand(rand_sed);
	char temp_char;
	while(!ifs.eof()) {
		ifs.get(temp_char);
		temp_char = (temp_char - (rand()%0xff))&0xff;
		ofs.put(temp_char);
	}

	ofs.close();
	ifs.close();

	int res = nomal_readfile(temp_file);
	remove(temp_file);
	ini_rf_pwr_tbl();

	return(res);
#endif

	if(file == "") return(false);
	if(!bin_format) 
    {
		int res = nomal_readfile(file,level);
		ini_rf_pwr_tbl();
		return(res);
    }

	ifstream ifs(file,ios::nocreate|ios::in|ios::binary);
	if(!ifs.is_open()) return(false);
        	
	CString temp_file = file + ".temp";
	ofstream ofs(temp_file);
	if(!ofs.is_open())
	{
		ifs.close();
		return(false);
	}
	
	srand(rand_sed);
	char temp_char;
	while(!ifs.eof())
	{
		ifs.get(temp_char);
		temp_char = (temp_char - (rand()%0xff))&0xff;
		ofs.put(temp_char);
	}

	ofs.close();
	ifs.close();

	int res = nomal_readfile(temp_file,CFG_FILE_OP_LEVEL0);
	remove(temp_file);

        ini_rf_pwr_tbl();
	return(res);


}

Cconfig_item *ConfigFile::search_item(int parindex)
{
	CdNode<Cconfig_item> *pItem = cfglist;
	while(pItem) {
		if(pItem->cur->ParIndex == parindex) 
			return(pItem->cur);
		pItem = pItem->next;
		if(pItem == cfglist) break;
	}
	return(NULL);
}

bool ConfigFile::ItemSet(int parindex,const CString &val)
{
	dirty = true;
	Cconfig_item *pItem = search_item(parindex);
	if(pItem) {
		pItem->value = val;
		return(true);
	}
	return(false);
}

bool ConfigFile::ItemSet(int parindex,double &val)
{
	dirty = true;
	Cconfig_item *pItem = search_item(parindex);
	if(pItem) {
		pItem->value.Format("%lf",val);
		return(true);
	}
	return(false);
}

bool ConfigFile::ItemSet(int parindex,unsigned long &val,bool hex_mode)
{
	dirty = true;
	Cconfig_item *pItem = search_item(parindex);
	if(pItem) {
		if(hex_mode) pItem->value.Format("%x",val);
		else pItem->value.Format("%d",val);
		return(true);
	}
	return(false);
}

bool ConfigFile::ItemSet(int parindex,long &val,bool hex_mode)
{
	dirty = true;

	val = (unsigned long)val;
	Cconfig_item *pItem = search_item(parindex);
	if(pItem) {
		if(hex_mode) pItem->value.Format("%x",val);
		else pItem->value.Format("%d",val);
		return(true);
	}
	return(false);
}

bool ConfigFile::ItemSet(int parindex,int &val)
{
	dirty = true;
	Cconfig_item *pItem = search_item(parindex);
	if(pItem) {
		pItem->value.Format("%d",val);
		return(true);
	}
	return(false);
}

bool ConfigFile::ItemIsValue(int parindex,const CString &val)
{
	Cconfig_item *pItem = search_item(parindex);
	if(pItem && (pItem->value == val)) return(true);
	return(false);
}

bool ConfigFile::ItemSelect(int parindex)
{
	Cconfig_item *pItem = search_item(parindex);

	if(pItem && (pItem->value == "yes")) return(true);
	return(false);
}

int ConfigFile::Vint(int parindex)
{
	Cconfig_item *pItem = search_item(parindex);
	if(!pItem) return(0);
	if(pItem->value == "") return(0);
	else return(atoi(pItem->value));
}

unsigned long ConfigFile::Vulong(int parindex,bool hex_source)
{
	Cconfig_item *pItem = search_item(parindex);
	if(!pItem) return(0);
	if(pItem->value == "") return(0);

	if(!hex_source) return(atol(pItem->value));
	else return(a16ToUint(pItem->value));
}

UINT64 ConfigFile::Vuint64(int parindex,bool hex_source)
{
	Cconfig_item *pItem = search_item(parindex);
	if(!pItem) return(0);
	if(pItem->value == "") return(0);

	if(!hex_source) return(_atoi64(pItem->value));
	else return(a16ToUint64(pItem->value));
}

double ConfigFile::VDouble(int parindex)
{
	Cconfig_item *pItem = search_item(parindex);
	if(!pItem) return(0);
	if(pItem->value == "") return(0);
	else return(atof(pItem->value));
}

const CString &ConfigFile::VCString(int parindex)
{
	static const CString temp = "";
	Cconfig_item *pItem = search_item(parindex);
	if(!pItem) return(temp);
	return(pItem->value);
}

unsigned long a16ToUint(const char *a16)
{
	unsigned long data = 0;
	if(!a16) return(data);

	char temp;
	while(temp = *a16++) {
		if((temp>= '0') && (temp <= '9')) data = data*16+temp-'0';
		else {
			temp |= 0x20;
			if((temp>= 'a') && (temp <= 'f')) data = data*16+temp-'a'+10;
			else break;
		}
	}
	return(data);
}

UINT64 a16ToUint64(const char *a16)
{
	UINT64 data = 0;
	if(!a16) return(data);

	char temp;
	while(temp = *a16++) {
		if((temp>= '0') && (temp <= '9')) data = data*16+temp-'0';
		else {
			temp |= 0x20;
			if((temp>= 'a') && (temp <= 'f')) data = data*16+temp-'a'+10;
			else break;
		}
	}
	return(data);
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
bool ConfigFile::get_bt_address(bool is_new,int &nap,int &uap,int &lap)
{
	unsigned long nap_uap = Vulong(slaveAddrNap_Uap,true);
	unsigned long _lap = Vulong(slaveAddrLap,true);

	nap_uap &= 0xffffff;
	_lap &= 0xffffff;
	if((_lap >= 0x9e8b00) && (_lap <= 0x9e8b3f)) {
		_lap = 0x9e8b40;
		ItemSet(slaveAddrLap,_lap,true);
	}
	if(_lap > Vulong(slaveTerminalAddrLap,true)) return(false);

	nap = (int)((nap_uap>>8)&0xffff);
	uap = (int)(nap_uap&0xff);
	lap = (int)(_lap&0xffffff);

	if(is_new) {
		_lap += 1;
		if((_lap >= 0x9e8b00) && (_lap <= 0x9e8b3f)) _lap = 0x9e8b40;
		ItemSet(slaveAddrLap,_lap,true);
		MarkDirty();
	}
	return(true);
}
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
void ConfigFile::get_bt_address(CString &address)
{
	unsigned long nap_uap = Vulong(slaveAddrNap_Uap,true);
	unsigned long _lap = Vulong(slaveAddrLap,true);

        address = "";
        address.Format("%.6X%.6X",nap_uap,_lap);	
}


void ConfigFile::get_end_address(CString &address)
{
	unsigned long nap_uap = Vulong(slaveAddrNap_Uap,true);
	unsigned long _lap = Vulong(slaveTerminalAddrLap,true);

        address = "";
        address.Format("%.6X%.6X",nap_uap,_lap);
}
void ConfigFile::get_default_address(CString &address)
{
	unsigned long nap_uap = Vulong(slaveDefaultNap_Uap,true);
	unsigned long _lap = Vulong(slaveDefaultLap,true);

        address = "";
        address.Format("%.6X%.6X", nap_uap, _lap);	
}
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
bool ConfigFile::set_bt_address(CString &address)
{
	unsigned long nap_uap = Vulong(slaveAddrNap_Uap,true);
	unsigned long _lap;
	
	address.MakeUpper();
	if(address.GetLength() == 6)
	{
		if(sscanf(address.GetBuffer(0),"%6X",&_lap) != 1) return(false);
		ItemSet(slaveAddrLap,_lap,true);
	}
	else if(address.GetLength() < 12) return(false);
	else
	{
		if(sscanf(address.GetBuffer(0),"%6X%6X",&nap_uap,&_lap) != 2) return(false);
		ItemSet(slaveAddrLap,_lap,true);
		ItemSet(slaveAddrNap_Uap,nap_uap,true);
	}
	MarkDirty();
	return(true);
}
bool ConfigFile::set_end_address(CString &address)
{
        unsigned long nap_uap = Vulong(slaveAddrNap_Uap,true);
	    unsigned long _lap;

        address.MakeUpper();
        if(address.GetLength() == 6)
        {
                if(sscanf(address.GetBuffer(0),"%6X",&_lap) != 1) return(false);
                ItemSet(slaveTerminalAddrLap,_lap,true);
        }
        else if(address.GetLength() < 12) return(false);
        else
        {
                if(sscanf(address.GetBuffer(0),"%6X%6X",&nap_uap,&_lap) != 2) return(false);
                ItemSet(slaveAddrNap_Uap,nap_uap,true);
                ItemSet(slaveTerminalAddrLap,_lap,true);
        }
        MarkDirty();
        return(true);
}
bool ConfigFile::set_default_address(CString &address)
{
        unsigned long nap_uap = Vulong(slaveDefaultNap_Uap,true);
	unsigned long _lap;

        address.MakeUpper();
        if(address.GetLength() == 6)
        {
                if(sscanf(address.GetBuffer(0),"%6X",&_lap) != 1) return(false);
                ItemSet(slaveDefaultLap,_lap,true);
        }
        else if(address.GetLength() < 12) return(false);
        else
        {
                if(sscanf(address.GetBuffer(0),"%6X%6X",&nap_uap,&_lap) != 2) return(false);
                ItemSet(slaveDefaultNap_Uap,nap_uap,true);
                ItemSet(slaveDefaultLap,_lap,true);
        }
        MarkDirty();
        return(true);	
}
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

bool ConfigFile::get_tx_rand_uid(bool is_new,int &nap,int &uap,int &lap)
{
	unsigned long nap_uap = Vulong(slaveAddrNap_Uap,true);
	unsigned long _lap = Vulong(slaveAddrLap,true);

	nap_uap &= 0xffffff;
	_lap &= 0xffffff;
	if((_lap >= 0x9e8b00) && (_lap <= 0x9e8b3f)) {
		_lap = 0x9e8b40;
		ItemSet(slaveAddrLap,_lap,true);
	}
	if(_lap > Vulong(slaveTerminalAddrLap,true)) return(false);

	if(is_new) {
		_lap += 1;
		if((_lap >= 0x9e8b00) && (_lap <= 0x9e8b3f)) _lap = 0x9e8b40;
		ItemSet(slaveAddrLap,_lap,true);
		MarkDirty();
	}
	nap = (int)((nap_uap>>8)&0xffff);
	uap = (int)(nap_uap&0xff);
	lap = (int)(_lap&0xffffff);

	return(true);
}

bool ConfigFile::get_tx_nap_uap(unsigned long nap_uap,int &nap,int &uap)
{
	nap_uap &= 0xffffff;
	nap = (int)((nap_uap>>8)&0xffff);
	uap = (int)(nap_uap&0xff);
	return(true);
}

bool ConfigFile::get_tx_nap_uap(int &nap,int &uap)
{
	unsigned long temp = Vulong(slaveAddrNap_Uap,true);
	return(get_tx_nap_uap(temp,nap,uap));
}

bool ConfigFile::is_same_default_address(struct bt_address_s &addr)
{
	if(!addr.is_valid) return(false);

	int nap;
	int uap;
	unsigned long lap = Vulong(slaveDefaultLap,true);

	get_tx_nap_uap(nap,uap);
	if(addr.uap != uap) return(false);
	if(addr.nap != nap) return(false);
	if((unsigned long)addr.lap != lap) return(false);
	return(true);
}


bool ConfigFile::is_same_default_addr(struct bt_address_s &addr)
{
    if(!addr.is_valid) return(false);
	
	CString temp;
	temp.Format("%.4X%.2X%.6X",addr.nap,addr.uap,addr.lap);
	if(temp == VCString(s_dut_address)) return(true);
	
	CString gaddr = VCString(slaveDefaultNapUapGroup);
	gaddr.MakeUpper();
	temp = "";
	temp.Format("%.4X%.2X",addr.nap,addr.uap);
	if(temp == gaddr) return(true);
	return(false);

}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
unsigned char ConfigFile::get_spk_ch_mode(void)
{
	unsigned char ch_mode = Vint(s_spk_ch_mode);
	if((ch_mode != 1) && (ch_mode != 2) && (ch_mode != 3)) ch_mode = 3;
	return(ch_mode);
}

unsigned char ConfigFile::get_mic_ch_mode(void)
{
	unsigned char ch_mode = Vint(s_mic_ch_mode);
	if((ch_mode != 1) && (ch_mode != 2) && (ch_mode != 3)) ch_mode = 3;
	return(ch_mode);
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void ConfigFile::ini_rf_pwr_tbl(void)
{
        CString temp = VCString(s_RFPwrTbl);
        int len = sscanf(temp.GetBuffer(0),"%lf %d %lf %d %lf %d %lf %d %lf %d %lf %d %lf %d %lf %d "
                                           "%lf %d %lf %d %lf %d %lf %d %lf %d %lf %d %lf %d %lf %d",
                         &rf_pwr_lv_tbl[0],&rf_pwr_index_tbl[0],
                         &rf_pwr_lv_tbl[1],&rf_pwr_index_tbl[1],
                         &rf_pwr_lv_tbl[2],&rf_pwr_index_tbl[2],
                         &rf_pwr_lv_tbl[3],&rf_pwr_index_tbl[3],
                         &rf_pwr_lv_tbl[4],&rf_pwr_index_tbl[4],
                         &rf_pwr_lv_tbl[5],&rf_pwr_index_tbl[5],
                         &rf_pwr_lv_tbl[6],&rf_pwr_index_tbl[6],
                         &rf_pwr_lv_tbl[7],&rf_pwr_index_tbl[7],
                         &rf_pwr_lv_tbl[8],&rf_pwr_index_tbl[8],
                         &rf_pwr_lv_tbl[9],&rf_pwr_index_tbl[9],
                         &rf_pwr_lv_tbl[10],&rf_pwr_index_tbl[10],
                         &rf_pwr_lv_tbl[11],&rf_pwr_index_tbl[11],
                         &rf_pwr_lv_tbl[12],&rf_pwr_index_tbl[12],
                         &rf_pwr_lv_tbl[13],&rf_pwr_index_tbl[13],
                         &rf_pwr_lv_tbl[14],&rf_pwr_index_tbl[14],
                         &rf_pwr_lv_tbl[15],&rf_pwr_index_tbl[15]
                         );
        if(len != (CRF_PWR_TBL_LEN*2))
        { 
                for(int i=0;i<CRF_PWR_TBL_LEN;i++)
                {
                        rf_pwr_lv_tbl[i] = RFPwrTblAry[i][0];
                        rf_pwr_index_tbl[i] = (int)RFPwrTblAry[i][1];
                }
        }


        temp = VCString(s_RFPwrStepTbl);
        len = sscanf(temp.GetBuffer(0),"%lf %lf %lf %lf %lf %lf %lf %lf",
                         &rf_pwr_step_tbl[7],
                         &rf_pwr_step_tbl[6],
                         &rf_pwr_step_tbl[5],
                         &rf_pwr_step_tbl[4],
                         &rf_pwr_step_tbl[3],
                         &rf_pwr_step_tbl[2],
                         &rf_pwr_step_tbl[1],
                         &rf_pwr_step_tbl[0]
                         );
        if(len != CRF_PWR_STEP_LEN)
        {
                for(int i=0;i<CRF_PWR_STEP_LEN;i++)
                {
                        rf_pwr_step_tbl[i] = RfPwrStepTblAry[i];
                }
        }
        else
        {
                bool res = true;
                for(int i=0;i<CRF_PWR_STEP_CENTER;i++) 
                {
                        if(rf_pwr_step_tbl[i] >= 0)
                        { 
                                res = false;
                                break;
                        }
                }
                if(rf_pwr_step_tbl[i++] != 0) res = false;
                for(; i < CRF_PWR_STEP_LEN; i++)
                {
                        if(rf_pwr_step_tbl[i] <= 0)
                        { 
                                res = false;
                                break;
                        }
                }
                if(!res)
                { 
                        for(i=0;i<CRF_PWR_STEP_LEN;i++)
                        {
                                rf_pwr_step_tbl[i] = RfPwrStepTblAry[i];
                        }
                }
        }
}

void ConfigFile::update_pwr_tbl(CString &pwr_cfj,CString &pwr_step)
{
        if(pwr_cfj != "") ItemSet(s_RFPwrTbl, pwr_cfj);
        if(pwr_step != "") ItemSet(s_RFPwrStepTbl, pwr_step);
        ini_rf_pwr_tbl();
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#if 0
bool ConfigFile::wflash_crc32_is_ok(unsigned long &xpv_crc32,unsigned long &xdv_crc32)
{
	xpv_crc32 = 0;
	xdv_crc32 = 0;
	unsigned long crc32 = 0;

	CString write_file = VCString(s_wfile_xpv_name);
	if(write_file == "") return(false);
	if(!cal_file_check_sum(write_file,crc32)) return(false);
	xpv_crc32 = crc32;

	write_file = VCString(s_wfile_xdv_name);
	if(write_file == "") return(false);
	if(!cal_file_check_sum(write_file,crc32)) return(false);
	xdv_crc32 = crc32;

	if((crc32 != Vulong(s_wfile_xdv_crc32,true)) || 
	   (xpv_crc32 != Vulong(s_wfile_xpv_crc32,true))) return(false);
	return(true);
}
#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void add_item(CString &des,char *str,int len)
{
	for(int i=0;i<len;i++) des += str[i];
}

unsigned long hex_str_to_ulong(char *str,int num)
{
	unsigned long data = 0;

	if(num > 8) num = 8;
	for(int i=0;i<num;i++) {
		char temp = str[i];
		if(('0' <= temp) && (temp <= '9')) temp -= '0';
		else if(('A' <= temp) && (temp <= 'F')) temp = temp-'A'+10;
		else if(('a' <= temp) && (temp <= 'f')) temp = temp-'a'+10;
		else temp = 0;
		
		data <<= 4;
		data |= temp;
	}
	return(data);
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int OpenRecordFile(const char* pfile,ofstream& fs,const char* url,bool new_file)
{
	if((pfile == NULL)||(pfile[0] == 0)) return(false);
	int ReturnValue = true;

	TCHAR szFilePath[MAX_PATH + 1]; 
	GetModuleFileName(NULL, szFilePath, MAX_PATH); 
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
	CString str_url =  szFilePath;

	str_url += "log";
	if(GetFileAttributes(str_url) != FILE_ATTRIBUTE_DIRECTORY) {
		mkdir(str_url);
		if(GetFileAttributes(str_url) != FILE_ATTRIBUTE_DIRECTORY) 
			return(false);
	}
	str_url += "\\";

	if(url && url[0]) {
		str_url += url;
		if(GetFileAttributes(str_url) != FILE_ATTRIBUTE_DIRECTORY) {
			mkdir(str_url);
			if(GetFileAttributes(str_url) != FILE_ATTRIBUTE_DIRECTORY) 
				return(false);
		}
		str_url += "\\";
	}
	CString file = str_url+pfile;

	if(new_file) DeleteFile(file);
	ifstream ifs(file,ios::nocreate|ios::in);
	if(ifs.is_open()) {
		ifs.seekg(0,ios::end);
		streampos ps = ifs.tellg();
		if(ps >= 0x20000) {
			ifs.close();
			unsigned long rands = (unsigned)time(NULL);
			CString fileBack;
			fileBack.Format("%d",rands);
			fileBack = fileBack+pfile;
			fileBack += ".old";
			fileBack = str_url+fileBack;
			rename(file,fileBack);
			ReturnValue = 2;
		}else ifs.close();
	}else ReturnValue = 2;
	
	fs.open(file,ios::app|ios::nocreate|ios::out);
	if(!fs.is_open()) {
		fs.clear();
		fs.open(file,ios::app|ios::out);
		if(!fs.is_open()) return(false);
	}
	return(ReturnValue);
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#if 0

int find_inpio_by_name(const char* pio)
{
	int pio_idx = 0;

	if(!pio || !pio[0]) return(IO_MAX);
	const char *name = in_pio_name[0];
	while(pio_idx < IO_MAX) {
		name = in_pio_name[pio_idx];
		if(!name) {
			pio_idx += 1;
			continue;
		}

		bool res = 0;
		int i = 0;
		for(i=0;name[i]&&pio[i];i++) {
			if(name[i] != pio[i]) break;
		}
		if(!pio[i]) return(pio_idx);
		pio_idx += 1;
	}
	return(IO_MAX);
}

int find_outpio_by_name(const char* pio)
{
	int pio_idx = 0;

	if(!pio || !pio[0]) return(OIO_MAX);
	const char *name = out_pio_name[0];
	while(pio_idx < OIO_MAX) {
		name = out_pio_name[pio_idx];
		if(!name) {
			pio_idx += 1;
			continue;
		}

		bool res = 0;
		int i = 0;
		for(i=0;name[i]&&pio[i];i++) {
			if(name[i] != pio[i]) break;
		}
		if(!pio[i]) return(pio_idx);
		pio_idx += 1;
	}
	return(OIO_MAX);
}

#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
unsigned long calculateCRC_32(unsigned long CRC,char data)
{
	const unsigned long CRCTBL_32[] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
	};

	unsigned long CRCTemp = (CRC^data)&0xff;
	CRCTemp = CRCTBL_32[CRCTemp];
	CRC = ((CRC>>8)^CRCTemp)&0xffffffff;
	return(CRC);
}

bool cal_file_check_sum(const char* file,unsigned long &crc_32)
{
	ifstream ifs(file,ios::nocreate|ios::in|ios::binary);
	if(!ifs.is_open()) return(false);

	unsigned long crc_32_temp = 0xffffffff;
	unsigned long cur_idx = 0;
	char temp;
	while(true) {
		ifs.get(temp);
		if(ifs.eof()||ifs.fail()) break;
		crc_32_temp = calculateCRC_32(crc_32_temp,temp);
	}
	crc_32_temp ^= 0xffffffff;
	crc_32 = crc_32_temp;

	ifs.close();
	return(true);
}

void round_1point(double &data)
{
        double temp = data * 10;
        temp += 0.5;
        temp = floor(temp)/10;
        data = temp;
}


bool adjust_hexdigital(CString &address)
{
	CString temp;
	for(int i=0;i<address.GetLength();i++)
	{
		char ctemp = address[i];
		if(('0' <= ctemp) && (ctemp <= '9')) temp += ctemp;
		else if(('a' <= ctemp) && (ctemp <= 'f')) temp += ctemp;
		else if(('A' <= ctemp) && (ctemp <= 'F')) temp += ctemp;
		else
		{ 
			address = temp;
			return(false);
		}
	}
	return(true);
}



bool get_format_bt_addr(CString &address,struct bt_address_s &addr)
{
        addr.is_valid = false;
        if(address.GetLength() != 12) return(false);
        else
        {
                address.MakeUpper();
                if(sscanf(address.GetBuffer(0),"%4X%2X%6X",&addr.nap,&addr.uap,&addr.lap) != 3) return(false); 
                addr.is_valid = true;
        }
        return(true);
}


/////////////////////////////////////////////////////////////////////////////
// HsConfig message handlers
