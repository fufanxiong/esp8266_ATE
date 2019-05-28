/*	err.h
	ydj 2008/10/15
 */
#ifndef __ERR_DEAL_H__
#define	__ERR_DEAL_H__

void ThrowErrMessage(enum ErrSort message,void *ppar = NULL,void *ppar2 = NULL);
void ErrMessageShow(enum ErrSort message,void *ppar = NULL,void *ppar2 = NULL);

enum ErrSort {
	READ_EADDRTBL = 10,					// �� eeprom ���ַ����
	NOCONNECT_DEVICE,					// �����豸����
	EADDRTBL_ATEND,						// ���� eeprom ������ʱ�ѵ����ַβ
	OpenDefaultRM_ERR,					// ���� DefaultRM �Ի�����
	OPENFCOUNTER_ERR,					// ���� frequency Session �Ի�����
	READFCOUNTER_ERR,					// ��Ƶ�ʼƳ���
	TESTOBJSET_ERR,						// ���Զ������ô���
	PASSWORD_ERR,						// �������
	CONFIGFNAME_ERR,					// ����������ļ�����
	CREATCONFIGFILE_ERR,				// ���������ļ�����
	CONFIGFILE_ERR,						// �����ļ����ݴ���
	CONFIGLINE_ERR,						// ���������ļ��������в�������
	CONNECTPWRBOAD_ERR,					// �޷����ӵ�Դ���ư�
	EEPREADFILENULL_ERR,				// ָ������� eeprom �����ļ�������
	EEPREADFILECFG_ERR,					// ָ������� eeprom �����ļ����ó���
	EEPWRITEFILEDATA_ERR,				// ָ��д��� eeprom �����ļ����ݳ���
	EEPCFGFILENULL_ERR,					// ָ�������� eeprom �����ļ�������

};













#endif //__ERR_DEAL_H__
