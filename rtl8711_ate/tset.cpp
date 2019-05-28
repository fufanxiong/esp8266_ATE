/*	tset.cpp
 */

#include "stdafx.h"
#include "Newdevice.h"
#include "Config.h"
#include "PD218.h"
#include "PD218Dlg.h"
#include "err.h"
#include "TestItem.h"
#include "tset.h"

#include <visa.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h>
#include <stdio.h> 
#include <stdlib.h>
#include <fstream.h>

#pragma comment (lib,"TestEngine.lib")
#pragma comment (lib,"uEnergyTest.lib")

static CString language = "en";
static int use_ch = 1;


enum {
		idx_WPSKEY,	// 0
		idx_BKPSKEY,
		idx_WCS,
		idx_BKCS,
	    idx_CURRENT_OFF,
	    idx_PWR_ON, 
		idx_BATTERY_ON,
		idx_BATTERY_OFF,
		idx_DUT_IN_MP,
		idx_DUT_IN_NORMAL,
		idx_CURRENT_ON,
	    idx_POWER_VREG,
		idx_CONNECT,
		idx_SPI_SW,
		idx_SPI_CL,
	    idx_CLOSE_BOX,
		idx_CLOSE_BOX_DELAY,
        idx_SPI_UNLOCK,
        idx_R_MAC_ADDRESS,
        idx_RADDRESS,
        idx_RNAME,
        idx_RXTAL,
		idx_PROG_IMG,
        idx_WR_IMG,
        idx_EXTERN_BURN,
        //write pskey
		idx_WR_PSKEY1, 
		idx_WR_PSKEY2,   
		idx_WR_PSKEY3,   
		idx_WR_PSKEY4,   
		idx_WR_PSKEY5,   
		idx_WR_PSKEY6,   
		idx_WR_PSKEY7,   
		idx_WR_PSKEY8,   
		idx_WR_PSKEY9,   
		idx_WR_PSKEY10,  
		idx_WR_PSKEY11,  
		idx_WR_PSKEY12,  
		idx_WR_PSKEY13,  
		idx_WR_PSKEY14,  
		idx_WR_PSKEY15,

		//read pskey
		idx_RD_PSKEY1,   ///PSKEY_TEST
		idx_RD_PSKEY2, 
		idx_RD_PSKEY3, 
		idx_RD_PSKEY4, 
		idx_RD_PSKEY5, 
		idx_RD_PSKEY6, 
		idx_RD_PSKEY7, 
		idx_RD_PSKEY8, 
		idx_RD_PSKEY9, 
		idx_RD_PSKEY10,
		idx_RD_PSKEY11,
		idx_RD_PSKEY12,
		idx_RD_PSKEY13,
		idx_RD_PSKEY14,
		idx_RD_PSKEY15,

		idx_DISABLE_VM,
		idx_CODEC_IO_CLR,
		idx_PIO_SET_DIR,
        idx_RVER,
		idx_ENABLE_VM,
		idx_CODEC_IO_WRITE,
        idx_WXTAL,
        //idx_CURRENT_BF_IO,
		//idx_CURRENT_AT_IO,
		//idx_CURRENT_XTAL,

		//idx_CURRENT_RF_PWR,
        idx_INPUT_ADDRESS,
        idx_WADDRESS,
        idx_W_MAC_ADDRESS,
        idx_WTriad,
		//idx_OP_DELAY,
		idx_CLR_XTAL_OFFSET,
        idx_SPI_LOCK,
		//bt reset
		idx_RESET_PSKEY,
		idx_RESET_TMODE,
		idx_RESET_XTAL,
        idx_RESET_RF,
		idx_RESET_NMODE,
		idx_RESET_AFTER_RF,
		
		idx_UART,
		idx_PIO0,
		idx_PIO1,
		idx_PIO2,
		idx_PIO3,
        idx_PIO4,
		idx_PIO5,
		idx_PIO6,
		idx_PIO7,
		idx_PIO8,
		idx_PIO9,
		idx_PIO10,
		idx_PIO11,
		idx_PIO12,
		idx_PIO13,
		idx_PIO14,
		idx_PIO15,
		idx_PCM,
		idx_PIO19,
		idx_PIO20,
		idx_PIO21,
		idx_PIO22,
		idx_PIO23,
		idx_PIO24,
		idx_PIO25,
		idx_PIO26,
		idx_PIO27,
		idx_PIO28,
		idx_PIO29,
		idx_PIO30,
		idx_PIO31,

		idx_AIO0,
		idx_AIO1,
		idx_MBAIS0,
		idx_MBAIS1,

        idx_OPEN_BOX,
		idx_VBUS_3V3,
		idx_USB_NP,
        idx_AUDIO_IO,

		
        idx_RF_CHH_EVM,
        idx_RF_CHM_EVM,
        idx_RF_CHL_EVM,

		
        idx_RF_CHH_SENS,
        idx_RF_CHM_SENS,
        idx_RF_CHL_SENS,
        idx_RF_SENS_ANY,
        idx_RF_CHH_FOFFSET,
        idx_RF_CHM_FOFFSET,
        idx_RF_CHL_FOFFSET,
        idx_RF_CHH_PWR,
        idx_RF_CHM_PWR,
        idx_RF_CHL_PWR,
        //idx_OP_RELEASE,
        idx_RF_TEST,
        //idx_OP_LOCK,
		idx_DISCONNECT,
        idx_XTAL_ADJ,
		idx_APOWER_ADJ_CHH,
		idx_APOWER_ADJ_CHM,
		idx_APOWER_ADJ_CHL,
		idx_RFPARA_SAVE,
		idx_CHARGE_ADJ,
        idx_IO_TEST,
        //idx_ENTER_TMODE,
        //idx_AUDIO_SYNC,
        idx_VDEF_ADDR,
        idx_VLABEL_ADDR,
        //idx_OFF_CURRENT_ON_DUT,
        //idx_OFF_CURRENT_OFF_DUT,
        //idx_OFF_CURRENT,
        //idx_OFF_CURRENT_SYNC,
        //idx_TEST_MODE_ON,
        //idx_TEST_MODE_OFF,
        //idx_OFF_CURRENT_TMODE,
		//idx_TVREG_OFF,
		//idx_BATTERY_OFF,
		//idx_PWR_OFF,

		idx_LED29,
		idx_LED30,

        idx_DUT_Btn,

        idx_TNAME_MAX
};

char *en_tname[idx_TNAME_MAX] = 
{
		"restore pskeys",
		"backup pskeys",
		"restore CS",
		"backup CS",
     	"pwr off current",
	    "dut pwr connect",
		"dut battery on",
		"dut battery off",
		"pwr on current",
	    "dut in MP mode",
	    "dut in normal mode",
		"dut power on",
		"connect spi",
		"spi switch on",
		"spi switch off",
	    "close tbox",
		"wait tbox ok",
		"bt dev unlock",
		"read MAC address",
        "read bt address",
        "read bt name",
        "read xtal",
		"program verify",
        "flash write",
        "extern flash burn",
		"CVC write",  //"psKey1 write",
		"psKey2 write",
		"psKey3 write",
		"psKey4 write",
		"psKey5 write",
		"psKey6 write",
		"psKey7 write",
		"psKey8 write",
		"psKey9 write",
		"psKey10 write",
		"psKey11 write",
		"psKey12 write",
		"psKey13 write",
		"psKey14 write",
		"psKey15 write",

		"psKey1 chk",
        "psKey2 chk",
		"psKey3 chk",
		"psKey4 chk",
		"psKey5 chk",
		"psKey6 chk",
		"psKey7 chk",
		"psKey8 chk",
		"psKey9 chk",
		"psKey10 chk",
		"psKey11 chk",
		"psKey12 chk",
		"psKey13 chk",
		"psKey14 chk",
		"psKey15 chk",

		"vm disable",
		"codec io clear",
		"set pio dir",
        "read major ver",
		"vm enable",
		"codec io write",
        "write xtal",
        //"current test(BF IO sel)",
		//"current test(AT IO sel)",
		//"current test(XTAL sel)",
		//"current test(XTAL sel)",
        "input MAC address",
        "write bt address",
        "write MAC address",
        "write Triad",
		//"op delay",
		"clear xtal offset",
		"bt dev lock",
        "reset bef pskey chk",
		"reset aft vm disabl",
        "reset aft clr xtal",
        "reset bef rf test",
		"reset aft vm enable",
		"reset aft rf test",
		
		"UART Test",
		"PIO[0] chk",
		"PIO[1] chk",
		"PIO[2] chk",
		"PIO[3] chk",
        "PIO[4] chk",
		"PIO[5] chk",
		"PIO[6] chk",
		"PIO[7] chk",
		"PIO[8] chk",
		"PIO[9] chk",
		"PIO[10] chk",
		"PIO[11] chk",
        "PIO[12] chk",
		"PIO[13] chk",
		"PIO[14] chk",
		"PIO[15] chk",
		"PCM_IN/OUT",
        "PCM_SYNC chk",
		"PCM_CLK chk",
		"PIO[21] chk",
		"PIO[22] chk",
		"PIO[23] chk",

		"PIO[24] chk",
		"PIO[25] chk",
		"PIO[26] chk",
		"PIO[27] chk",
		"PIO[28] chk",
		"LED[0] chk",
		"LED[1] chk",
		"LED[2] chk",

		"AIO[0] chk",
		"AIO[1] chk",
		"M_BIAS_A",
        "M_BIAS_B",
        "open tbox",
		"USB 3V3 test",
        "USB N/P test",
		
        "audio io",


        "RF CHH EVM",
        "RF CHM EVM",
        "RF CHL EVM",

		
        "RF CHH Sens",
        "RF CHM Sens",
        "RF CHL Sens",
        "RF Sens ANY",
        "RF CHH freqOffset",
        "RF CHM freqOffset",
        "RF CHL freqOffset",
        "RF CHH Power",
        "RF CHM Power",
        "RF CHL Power",
        //"op release",
        "RF test",
       //"op lock",
		"disconn spi",
        "xtal adjust",
		"power cal CHH",
		"power cal CHM",
		"power cal CHL",
		"save rf para",
		"charge adjust",
        "io test",
        //"sync(audio)",
        "def addr verify",
        "label addr verify",

		"LED[0] blue",  
		"LED[1] white",

        "vreg btn chk",

		 //"dut pwr off",
		 //"dut battery off",
		 //"test board pwr off",
        //"dut pwr on",
        //"dut pwr off",
        //"pwr off current",
        //"sync(poweroff)",
        //"test port pwr on",
        //"test port pwr off",
        //"sync(repower)",
};

