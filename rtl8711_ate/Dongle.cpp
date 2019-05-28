/*	device.cpp
 */

#include "stdafx.h"
#include "Newdevice.h"
#include "Config.h"
#include "PD218.h"
#include "PD218Dlg.h"
#include "err.h"
#include "Dongle.h"
#include "TestEngine.h"
#include "mtk_lib.h"

#include <visa.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h>
#include <stdio.h> 
#include <stdlib.h>
#include <fstream.h>


#pragma comment (lib,"TestEngine.lib")
#pragma comment (lib,"mtk.lib")
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
cmesg_buf::cmesg_buf(const char* Mutex_str)
{
	hMutex_mesg_buf = OpenMutex(MUTEX_ALL_ACCESS,false,Mutex_str);
	if(NULL == hMutex_mesg_buf) {
		hMutex_mesg_buf = CreateMutex(NULL,false,Mutex_str);
	}
	bt_mesg_buf_ridx = 0;
	bt_mesg_buf_widx = 0;
}

cmesg_buf::~cmesg_buf()
{
	if(hMutex_mesg_buf) {
		ReleaseMutex(hMutex_mesg_buf);
		CloseHandle(hMutex_mesg_buf);
	}
}

void cmesg_buf::put_data_to_mesg_buf(unsigned long data)
{
	bt_mesg_buf[bt_mesg_buf_widx] = data;
	if(++bt_mesg_buf_widx >= BT_MESG_BUF_LEN) bt_mesg_buf_widx = 0;
}

unsigned long cmesg_buf::get_data_from_mesg_buf(void)
{
	unsigned long data = (unsigned long)bt_mesg_buf[bt_mesg_buf_ridx];
	if(++bt_mesg_buf_ridx >= BT_MESG_BUF_LEN) bt_mesg_buf_ridx = 0;
	return(data);
}

bool cmesg_buf::put_mesg(int mesg_id,char *par,int par_len)
{
	if(!par) par_len = 0;
	if(par_len > BT_MESG_ITEM_LEN) return(false);
	if(WaitForSingleObject(hMutex_mesg_buf,INFINITE) != WAIT_OBJECT_0) 
		return(false);

	int left_mesg_rom;
	if(bt_mesg_buf_widx >= bt_mesg_buf_ridx)
		left_mesg_rom = BT_MESG_BUF_LEN + bt_mesg_buf_ridx - 
				bt_mesg_buf_widx;
	else left_mesg_rom = bt_mesg_buf_ridx - bt_mesg_buf_widx;
	
	if(left_mesg_rom >= (par_len+2)) {
		put_data_to_mesg_buf(mesg_id);
		put_data_to_mesg_buf(par_len);
		if(par) {
			for(int i=0;i<par_len;i++) put_data_to_mesg_buf(par[i]);
		}		
	}

	ReleaseMutex(hMutex_mesg_buf);
	return(true);
}

bool cmesg_buf::put_mesg(int mesg_id,unsigned long *par,int par_len)
{
	if(!par) par_len = 0;
	if(par_len > BT_MESG_ITEM_LEN) return(false);
	if(WaitForSingleObject(hMutex_mesg_buf,INFINITE) != WAIT_OBJECT_0) 
		return(false);

	int left_mesg_rom;
	if(bt_mesg_buf_widx >= bt_mesg_buf_ridx)
		left_mesg_rom = BT_MESG_BUF_LEN + bt_mesg_buf_ridx - 
				bt_mesg_buf_widx;
	else left_mesg_rom = bt_mesg_buf_ridx - bt_mesg_buf_widx;
	
	if(left_mesg_rom >= (par_len+2)) {
		put_data_to_mesg_buf(mesg_id);
		put_data_to_mesg_buf(par_len);
		if(par) {
			for(int i=0;i<par_len;i++) put_data_to_mesg_buf(par[i]);
		}		
	}

	ReleaseMutex(hMutex_mesg_buf);
	return(true);
}

