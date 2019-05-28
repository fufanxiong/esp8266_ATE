/*	err.cpp
 */

#include "stdafx.h"
#include "err.h"


void ThrowErrMessage(enum ErrSort message,void *ppar,void *ppar2)
{
	ErrMessageShow(message,ppar,ppar2);
}

void ErrMessageShow(enum ErrSort message,void *ppar,void *ppar2)
{
	CString err_str;

	switch(message) {
	case READ_EADDRTBL:
		err_str  = "不能读入 EEPROM 地址表!\n";
		break;
	case NOCONNECT_DEVICE:
		err_str = "还未与设备建立联系!\n";
		break;
	case EADDRTBL_ATEND:
		err_str = "已到 EEPROM 地址表尾!\n";
		break;
	case OpenDefaultRM_ERR:
		err_str = "无法建立 DefaultRM 对话!\n";
		break;
	case OPENFCOUNTER_ERR:
		err_str = "在端口 "+
			*((CString*)ppar)+
			" 上不能打开频率计 "+
			*((CString*)ppar2) +
			" !\n";
		break;
	case READFCOUNTER_ERR:
		err_str = "无法读连在端口 "+
			*((CString*)ppar)+
			" 上的频率计 "+
			*((CString*)ppar2) +
			" !\n";
		break;
	case TESTOBJSET_ERR:
		err_str = "测试对象设置错误!\n";
		break;
	case PASSWORD_ERR:
		err_str = "密码错误!\n";
		break;
	case CONFIGFNAME_ERR:
		err_str = "输入的配置文件错误!\n";
		break;
	case CREATCONFIGFILE_ERR:
		err_str = "创建配置文件错误!\n";
		break;
	case CONFIGFILE_ERR:
		err_str = "配置文件内容错误!\n";
		break;
	case CONFIGLINE_ERR:
		err_str = "进行配置文件操作的行参数错误!\n";
		break;
	case CONNECTPWRBOAD_ERR:
		err_str = "无法连接电源控制板!\n";
		break;
	case EEPREADFILENULL_ERR:
		err_str = "指定读入的 eeprom 配置文件不存在!\n";
	break;	
	case EEPWRITEFILEDATA_ERR:
		err_str = "指定写入的 eeprom 配置文件数据出错!\n";
	break;
	case EEPREADFILECFG_ERR:
		err_str = "指定读入的 eeprom 配置文件配置出错!\n";
	break;
	case EEPCFGFILENULL_ERR:
		err_str = "指定操作的 eeprom 配置文件不存在!\n";
	break;
		

		
	default:
		err_str = "Undefine err message detect!\n";
		break;
	}
	AfxMessageBox(err_str.GetBuffer(0));
}







