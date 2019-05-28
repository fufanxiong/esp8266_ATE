#pragma once

#ifndef _TEST_HEADER_H_
#define _TEST_HEADER_H_

#define DTC_SUPPORT_PIN_MAX 22


/*tagTxPacketTypeStru*/
typedef struct{
	unsigned char type;
	unsigned short length;
	char *szText;
} PACKET_TYPE_STRU;

//////////////////////////////////////////////////////////////////////////
#define PACKET_TYPE_LIST_COUNT 9
static PACKET_TYPE_STRU packetTypeList[] = {
	4,  27,  _T("DH1"),
	11, 183, _T("DH3"),
	15, 339, _T("DH5"),
	20, 54,  _T("2DH1"),
	26, 367, _T("2DH3"),
	30, 679, _T("2DH5"),
	24, 83,   _T("3DH1"),
	27, 552,  _T("3DH3"),
	31, 1021, _T("3DH5")
};

#endif