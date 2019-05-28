/*	err.h
	ydj 2008/10/15
 */
#ifndef __ERR_DEAL_H__
#define	__ERR_DEAL_H__

void ThrowErrMessage(enum ErrSort message,void *ppar = NULL,void *ppar2 = NULL);
void ErrMessageShow(enum ErrSort message,void *ppar = NULL,void *ppar2 = NULL);

enum ErrSort {
	READ_EADDRTBL = 10,					// 读 eeprom 表地址出错
	NOCONNECT_DEVICE,					// 连接设备出错
	EADDRTBL_ATEND,						// 建立 eeprom 数据区时已到表地址尾
	OpenDefaultRM_ERR,					// 建立 DefaultRM 对话出错
	OPENFCOUNTER_ERR,					// 建立 frequency Session 对话出错
	READFCOUNTER_ERR,					// 读频率计出错
	TESTOBJSET_ERR,						// 测试对象设置错误
	PASSWORD_ERR,						// 密码错误
	CONFIGFNAME_ERR,					// 输入的配置文件错误
	CREATCONFIGFILE_ERR,				// 创建配置文件错误
	CONFIGFILE_ERR,						// 配置文件内容错误
	CONFIGLINE_ERR,						// 进行配置文件操作的行参数错误
	CONNECTPWRBOAD_ERR,					// 无法连接电源控制板
	EEPREADFILENULL_ERR,				// 指定读入的 eeprom 配置文件不存在
	EEPREADFILECFG_ERR,					// 指定读入的 eeprom 配置文件配置出错
	EEPWRITEFILEDATA_ERR,				// 指定写入的 eeprom 配置文件数据出错
	EEPCFGFILENULL_ERR,					// 指定操作的 eeprom 配置文件不存在

};













#endif //__ERR_DEAL_H__