char *ch_tname[idx_TNAME_MAX] = 
{
	 	"恢复 pskeys",
		"备份 pskeys",
		"恢复 CS",
		"备份 CS",
	    "关机电流",
	    "dut 接通电源",
	    "dut 上电",
		"dut 断电",
		"dut 进入MP模式",
		"dut 进入normal模式",
		"开机电流",
		"dut 开机",
		"连接 spi",
		"spi 通路打开",
		"spi 通路关闭",
	    "关闭屏蔽箱",
		"屏蔽箱关闭OK",
        "dut 解锁",	
        "读MAC地址",
        "读蓝牙地址",
        "读蓝牙名字",
        "读晶体参数",
		"固件验证",
        "烧录软件",
        "烧录分区",
		"写 CVC ",  //"写 psKey1 ", 
		"写 psKey2",  
		"写 psKey3",  
		"写 psKey4",  
		"写 psKey5",  
		"写 psKey6",  
		"写 psKey7",  
		"写 psKey8",  
		"写 psKey9",  
		"写 psKey10", 
		"写 psKey11", 
		"写 psKey12", 
		"写 psKey13", 
		"写 psKey14", 
		"写 psKey15",

		"读 psKey1",        
        "读 psKey2",                                                             
    	"读 psKey3",
		"读 psKey4",  
		"读 psKey5",  
		"读 psKey6",  
		"读 psKey7",  
		"读 psKey8",  
		"读 psKey9",  
		"读 psKey10", 
		"读 psKey11", 
		"读 psKey12", 
		"读 psKey13", 
		"读 psKey14", 
		"读 psKey15",

		"VM 不使能",
		"编解码端口清零",
		"设置 pio 输入输出",
        "读WIFI模组版本号",
		"VM 使能",
		"写编解码端口",
        "写晶体参数",
        //"电流测试(BF IO sel)",
		//"电流测试(AT IO sel)",
		//"电流测试(XTAL sel)",
		//"电流测试(RF PWR sel)",
        "分配MAC地址",
        "写蓝牙地址",
        "写MAC地址",
        "写三元组",
		//"操作延时",
		"清除晶体偏移",
		"测试板锁定",
		"PSKEY 测试前复位",
        "VM不使能后复位",
		"清晶体偏移后复位",
        "8852测试前复位",
		"VM使能后复位",
		"8852测试后复位",
		
        "UART Test",
		"PIO[0] 测试",
		"PIO[1] 测试",
		"PIO[2] 测试",
		"PIO[3] 测试",
		"PIO[4] 测试",
		"PIO[5] 测试",
		"PIO[6] 测试",
		"PIO[7] 测试",
		"PIO[8] 测试",
		"PIO[9] 测试",
		"PIO[10] 测试",
		"PIO[11] 测试",
		"PIO[12] 测试",
		"PIO[13] 测试",
		"PIO[14] 测试",
		"PIO[15] 测试",
		"PCM_IN/OUT",
		"PCM_SYNC",
		"PCM_CLK 测试",
		"PIO[21] 测试",
		"PIO[22] 测试",
		"PIO[23] 测试",
		"PIO[24] 测试",
		"PIO[25] 测试",
		"PIO[26] 测试",
		"PIO[27] 测试",
		"PIO[28] 测试",
		"LED[0] 测试",
		"LED[1] 测试",
		"LED[2] 测试",

		"AIO[0] 测试",
		"AIO[1] 测试",
		"M_BIAS_A",
        "M_BIAS_B",
        "打开屏蔽箱",
		"USB 3V3 测试",
		"USB N/P 测试",
        "音频端口测试",

        "高频点EVM",
        "中频点EVM",
        "低频点EVM",

		
        "高频点灵敏度",
        "中频点灵敏度",
        "低频点灵敏度",
        "HOPON 灵敏度",
        "高频点频率偏移",
        "中频点频率偏移",
        "低频点频率偏移",
        "高频点功率测试",
        "中频点功率测试",
        "低频点功率测试",
        //"操作释放",
        //"8852 测试",		//FU CHANGE NAME
        "N4010A RF 测试",
        //"操作占有",
		"断开 spi",
        //"退出测试模式",
        //"金机测 RF 功率",
        "晶体校正",
		"高频点功率校正",
        "中频点功率校正",
        "低频点功率校正",
		"切换到正常模式",
		"充电校正",
        "IO 端口测试",
        //"同步等待(音频)",
        "默认地址验证",
        "贴纸地址验证",
        //"dut关机",
		//"dut断开电源",
		//"测试板断电",

		"LED[0] 蓝", 
		"LED[1] 白",

		"Vreg按键检测",



};

//---------------------------------------------------------------------------------------------------------------------------
char *get_tname(int idx)
{	
	if(idx >= idx_TNAME_MAX) return("err item");
	//if(use_ch == 1) return(ch_tname[idx]);
	//return(en_tname[idx]);
	return(ch_tname[idx]);
}

//============================================================================================================================
//============================================================================================================================

