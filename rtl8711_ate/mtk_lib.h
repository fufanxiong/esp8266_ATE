//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// mtk_lib.h : Í·ÎÄ¼þ
#ifdef MTK_API
#else
#define	MTK_API _declspec(dllimport)
#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
struct MTK_API ignor_address_s {
	unsigned long start_address;
	unsigned long end_address;
	bool filter_by_skip;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool MTK_API connect_usb_device(USHORT USB_VID,USHORT USB_PID,bool overlapped);
void MTK_API disconnect_usb_device(void);
bool MTK_API is_usb_dev_connected(USHORT USB_VID,USHORT USB_PID);
bool MTK_API dongle_write_data(int rid,char* data,int len);
bool MTK_API dongle_read_data(int rid,char* data,int &len);


void MTK_API get_uuid_from_str(const char* str,UUID &uid);
void MTK_API ErrorMegGet(char* &mesg);
bool MTK_API cal_file_check_sum(const char* file,unsigned long len,
			   unsigned long &check_sum,unsigned long &crc_16,
			   unsigned long &crc_32,
			   const struct ignor_address_s *ignor_arry);
bool MTK_API cal_ram_buf_check_sum(char* buf,unsigned long len,
			   unsigned long &check_sum,unsigned long &crc_16,
			   unsigned long &crc_32,
			   const struct ignor_address_s *ignor_arry );

#define	ErrorMesgGet(mesg)	do{		\
	char *ptemp = NULL;			\
	ErrorMegGet(ptemp);			\
	if(ptemp) mesg += ptemp;		\
	delete []ptemp;				\
}while(0)

