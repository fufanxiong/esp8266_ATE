/*	device.h
 */
#ifndef __DONGLE_H__
#define	__DONGLE_H__
#include "CommHead.h"

class CDialog;

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#define	DONGLE_USB_VID		0x0a13
#define	DONGLE_USB_PID		0x1020

#define	BT_MESG_BUF_LEN		512
#define	BT_MESG_ITEM_LEN	64
#define	RW_BUF_LEN		64

#define	BT_DI_CONNECT_TIME	4
#define	BT_DI_CONNECT_TIME1st	8




enum {
	// the next for usb
	UHID_CMD_test = 1,
	UHID_CMD_clr_pdl,
	UHID_CMD_que_statu,
	UHID_CMD_inquiring,
	UHID_CMD_connection_report,
	UHID_CMD_addr_name_report,
	UHID_CMD_io_read,
	UHID_CMD_io_write,
	UHID_CMD_io_map,

	// the next for app
	MSG_NULL,
	MSG_CONNECT_USB_DONGLE,
	MSG_PAIR,
	MSG_READ_PIO,
	MSG_io_read,
	MSG_io_write,
	MSG_io_map,
	MSG_END
};

enum
{
    AppStateUninitialised = 0, 	/* Dongle is not initialised for operation */
    AppStateInitialising,  	/* Dongle is currently initialising */
    AppStateIdle,          	/* Dongle is idle, with or without connections */
    AppStateInquiring,     	/* Dongle is looking for devices */
    AppStateSearching,     	/* Dongle is performing an SDP search on a specific device */
    AppStateConnecting,    	/* Dongle is connecting to a specific device */
    AppStateStreaming,     	/* Dongle is streaming audio via a2dp */
    AppStateInCall,        	/* Dongle is setting-up or managing an active call via hsp/hfp */
    AppStateEnteringDfu,   	/* Dongle is waiting to enter DFU mode */
    
    AppStateLowBattery,    	/* Only change the LED flash pattern */
    AppStatePoweredOff,    	/* Only change the LED flash pattern */
    MaxAppStates
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
struct cmesg_item {
	int id;
	int len;
	unsigned long data[BT_MESG_ITEM_LEN];
};

struct bdaddr_s {
	int lap; 
	int uap; 
	int nap; 
};

class cmesg_buf
{
private:
	cmesg_buf& operator = (const cmesg_buf &x);
	cmesg_buf(cmesg_buf &x);

	void put_data_to_mesg_buf(unsigned long data);
	unsigned long get_data_from_mesg_buf(void);
protected:
	HANDLE hMutex_mesg_buf;
	unsigned long bt_mesg_buf[BT_MESG_BUF_LEN];
	int bt_mesg_buf_ridx;
	int bt_mesg_buf_widx;
public:
	cmesg_buf(const char* Mutex_str);
	virtual ~cmesg_buf();

	bool put_mesg(int mesg_id,char *par,int par_len);
	bool put_mesg(int mesg_id,unsigned long *par,int par_len);
	bool get_mesg(int &mesg_id,unsigned long *par,int &par_len);
	bool put_mesg(struct cmesg_item &msg) { return put_mesg(msg.id,msg.data,msg.len); };
	bool get_mesg(struct cmesg_item &msg) { return get_mesg(msg.id,msg.data,msg.len); };
	bool clean_mesg_buffer(void);
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class usb_dongle
{
private:
	usb_dongle& operator = (const usb_dongle &x);
	usb_dongle(usb_dongle &x);

	static bool usb_dongle_have_created;
	static DWORD WINAPI usb_hid_recive(LPVOID lpParameter);
	static DWORD WINAPI BtMonitor(LPVOID lpParameter);
protected:
	CDialog* dlg;
	cmesg_buf* bt_mesg_buf;
	cmesg_buf* uhidrx_mesg_buf;
	HANDLE hthread_btmonitor;
	DWORD hthreadId_btmonitor;
	HANDLE hthread_uhidrx;
	DWORD hthreadId_uhidrx;
	bool dongle_connected;

	int sys_statu;
	int bt_connect;
	struct bdaddr_s baddress;

	bool op_que_finish;
	bool op_is_ok;
	unsigned long io_mask;
	unsigned long io_bits;

	void send_app_mesg(UINT message,WPARAM wParam,LPARAM lParam);
	void get_app_mesg(struct cmesg_item &mesg);
	bool get_usb_mesg(struct cmesg_item &mesg);
	bool wait_usb_mesg(struct cmesg_item &ack_mesg,int try_times = 5);
	bool send_usb_mesg(int cmd,unsigned long *buf,int len);
	bool send_usb_pio_mesg(int cmd,unsigned long mask,unsigned long bits);
	bool get_usb_pio_mesg_par(struct cmesg_item &mesg,unsigned long& mask,unsigned long& bits);
	bool put_app_mesg_remap_io_port(void);
	bool put_app_mesg_read_io_port(unsigned long mask,unsigned long bits);
	bool put_app_mesg_write_io_port(unsigned long mask,unsigned long bits);
public:
	usb_dongle(CDialog* dialog);
	virtual ~usb_dongle();

	bool initial(void);
	void put_app_mesg(int mesg_id,unsigned long *par,int par_len);
	void put_app_mesg(struct cmesg_item &mesg) { put_app_mesg(mesg.id,mesg.data,mesg.len); };
	bool read_io(unsigned long& mask,unsigned long& bits);
	bool write_io(unsigned long mask,unsigned long bits);
};








#endif //__DONGLE_H__