#define	PSKEY_TEST(KEY,KEY_NAME, TID)	do {						   \
	if(cfg.ItemSelect(s_psKey##KEY##_sel)) {					       \
		pTItem = new CTObj_psKeyCheck(get_tname(idx_RD_PSKEY##KEY),	   \
		                this,                                          \
						TID,                                           \
						s_psKey##KEY##_index,			               \
						s_psKey##KEY##_value,			               \
						s_psKey##KEY##_string);			               \
		pTItem->dNode.add_tail(TItemList);					           \
	}										                           \
}while(0)


#define	WPSKEY_TEST(KEY,KEY_NAME, TID)	do {						    \
	if(cfg.ItemSelect(s_wpsKey##KEY##_sel)) {					        \
		pTItem = new CTObj_write_pskey(get_tname(idx_WR_PSKEY##KEY),	\
		                this,                                           \
						TID,                                            \
						s_wpsKey##KEY##_index);			                \
		pTItem->dNode.add_tail(TItemList);					            \
	}										                            \
}while(0)

#define GEN_WRITE_PSK_TITEM()		do {										            \
		WPSKEY_TEST(1,"psKey1", TID_WR_PSKEY1);											    \
		WPSKEY_TEST(2,"psKey2", TID_WR_PSKEY2);											    \
		WPSKEY_TEST(3,"psKey3", TID_WR_PSKEY3);											    \
		WPSKEY_TEST(4,"psKey4", TID_WR_PSKEY4);											    \
		WPSKEY_TEST(5,"psKey5", TID_WR_PSKEY5);											    \
		WPSKEY_TEST(6,"psKey6", TID_WR_PSKEY6);											    \
		WPSKEY_TEST(7,"psKey7", TID_WR_PSKEY7);											    \
		WPSKEY_TEST(8,"psKey8", TID_WR_PSKEY8);											    \
		WPSKEY_TEST(9,"psKey9", TID_WR_PSKEY9);											    \
		WPSKEY_TEST(10,"psKey10", TID_WR_PSKEY10);											\
		WPSKEY_TEST(11,"psKey11", TID_WR_PSKEY11);											\
		WPSKEY_TEST(12,"psKey12", TID_WR_PSKEY12);											\
		WPSKEY_TEST(13,"psKey13", TID_WR_PSKEY13);											\
		WPSKEY_TEST(14,"psKey14", TID_WR_PSKEY14);											\
		WPSKEY_TEST(15,"psKey15", TID_WR_PSKEY15);											\
}while(0)

#define GEN_READ_PSK_TITEM()		do {									 	            \
		PSKEY_TEST(1,"psKey1", TID_RD_PSKEY1);												\
		PSKEY_TEST(2,"psKey2", TID_RD_PSKEY2);												\
		PSKEY_TEST(3,"psKey3", TID_RD_PSKEY3);												\
		PSKEY_TEST(4,"psKey4", TID_RD_PSKEY4);												\
		PSKEY_TEST(5,"psKey5", TID_RD_PSKEY5);												\
		PSKEY_TEST(6,"psKey6", TID_RD_PSKEY6);												\
		PSKEY_TEST(7,"psKey7", TID_RD_PSKEY7);												\
		PSKEY_TEST(8,"psKey8", TID_RD_PSKEY8);												\
		PSKEY_TEST(9,"psKey9", TID_RD_PSKEY9);												\
		PSKEY_TEST(10,"psKey10", TID_RD_PSKEY10);											\
		PSKEY_TEST(11,"psKey11", TID_RD_PSKEY11);											\
		PSKEY_TEST(12,"psKey12", TID_RD_PSKEY12);											\
		PSKEY_TEST(13,"psKey13", TID_RD_PSKEY13);											\
		PSKEY_TEST(14,"psKey14", TID_RD_PSKEY14);											\
		PSKEY_TEST(15,"psKey15", TID_RD_PSKEY15);											\
}while(0)



#define	IO_TEST(IO, TID)	do {							               \
	if(cfg.ItemSelect(s_pio_check##IO##_sel)) {					           \
		pTItem = new CTObj_io_port(get_tname(idx_PIO##IO),this, TID, IO);  \
		pTItem->dNode.add_tail(TItemList);					               \
	}										                               \
}while(0)
 
#define LED_IO_TEST(IO, TID) do {                                          \
	if(cfg.ItemSelect(s_pio_check##IO##_sel)) {					           \
		pTItem = new CTObj_led_io(get_tname(idx_PIO##IO),this, TID, IO);   \
		pTItem->dNode.add_tail(TItemList);					               \
	}										                               \
}while(0)


#define LED_COLOR_TEST(IO, TID, LED_NUM,RED_PIO_GRP, GREEN_PIO_GRP, BLUE_PIO_GRP, \
	                 RED_PIO_NUM, GREEN_PIO_NUM, BLUE_PIO_NUM, CFG_ITEM) do {   \
	 if(cfg.ItemSelect(s_pio_check##IO##_sel)) {					            \
		pTItem = new CTObj_rgb_chk(get_tname(idx_LED##IO),this, TID, LED_NUM,   \
						   RED_PIO_GRP, GREEN_PIO_GRP, BLUE_PIO_GRP,            \
						   RED_PIO_NUM, GREEN_PIO_NUM, BLUE_PIO_NUM, CFG_ITEM   \
						   );                                                   \
		pTItem->dNode.add_tail(TItemList);					                    \
	 }										                                    \
}while(0)


#define VREG_BUTTON_TEST(IO, TID, BTN_PIO_GRP, BTN_PIO_NUM) do {       \
	if(cfg.ItemSelect(s_dut_mot##IO##_sel)) {					               \
		pTItem = new CTObj_button_chk(get_tname(idx_DUT_Btn), this,TID,        \
                                       BTN_PIO_GRP, BTN_PIO_NUM);              \
		pTItem->dNode.add_tail(TItemList);					                   \
	 }										                                   \
}while(0)


#define	AIO_TEST(IO,TID)	do {							                \
	if(cfg.ItemSelect(s_aio_check##IO##_sel)) {					            \
		pTItem = new CTObj_Aio_port(get_tname(idx_AIO##IO), this, TID);		\
		pTItem->dNode.add_tail(TItemList);					                \
	}										                                \
}while(0)

#define	MBIAS_TEST(IO,TID)	do {							                \
	if(cfg.ItemSelect(s_mbias_check##IO##_sel)) {					        \
		pTItem = new CTObj_mbias(get_tname(idx_MBAIS##IO), this, TID);		\
		pTItem->dNode.add_tail(TItemList);					                \
	}										                                \
}while(0)

#define	VBUS_USB3V3_IO_TEST(TID)	do {							         \
	if(cfg.ItemSelect(s_vbus_usb3v3_sel)) {						             \
		pTItem = new CTObj_vbus_usb33_io(get_tname(idx_VBUS_3V3), this,TID); \
		pTItem->dNode.add_tail(TItemList);					                 \
	}										                                 \
}while(0)


//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------

CDutGlodenPar::CDutGlodenPar(int cfj_key_idx,ConfigFile &cfg)
{
	id = cfj_key_idx;
	update_from_cfg(cfg);
}

void CDutGlodenPar::update_to_cfg(ConfigFile &cfg)
{
	int idx = id;
	
	addr.MakeUpper();
    cfg.ItemSet(idx++,addr);
    cfg.ItemSet(idx++,ver);
    cfg.ItemSet(idx++,name);
    cfg.ItemSet(idx++,current_min);
    cfg.ItemSet(idx++,current_max);
    cfg.ItemSet(idx++,ch1_rf_avpwr);
    cfg.ItemSet(idx++,ch2_rf_avpwr);
    cfg.ItemSet(idx++,ch3_rf_avpwr);
    cfg.ItemSet(idx++,rf_inputpwr);
	cfg.ItemSet(idx++,rf_inputevm);
	cfg.ItemSet(idx++,ch1_rf_evm);
    cfg.ItemSet(idx++,ch2_rf_evm);
    cfg.ItemSet(idx++,ch3_rf_evm);
    cfg.ItemSet(idx++,rf_fre_offset);
    cfg.ItemSet(idx++,sens_pwr);
}

void CDutGlodenPar::update_from_cfg(ConfigFile &cfg)
{
	    int idx = id;
	
	    addr = cfg.VCString(idx++);
        ver = cfg.VCString(idx++);
        name = cfg.VCString(idx++);
        current_min = cfg.VDouble(idx++);
        current_max = cfg.VDouble(idx++);
        ch1_rf_avpwr = cfg.VDouble(idx++);
        ch2_rf_avpwr = cfg.VDouble(idx++);
        ch3_rf_avpwr = cfg.VDouble(idx++);
        rf_inputpwr = cfg.VDouble(idx++);
		rf_inputevm = cfg.VDouble(idx++);
		ch1_rf_evm = cfg.VDouble(idx++);
        ch2_rf_evm = cfg.VDouble(idx++);
        ch3_rf_evm = cfg.VDouble(idx++);
        rf_fre_offset = cfg.Vint(idx++);
        sens_pwr = cfg.VDouble(idx++);
	
	addr.MakeUpper();
}


//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------

CDutSensorPar::CDutSensorPar(int cfj_key_idx, ConfigFile &cfg)
{
	id = cfj_key_idx;
	update_from_cfg(cfg);
}

void CDutSensorPar::update_to_cfg(ConfigFile &cfg)
{
	int idx = (id+1);	
	cfg.ItemSet(idx++,led0_red_adc_min);
	cfg.ItemSet(idx++,led0_red_adc_max);
	cfg.ItemSet(idx++,led0_green_adc_min);
	cfg.ItemSet(idx++,led0_green_adc_max);
	cfg.ItemSet(idx++,led0_blue_adc_min);
	cfg.ItemSet(idx++,led0_blue_adc_max);
	cfg.ItemSet(idx++,led0_red_adc_adj);
	cfg.ItemSet(idx++,led0_green_adc_adj);
	cfg.ItemSet(idx++,led0_blue_adc_adj); 
	cfg.ItemSet(idx++,led0_red_offset);
	cfg.ItemSet(idx++,led0_green_offset);
	cfg.ItemSet(idx++,led0_blue_offset);

	cfg.ItemSet(idx++,led1_red_adc_min);
	cfg.ItemSet(idx++,led1_red_adc_max);
	cfg.ItemSet(idx++,led1_green_adc_min);
	cfg.ItemSet(idx++,led1_green_adc_max);
	cfg.ItemSet(idx++,led1_blue_adc_min);
	cfg.ItemSet(idx++,led1_blue_adc_max);
	cfg.ItemSet(idx++,led1_red_adc_adj);
	cfg.ItemSet(idx++,led1_green_adc_adj);
	cfg.ItemSet(idx++,led1_blue_adc_adj);
	cfg.ItemSet(idx++,led1_red_offset);
	cfg.ItemSet(idx++,led1_green_offset);
	cfg.ItemSet(idx++,led1_blue_offset);
}

void CDutSensorPar::update_from_cfg(ConfigFile &cfg)
{
	int idx = (id+1);  //s_Uxx_sel+1
	
	led0_red_adc_min   = cfg.Vint(idx++);
	led0_red_adc_max   = cfg.Vint(idx++);
	led0_green_adc_min = cfg.Vint(idx++);
	led0_green_adc_max = cfg.Vint(idx++);
	led0_blue_adc_min  = cfg.Vint(idx++);
	led0_blue_adc_max  = cfg.Vint(idx++);
	led0_red_adc_adj   = cfg.Vint(idx++);
	led0_green_adc_adj = cfg.Vint(idx++);
	led0_blue_adc_adj  = cfg.Vint(idx++);
	led0_red_offset    = cfg.Vint(idx++);
	led0_green_offset  = cfg.Vint(idx++);
    led0_blue_offset   = cfg.Vint(idx++);
	
	led1_red_adc_min   = cfg.Vint(idx++);
	led1_red_adc_max   = cfg.Vint(idx++);
	led1_green_adc_min = cfg.Vint(idx++);
	led1_green_adc_max = cfg.Vint(idx++);
	led1_blue_adc_min  = cfg.Vint(idx++);
	led1_blue_adc_max  = cfg.Vint(idx++);
	led1_red_adc_adj   = cfg.Vint(idx++);
	led1_green_adc_adj = cfg.Vint(idx++);
	led1_blue_adc_adj  = cfg.Vint(idx++);
	led1_red_offset    = cfg.Vint(idx++);
	led1_green_offset  = cfg.Vint(idx++);
    led1_blue_offset   = cfg.Vint(idx++);

}


//--------------------------------------------------------------------------------------------------------------------

testSet::testSet(void)
{
         tlist = NULL;
		 pSpiObj = NULL;
         idc_CurTestObj = IDC_CurTestObj;
         idc_CurTestItem = IDC_CurTestItem;
         dev_id = TEST_DEV_ID_A;

		 pass_num = 0;
         fail_num = 0;
         tItemSum = 0;
		 TItemList = NULL;
         cur_test_item = NULL;

         testRunning = NULL;
         hthread = NULL;
         hthreadId = NULL;
		 backup_pskey = NULL;
		 //myProc_Handle = NULL;
		 //myProc_HandleId = NULL;

         own_op_lock = false;
         addr.is_valid = false;
         r_addr.is_valid = false;
         pdev_addr = NULL;
	
         mt8852_res = new struct tSetResult();

		 need_input_addr_first = false;
		 //add
		 rf_is_tested = false;
		 xtal_adj_is_tested = false;

		 is_golden = false;
		 gloden_vamplitude = 0;
		 
		 file_crc32_cal = 0;
		 file_crc32_err = 1;
		 xpv_crc32 = 0;
		 xdv_crc32 = 0;

}

testSet::~testSet()
{
        DeleteTestItemList();
	
		if(pSpiObj) delete pSpiObj;
	    pSpiObj = NULL;

		delete mt8852_res;
        mt8852_res = NULL;

		if(backup_pskey) delete(backup_pskey);
	    backup_pskey = NULL;

		/////spi_port?NULL//////
}


bool testSet::CreatTestItem_write_file(testSet *Tset, ConfigFile &cfg,bool *cd)
{
#if 0
    #define	add_backup_pskey(NUM)							\
	do{	pskey = cfg.Vulong(s_wfsave_ps_key##NUM,true);				\
	    if(pskey > 0) {								\
	    if(!Tset->backup_pskey) 					\
			Tset->backup_pskey = new backup_pskey_contaner();	\
		 Tset->backup_pskey->add(pskey);				\
		}									\
	}while (0)
	CTestItem *pTItem;
	
	if(cfg.ItemSelect(s_wfsave_ps_sel)) {
		unsigned long pskey;
		add_backup_pskey(1);
		add_backup_pskey(2);
		add_backup_pskey(3);
		add_backup_pskey(4);
		add_backup_pskey(5);
		add_backup_pskey(6);
		add_backup_pskey(7);
		add_backup_pskey(8);
		add_backup_pskey(9);
		add_backup_pskey(10);
	}

	if(Tset->backup_pskey)
	{
	    //CONNECT SPI
		pTItem = new CTObj_conn_spi(get_tname(idx_CONNECT), this, TID_CN_DUT);
	    pTItem->dNode.add_tail(TItemList);

	    struct bk_pskey_item *item;
	    item = Tset->backup_pskey->head;

	    while(item) 
	    {
		   pTItem = new CTObj_rback_pskey(get_tname(idx_BKPSKEY),item, this, TID_BK_PSKEY);
		   pTItem->dNode.add_tail(TItemList);
		   item = item->next;
	    }
	    
		//DISCONNECT SPI
		pTItem = new CTObj_discn_spi(get_tname(idx_DISCONNECT), this, TID_DISCN_DUT);
	    pTItem->dNode.add_tail(TItemList);
	}
	
	//FLASH WRITE
	pTItem = new CTObj_flash_write(get_tname(idx_WR_IMG), this, TID_WFILE, cd);
	pTItem->dNode.add_tail(TItemList);

	//CONNECT SPI
	pTItem = new CTObj_conn_spi(get_tname(idx_CONNECT), this, TID_CN_DUT);
	pTItem->dNode.add_tail(TItemList);

	
	if(Tset->backup_pskey)
	{
	    struct bk_pskey_item *item;
	    item = Tset->backup_pskey->head;

	    while(item) 
	    {
		   pTItem = new CTObj_wback_pskey(get_tname(idx_WPSKEY),item, this,TID_WB_PSKEY);
		   pTItem->dNode.add_tail(TItemList);
		   item = item->next;
	    }
	}
#endif
	return(true);
}

//yhl 2018/11/01
bool testSet::ShowTestItemList(void)
{
	tlist->DeleteAllItems();
	while(tlist->DeleteColumn(0));
	tlist->SetExtendedStyle(LVS_EX_FULLROWSELECT);
	tlist->SetHeadings(_T("No,30;Test   Items,110;Range,200;Parameter,200;P/F,30"));
	//tlist->SetHeadings(_T("No,28;Test   Items,100;P/F,32;Range,135;Parameter,257"));
	CdNode<CTestItem> *pItem = TItemList;
	while(pItem) 
    {
		pItem->cur->ClearResult();
		pItem->cur->gen_prompt();
		pItem = pItem->next;
		if(pItem == TItemList) break;
	}
	return(true);
}


void testSet::DeleteTestItemList(void)
{
	CdNode<CTestItem> *pItem;
	CTestItem *pTItem;
	do{
		if(!(pItem = TItemList)) break;
		pTItem = pItem->cur;
		pItem->del(TItemList);
		delete(pTItem);
	}while (true);

}

void testSet::ToReadyState(void)
{
	const int item = tlist->GetItemCount();
	for(int i=0;i<item;i++) 
    {
		tlist->SetItemColor(i,pos_idx,TT_DEFAULT,BK_DEFAULT);
                tlist->SetItemText(i,pos_P_F,"");
		tlist->SetItemText(i,pos_param,"");
	}
        pdev_addr = NULL;
}


void testSet::sle_item(CTestItem *item)
{
	tlist->SetItemColor(item->index,pos_idx,TT_DEFAULT,BK_HOT);
	tlist->EnsureVisible(item->index,TRUE);
}

void testSet::unsle_item(CTestItem *item)
{
	tlist->SetItemColor(item->index,pos_idx,TT_DEFAULT,BK_DEFAULT);
}

void testSet::show_item_par(CTestItem *item)
{
    item->param.Replace(","," ");
	tlist->SetItemColor(item->index,pos_idx,TT_DEFAULT,BK_HOT);
	tlist->SetItemText(item->index,pos_param,item->param);
}

void testSet::show_item_result(CTestItem *item)
{
    //显示测试结果
	show_item_par(item);
	if(true == item->result) 
    	{
		tlist->SetItemText(item->index,pos_P_F,"  P");
		tlist->SetItemColor(item->index,pos_P_F,TT_PASS,BK_DEFAULT);
	} 
	else
	{
		tlist->SetItemText(item->index,pos_P_F,"  F");
		tlist->SetItemColor(item->index,pos_P_F,TT_FAIL,BK_DEFAULT);
	}
        unsle_item(item);
}

void testSet::show_range(CTestItem *item)
{
        tlist->SetItemText(item->index,pos_range,item->item_range);
}


int testSet::__DoItemTest(CTestItem *item)
{
        int Res = NO_ERR;

	Res = item->ItemTestReady();
	if(Res != NO_ERR) return(Res);
         
	
	item->update_range();
	Res = item->TestOp();

	if(Res != NO_ERR) 
		item->ItemTestClean();
	else 
		Res = item->ItemTestClean();
	return(Res);
}


void testSet::DoSelfMsg(WPARAM wParam)
{
    switch(wParam)
    {
        case MSG_SEL_ITEM:
                if(!cur_test_item) return;
                sle_item(cur_test_item);
                break;
        case MSG_SHOW_RESULT:
                if(!cur_test_item) return;
                show_item_result(cur_test_item);
                break;
        case MSG_SHOW_PAR:
                if(!cur_test_item) return;
                show_item_par(cur_test_item);
                break;
        case MSG_SHOW_RANGE:
                if(!cur_test_item) return;
                show_range(cur_test_item);
                break;
        case MSG_READY:
                ToReadyState();
                break;
        case MSG_FINISH:
                ToFinishState();
                break;
     }
}

int testSet::DoItemTest(CTestItem *item,int err_code)
{
	if((err_code != NO_ERR) && item->disable_run_when_err())
	{	
		return(false);
	}
	    //if((err_code != NO_ERR)) return(false);
        int ErrSort = NO_ERR;
        cur_test_item = item;
        TSET_MSG(MSG_SEL_ITEM,item->tset);

        ErrSort = __DoItemTest(item);

        TSET_MSG(MSG_SHOW_RESULT,item->tset);  //显示测试结果
        return(ErrSort);
}

bool testSet::AllTestItemOk(void)
{
	CdNode<CTestItem> *pItem = TItemList;	    

	while(pItem)
    {
		if(!pItem->cur->result) return(false);
		pItem = pItem->next;
		if(pItem == TItemList) break;
	}
	return(true);
}

void testSet::ToFinishState(void)
{
        bool result = test_result;
	char *show_mac;
        extern CString str_test_result_ok;
        extern CString str_test_result_fail;
        
        //all test ok & this address is writen to device so invalid it
        if(result && (pdev_addr == &addr))
        {
            addr.is_valid = false;
        }

	    const int item = tlist->GetItemCount();
	    for(int i=0;i<item;i++)
		{
		   tlist->SetItemColor(i,pos_idx,TT_DEFAULT,BK_DEFAULT);
		}
	    POSITION pos = tlist->GetFirstSelectedItemPosition();
	    while(pos != NULL)
		{
		  int iSel = tlist->GetNextSelectedItem(pos);
		  tlist->SetItemState(iSel,pos_idx,LVIS_SELECTED);
		}

	    if(!result) {
           CString show;
		   show.Format("%s:%d", str_test_result_fail, fail_titem_id);
		   pMainWnd->GetDlgItem(idc_CurTestItem)->SetWindowText(show);
		} 
	    else 
		{		
			CString show = "";
			if(dev_id == TEST_DEV_ID_A)
				show = pMainWnd->m_adev_address;
			else
				show = pMainWnd->m_bdev_address;

			show_mac = (char*)((LPCTSTR)show);
			strupr(show_mac);//小写变大写//yhl 2018/10/31
		   /*if(pdev_addr)
		   {
			  show = "";
			  if(is_golden)
				show.Format("%s:%.4X%.2X %.6X(G)",prompt_test_name,pdev_addr->nap,pdev_addr->uap,pdev_addr->lap);
			  else
				show.Format("%s:%.4X%.2X %.6X",prompt_test_name,pdev_addr->nap,pdev_addr->uap,pdev_addr->lap);
		   }*/
		   pMainWnd->GetDlgItem(idc_CurTestObj)->SetWindowText(show);
		   pMainWnd->GetDlgItem(idc_CurTestItem)->SetWindowText(str_test_result_ok);
		}
}


//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
//click
bool testSet::CreatTestItemList(testSet *Tset,ConfigFile &cfg)
{		
    DeleteTestItemList();	
    CTestItem *pTItem;

	if(Tset->backup_pskey) delete(Tset->backup_pskey);
	Tset->backup_pskey = NULL;

	//tsel.reset_delay = cfg.Vulong(s_RstDelayTime);
    //tsel.cn_delay = cfg.Vulong(s_cn_dut_delay);
	
	language = cfg.VCString(ParLanguage);
	if(language != "en") 
		use_ch = 1;       //chinese
	else 
		use_ch = 0;       //english


//----------------------------------------------------------------------------------------------------------    
 
	//bool clr_XtalOffset_set = false;
	need_input_addr_first = false;
#if 1
	//关闭屏蔽箱
	if(pCfgf.ItemSelect(parTestBoxNeed))
	{
		pTItem = new CTObj_close_tbox(get_tname(idx_CLOSE_BOX),this,TID_CLOSE_BOX);
		pTItem->dNode.add_tail(TItemList);

	}	
#endif
//----------------------------------------------------------------------------------------------------------------------
#if 1
	//获取输入MAC地址//这里软件做了预处理
	if(cfg.ItemSelect(slaveWAddressSel))
	{ 
		pTItem = new CTObj_input_addr(get_tname(idx_INPUT_ADDRESS), this, TID_INPUT_ADDR);
		pTItem->dNode.add_tail(TItemList);
	}
#endif
	//test board pwr on,14
	//打开电源
	pTItem = new CTObj_pwrDut(get_tname(idx_PWR_ON), this, TID_POWER_BOARD);
	pTItem->dNode.add_tail(TItemList);
#if 1
#if 1//yhl 2018/10/30
	/* TODO  *///yhl 2018/10/27.gpio21=0;
	//PULL UP CHIP PIN gpio21=0;
	//在对应的testop oper
	pTItem = new CTObj_dut_into_mp_mode(get_tname(idx_DUT_IN_MP), this, TID_INTO_MP_MODE);
	pTItem->dNode.add_tail(TItemList);
	
	//set dut power off
	pTItem = new CTObj_Battery_off(get_tname(idx_BATTERY_OFF), this, TID_BATTERY_OFF);
	pTItem->dNode.add_tail(TItemList);

	

	//set dut power on
	pTItem = new CTObj_Battery_on(get_tname(idx_BATTERY_ON), this, TID_BATTERY_ON);
	pTItem->dNode.add_tail(TItemList);	

	//set dut to normal mode//模式转换后都必须重启才有效	
	//pTItem = new CTObj_dut_into_normal_mode(get_tname(idx_DUT_IN_NORMAL), this, TID_INTO_NORMAL_MODE);
	//pTItem->dNode.add_tail(TItemList);

	//set dut into normal
#endif
#if 0
	pTItem = new CTObj_dut_into_normal_mode(get_tname(idx_DUT_IN_NORMAL), this, TID_INTO_MP_MODE);
	pTItem->dNode.add_tail(TItemList);
	//set dut power off
	pTItem = new CTObj_Battery_off(get_tname(idx_BATTERY_OFF), this, TID_BATTERY_OFF);
	pTItem->dNode.add_tail(TItemList);
	//set dut power on
	pTItem = new CTObj_Battery_on(get_tname(idx_BATTERY_ON), this, TID_BATTERY_ON);
	pTItem->dNode.add_tail(TItemList);
#endif
	
#if 0	
	//电流测试
	if(pCfgf.ItemSelect(s_PwrOn_cur_sel))
	{		
		pTItem = new CTObj_current(get_tname(idx_CURRENT_ON), this, TID_CURENT_PWR_ON, s_PwrOn_cur_sel, 1);
		pTItem->dNode.add_tail(TItemList);
    }
#endif

#if 0	
	//读软件版本//要在normal模式
	if(cfg.ItemSelect(Par_read_soft_ver))
	{
		pTItem = new CTObj_ShowSoftVer(get_tname(idx_RVER), this, TID_READ_VER);
		pTItem->dNode.add_tail(TItemList);
	}
#endif	



#if 0
	//读软件版本//要在normal模式
	if(cfg.ItemSelect(Par_read_soft_ver))
	{
		pTItem = new CTObj_ShowSoftVer(get_tname(idx_RVER), this, TID_READ_VER);
		pTItem->dNode.add_tail(TItemList);
	}
#endif

	
#if 0	
	//ADC测试
	if(pCfgf.ItemSelect(cfg_adc_chk_sel))
	{
		//set dut power on
		pTItem = new CTObj_Battery_on(get_tname(idx_BATTERY_ON), this, TID_BATTERY_ON);
		pTItem->dNode.add_tail(TItemList);
		
		pTItem = new CTObj_current(get_tname(idx_CURRENT_ON), this, TID_CURENT_PWR_ON, s_PwrOn_cur_sel, 1);
		pTItem->dNode.add_tail(TItemList);

		//set dut power off
		pTItem = new CTObj_Battery_off(get_tname(idx_BATTERY_OFF), this, TID_BATTERY_OFF);
		pTItem->dNode.add_tail(TItemList);
    	}
#endif
       #if 0//yhl
	//set dut to MP mode//模式转换后都必须重启才有效		
	pTItem = new CTObj_dut_into_mp_mode(get_tname(idx_DUT_IN_MP), this, TID_INTO_MP_MODE);
	pTItem->dNode.add_tail(TItemList);
	//set dut power off
	pTItem = new CTObj_Battery_off(get_tname(idx_BATTERY_OFF), this, TID_BATTERY_OFF);
	pTItem->dNode.add_tail(TItemList);
	//set dut power on
	pTItem = new CTObj_Battery_on(get_tname(idx_BATTERY_ON), this, TID_BATTERY_ON);
	pTItem->dNode.add_tail(TItemList);
	#endif
#if 0
	pTItem = new RF_test_integration(get_tname(idx_RF_TEST), this, TID_MT8852_TEST);
	pTItem->dNode.add_tail(TItemList);
#endif

#if 1
	//RF相关测试//要在mp模式
	if(cfg.ItemSelect(s_N4010A_TEST_Sel))
	{
		//all rf test
		
		pTItem = new RF_test_integration(get_tname(idx_RF_TEST), this, TID_MT8852_TEST);
		pTItem->dNode.add_tail(TItemList);
		
             #if 0
		pTItem = new CTObj_Battery_off(get_tname(idx_BATTERY_OFF), this, TID_BATTERY_OFF);
		pTItem->dNode.add_tail(TItemList);

	

		//set dut power on
		pTItem = new CTObj_Battery_on(get_tname(idx_BATTERY_ON), this, TID_BATTERY_ON);
		pTItem->dNode.add_tail(TItemList);	
		#endif
		
//频率校准			
#if 1//yhl
		if(cfg.ItemSelect(ParN4010A_FreTestSel))
		{
			pTItem = new CTObj_N4010A_fre(get_tname(idx_XTAL_ADJ), this, TID_ADJ_XTAL);
			pTItem->dNode.add_tail(TItemList);
		}
#endif

		bool adjustpowerflag = true;
		int idx,tid;
//功率校准
#if 1//yhl
		if(cfg.ItemSelect(ParN4010A_PWLChTestSel))
		{
			if(adjustpowerflag)
			{
				idx = idx_APOWER_ADJ_CHL;
				tid = TID_ADJ_APOWER;
			}
			else
			{
				idx = idx_RF_CHL_PWR;
				tid = TID_N4010A_PWR1;
			}

			pTItem = new CTObj_N4010A_pwr(get_tname(idx), 1, this,adjustpowerflag, tid);
			pTItem->dNode.add_tail(TItemList);
			
			//if(adjustpowerflag)adjustpowerflag = false;
		}
#endif

#if 1//yhl
		if(cfg.ItemSelect(ParN4010A_PWMChTestSel))
		{
			if(adjustpowerflag)
			{
				idx = idx_APOWER_ADJ_CHM;
				tid = TID_ADJ_APOWER;
			}
			else
			{
				idx = idx_RF_CHM_PWR;
				tid = TID_N4010A_PWR2;
			}
			pTItem = new CTObj_N4010A_pwr(get_tname(idx), 2, this,adjustpowerflag, tid);
			pTItem->dNode.add_tail(TItemList);

			//if(adjustpowerflag)adjustpowerflag = false;
		}
#endif	

#if 1//yhl
		if(cfg.ItemSelect(ParN4010A_PWHChTestSel))
		{
			if(adjustpowerflag)
			{
				idx = idx_APOWER_ADJ_CHH;
				tid = TID_ADJ_APOWER;
			}
			else
			{
				idx = idx_RF_CHH_PWR;
				tid = TID_N4010A_PWR3;
			}
			pTItem = new CTObj_N4010A_pwr(get_tname(idx), 3, this,adjustpowerflag, tid);
			pTItem->dNode.add_tail(TItemList);

			//if(adjustpowerflag)adjustpowerflag = false;
		}
#endif

//发送质量EVM
#if 1//yhl
		if(cfg.ItemSelect(Par_lf_evm_chk))
		{
			pTItem = new CTObj_N4010A_EVM(get_tname(idx_RF_CHL_EVM),1, this, TID_N4010A_EVM1);
			pTItem->dNode.add_tail(TItemList);
		}
		
		if(cfg.ItemSelect(Par_mf_evm_chk))
		{
			pTItem = new CTObj_N4010A_EVM(get_tname(idx_RF_CHM_EVM), 2, this, TID_N4010A_EVM2);
			pTItem->dNode.add_tail(TItemList);
		}
		
		if(cfg.ItemSelect(Par_hf_evm_chk))
		{
			pTItem = new CTObj_N4010A_EVM(get_tname(idx_RF_CHH_EVM), 3, this, TID_N4010A_EVM3);
			pTItem->dNode.add_tail(TItemList);
		}
#endif

//灵敏度
#if 1
		if(cfg.ItemSelect(ParN4010A_SSLChTestSel))
		{
			pTItem = new CTObj_N4010A_sens(get_tname(idx_RF_CHL_SENS),1, this, TID_N4010A_SENS1);
			pTItem->dNode.add_tail(TItemList);
		}
#endif
#if 1
		if(cfg.ItemSelect(ParN4010A_SSMChTestSel))
		{
			pTItem = new CTObj_N4010A_sens(get_tname(idx_RF_CHM_SENS), 2, this, TID_N4010A_SENS2);
			pTItem->dNode.add_tail(TItemList);
		}
#endif
#if 1
		if(cfg.ItemSelect(ParN4010A_SSHChTestSel))
		{
			pTItem = new CTObj_N4010A_sens(get_tname(idx_RF_CHH_SENS), 3, this, TID_N4010A_SENS3);
			pTItem->dNode.add_tail(TItemList);
		}
#endif
	}
#endif

#endif
//from here to enter normal test mode.
//------------------------------------------------------------------------------
	/* TODO  *///yhl 2018/10/27.gpio21=1;
	//set gpio21=1;
	//在对应的testop修改
	#if 1
	pTItem = new CTObj_dut_into_normal_mode(get_tname(idx_DUT_IN_NORMAL), this, TID_INTO_MP_MODE);
	pTItem->dNode.add_tail(TItemList);

	//set dut power off	
	pTItem = new CTObj_Battery_off(get_tname(idx_BATTERY_OFF), this, TID_BATTERY_OFF);
	pTItem->dNode.add_tail(TItemList);

	#if 1
	//set dut power on
	pTItem = new CTObj_Battery_on(get_tname(idx_BATTERY_ON), this, TID_BATTERY_ON);
	pTItem->dNode.add_tail(TItemList);	
	#endif
	#endif
//------------------------------------------------------------------------------
#if 1
	//电流测试
	if(pCfgf.ItemSelect(s_PwrOn_cur_sel))
	{		
		pTItem = new CTObj_current(get_tname(idx_CURRENT_ON), this, TID_CURENT_PWR_ON, s_PwrOn_cur_sel, 1);
		pTItem->dNode.add_tail(TItemList);
    }
#endif

#if 1/* TODO  *///yhl 2018/10/27
	//读软件版本//要在normal模式
	if(cfg.ItemSelect(Par_read_soft_ver))
	{
		pTItem = new CTObj_ShowSoftVer(get_tname(idx_RVER), this, TID_READ_VER);
		pTItem->dNode.add_tail(TItemList);
	}
#endif

#if 1
	//IO口测试//要在normal模式
	if(cfg.ItemSelect(Par_8711_io_chk))
	{	
		pTItem = new CTObj_io_port(get_tname(idx_PIO6),this, TID_PIO6, 0); //6,7,8
		pTItem->dNode.add_tail(TItemList);
	}
#endif


#if 0				//two io test combine one io test
	if(cfg.ItemSelect(Par_mcu_io_chk))
	{		
		//IO_TEST(0, TID_PIO0);  //MCU_R
		pTItem = new CTObj_io_port(get_tname(idx_PIO0),this, TID_PIO0, 0); //0,1,2,3,4,5
		pTItem->dNode.add_tail(TItemList);	
	}
#endif



	//写MAC地址//要在mp模式
#if 1//yhl 2018/10/25
	if(cfg.ItemSelect(slaveWAddressSel))
    {
  #if 0
		pTItem = new CTObj_waddr(get_tname(idx_W_MAC_ADDRESS), this, TID_WRITE_ADDR);
		pTItem->dNode.add_tail(TItemList);
#endif
		pTItem = new CTObj_wtriad(get_tname(idx_WTriad), this, TID_WTriad);
			pTItem->dNode.add_tail(TItemList);
    }
#endif

#if 1//yhl 2018/11/20
	//读MAC地址
	if(cfg.ItemSelect(s_readAddress_sel))
	{
		pTItem = new CTObj_DisplayMAC(get_tname(idx_R_MAC_ADDRESS),this,TID_READ_ADDR);
		pTItem->dNode.add_tail(TItemList);
	}
#endif

#if 0
	//保存参数 + 掉电
	if(cfg.ItemSelect(cfg_w_mac_addr_sel) 
		|| (cfg.ItemSelect(s_N4010A_TEST_Sel) 
		&& (cfg.ItemSelect(ParN4010A_FreTestSel) 
		|| cfg.ItemSelect(ParN4010A_PWLChTestSel)
		|| cfg.ItemSelect(ParN4010A_PWMChTestSel)
		|| cfg.ItemSelect(ParN4010A_PWHChTestSel))))
	{
		pTItem = new CTObj_RFPARASAVE(get_tname(idx_RFPARA_SAVE), this, TID_SAVE_RFPARA);
		pTItem->dNode.add_tail(TItemList);
	}
#endif
#if 0
	//set dut power off
	pTItem = new CTObj_Battery_off(get_tname(idx_BATTERY_OFF), this, TID_BATTERY_OFF);
	pTItem->dNode.add_tail(TItemList);
#endif
	
#if 0
	//打开屏蔽箱,同时会打开LED灯
	if(pCfgf.ItemSelect(parTestBoxNeed))
    {
         pTItem = new CTObj_open_tbox(get_tname(idx_OPEN_BOX),this,TID_OPEN_TBOX);
         pTItem->dNode.add_tail(TItemList);
    }
#endif
#if 0
#if 0
	//dut battery on,13,12
	pTItem = new CTObj_Battery_on(get_tname(idx_BATTERY_ON), this, TID_BATTERY_ON);
    pTItem->dNode.add_tail(TItemList);


//----------------------------------------------------------------------------------------------------------------------
	//关闭屏蔽箱
	if(pCfgf.ItemSelect(parTestBoxNeed))
	{
		pTItem = new CTObj_close_tbox(get_tname(idx_CLOSE_BOX),this,TID_CLOSE_BOX);
		pTItem->dNode.add_tail(TItemList);

		//pTItem = new CTObj_tbox_delay(get_tname(idx_CLOSE_BOX_DELAY),this, TID_CLOSE_BOX_DELAY);
        //pTItem->dNode.add_tail(TItemList);
	}
	
	
	//关机电流
	if(cfg.ItemSelect(s_PwrOff_cur_sel))
	{
		pTItem = new CTObj_current(get_tname(idx_CURRENT_OFF), this, TID_CURENT_PWR_OFF, s_PwrOff_cur_sel, 0);
		pTItem->dNode.add_tail(TItemList);
    }
#endif

//----------------------------------------------------------------------------------------------------------------------	

#if 0	
	//spi switch on
	pTItem = new CTObj_spi_switch_on(get_tname(idx_SPI_SW), this, TID_SPI_SW_ON);
	pTItem->dNode.add_tail(TItemList);


	//MCU IO开机
	//set dut power on,10
	pTItem = new CTObj_Vreg_on(get_tname(idx_POWER_VREG), this, TID_POWER_VREG);
	pTItem->dNode.add_tail(TItemList);
#endif	

//----------------------------------------------------------------------------------------------------------------------
#if 0
	//spi unLock
	if(cfg.ItemSelect(slaveSpiunLockSel))
	{
	    pTItem = new CTObj_SpiUnlock(get_tname(idx_SPI_UNLOCK), this, TID_SPI_UNLOCK);
	    pTItem->dNode.add_tail(TItemList);
	}

//----------------------------------------------------------------------------------------------------------------------
#endif	//by long

#if 0
	//FLASH WRITE
	pTItem = new CTObj_flash_write(get_tname(idx_WR_IMG), this, TID_WFILE, NULL);
	pTItem->dNode.add_tail(TItemList);
#endif
	
#if 0
	if(cfg.ItemSelect(s_write_file_sel))    //仅仅写
	{
		CreatTestItem_write_file(Tset, cfg, NULL);
	}
	else if(cfg.ItemSelect(s_verify_write_sel))  //验证不同时也认为是pass,需要写，验证相同时，不需要写
	{
		pTItem = new CTObj_flash_verify(get_tname(idx_PROG_IMG),this, TID_VERIFY_WFILE, true);
		pTItem->dNode.add_tail(TItemList);
		CreatTestItem_write_file(Tset, cfg, &(((CTObj_flash_verify*)pTItem)->verify_result));
	}
	else if(cfg.ItemSelect(s_program_verify_sel))  //仅仅验证不同时也认为是fail
	{
		pTItem = new CTObj_flash_verify(get_tname(idx_PROG_IMG), this, TID_VERIFY_WFILE, false);
		pTItem->dNode.add_tail(TItemList);

		pTItem = new CTObj_conn_spi(get_tname(idx_CONNECT), this, TID_CN_DUT);
        pTItem->dNode.add_tail(TItemList);
	}
	else

	{

		//spi连接
	    pTItem = new CTObj_conn_spi(get_tname(idx_CONNECT), this, TID_CN_DUT);
        pTItem->dNode.add_tail(TItemList);

		pTItem = new CTObj_io_port(get_tname(idx_PIO9),this, TID_PIO9, 9);  \
		pTItem->dNode.add_tail(TItemList);


		pTItem = new CTObj_N4010A_fre(get_tname(idx_XTAL_ADJ), this, TID_ADJ_XTAL);
		pTItem->dNode.add_tail(TItemList);

		pTItem = new CTObj_N4010A_pwr(get_tname(idx_APOWER_ADJ), this, TID_ADJ_APOWER);
		pTItem->dNode.add_tail(TItemList);
		
		pTItem = new CTObj_N4010A_sens(get_tname(idx_RF_CHL_SENS), 0, this, TID_MT8852_SENS1);
		pTItem->dNode.add_tail(TItemList);

		pTItem = new CTObj_RFPARASAVE(get_tname(idx_RFPARA_SAVE), this, TID_SAVE_RFPARA);
		pTItem->dNode.add_tail(TItemList);


	}
#endif
//----------------------------------------------------------------------------------------------------------------------
#if 0	//by long	
	//vm disable
    pTItem = new CTObj_vm_enable(get_tname(idx_DISABLE_VM), this, TID_VM_DISABLE, VM_DISABLE);
	pTItem->dNode.add_tail(TItemList);

	pTItem = new CTObj_reset(get_tname(idx_RESET_TMODE), this, TID_BT_RESET, cfg.Vulong(s_vmDisableTime), false);
	pTItem->dNode.add_tail(TItemList);


//----------------------------------------------------------------------------------------------------------------------
	
	//开机电流
	if(pCfgf.ItemSelect(s_PwrOn_cur_sel))
	{
		pTItem = new CTObj_current(get_tname(idx_CURRENT_ON), this, TID_CURENT_PWR_ON, s_PwrOn_cur_sel, 1);
		pTItem->dNode.add_tail(TItemList);
    }
	
//----------------------------------------------------------------------------------------------------------------------

    //烧录flash分区文件
	if(cfg.ItemSelect(s_extern_flash_burn))
	{
	   	pTItem = new CTObj_extern_flash(get_tname(idx_EXTERN_BURN), this, TID_EXTERN_BURN);
		pTItem->dNode.add_tail(TItemList);
	}
#endif	//by long

#if 0
//---------------------------------------------------------------------------------------------------------------------   
    //显示地址输入框
	if((cfg.ItemSelect(slaveWAddressSel)  ||
		cfg.ItemSelect(s_addr_verify_sel) || 
		cfg.ItemSelect(s_defaddr_sel))    && 
		cfg.ItemSelect(s_InputAddress_man))
	{
		need_input_addr_first = true;	
		pTItem = new CTObj_input_addr(get_tname(idx_INPUT_ADDRESS),this, TID_INPUT_ADDR);
		pTItem->dNode.add_tail(TItemList);
	}
#endif	//by long	
//---------------------------------------------------------------------------------------------------------------------		 
 #if 0
	//读蓝牙地址
	if(pCfgf.ItemSelect(s_readAddress_sel))
	{
	    pTItem = new CTObj_read_addr(get_tname(idx_RADDRESS),this,TID_READ_ADDR);
	    pTItem->dNode.add_tail(TItemList);
	}
	 
//---------------------------------------------------------------------------------------------------------------------
    //贴纸地址验证
	if(cfg.ItemSelect(s_addr_verify_sel) && cfg.ItemSelect(s_InputAddress_man))
    {
		pTItem = new CTObj_addr_verify(get_tname(idx_VLABEL_ADDR),this,TID_LABEL_ADDR_VERIFY);
		pTItem->dNode.add_tail(TItemList);
    }

	//默认地址验证
	if(cfg.ItemSelect(s_defaddr_sel))
	{
		pTItem = new CTObj_defaddr(get_tname(idx_VDEF_ADDR),this,TID_DEFADDR_VERIFY);
		pTItem->dNode.add_tail(TItemList);
	}

	//读蓝牙版本
	if(cfg.ItemSelect(s_verRead_sel))
    {
		pTItem = new CTObj_read_ver(get_tname(idx_RVER),this,TID_READ_VER);
		pTItem->dNode.add_tail(TItemList);
    }

	//读蓝牙名称
	if(cfg.ItemSelect(s_readName_sel))
    {
        pTItem = new CTObj_read_name(get_tname(idx_RNAME),this,TID_READ_NAME);
        pTItem->dNode.add_tail(TItemList);
    }


	//写蓝牙地址
	if(cfg.ItemSelect(slaveWAddressSel))
    {
		if(!cfg.ItemSelect(s_InputAddress_man))
		{ 
		    pTItem = new CTObj_input_addr(get_tname(idx_INPUT_ADDRESS), this, TID_INPUT_ADDR);
		    pTItem->dNode.add_tail(TItemList);
		}
	
	    pTItem = new CTObj_waddr(get_tname(idx_WADDRESS), this, TID_WRITE_ADDR);
	    pTItem->dNode.add_tail(TItemList);
	}

	//写XTAL trim
	if(cfg.ItemSelect(slaveXtalSel) && cfg.ItemSelect(slaveXtalNeedwPar))
    {
        pTItem = new CTObj_wxtal(get_tname(idx_WXTAL),this,TID_WRITE_XTAL);
        pTItem->dNode.add_tail(TItemList);
    }

//------------------------------------------------------------------------------------------------------------------

	//写PSKEY测试
	if(cfg.ItemSelect(s_psk_write_sel))
	{
		GEN_WRITE_PSK_TITEM();
	}


	//将读到的PSKEY和现有的值做比较验证
	if(cfg.ItemSelect(s_psk_ck_sel))
	{
		if(cfg.ItemSelect(s_psk_ck_rest_sel))  
		{
			pTItem = new CTObj_reset(get_tname(idx_RESET_PSKEY), this, TID_BT_RESET, cfg.Vulong(s_psk_ck_restTime),false);
			pTItem->dNode.add_tail(TItemList);
		}
		GEN_READ_PSK_TITEM();
	}
	
//-------------------------------------------------------------------------------------------------------------------
	
	//if(cfg.ItemSelect(s_pio_test_sel))
	//{

#if 0
		pTItem = new CTObj_vm_enable(get_tname(idx_DISABLE_VM), this, TID_VM_DISABLE, VM_DISABLE);
		pTItem->dNode.add_tail(TItemList);
		
		pTItem = new CTObj_codec_io_read(get_tname(idx_CODEC_IO_CLR),this, TID_CODEC_IO_CLR, true);
		pTItem->dNode.add_tail(TItemList);

		pTItem = new CTObj_reset(get_tname(idx_RESET_AUDIO), this, TID_BT_RESET, cfg.Vulong(s_pioResetTime));
		pTItem->dNode.add_tail(TItemList);

		pTItem = new CTObj_vm_enable(get_tname(idx_ENABLE_VM), this, TID_VM_ENABLE, VM_ENABLE);
		pTItem->dNode.add_tail(TItemList);
		
		pTItem = new CTObj_codec_io_read(get_tname(idx_CODEC_IO_WRITE), this, TID_CODEC_IO_STORE, false);
		pTItem->dNode.add_tail(TItemList);

#endif
		
		//pTItem = new CTObj_set_io_dir(get_tname(idx_PIO_SET_DIR), this, TID_PIO_SET_DIR);
		//pTItem->dNode.add_tail(TItemList);

		IO_TEST(0, TID_PIO0);  //pio0
		IO_TEST(1, TID_PIO1);  //pio1
		IO_TEST(2, TID_PIO2);  //pio2
		IO_TEST(3, TID_PIO3);  //pio3
		IO_TEST(4, TID_PIO4);  //pio4
        IO_TEST(5, TID_PIO5);  //pio5
		IO_TEST(6, TID_PIO6);  //pio6
		IO_TEST(7, TID_PIO7);  //pio7
		IO_TEST(8, TID_PIO8);  //pio8
		IO_TEST(9, TID_PIO9);  //pio9
		IO_TEST(10,TID_PIO10);   //pio10
		IO_TEST(11, TID_PIO11);  //pio11
		IO_TEST(12, TID_PIO12);  //pio12
		IO_TEST(13, TID_PIO13);  //pio13
		IO_TEST(14, TID_PIO14);  //pio14
		IO_TEST(15, TID_PIO15);  //pio15

		//uart cts/rts, pio16,
		if(cfg.ItemSelect(s_uart_cts_rts_io_sel))
		{
		   pTItem = new CTObj_uart_cts_rts(get_tname(idx_UART),this, TID_UART_IO);
		   pTItem->dNode.add_tail(TItemList);
		}
		
		//pcm in/out, self test, pio17,18
        if(cfg.ItemSelect(s_pcm_in_out_io_sel))
		{
		   pTItem = new CTObj_pcm_in_out(get_tname(idx_PCM),this, TID_PCM_IO);
		   pTItem->dNode.add_tail(TItemList);
		}

		IO_TEST(19, TID_PIO19);  //pio19, pcm sync
		IO_TEST(20, TID_PIO20);  //pio20, pcm clk
		IO_TEST(21, TID_PIO21);  //pio21
		IO_TEST(22, TID_PIO22);  //pio22
		IO_TEST(23, TID_PIO23);  //pio23
		IO_TEST(24, TID_PIO24);  //pio24
		IO_TEST(25, TID_PIO25);  //pio25
		IO_TEST(26, TID_PIO26);  //pio26
		IO_TEST(27, TID_PIO27);  //pio27
		IO_TEST(28, TID_PIO28);  //pio28
		
		if(!cfg.ItemSelect(s_check_led_use_fiber))
		{
			LED_IO_TEST(29, TID_PIO29);  //pio29
			LED_IO_TEST(30, TID_PIO30);  //pio30
			LED_IO_TEST(31, TID_PIO31);  //pio31
		}
		else
		{
			//dut1 LED color chk
			if(Tset->dev_id == TEST_DEV_ID_A)
			{
				if(cfg.ItemSelect(s_U45_sel))
				{
					LED_COLOR_TEST(29, TID_PIO29, 0,
						MCU_PIO_DUT1_RED_0, MCU_PIO_DUT1_GREEN_0, MCU_PIO_DUT1_BLUE_0,
	                       MCU_PIO_NUM_DUT1_RED_0, MCU_PIO_NUM_DUT1_GREEN_0, MCU_PIO_NUM_DUT1_BLUE_0, 
						   s_U45_r0_ad_adj);
					
					LED_COLOR_TEST(30, TID_PIO30, 1,
						MCU_PIO_DUT1_RED_0, MCU_PIO_DUT1_GREEN_0, MCU_PIO_DUT1_BLUE_0,
	                       MCU_PIO_NUM_DUT1_RED_0, MCU_PIO_NUM_DUT1_GREEN_0, MCU_PIO_NUM_DUT1_BLUE_0, 
						   s_U45_r1_ad_adj);	
				}
				
				if(cfg.ItemSelect(s_U51_sel))
				{
					LED_COLOR_TEST(29, TID_PIO29, 0,
						MCU_PIO_DUT1_RED_1, MCU_PIO_DUT1_GREEN_1, MCU_PIO_DUT1_BLUE_1,
	                       MCU_PIO_NUM_DUT1_RED_1, MCU_PIO_NUM_DUT1_GREEN_1, MCU_PIO_NUM_DUT1_BLUE_1, 
						   s_U51_r0_ad_adj);
					
					LED_COLOR_TEST(30, TID_PIO30, 1,
						MCU_PIO_DUT1_RED_1, MCU_PIO_DUT1_GREEN_1, MCU_PIO_DUT1_BLUE_1,
	                       MCU_PIO_NUM_DUT1_RED_1, MCU_PIO_NUM_DUT1_GREEN_1, MCU_PIO_NUM_DUT1_BLUE_1, 
						   s_U51_r1_ad_adj);
				}
				
				if(cfg.ItemSelect(s_U56_sel))
				{
					LED_COLOR_TEST(29, TID_PIO29, 0,
						MCU_PIO_DUT1_RED_2, MCU_PIO_DUT1_GREEN_2, MCU_PIO_DUT1_BLUE_2,
	                       MCU_PIO_NUM_DUT1_RED_2, MCU_PIO_NUM_DUT1_GREEN_2, MCU_PIO_NUM_DUT1_BLUE_2, 
						   s_U56_r0_ad_adj);
					
					LED_COLOR_TEST(30, TID_PIO29, 1,
						MCU_PIO_DUT1_RED_2, MCU_PIO_DUT1_GREEN_2, MCU_PIO_DUT1_BLUE_2,
	                       MCU_PIO_NUM_DUT1_RED_2, MCU_PIO_NUM_DUT1_GREEN_2, MCU_PIO_NUM_DUT1_BLUE_2, 
						   s_U56_r1_ad_adj);
				}
			}
			else
			{		
				//dut2 LED color chk
				if(cfg.ItemSelect(s_U49_sel))
				{
					LED_COLOR_TEST(29, TID_PIO29, 0,
						MCU_PIO_DUT2_RED_0, MCU_PIO_DUT2_GREEN_0, MCU_PIO_DUT1_BLUE_0,
	                       MCU_PIO_NUM_DUT2_RED_0, MCU_PIO_NUM_DUT2_GREEN_0, MCU_PIO_NUM_DUT2_BLUE_0, 
						   s_U49_r0_ad_adj);
					
					LED_COLOR_TEST(30, TID_PIO30, 1,
						MCU_PIO_DUT2_RED_0, MCU_PIO_DUT2_GREEN_0, MCU_PIO_DUT1_BLUE_0,
	                       MCU_PIO_NUM_DUT2_RED_0, MCU_PIO_NUM_DUT2_GREEN_0, MCU_PIO_NUM_DUT2_BLUE_0, 
						   s_U49_r1_ad_adj);
				}
				
				if(cfg.ItemSelect(s_U55_sel))
				{
					LED_COLOR_TEST(29, TID_PIO29, 0,
						MCU_PIO_DUT2_RED_1, MCU_PIO_DUT2_GREEN_1, MCU_PIO_DUT1_BLUE_1,
	                       MCU_PIO_NUM_DUT2_RED_1, MCU_PIO_NUM_DUT2_GREEN_1, MCU_PIO_NUM_DUT2_BLUE_1, 
						   s_U55_r0_ad_adj);
					
					LED_COLOR_TEST(30, TID_PIO30, 1,
						MCU_PIO_DUT2_RED_1, MCU_PIO_DUT2_GREEN_1, MCU_PIO_DUT1_BLUE_1,
	                       MCU_PIO_NUM_DUT2_RED_1, MCU_PIO_NUM_DUT2_GREEN_1, MCU_PIO_NUM_DUT2_BLUE_1, 
						   s_U55_r1_ad_adj);
				}
				
				if(cfg.ItemSelect(s_U60_sel))
				{
					LED_COLOR_TEST(29, TID_PIO29, 0,
						MCU_PIO_DUT2_RED_2, MCU_PIO_DUT2_GREEN_2, MCU_PIO_DUT1_BLUE_2,
	                       MCU_PIO_NUM_DUT2_RED_2, MCU_PIO_NUM_DUT2_GREEN_2, MCU_PIO_NUM_DUT2_BLUE_2, 
						   s_U60_r0_ad_adj);
					
					LED_COLOR_TEST(30, TID_PIO30, 1,
						MCU_PIO_DUT2_RED_2, MCU_PIO_DUT2_GREEN_2, MCU_PIO_DUT1_BLUE_2,
	                       MCU_PIO_NUM_DUT2_RED_2, MCU_PIO_NUM_DUT2_GREEN_2, MCU_PIO_NUM_DUT2_BLUE_2, 
						   s_U60_r1_ad_adj);
				}
			}
		}
		
		AIO_TEST(0, TID_AIO0);
		AIO_TEST(1, TID_AIO1);
		
		MBIAS_TEST(0, TID_MIBAS_IO0);
		MBIAS_TEST(1, TID_MIBAS_IO1);	
		VBUS_USB3V3_IO_TEST(TID_VBUS_3V3);
		
		if(cfg.ItemSelect(s_usb_np_io_sel))
		{
			pTItem = new CTObj_usb_np_io(get_tname(idx_USB_NP), this, TID_USB_NP_IO);
			pTItem->dNode.add_tail(TItemList);
		}
		
		//音频IO
		if(cfg.ItemSelect(s_audio_mic_spk_sel))
		{
			pTItem = new CTObj_audio_io(get_tname(idx_AUDIO_IO), this, TID_AUDIO_IO);
			pTItem->dNode.add_tail(TItemList);	
		}
    //}


//------------------------------------------------------------------------------------------------------------------

    //vm enable
    pTItem = new CTObj_vm_enable(get_tname(idx_ENABLE_VM), this, TID_VM_ENABLE, VM_ENABLE);
	pTItem->dNode.add_tail(TItemList);

	//pTItem = new CTObj_reset(get_tname(idx_RESET_NMODE), this, TID_BT_RESET, cfg.Vulong(s_vmEnableTime), false);
	//pTItem->dNode.add_tail(TItemList);



//------------------------------------------------------------------------------------------------------------------    

	//晶体校正/ 频率校准
	if(cfg.ItemSelect(slaveXtalSel))
	{		
		pTItem = new CTObj_clr_XtalOffset(get_tname(idx_CLR_XTAL_OFFSET), this, TID_CLR_XTAL);
		pTItem->dNode.add_tail(TItemList);
		
		pTItem = new CTObj_reset(get_tname(idx_RESET_XTAL), this, TID_BT_RESET, cfg.Vulong(s_clrXtalResetTime),false);
		pTItem->dNode.add_tail(TItemList);		
		
		pTItem = new CTObj_crystal_adj(get_tname(idx_XTAL_ADJ), this, TID_ADJ_XTAL);
		pTItem->dNode.add_tail(TItemList);
	}


//------------------------------------------------------------------------------------------------------------------

    //RF test
	if(cfg.ItemSelect(s_MT8852_TEST_Sel))
	{
//#if 0
		if(cfg.ItemSelect(slaveXtalSel))
		{ 
			pTItem = new CTObj_reset(get_tname(idx_RESET_RF), this, TID_BT_RESET, cfg.Vulong(s_brfRstTryDelayTime),false);
			pTItem->dNode.add_tail(TItemList);
		}      
//#endif

		pTItem = new CTObj_mt8852_test(get_tname(idx_RF_TEST), this, TID_MT8852_TEST);
		pTItem->dNode.add_tail(TItemList);


		//RF 功率测试
		if (cfg.ItemSelect(ParMT8852x_OPLFTestSel))
		{  
			 pTItem = new CTObj_mt8852_pwr(get_tname(idx_RF_CHL_PWR), 1, this, TID_MT8852_PWR1);
             pTItem->dNode.add_tail(TItemList);
		}
		if(cfg.ItemSelect(ParMT8852x_OPMFTestSel))
		{
			 pTItem = new CTObj_mt8852_pwr(get_tname(idx_RF_CHM_PWR), 2, this, TID_MT8852_PWR2);
             pTItem->dNode.add_tail(TItemList);
		}
		if(cfg.ItemSelect(ParMT8852x_OPHFTestSel))
		{
			 pTItem = new CTObj_mt8852_pwr(get_tname(idx_RF_CHH_PWR), 3, this, TID_MT8852_PWR3);
             pTItem->dNode.add_tail(TItemList);
		}

		//RF 频率偏移测试
		if(cfg.ItemSelect(ParMT8852x_ICLFTestSel))
		{
			 pTItem = new CTObj_mt8852_offset(get_tname(idx_RF_CHL_FOFFSET), 1, this, TID_MT8852_FREQ1);
             pTItem->dNode.add_tail(TItemList);
		}
		if(cfg.ItemSelect(ParMT8852x_ICMFTestSel))
		{
			 pTItem = new CTObj_mt8852_offset(get_tname(idx_RF_CHM_FOFFSET), 2, this, TID_MT8852_FREQ2);
             pTItem->dNode.add_tail(TItemList);
		}
        if(cfg.ItemSelect(ParMT8852x_ICHFTestSel))
		{
			 pTItem = new CTObj_mt8852_offset(get_tname(idx_RF_CHH_FOFFSET), 3, this, TID_MT8852_FREQ3);
             pTItem->dNode.add_tail(TItemList);
		}

		 //RF 灵敏度测试
		 bool ss_lch_sel = cfg.ItemSelect(ParMT8852x_SSLFTestSel);
		 bool ss_mch_sel = cfg.ItemSelect(ParMT8852x_SSMFTestSel);
		 bool ss_hch_sel = cfg.ItemSelect(ParMT8852x_SSHFTestSel);
		 bool op_hopping,ic_hopping,ss_hopping,hopping = false;
         if(pBtTestSet)
		 {
			 pBtTestSet->hopping_st(op_hopping,ic_hopping,ss_hopping);
			 if(ss_hopping) hopping = true;
		 }
		 else if(cfg.VCString(ParMT8852x_SSHopping) == "HOPON") hopping = true;
         if(hopping)
		 {
			 if(ss_lch_sel || ss_mch_sel || ss_hch_sel)
			 { 
				 pTItem = new CTObj_mt8852_sens(get_tname(idx_RF_SENS_ANY), 1, this, TID_MT8852_SENSANY);
				 pTItem->dNode.add_tail(TItemList);
			 }
		 }
		 else
		 {
			 if(ss_lch_sel)
			 { 
				 pTItem = new CTObj_mt8852_sens(get_tname(idx_RF_CHL_SENS), 1, this, TID_MT8852_SENS1);
				 pTItem->dNode.add_tail(TItemList);
			 }
			 if(ss_mch_sel)
			 { 
				 pTItem = new CTObj_mt8852_sens(get_tname(idx_RF_CHM_SENS), 2, this, TID_MT8852_SENS2);
				 pTItem->dNode.add_tail(TItemList);
			 }
			 if(ss_hch_sel)
			 { 
				 pTItem = new CTObj_mt8852_sens(get_tname(idx_RF_CHH_SENS), 3, this, TID_MT8852_SENS3);
				 pTItem->dNode.add_tail(TItemList);
			 }
		 }
    }

//------------------------------------------------------------------------------------------------------------------	

    if(cfg.ItemSelect(s_RstAfterTest_sel))
	{
		pTItem = new CTObj_reset(get_tname(idx_RESET_AFTER_RF),this, TID_BT_RESET, cfg.Vulong(s_RstTryDelayTime), false);
		pTItem->dNode.add_tail(TItemList);
	}

//------------------------------------------------------------------------------------------------------------------
	
	//充电校正
	if(cfg.ItemSelect(s_charge_sel))
	{
		pTItem = new CTObj_charge_adj(get_tname(idx_CHARGE_ADJ), this, TID_CHARGE_ADJ);
		pTItem->dNode.add_tail(TItemList);
	}


//------------------------------------------------------------------------------------------------------------------
 	//按键检测   
	if(Tset->dev_id == TEST_DEV_ID_A)
	{
		VREG_BUTTON_TEST(1, TID_BTN, MCU_PIO_DUT1_BTN_1, MCU_PIO_NUM_DUT1_BTN_1);
		VREG_BUTTON_TEST(3, TID_BTN, MCU_PIO_DUT1_BTN_2, MCU_PIO_NUM_DUT1_BTN_2);
		VREG_BUTTON_TEST(5, TID_BTN, MCU_PIO_DUT1_BTN_3, MCU_PIO_NUM_DUT1_BTN_3);
		
	}
	else
	{
		VREG_BUTTON_TEST(2, TID_BTN, MCU_PIO_DUT2_BTN_1, MCU_PIO_NUM_DUT2_BTN_1);
		VREG_BUTTON_TEST(4, TID_BTN, MCU_PIO_DUT2_BTN_2, MCU_PIO_NUM_DUT2_BTN_2);
		VREG_BUTTON_TEST(6, TID_BTN, MCU_PIO_DUT2_BTN_3, MCU_PIO_NUM_DUT2_BTN_3);
	}

//------------------------------------------------------------------------------------------------------------------	
	
	//SPI lock
	if(cfg.ItemSelect(slaveSpiLockSel)) 
	{
		pTItem = new CTObj_SpiLock(get_tname(idx_SPI_LOCK), this, TID_SPI_LOCK);
		pTItem->dNode.add_tail(TItemList);	
	}	

//------------------------------------------------------------------------------------------------------------------

	//spi disconnect
	pTItem = new CTObj_discn_spi(get_tname(idx_DISCONNECT), this, TID_DISCN_DUT);
	pTItem->dNode.add_tail(TItemList); 
	
	
//------------------------------------------------------------------------------------------------------------------	

	//打开屏蔽箱,同时会打开LED灯
	if(pCfgf.ItemSelect(parTestBoxNeed))
    {
         pTItem = new CTObj_open_tbox(get_tname(idx_OPEN_BOX),this,TID_OPEN_TBOX);
         pTItem->dNode.add_tail(TItemList);
    }
#endif
#endif
    return(true);
}


//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
const char *str[MAX_DUT_GLODEN_NUM+1] =
{
	    "DUT",
		"golden11",
		"golden12",
		"golden13",
		"golden21",
		"golden22",
		"golden23",
		"golden31",
		"golden32",
		"golden33",
		"golden41",
		"golden42",
		"golden43",
		"goldenxx",
};


int testSet::gloden_idx(struct bt_address_s *addr)
{
        if(!addr) 
			addr = &r_addr;
        if(!addr->is_valid) 
			return(0);

        CString temp;
		temp = addr->mac_addr;
        //temp.Format("%.4X%.2X%.6X",addr->nap,addr->uap,addr->lap);
        for(int i=1;i<MAX_DUT_GLODEN_NUM;i++) 
        {
			if(temp == pMainWnd->dutGloden[i]->addr) 
			{
				return(i);
			}
        }
/*
        temp = "";
        temp.Format("%.4X%.2X",addr->nap,addr->uap);
        if(temp == golden_group) return(MAX_DUT_GLODEN_NUM);
 */       
        return(0);
}


const char* testSet::gloden_idx_name(struct bt_address_s *addr)
{
        int idx = gloden_idx(addr);
        return(str[idx]);
}

const char* testSet::gloden_idx_name(int idx)
{
        if(idx < MAX_DUT_GLODEN_NUM) return(str[idx]);
        return(str[0]);
}
int testSet::golden_idx_in_group(int idx)
{
        idx = idx%3;
        if(!idx) idx = 3;
        return(idx);
}

bool testSet::is_default_addr(void)
{
        return(pCfgf.is_same_default_addr(r_addr));  //should read bt addr
}


//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------


void set_title_show(CString *title)
{
        CString show_tile = *title;
        if(pMainWnd->tSetA->pSpiObj->is_dut_spi_allocated())
        {
                show_tile += "  DUT1:";
               // show_tile += pMainWnd->tSetA->pSpiObj->spi_port->PortName;
		  show_tile += pRTL8711_1_Com->using_port_name;		
        }
	 if(pMainWnd->tSetB->pSpiObj->is_dut_spi_allocated())
        {
			show_tile += "  DUT2:";
			//show_tile += pMainWnd->tSetB->pSpiObj->spi_port->PortName;
			show_tile += pRTL8711_1_Com->using_port_name;
        }
        *title = show_tile;
}