bool cmesg_buf::get_mesg(int &mesg_id,unsigned long *par,int &par_len)
{
	if(bt_mesg_buf_ridx == bt_mesg_buf_widx) return(false);
	if(WaitForSingleObject(hMutex_mesg_buf,INFINITE) != WAIT_OBJECT_0) 
		return(false);

	mesg_id = get_data_from_mesg_buf();
	par_len = get_data_from_mesg_buf();
	for(int i=0;i<par_len;i++) par[i] = get_data_from_mesg_buf();
	
	ReleaseMutex(hMutex_mesg_buf);
	return(true);
}

bool cmesg_buf::clean_mesg_buffer(void)
{
	if(WaitForSingleObject(hMutex_mesg_buf,INFINITE) != WAIT_OBJECT_0) 
		return(false);
	bt_mesg_buf_ridx = bt_mesg_buf_widx;
	
	ReleaseMutex(hMutex_mesg_buf);
	return(true);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
const char hMutex_bt_mesg_bufName[] = "bt_mesg_buf EventContainer mutex";
const char hMutex_uhidrx_mesg_bufName[] = "hMutex_uhidrx_mesg_bufName";
bool usb_dongle::usb_dongle_have_created = 0;

usb_dongle::usb_dongle(CDialog* dialog)
{
	dlg = dialog;
	bt_mesg_buf = new cmesg_buf(hMutex_bt_mesg_bufName);
	uhidrx_mesg_buf = new cmesg_buf(hMutex_uhidrx_mesg_bufName);
	if(!usb_dongle_have_created) {
		usb_dongle_have_created = 1;
		put_app_mesg(MSG_CONNECT_USB_DONGLE,NULL,0);
		hthread_btmonitor = CreateThread(NULL,
						 0,
						 BtMonitor,
						 (usb_dongle*)this,
						 0,
						 &hthreadId_btmonitor);
		hthread_uhidrx = CreateThread(NULL,
						 0,
						 usb_hid_recive,
						 (usb_dongle*)this,
						 0,
						 &hthreadId_uhidrx);
	}
}

usb_dongle::~usb_dongle()
{
	if(hthread_btmonitor) TerminateThread(hthread_btmonitor,EXIT_FAILURE);
	CloseHandle(hthread_btmonitor);
	hthread_btmonitor = NULL;
	hthreadId_btmonitor = NULL;
	if(hthread_uhidrx) TerminateThread(hthread_uhidrx,EXIT_FAILURE);
	CloseHandle(hthread_uhidrx);
	hthread_uhidrx = NULL;
	hthreadId_uhidrx = NULL;

	if(bt_mesg_buf) delete bt_mesg_buf;
	bt_mesg_buf = NULL;
	if(uhidrx_mesg_buf) delete uhidrx_mesg_buf;
	uhidrx_mesg_buf = NULL;

	usb_dongle_have_created = 0;
}

void usb_dongle::send_app_mesg(UINT message,WPARAM wParam,LPARAM lParam)
{
	dlg->SendMessage(message,wParam,lParam);
}

void usb_dongle::get_app_mesg(struct cmesg_item &mesg) 
{
	bt_mesg_buf->get_mesg(mesg);
}

bool usb_dongle::initial(void)
{
	return(true);
}

bool usb_dongle::get_usb_mesg(struct cmesg_item &mesg)
{
	cmesg_buf* pmesg = uhidrx_mesg_buf;
	return(pmesg->get_mesg(mesg));
}

bool usb_dongle::wait_usb_mesg(struct cmesg_item &ack_mesg,int try_times)
{
	cmesg_buf* pmesg = uhidrx_mesg_buf;
	struct cmesg_item rx_mesg = {MSG_NULL,0};

	if(!try_times) try_times = 40;
	for(int i=0;i<try_times;i++) {
		rx_mesg.id = MSG_NULL;
		get_usb_mesg(rx_mesg);
		if(MSG_NULL != rx_mesg.id) {
			if((rx_mesg.id == ack_mesg.id) && (rx_mesg.len >= ack_mesg.len)){
				int j = 0;
				for(j=0;j<ack_mesg.len;j++) 
					if(rx_mesg.data[j] != ack_mesg.data[j]) break;
				if(j == ack_mesg.len) return(true);
			}
		}
		Sleep(50);
	}
	return(false);
}

bool usb_dongle::send_usb_mesg(int cmd,unsigned long *buf,int len)
{
	char data[32] = {0};

	data[0] = (char)cmd;
	data[1] = (char)len;

	if(len > (32-2)) len = 32-2;
	for(int i=0;i<len;i++) data[i+2] = (char)buf[i];
	return(dongle_write_data(0,data,len+2));
}

bool usb_dongle::send_usb_pio_mesg(int cmd,unsigned long mask,unsigned long bits)
{
	struct cmesg_item mesg = { cmd,8,{0} };
	unsigned long *pstr = mesg.data;

	mask &= 0x003fffff;
	bits &= mask;
	if(!mask) return(false);

	*pstr++ = (char)((mask>>24)&0xff);
	*pstr++ = (char)((mask>>16)&0xff);
	*pstr++ = (char)((mask>>8)&0xff);
	*pstr++ = (char)(mask&0xff);

	*pstr++ = (char)((bits>>24)&0xff);
	*pstr++ = (char)((bits>>16)&0xff);
	*pstr++ = (char)((bits>>8)&0xff);
	*pstr++ = (char)(bits&0xff);
	send_usb_mesg(mesg.id,mesg.data,mesg.len);
	return(true);
}

bool usb_dongle::get_usb_pio_mesg_par(struct cmesg_item &mesg,
				      unsigned long& mask,unsigned long& bits)
{
	if(mesg.len < 8) return(false);
	unsigned long mask_temp = 0;
	unsigned long bits_temp = 0;

	unsigned long *pdata = mesg.data;
	mask_temp = *pdata++;
	mask_temp = (mask_temp<<8)|(*pdata++&0xff);
	mask_temp = (mask_temp<<8)|(*pdata++&0xff);
	mask_temp = (mask_temp<<8)|(*pdata++&0xff);

	bits_temp = *pdata++;
	bits_temp = (bits_temp<<8)|(*pdata++&0xff);
	bits_temp = (bits_temp<<8)|(*pdata++&0xff);
	bits_temp = (bits_temp<<8)|(*pdata++&0xff);

	mask = mask_temp;
	bits = bits_temp;
	return(true);
}

bool usb_dongle::put_app_mesg_remap_io_port(void)
{
	struct cmesg_item mesg = { UHID_CMD_io_map,2,{0} };

	unsigned long mask = 0x003f0000;
	unsigned long data = mask;
	mesg.data[0] = mask;
	mesg.data[1] = data;
	put_app_mesg(mesg);
	return(true);
}

bool usb_dongle::put_app_mesg_read_io_port(unsigned long mask,unsigned long data)
{
	struct cmesg_item mesg = { UHID_CMD_io_read,2,{0} };

	mask &= 0x003fffff;
	data &= mask;
	if(!mask) return(false);
	mesg.data[0] = mask;
	mesg.data[1] = data;
	put_app_mesg(mesg);
	return(true);
}

bool usb_dongle::put_app_mesg_write_io_port(unsigned long mask,unsigned long data)
{
	struct cmesg_item mesg = { UHID_CMD_io_write,2,{0} };

	mask &= 0x003fffff;
	data &= mask;
	if(!mask) return(false);
	mesg.data[0] = mask;
	mesg.data[1] = data;
	put_app_mesg(mesg);
	return(true);
}

DWORD WINAPI usb_dongle::usb_hid_recive(LPVOID lpParameter)
{
	usb_dongle* dongle = (usb_dongle*)lpParameter;

	cmesg_buf* pmesg = dongle->uhidrx_mesg_buf;
	char read_buf[RW_BUF_LEN] = {0};
	int read_len = 0;
	bool result = 0;
	int mesg_len = 0;
	int mesg_id = 0;

	while(true) {
		result = dongle_read_data(0,read_buf,read_len);
		if(result && read_len) {
			mesg_id = read_buf[0];
			mesg_len = read_buf[1];
			pmesg->put_mesg(mesg_id,read_buf+2,mesg_len);
		}
		if(!result) Sleep(100);
	}
	return(0);
}

DWORD WINAPI usb_dongle::BtMonitor(LPVOID lpParameter)
{
	usb_dongle* dongle = (usb_dongle*)lpParameter;
	bool& dongle_connected = dongle->dongle_connected;
	int& sys_statu = dongle->sys_statu;
	int& bt_connect = dongle->bt_connect;
	struct bdaddr_s& baddress = dongle->baddress;

	cmesg_buf* pmesg = dongle->bt_mesg_buf;
	struct cmesg_item bt_mesg = {MSG_NULL,0};
	struct cmesg_item bt_mesg_ack = {MSG_NULL,0};
	bool result = 0;
	CString temp;
	unsigned long *pdata;

	unsigned long io_mask;
	unsigned long io_bits;
	struct cmesg_item io_op_mesg;
	bool io_op_busy = 0;
	int io_op_try_times = 0;
	int io_op_cry_cycle = 0;
	const int io_op_try_max_times = 5;


while(true) {
	// message from usb dongle
	bt_mesg.id = MSG_NULL;
	dongle->get_usb_mesg(bt_mesg);
	switch(bt_mesg.id) {
	case UHID_CMD_que_statu:
		if(bt_mesg.data[0] < MaxAppStates) {
			sys_statu = bt_mesg.data[0];
			if((sys_statu > AppStateIdle) && (bt_connect <= BT_DI_CONNECT_TIME))
				bt_connect = BT_DI_CONNECT_TIME;
		}
	break;
	case UHID_CMD_addr_name_report:
		sys_statu = bt_mesg.data[0];
		if(bt_mesg.len >= 6) {
			if(bt_connect <= BT_DI_CONNECT_TIME) bt_connect = BT_DI_CONNECT_TIME;
			pdata = &bt_mesg.data[1];
			baddress.lap = ((pdata[3]&0xff)<<16)|((pdata[4]&0xff)<<8)|(pdata[5]&0xff);
			baddress.uap = pdata[2]&0xff;
			baddress.nap = ((pdata[0]&0xff)<<8)|(pdata[1]&0xff);
		}else {
			if((sys_statu <= AppStateIdle) && bt_connect) bt_connect -= 1;
		}
	break;
	case UHID_CMD_io_read:
		io_op_busy = 0;
		dongle->op_is_ok = dongle->get_usb_pio_mesg_par(bt_mesg,io_mask,io_bits);
		if(dongle->op_is_ok) {
			dongle->io_mask = io_mask;
			dongle->io_bits = io_bits;
		}
		dongle->op_que_finish = 1;
	break;
	case UHID_CMD_io_write:
		io_op_busy = 0;
		dongle->op_is_ok = dongle->get_usb_pio_mesg_par(bt_mesg,io_mask,io_bits);
		if(dongle->op_is_ok) {
			dongle->io_mask = io_mask;
			dongle->io_bits = io_bits;
		}
		dongle->op_que_finish = 1;
	break;
	case UHID_CMD_io_map:
		io_op_busy = 0;
		dongle->op_is_ok = dongle->get_usb_pio_mesg_par(bt_mesg,io_mask,io_bits);
		if(dongle->op_is_ok) {
			dongle->io_mask = io_mask;
			dongle->io_bits = io_bits;
		}
		dongle->op_que_finish = 1;
	break;
	}

	// message from app
	bt_mesg.id = MSG_NULL;
	dongle->get_app_mesg(bt_mesg);
	switch(bt_mesg.id) {
	case MSG_CONNECT_USB_DONGLE:
		dongle_connected = 
			connect_usb_device(DONGLE_USB_VID,DONGLE_USB_PID,0);
		Sleep(100);
		if(dongle_connected) dongle->put_app_mesg_remap_io_port();
		bt_connect = 0;
	break;
	case MSG_PAIR:
		if(!dongle_connected) break;
		if(sys_statu > AppStateIdle) break;
		if((sys_statu == AppStateIdle) && bt_connect) break;
		if(sys_statu >= AppStateInquiring) break;
		bt_connect = BT_DI_CONNECT_TIME1st;
		dongle->send_usb_mesg(UHID_CMD_inquiring,0,0);
	break;
	case UHID_CMD_io_read:
		if(io_op_busy) dongle->put_app_mesg(bt_mesg);
		else {
			dongle->op_is_ok = 0;
			io_op_busy = dongle->send_usb_pio_mesg(bt_mesg.id,
							       bt_mesg.data[0],
							       bt_mesg.data[1]);
			if(io_op_busy) {
				io_op_cry_cycle = 0;
				io_op_try_times = 0;
				io_op_mesg = bt_mesg;
			}
		}
	break;
	case UHID_CMD_io_write:
		if(io_op_busy) dongle->put_app_mesg(bt_mesg);
		else {
			dongle->op_is_ok = 0;
			io_op_busy = dongle->send_usb_pio_mesg(bt_mesg.id,
							       bt_mesg.data[0],
							       bt_mesg.data[1]);
			if(io_op_busy) {
				io_op_cry_cycle = 0;
				io_op_try_times = 0;
				io_op_mesg = bt_mesg;
			}
		}
	break;
	case UHID_CMD_io_map:
		if(io_op_busy) dongle->put_app_mesg(bt_mesg);
		else {
			dongle->op_is_ok = 0;
			io_op_busy = dongle->send_usb_pio_mesg(bt_mesg.id,
							       bt_mesg.data[0],
							       bt_mesg.data[1]);
			if(io_op_busy) {
				io_op_cry_cycle = 0;
				io_op_try_times = 0;
				io_op_mesg = bt_mesg;
			}
		}
	break;
	}

	Sleep(10);
	if(io_op_busy && (io_op_cry_cycle++ >100/10)) {
		io_op_cry_cycle = 0;
		if(io_op_try_times++ > 5) {
			io_op_busy = 0;
			dongle->op_que_finish = 1;
		}else {
			dongle->send_usb_pio_mesg(io_op_mesg.id,
						  io_op_mesg.data[0],
						  io_op_mesg.data[1]);
		}
	}
}

	if(dongle->dongle_connected) disconnect_usb_device();
	dongle->dongle_connected = NULL;
	return(0);
}


void usb_dongle::put_app_mesg(int mesg_id,unsigned long *par,int par_len) 
{
	bt_mesg_buf->put_mesg(mesg_id,par,par_len);
}

bool usb_dongle::read_io(unsigned long& mask,unsigned long& bits)
{
	int max_wait_time = 1200/10;
	op_que_finish = 0;
	op_is_ok = 0;
	if(!put_app_mesg_read_io_port(mask,bits)) return(false);
	for(int i=0;i<max_wait_time;i++) {
		Sleep(10);
		if(!op_que_finish) continue;
		if(op_is_ok) {
			mask = io_mask;
			bits = io_bits;
		}
		return(op_is_ok);
	}
	return(false);
}

bool usb_dongle::write_io(unsigned long mask,unsigned long bits)
{
	int max_wait_time = 1200/10;
	op_que_finish = 0;
	op_is_ok = 0;
	if(!put_app_mesg_write_io_port(mask,bits)) return(false);
	for(int i=0;i<max_wait_time;i++) {
		Sleep(10);
		if(!op_que_finish) continue;
		if(op_is_ok) {
			mask = io_mask;
			bits = io_bits;
		}
		return(op_is_ok);
	}
	return(false);
}



