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
		err_str  = "���ܶ��� EEPROM ��ַ��!\n";
		break;
	case NOCONNECT_DEVICE:
		err_str = "��δ���豸������ϵ!\n";
		break;
	case EADDRTBL_ATEND:
		err_str = "�ѵ� EEPROM ��ַ��β!\n";
		break;
	case OpenDefaultRM_ERR:
		err_str = "�޷����� DefaultRM �Ի�!\n";
		break;
	case OPENFCOUNTER_ERR:
		err_str = "�ڶ˿� "+
			*((CString*)ppar)+
			" �ϲ��ܴ�Ƶ�ʼ� "+
			*((CString*)ppar2) +
			" !\n";
		break;
	case READFCOUNTER_ERR:
		err_str = "�޷������ڶ˿� "+
			*((CString*)ppar)+
			" �ϵ�Ƶ�ʼ� "+
			*((CString*)ppar2) +
			" !\n";
		break;
	case TESTOBJSET_ERR:
		err_str = "���Զ������ô���!\n";
		break;
	case PASSWORD_ERR:
		err_str = "�������!\n";
		break;
	case CONFIGFNAME_ERR:
		err_str = "����������ļ�����!\n";
		break;
	case CREATCONFIGFILE_ERR:
		err_str = "���������ļ�����!\n";
		break;
	case CONFIGFILE_ERR:
		err_str = "�����ļ����ݴ���!\n";
		break;
	case CONFIGLINE_ERR:
		err_str = "���������ļ��������в�������!\n";
		break;
	case CONNECTPWRBOAD_ERR:
		err_str = "�޷����ӵ�Դ���ư�!\n";
		break;
	case EEPREADFILENULL_ERR:
		err_str = "ָ������� eeprom �����ļ�������!\n";
	break;	
	case EEPWRITEFILEDATA_ERR:
		err_str = "ָ��д��� eeprom �����ļ����ݳ���!\n";
	break;
	case EEPREADFILECFG_ERR:
		err_str = "ָ������� eeprom �����ļ����ó���!\n";
	break;
	case EEPCFGFILENULL_ERR:
		err_str = "ָ�������� eeprom �����ļ�������!\n";
	break;
		

		
	default:
		err_str = "Undefine err message detect!\n";
		break;
	}
	AfxMessageBox(err_str.GetBuffer(0));
}







