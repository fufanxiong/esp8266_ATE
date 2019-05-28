/*	device.cpp
 */

#include "stdafx.h"
#include "Newdevice.h"
#include "Config.h"
#include "PD218.h"
#include "PD218Dlg.h"
#include "err.h"
#include "tset.h"
#include "TestItem.h"
#include "Dongle.h"
#include "TestEngine.h"
#include "TestFlash.h"
#include "uEnergyTest.h"

#include <visa.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h>
#include <stdio.h> 
#include <stdlib.h>
#include <fstream.h>


#pragma comment (lib,"TestEngine.lib")
#pragma comment (lib,"uEnergyTest.lib")

#define DEFAULT_TIMEOUT                 5000                    // 超时:5000ms 
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

//int testtime;
extern int  parse_mac_txt(void);
extern int  esp_send_mac(void);
extern char mac_send[];
//------------------------CSR-----------------------------------------------
//-----------------------------------------------------------------------
bool OpenRecordFile(const char* pfile,ofstream& fs,const char* url)
{
	if((pfile == NULL)||(pfile[0] == 0)) {
		ThrowErrMessage(EEPREADFILENULL_ERR);
		return(false);
	}

	TCHAR szFilePath[MAX_PATH + 1];
	GetModuleFileName(NULL, szFilePath, MAX_PATH); 
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
	CString str_url =  szFilePath;

	str_url += url;
	if(GetFileAttributes(str_url) != FILE_ATTRIBUTE_DIRECTORY) {
		mkdir(str_url);
		if(GetFileAttributes(str_url) != FILE_ATTRIBUTE_DIRECTORY) 
			return(false);
	}
	str_url += "\\";
	CString file = str_url+pfile;
	
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
		}else ifs.close();
	}
	
	fs.open(file,ios::app|ios::nocreate|ios::out);
	if(!fs.is_open()) {
		fs.clear();
		fs.open(file,ios::app|ios::out);
		if(!fs.is_open()) return(false);
	}
	return(true);
}

bool SearchFromChararry(char *&src,const char *mask,char* &save_str)
{
	char *phead;
	const char *pmask;

	if(!mask || !*mask) return(false);
	do{
		phead = src;
		pmask = mask;
		while(*phead++ == *pmask++) {
			if(!*pmask) return(true);	// search success
			if(!*phead) return(false);	// from src to src end match mask
		}
		if(save_str) *save_str++ = *src;
	}while (*++src);
	return(false);
}

bool SearchFromChararry(char* &src,const char *mask)
{
	char *phead;
	const char *pmask;

	if(!mask || !*mask) return(false);
	do{
		phead = src;
		pmask = mask;
		while(*phead++ == *pmask++) {
			if(!*pmask) return(true);	// search success
			if(!*phead) return(false);	// from src to src end match mask
		}
	}while (*++src);
	return(false);
}

bool AdjustBufCharToNonzero(char *st,char *end)
{
	bool res = false;

	while(st < end) {
		if(!*st) {
			*st = (char)0xff;
			res = true;
		}
		st++;
	}
	return(res);
}

bool mem_search(char* &src,unsigned long s_len,const char *mask,unsigned long m_len)
{
	if(!m_len) return(true);
	while(s_len) {
		unsigned long i = 0;
		for(i=0;i<m_len;i++) {
			if(i >= s_len) return(false);
			if(src[i] != mask[i]) break;
		}
		if(i >= m_len) return(true);
		src += 1;
		s_len -= 1;
	}
	return(false);
}

//-------------------------------------------------------------------------------------------------
CString &___gen_str_from_hex_data(char data,CString &str)
{
	char temp;

	temp = (data>>4)&0x0f;
	if(temp >= 10) temp += 'a' - 10;
	else temp += '0';
	str += temp;

	temp = data&0x0f;
	if(temp >= 10) temp += 'a' - 10;
	else temp += '0';
	str += temp;

	return(str);
}

CString &___gen_str_from_hex_data(unsigned long data,CString &str)
{
	char temp;

	if(data > 0xffff) return(str);

	temp = (char)((data>>8)&0xff);
	str = ___gen_str_from_hex_data(temp,str);

	temp = (char)(data&0xff);
	str = ___gen_str_from_hex_data(temp,str);
	return(str);
}

CString &gen_str_from_data(const char *char_arry,unsigned int len,
			   CString &str,bool is_char = true)
{
	if(len > 1000) {
		return(str);
	}
	
	if(!len && is_char) str = char_arry;
	else {
		str = "";
		for(unsigned int i=0;i<len;i++) {
			if(is_char) str += char_arry[i];
			else str = ___gen_str_from_hex_data(char_arry[i],str);
		}
	}
	return(str);
}

CString &gen_str_from_data(unsigned long data,CString &str)
{
	str = "";
	str = ___gen_str_from_hex_data(data,str);
	return(str);
}

unsigned long char_2_hex(char ch)
{ 
	ch |= 0x20;
	if((ch <= '9')&&(ch >= '0')) return(ch-'0');
	else if((ch >= 'a') && (ch <= 'f')) return(ch-'a' + 10);
	else return(0);
}

unsigned long char_2_hex(char chH,char chL)
{ 
       unsigned long temp = char_2_hex(chH);
       temp = (temp*0x10) + char_2_hex(chL);
       return(temp);
}

bool is_name_null(CString name)
{
	if((name == "") ||
	(name == "NULL") ||
	(name == "null")) return(true);
	return(false);
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
SPISourceContaner::~SPISourceContaner()
{
	while(hlist) {
		SPISource* item = hlist;
		hlist = hlist->next;
		delete(item);
	}
}

SPISource* SPISourceContaner::add(bool maped,const CString &name,const CString &NUM,int Type)
{
	SPISource* item = new SPISource(maped,name,NUM,Type);

	if(!hlist) {
		hlist = item;
		return(item);
	}
	
	SPISource* ireter = hlist;
	while(ireter->next) ireter = ireter->next;
	ireter->next = item;
	return(item);
}

SPISource* SPISourceContaner::del(class SPISource* obj)
{
	SPISource* ireter = NULL;
	SPISource* prev = hlist;

	if(!obj) return(hlist);
	if(obj == prev) {
		hlist = hlist->next;
		delete(obj);
	}

	while(ireter = prev->next) {
		if(ireter == obj) {
			prev->next = ireter->next;
			delete(obj);
			return(prev);
		}
		prev = ireter;
	}
	return(hlist);
}

class SPISource* SPISourceContaner::get_idle(void)
{
	SPISource* ireter = hlist;

	while(ireter) {
		if(!ireter->maping) return(ireter);
		ireter = ireter->next;
	}
	return(NULL);
}

class SPISource* SPISourceContaner::get_nex(SPISource* cur)
{
	SPISource* ireter = hlist;

	while(ireter) {
		if(cur == ireter) return(ireter->next);
		ireter = ireter->next;
	}
	return(NULL);
}

class SPISource* SPISourceContaner::get_port(const CString &name)
{
	SPISource* ireter = hlist;

	while(ireter) {
		if(ireter->port_name() == name) return(ireter);
		ireter = ireter->next;
	}
	return(NULL);
}


const CString& SPISource::get_trans(void)
{
	if(trans != "") return(trans);
	char* newTransStr = new char[port_num().GetLength() + 1];
	uint32 device;
	int32 status = flmConvertSpiPort(port_num(),newTransStr,&device);
	if(status != TFL_OK )
	{
		delete[] newTransStr;
		return(trans);
	}
	
	CString port;
	if(port_num().Find("USB") > 0) 
	{
		port.Format(" -usb %d",device);
	}
	else 
	{
		port.Format(" -lpt %d",device);
	}
	
	delete[] newTransStr;
	trans = port;
	return(trans);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
backup_pskey_contaner::~backup_pskey_contaner()
{
	struct bk_pskey_item* item;
	while(head) {
		item = head;
		head = head->next;
		if(item->data) delete []item->data;
		delete item;
	}
}

struct bk_pskey_item* backup_pskey_contaner::add(unsigned long key)
{
	struct bk_pskey_item *item = new struct bk_pskey_item();
	item->data_len = 0;
	item->data = NULL;
	item->pskey = key;

	item->next = head;
	head = item;
	return(item);
}

struct bk_pskey_item* backup_pskey_contaner::set_data(
	struct bk_pskey_item* item,int len,const unsigned short *data)
{
	if(item->data) delete []item->data;
	item->data = NULL;

	item->data_len = len;
	if(data && len) {
		item->data = new unsigned short[len];
		for(int i=0;i<len;i++) item->data[i] = data[i];
	}
	return(item);
}




//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#define	INVALID_COM_INDEX	0xff
const char COM_Port_Name[COM_SOURCEMAX][6] = {
	"COM1",  "COM2",   "COM3",   "COM4",   "COM5",   "COM6",   "COM7",   "COM8",
	"COM9",  "COM10",  "COM11",  "COM12",  "COM13",  "COM14",  "COM15",  "COM16",
	"COM17", "COM18",  "COM19",  "COM20",  "COM21",  "COM22",  "COM23",  "COM24",
	"COM25", "COM26",  "COM27",  "COM28",  "COM29",  "COM30",  "COM31",  "COM32",

	"COM33", "COM34",  "COM35",  "COM36",  "COM37",  "COM38",  "COM39",  "COM40",
	"COM41", "COM42",  "COM43",  "COM44",  "COM45",  "COM46",  "COM47",  "COM48",
	"COM49", "COM50",  "COM51",  "COM52",  "COM53",  "COM54",  "COM55",  "COM56",
	"COM57", "COM58",  "COM59",  "COM60",  "COM61",  "COM62",  "COM63",  "COM64",
};

bool CComDevice::com_source_mapped = false;
int  CComDevice::com_map[COM_SOURCEMAX];
port_list<PortSource>* CComDevice::list_head = NULL;

bool CComDevice::is_com_index_valid(int index)
{
	if(index >= COM_SOURCEMAX) return(false);
	if(com_map[index] < COM_SOURCEMAX) return(true);
	return(false);
}

bool CComDevice::is_com_index_can_used(int index)
{
	if(!is_com_index_valid(index)) return(false);
	if(port_list<PortSource>::DevUsed(COM_Port_Name[index],list_head)) return(false);  //port_list--DevUsed
	return(true);
}

void CComDevice::DevSourceMap(void)
{
	HKEY hKey;
	DWORD size,type;
	const int datalength = 32;
	const int queryTimes = 32;
	char data[datalength];
        char data1[datalength];
	int i = 0,j = 0,temp;

	if(!com_source_mapped) {
		for(i=0;i<COM_SOURCEMAX;i++) com_map[i] = INVALID_COM_INDEX;
		com_source_mapped = true;
	}

	LPCTSTR strKey="HARDWARE\\DEVICEMAP\\SERIALCOMM";
	if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,strKey,NULL,KEY_QUERY_VALUE,&hKey)== ERROR_SUCCESS)
	{
		i = 0;
		do{
			size = datalength;
			type = REG_SZ;
			if(::RegEnumValue(hKey,i,data,&size,NULL,&type,NULL,NULL)
			    == ERROR_NO_MORE_ITEMS) break;
			
			size = datalength;
			type = REG_SZ;
			if((::RegQueryValueEx(hKey,data,0,&type,(BYTE*)data1,&size)
			   == ERROR_SUCCESS) && (data1[0] == 'C')&&(data1[1] == 'O')&&
			   (data1[2] == 'M') && ((data1[3] > '0')&&(data1[3] <= '9'))) {
				++j;
				temp = data1[3]-'0';
				if((data1[4] >= '0')&&(data1[4] <= '9') && (!data1[5])) {
					temp = temp*10+data1[4]-'1';
				}else if(!data1[4]) {
					temp -= 1;
				}else continue;
				
				if(temp >= COM_SOURCEMAX) continue;
				com_map[temp] = temp;
			}
		}while (i++ < queryTimes);
	}
	RegCloseKey(hKey);

	if(j) {
		for(i=0;i<COM_SOURCEMAX;i++) if(com_map[i] < COM_SOURCEMAX) return;
	}

// 如果通过注册表操作不成功，则用以下方法
	HANDLE 	hCom;
	for(i=0;i<COM_SOURCEMAX;i++) {
		if(com_map[i] < COM_SOURCEMAX) continue;

		hCom = CreateFile(COM_Port_Name[i],		// 串口名称或设备路径
				  GENERIC_READ|GENERIC_WRITE,	// 读写方式
				  0,				// 共享方式：独占
				  NULL,				// 默认的安全描述符
				  OPEN_EXISTING,		// 创建方式
				  0,				// 不需设置文件属性
				  NULL				// 不需参照模板文件
				  );
		if(hCom != INVALID_HANDLE_VALUE) com_map[i] = i;
		CloseHandle(hCom);
	}
}

//-------------------------------------------------------------------------------------------------
CComDevice::CComDevice(const CString &dev,const CString &com,const CString &probe_name)
	:port(com,dev,probe_name)
{
	hCom = INVALID_HANDLE_VALUE;
	for(int i=0;i<RxBufSize;i++) rxBuf[i] = 0;

	CComDevice::DevSourceMap();
}

//-------------------------------------------------------------------------------------------------
CComDevice::~CComDevice()
{
	port.dependFromList(list_head);
	if(hCom != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hCom);
	}
	hCom = INVALID_HANDLE_VALUE;
}

//-------------------------------------------------------------------------------------------------
bool CComDevice::openCom(const CString &Com,int Baud,int Parity)
{
	CString use_com = Com;
	if(use_com.GetLength() > 4) use_com = "\\\\.\\" + use_com;

	hCom = CreateFile(use_com,				// 串口名称或设备路径
			  GENERIC_READ|GENERIC_WRITE,		// 读写方式
			  0,					// 共享方式：独占
			  NULL,					// 默认的安全描述符
			  OPEN_EXISTING,			// 创建方式
			  0,					// 不需设置文件属性
			  NULL					// 不需参照模板文件
			  );
	if(hCom == INVALID_HANDLE_VALUE) return(false);		// 打开串口失败

	DCB dcb;						// 串口控制块
	COMMTIMEOUTS timeouts = {				// 串口超时控制参数
		0,						// 读字符间隔超时时间:不启用
		4,						// 读操作时每字符的时间: 4 ms (n个字符总共为n ms)
		7000,						// 基本的(额外的)读超时时间: 7000 ms
		2,						// 写操作时每字符的时间: 2 ms (n个字符总共为n ms)
		1000						// 基本的(额外的)写超时时间: 1000ms
	};
	
	GetCommState(hCom,&dcb);				// 取DCB

	dcb.fOutxCtsFlow = FALSE;				// 禁止硬流控, 因为本系统不需要 
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE; 
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fAbortOnError = false;

	dcb.BaudRate = Baud;
	dcb.ByteSize = 8;
	dcb.Parity = Parity;
	dcb.StopBits = ONESTOPBIT;
	dcb.fBinary = true; 
	SetCommState(hCom,&dcb);				// 设置DCB

	SetupComm(hCom,4096,1024);				// 设置输入输出缓冲区大小
	SetCommTimeouts(hCom,&timeouts);			// 设置超时

	DWORD dwError;						// ie:CE_RXOVER
   	COMSTAT cs;
	ClearCommError(hCom,&dwError,&cs);
	PurgeComm(hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);

	using_port_name = Com;
	return(true);
}
//yhl 2018/11/26
//-------------------------------------------------------------------------------------------------
bool CComDevice::connect(const CString &dName, int high_low, int Parity)
{
	if(!port_maped()) {
		CString com_temp;
		for(int i=0;i<COM_SOURCEMAX;i++) {
			if(!is_com_index_can_used(i)) continue;
			if(port.key.is_probe_name_null() &&
			   port.key.is_port_name_null() ) continue;
			if(port.key.is_probe_name_null() && 
			   (port.key.PortName != COM_Port_Name[i])) continue;
			
			com_temp = COM_Port_Name[i];
			if(!openCom(com_temp,port_baud(),port_parity())) continue;
			if(port.key.is_probe_name_null() || DevValid(high_low)) {
				port.key.PortName = COM_Port_Name[i];
				port.key.maping = true;
				port.addToList(list_head);
				break;
			}
			CloseHandle(hCom);
			hCom = INVALID_HANDLE_VALUE;
		}
		return(port.key.maping);
	}

	if((hCom == INVALID_HANDLE_VALUE) &&
	   !openCom(port_name(),port_baud(),port_parity())) return(false);
	return(DevValid(high_low));
}

//-------------------------------------------------------------------------------------------------
void CComDevice::disconnect(void)
{
	
	if(INVALID_HANDLE_VALUE != hCom) CloseHandle(hCom);
	hCom = INVALID_HANDLE_VALUE;	
}

//-------------------------------------------------------------------------------------------------
void CComDevice::record_err_cmd_data(const char *s_str,unsigned int s_len,
				     const char *v_str,unsigned int v_len)
{
	if(!pCfgf.ItemSelect(ParErrCmdRecord)) return;
	
	ofstream fs;
	if(OpenRecordFile("cmd.txt",fs,"cmd log")) {
		CString show = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
		fs<<"\n"<<show<<"\terr cmd by Dev: "<<port.key.DevName
			<<"\tOn port:"<<using_port_name<<endl;

		fs<<"send data :{ "<<gen_str_from_data(s_str,s_len,show)<<" }"<<endl;
		fs<<"mask data :{"<<gen_str_from_data(v_str,v_len,show)<<" }"<<endl;
		fs<<"back data :{"<<gen_str_from_data(rxBuf,rxBuf_len,show)<<" }"<<endl;
		fs.close();
	}
}

//-------------------------------------------------------------------------------------------------
void CComDevice::record_err_cmd_data(unsigned long s_data,unsigned long v_data)
{
/*	if(!pCfgf.ItemSelect(ParErrCmdRecord)) return;
	
	ofstream fs;
	if(OpenRecordFile("cmd.txt",fs,"cmd log")) {
		CString show = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
		fs<<"\n"<<show<<"\terr cmd by Dev: "<<port.key.DevName
			<<"\tOn port:"<<using_port_name<<endl;

		fs<<"send data :{ "<<gen_str_from_data(s_data,show)<<" }"<<endl;
		fs<<"mask data :{"<<gen_str_from_data(v_data,show)<<" }"<<endl;
		fs<<"back data :{"<<gen_str_from_data(rxBuf,rxBuf_len,show,false)<<" }"<<endl;
		fs.close();
	}*/
}

//-------------------------------------------------------------------------------------------------
bool CComDevice::ClearRxBuf(unsigned length)
{
	DWORD dwError;
   	COMSTAT cs;
	unsigned long nBytesRead = 0;

	if(length > 500) length = 500;
	for(unsigned i=0;i<length;i++) {		// clear the rx buffer
		Sleep(3);
		ClearCommError(hCom,&dwError,&cs);
		if(cs.cbInQue > (RxBufSize-1)) PurgeComm(hCom,PURGE_RXCLEAR);
		else if(cs.cbInQue) {
			ReadFile(hCom,rxBuf,cs.cbInQue,&nBytesRead,NULL);
		}
		if(!cs.cbInQue) break;
		Sleep(10);
	}

	return(!!i);
}

//-------------------------------------------------------------------------------------------------
bool CComDevice::send(const char *str,unsigned int len,unsigned PurgeRx)
{
	if(hCom == INVALID_HANDLE_VALUE) return(false);
	
	DWORD dwError;
	COMSTAT ComStat;
	if(::ClearCommError(hCom,&dwError,&ComStat) && (dwError > 0))
		::PurgeComm(hCom,PURGE_TXCLEAR);
	if(PurgeRx) ClearRxBuf(PurgeRx);

	DWORD dwNumWrited;
	if(!len) len = strlen(str);
	WriteFile(hCom,str,len,&dwNumWrited,NULL);

	return(dwNumWrited == len);
}

//-------------------------------------------------------------------------------------------------
bool CComDevice::recive_by_length(char *str,unsigned int &len,unsigned long time)
{
	if(hCom == INVALID_HANDLE_VALUE) return(false);
	if(!str) return(false);

	DWORD dwError;
	COMSTAT ComStat;
	if(::ClearCommError(hCom,&dwError,&ComStat)&& (dwError >0)) {
		if(ComStat.cbInQue >= 0x100) {
			::PurgeComm(hCom,PURGE_RXCLEAR);
			return(false);
		}
	}

	bool time_state = false;
	COMMTIMEOUTS time_temp;
	if(time) {
		if((GetCommTimeouts(hCom,&time_temp) == 1) &&
		   (time != time_temp.ReadTotalTimeoutConstant)) {
			time_state = true;
			COMMTIMEOUTS Time = time_temp;
			Time.ReadTotalTimeoutConstant = time;
			SetCommTimeouts(hCom,&Time);
		}
	}

	DWORD dwNumRead = 0;	// 串口已接收的数据长度
	if(len) ReadFile(hCom,str,len,&dwNumRead,NULL);
	else {
		DWORD will_read_len = 1;
		ReadFile(hCom,str,will_read_len,&dwNumRead,NULL);
		for(int i=0;i<10;i++) {
			if(!dwNumRead) break;
			Sleep(50);
			ClearCommError(hCom,&dwError,&ComStat);
			if(!ComStat.cbInQue) break;
			will_read_len = ComStat.cbInQue + dwNumRead;
			if(will_read_len > (RxBufSize-2)) 
				will_read_len = will_read_len - (RxBufSize-2);
			else will_read_len = ComStat.cbInQue;

			DWORD dwNumRead_temp = 0;
			ReadFile(hCom,&str[dwNumRead],will_read_len,&dwNumRead_temp,NULL);
			dwNumRead += dwNumRead_temp;
			if(will_read_len != dwNumRead_temp) break;
		}
	}
	
	if(time_state == true) SetCommTimeouts(hCom,&time_temp);
	rxBuf_len += dwNumRead;

	if(!len) {
		len = dwNumRead;
		if(dwNumRead) return(true);
	}else if(len == dwNumRead) return(true);

	return(false);
}

//-------------------------------------------------------------------------------------------------
bool CComDevice::recive_by_mask(char *d_str,unsigned int d_len,
				const char *m_str,unsigned int m_len,
				unsigned long time,unsigned int d_len_max)
{
	rxBuf_len = 0;
	char *rx_buf_temp = rxBuf;
	const int rx_buf_temp_len = RxBufSize;
	char *rx_buf_temp_end = &rxBuf[rx_buf_temp_len];

	rxBuf[0] = 0;
	if(m_str && !m_len) m_len = strlen(m_str);
	if((d_len + m_len) >= rx_buf_temp_len) return(false);
	if(!m_str || (m_len <= 0)) {
		if(!d_str) return(false);
		unsigned int rx_len = d_len;
		bool Res = recive_by_length(rx_buf_temp,rx_len,time);
		if(!Res) return(false);
		if(!d_len) {
			rxBuf[rx_len] = 0;
			if(rx_len < d_len_max) d_len_max = rx_len;
			memcpy(d_str,rx_buf_temp,d_len_max);
			d_str[d_len_max] = 0;
		}else {
			if(d_len > rx_len) return(false);
			rxBuf[d_len] = 0;
			memcpy(d_str,rx_buf_temp,d_len);
			d_str[d_len] = 0;
		}
		return(true);
	}

	bool searchRes = false;
	unsigned RSearchTimes = 1024;				// research times 1024
	char *head = rx_buf_temp;
	char *next_rx_buf = head;
	do{
		unsigned int length = head+m_len-next_rx_buf;
		if((length <= 0)||(length > m_len)) break;
		if((head+length) >= rx_buf_temp_end) break;
		if(!recive_by_length(next_rx_buf,length,time)) break;

		next_rx_buf = head+m_len;
		if(searchRes = mem_search(head,m_len,m_str,m_len)) break;
	}while (RSearchTimes--);

	if(!searchRes) return(false);
	if(!d_len) return(true);
	if(!d_str) return(false);

	if((next_rx_buf+d_len) >=  rx_buf_temp_end) return(false);
	bool Res = recive_by_length(next_rx_buf,d_len,time);
	if(!Res) return(false);
	memcpy(d_str,next_rx_buf,d_len);
	return(true);
}

//-------------------------------------------------------------------------------------------------
bool CComDevice::recive(char *d_str,unsigned int d_len,unsigned long time,unsigned int d_len_max)
{
	bool res = true;

	res = recive_by_mask(d_str,d_len,NULL,0,time,d_len_max);
	return(res);
}

//-------------------------------------------------------------------------------------------------
bool CComDevice::___send_search_get(const char *s_str,unsigned int s_len,
				 char *d_str,unsigned int d_len,
				 const char *v_str,unsigned int v_len,
				 unsigned long time,unsigned PurgeRx)
{
	bool res = true;

	res = send(s_str,s_len,PurgeRx);
	if(res) {
		res = recive_by_mask(d_str,d_len,v_str,v_len,time);
		if(!res && v_str) record_err_cmd_data(s_str,s_len,v_str,v_len);
	}

	return(res);
}

//-------------------------------------------------------------------------------------------------
bool CComDevice::send_search_get(const char *s_str,unsigned int s_len,
				 char *d_str,unsigned int d_len,
				 const char *v_str,unsigned int v_len,
				 unsigned long time,unsigned PurgeRx)
{
	bool res = ___send_search_get(s_str,s_len,d_str,d_len,v_str,v_len,time,PurgeRx);
	if(!res) record_err_cmd_data(s_str,s_len);

	return(res);
}

//-------------------------------------------------------------------------------------------------
bool CComDevice::send_and_search(const char *s_str,unsigned int s_len,
				 const char *d_str,unsigned int d_len,
				 unsigned long time,unsigned PurgeRx)
{
	bool res = true;
	res = ___send_search_get(s_str,s_len,NULL,0,d_str,d_len,time,PurgeRx);
	if(!res) record_err_cmd_data(s_str,s_len);
	return(res);
}

//-------------------------------------------------------------------------------------------------
#if 1
bool CComDevice::send_and_get(const char *s_str,unsigned int s_len,
			      char *d_str,unsigned int d_len,
			      unsigned long time,unsigned PurgeRx)
{
	bool res = true;
	res = ___send_search_get(s_str,s_len,d_str,d_len,NULL,0,time,PurgeRx);
	if(!res) record_err_cmd_data(s_str,s_len);
	return(res);
}
#else
bool CComDevice::send_and_get_esp(char *s_str,unsigned int s_len,
			      char *d_str,unsigned int d_len,
			      unsigned long time,unsigned PurgeRx)
{
	bool res = true;
	res = ___send_search_get(s_str,s_len,d_str,d_len,NULL,0,time,PurgeRx);
	if(!res) record_err_cmd_data(s_str,s_len);
	return(res);
}
#endif
//-------------------------------------------------------------------------------------------------
bool CComDevice::send_and_verify(const char *s_str,unsigned int s_len,
				 const char *d_str,unsigned int d_len,
				 unsigned long time,unsigned PurgeRx)
{
	bool res = true;
	char *buf_temp = NULL;

	if(d_str && !d_len) d_len = strlen(d_str);
	buf_temp = new char[d_len+1];

	res = ___send_search_get(s_str,s_len,buf_temp,d_len,NULL,0,time,PurgeRx); 
	if(res) res = !memcmp(buf_temp,d_str,d_len);
	if(!res) record_err_cmd_data(s_str,s_len,d_str,d_len);
	
	delete []buf_temp;
	return(res);
}

//-------------------------------------------------------------------------------------------------
bool CComDevice::send_search_get(const char *s_str,unsigned int s_len,
				 unsigned long *d_data,unsigned int d_len,
				 const char *v_str,unsigned int v_len,
				 unsigned long time,unsigned PurgeRx)
{
	char *buf = new char[d_len*2+1];;
	bool res = true;
	res = send_search_get(s_str,s_len,
			      buf,d_len*2,
			      v_str,v_len,
			      time,PurgeRx);
	if(!res) record_err_cmd_data(s_str,s_len);
	else {
		unsigned long data_temp;
		for(unsigned long i=0;i<d_len;i++) {
			data_temp = (char_2_hex(buf[i*2])<<4);
			data_temp += char_2_hex(buf[i*2+1]);
			d_data[i] = data_temp;
		}
	}
	delete []buf;
	return(res);
}



//-------------------------------------------------------------------------------------------------
//mcu serial port
//-------------------------------------------------------------------------------------------------
                             //CComDevice(const CString &dev,const CString &com,const CString &probe_name)
CMCU_COM::CMCU_COM(const CString &devName):CComDevice(devName,"","mcu serial port")
{
}


bool CMCU_COM::DevValid(int high_low)
{
    CString cmd_str;
	cmd_str.Format("MCU+NC(%d)\r\n", high_low);
	if(!send_and_verify(cmd_str, 0, "MCU+OK\r\n", 0)) 
	{
		return(false);	
	}
	else
	{
	    return true;
	}
}


bool CMCU_COM::set_test_board_short_pc_on(int mcu_id, char pin_grp, int pin_num, int high_low)
{
    return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));

}

bool CMCU_COM::set_test_board_short_pc_off(int mcu_id, char pin_grp, int pin_num, int high_low)
{
    return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}


bool CMCU_COM::set_dut_pwr_on(int mcu_id, char pin_grp, int pin_num, int high_low)
{
   return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));

}
bool CMCU_COM::set_dut_pwr_off(int mcu_id, char pin_grp, int pin_num, int high_low)
{
   return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));

}


bool CMCU_COM::set_dut_bat_on(int mcu_id, char pin_grp, int pin_num, int high_low)
{
   return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));

}
bool CMCU_COM::set_dut_bat_off(int mcu_id, char pin_grp, int pin_num, int high_low)
{
   return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));

}

bool CMCU_COM::set_dut_current_on(int mcu_id,char pin_grp, int pin_num, int high_low)
{
	return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}
bool CMCU_COM::set_dut_current_off(int mcu_id,char pin_grp, int pin_num, int high_low)
{
	return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}

bool CMCU_COM::set_dut_vreg_on(int mcu_id,char pin_grp, int pin_num, int high_low)
{
	bool rtn;

	rtn = mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low);
	Sleep(500);
	
	return (rtn);
}
bool CMCU_COM::set_dut_vreg_off(int mcu_id,char pin_grp, int pin_num, int high_low)
{
	
	return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}


bool CMCU_COM::set_dut_5v_on(int mcu_id,char pin_grp, int pin_num, int high_low)
{
	return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}
bool CMCU_COM::set_dut_5v_off(int mcu_id,char pin_grp, int pin_num, int high_low)
{
	return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}


bool CMCU_COM::set_dut_spi_on(int mcu_id,char pin_grp, int pin_num, int high_low)
{
	return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}
bool CMCU_COM::set_dut_spi_off(int mcu_id,char pin_grp, int pin_num, int high_low)
{
	return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}

bool CMCU_COM::set_dut_led_on(int mcu_id,char pin_grp, int pin_num, int high_low)
{
    return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}
bool CMCU_COM::set_dut_led_off(int mcu_id,char pin_grp, int pin_num, int high_low)
{
	return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}

bool CMCU_COM::set_dut2_rout_meter_on(int mcu_id,char pin_grp, int pin_num, int high_low)
{
   return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}
bool CMCU_COM::set_dut1_rout_meter_on(int mcu_id,char pin_grp, int pin_num, int high_low)
{
	return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}

bool CMCU_COM::set_dut_spk_amp_on(int mcu_id,char pin_grp, int pin_num, int high_low)
{
    return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}
bool CMCU_COM::set_dut_spk_amp_off(int mcu_id,char pin_grp, int pin_num, int high_low)
{
   return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}



bool CMCU_COM::set_dut2_rout_audio_on(int mcu_id,char pin_grp, int pin_num, int high_low)
{
   return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}
bool CMCU_COM::set_dut1_rout_audio_on(int mcu_id,char pin_grp, int pin_num, int high_low)
{
    return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}


bool CMCU_COM::set_rout_8852_on(int mcu_id,char pin_grp, int pin_num, int high_low)
{
   return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}
bool CMCU_COM::set_rout_freq_on(int mcu_id,char pin_grp, int pin_num, int high_low)
{
   return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}

bool CMCU_COM::set_rout_4010_on(int mcu_id,char pin_grp, int pin_num, int high_low)
{
	return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}

bool CMCU_COM::set_4010_route_dut2(int mcu_id,char pin_grp, int pin_num, int high_low)
{
	return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}
bool CMCU_COM::set_4010_route_dut1(int mcu_id,char pin_grp, int pin_num, int high_low)
{
	return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}

bool CMCU_COM::set_8852_route_dut2(int mcu_id,char pin_grp, int pin_num, int high_low)
{
   return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}
bool CMCU_COM::set_8852_route_dut1(int mcu_id,char pin_grp, int pin_num, int high_low)
{
   return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}

bool CMCU_COM::set_freq_route_dut2(int mcu_id,char pin_grp, int pin_num, int high_low)
{
   return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}
bool CMCU_COM::set_freq_route_dut1(int mcu_id,char pin_grp, int pin_num, int high_low)
{
  return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}

bool CMCU_COM::set_motor_down(int mcu_id,char pin_grp, int pin_num, int high_low)
{
   return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}
bool CMCU_COM::set_motor_up(int mcu_id,char pin_grp, int pin_num, int high_low)
{
   return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}



//read cmd

bool CMCU_COM::get_dut_rgb(int mcu_id,char pin_grp, int pin_num, char *rx_buf)
{
        CString cmd_str;
	    cmd_str.Format("MCU+ADC_SET(%d,%c,%04x)\r\n", mcu_id, pin_grp, (1 << pin_num));		
		if(!send_and_verify(cmd_str, 0, "MCU+OK\r\n", 0))
		{
			return (false);
		}

		cmd_str.Format("MCU+ADC_RD(%d,%c,%04x)\r\n", mcu_id, pin_grp, (1 << pin_num));
		if(!send_and_get(cmd_str, 0, &rx_buf[0], 0))
		{
			return (false);
		}

    return (true);

}


//led indication cmd
bool CMCU_COM::set_LED_on(int mcu_id,char pin_grp, int pin_num, int high_low)
{
   return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}
bool CMCU_COM::set_LED_off(int mcu_id,char pin_grp, int pin_num, int high_low)
{
   return (mcu_deal_cmd(mcu_id, pin_grp, pin_num, high_low));
}

bool CMCU_COM::set_LED_flash(int mcu_id,char pin_grp, int pin_num,unsigned long delay)
{
	//if(!pMainWnd) return false;
	while(!pMainWnd->start_btn_is_pressed)
	{
		set_LED_on(mcu_id, pin_grp, pin_num);
		Sleep(delay);
		set_LED_off(mcu_id, pin_grp, pin_num);
	}
   return true;
}




bool CMCU_COM::mcu_deal_cmd(int mcu_id, char pin_grp, int pin_num, int high_low)
{
	CString cmd_str;
	bool status;
	int count;
#if 0
	    cmd_str.Format("MCU+SET(%d,%c,%04x,5)\r\n", mcu_id, pin_grp, (1 << pin_num));		
		if(!send_and_verify(cmd_str, 0, "MCU+OK\r\n", 0))
		{
			return (false);
		}

		cmd_str.Format("MCU+WR(%d,%c,%04x,%04x)\r\n", mcu_id, pin_grp, (1 << pin_num), (high_low << pin_num));
		if(!send_and_verify(cmd_str, 0, "MCU+OK\r\n", 0))
		{
			return (false);
		}
#endif
    
     //set
	 count = 3;
	 cmd_str.Format("MCU+SET(%d,%c,%04x,5)\r\n", mcu_id, pin_grp, (1 << pin_num));
	 while(count--)
	 {
		 status = send_and_verify(cmd_str, 0, "MCU+OK\r\n", 0);
		 if(status == true) break;   
	 }
	 if(status == false) return (false);

     
	 //write
	 count = 3;
	 cmd_str.Format("MCU+WR(%d,%c,%04x,%04x)\r\n", mcu_id, pin_grp, (1 << pin_num), (high_low << pin_num));
	 while(count--)
	 {
	    status = send_and_verify(cmd_str, 0, "MCU+OK\r\n", 0);
		if(status == true) break;
	 }
     if(status == false) return (false);

    return (true);
}



//=================================================================================================
//=================================================================================================
//CBsDevice::CBsDevice(const CString &Com,unsigned long id)
CBsDevice::CBsDevice()
{
	iHandle = 0;
	spi_allocated = false;
	//backup_pskey = NULL;

	//addr.is_valid = 0;
	//addr.nap = 0;
	//addr.uap = 0;
	//addr.lap = 0;
	//plast_use_addr = NULL;
	//last_use_addr.is_valid = 0;
	

	//r_addr.is_valid = 0;

	//file_load = 0;
	//file_crc32_cal = 0;
	//file_crc32_err = 1;
	//xpv_crc32 = 0;
	//xdv_crc32 = 0;
}

CBsDevice::~CBsDevice()
{
#if 0
	if(iHandle) closeTestEngine(iHandle);
#endif
	if(iHandle) uetClose(iHandle);
	iHandle = 0;

	/* if(backup_pskey) delete(backup_pskey);
	backup_pskey = NULL; */

}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool CBsDevice::connect(const CString &dName)
//bool CBsDevice::connect(testSet *tset, const CString &dName)
{
	//int status = bccmdEnableDeviceConnect(tset->pSpiObj->iHandle);
	
	
	return(false);
}

void CBsDevice::disconnect(void)
{
}



CString CBsDevice::errInfo(void)
{
        char szError[256] = {0};

	//GetError(szError);
	//	teGetLastError();
	CString sErrorD(szError);
	return sErrorD;
}



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool GPIB_DEV::DevRMCreated = false;
ViSession GPIB_DEV::DEVRM;
port_list<PortSource>* GPIB_DEV::list_head = NULL;

GPIB_DEV::GPIB_DEV(const CString &name,const CString &Addr,const CString &probe_name)
	:port(Addr,name,probe_name)
{
	port.key.DevName = name;
	port.key.PortName = Addr;
	Created = false;
	Session = VI_SUCCESS+1;
    
	if((false == DevRMCreated) && (viOpenDefaultRM(&DEVRM) < VI_SUCCESS)) 
	{
		ThrowErrMessage(OpenDefaultRM_ERR);
		return;
	}
	DevRMCreated = true;
}

GPIB_DEV::~GPIB_DEV()
{
	if(dev_is_ready()) viClose(Session);
	port.dependFromList(list_head);
	if(true == DevRMCreated) DevRMCreated = true;
}

int GPIB_DEV::CreatSession(void)
{
	bool flag = false;

	ViChar buffer[VI_FIND_BUFLEN];
	ViRsrc matches = buffer;
	ViUInt32 nmatches;
	ViFindList list;
	char char_name[1024] = {0};
	const CString &Addr = port.key.PortName;
	const CString &probe_name = port.key.connect_probe_name;

	if(DevRMCreated == false) return(false);
	viFindRsrc(DEVRM,"GPIB[0-1]*::?*INSTR",&list,&nmatches,matches);
	ViSession TempSession;

	while(nmatches--) {
		if((port.key.is_probe_name_null() && (Addr != matches)) || (DevUsed(matches))) {
			if(nmatches) viFindNext(list,matches);
			continue;
		}
		ViStatus Status = viOpen(DEVRM,matches,VI_NULL,VI_NULL,&TempSession);

		if(Status != VI_SUCCESS) {
			viClose(TempSession);
			if(nmatches) viFindNext(list,matches);
			continue;
		}
		
		for(int i=0;i<2;i++) {
			if(viPrintf(TempSession,"*RST\n") != VI_SUCCESS) goto __try_agian;
			if(viPrintf(TempSession,"*CLS\n") != VI_SUCCESS) goto __try_agian;

			char_name[0] = 0;
			if(!port.key.is_probe_name_null()) {
				if(viQueryf(TempSession,"*IDN?\n","%t",char_name) != VI_SUCCESS)
				goto __try_agian;
			}

			flag = true;
			break;
		__try_agian:
			viClear(TempSession);
			Sleep(1000);
		}
		viClose(TempSession);
		
		CString DevName = char_name;
		if(port.key.is_probe_name_null() || ((true == flag)&&(DevName.Find(probe_name) >= 0))) break;
		flag = false;
		if(nmatches) viFindNext(list,matches);
	}
	viClose(list);
	
	if(true == flag){
		if(viOpen(DEVRM,matches,VI_NULL,VI_NULL,&Session) == VI_SUCCESS) {
			port.key.PortName = matches;
		}else {
			flag = false;
			viClose(Session);
		}
	}
	return(flag);
}

bool GPIB_DEV::connect(void)
{
	if(port.key.maping) return(true);

	if(CreatSession() == 1) {
		Created = true;
		port.key.maping = true;
		port.addToList(list_head);
		return(true);
	}
	return(false);
}


int GPIB_DEV::ReleaseSession(void)
{
	if(dev_is_ready()) viClose(Session);
	port.dependFromList(list_head);
	port.key.maping = false;
	Created = false;
	return(true);
}


//-------------------------------------------------------------------------------------------------
// frequency counter
//-------------------------------------------------------------------------------------------------


FCOUNTER_DEV::FCOUNTER_DEV(const CString &name)
{
	dev_statu = 0;
	test_obj_is_bs = 0;
	old_test_obj_is_bs = 125;
	Tobj = NULL;
	dev_paramter_ini();
}

FCOUNTER_DEV::~FCOUNTER_DEV()
{
}

void FCOUNTER_DEV::dev_paramter_ini(void)
{
	ConfigFile &cfg = pCfgf;

	test_obj_is_bs = false;
	if(old_test_obj_is_bs == (int)test_obj_is_bs) return;
	old_test_obj_is_bs = test_obj_is_bs;
	reset_device_time = 1000;

	reset_device_time = 0;//cfg.Vulong(ParFreqRestTime);
	wait_data_time = 0;//cfg.Vulong(ParFreqWaitDataTime);
	if(!wait_data_time) wait_data_time = 3000;
}

bool FCOUNTER_DEV::creat_dev(const CString &name,class FCOUNTER_DEV *&dev)
{
	if(dev) return(true);
	if(name == "53131A") dev = new C53131A_DEV(name);
	else if(name == "53131B") dev = new C53131B_DEV(name);
	else return(false);
	return(true);
}

void FCOUNTER_DEV::prompt_message(const CString &message,class CTestItem *Tobj)
{
	if(!Tobj) return;
	Tobj->param = message;
	pMainWnd->SendMessage(WM_FLASH_CUR_ITEM_NO_RESUL,(WPARAM)NULL,(LPARAM)Tobj);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------


C53131_DEV::C53131_DEV(const CString &name):FCOUNTER_DEV(name),GPIB_DEV(name)
{
}

C53131_DEV::~C53131_DEV()
{
}

void C53131_DEV::dev_paramter_ini(void)
{
	FCOUNTER_DEV::dev_paramter_ini();
}

bool C53131_DEV::connect(void)
{
	dev_paramter_ini();
	bool res = GPIB_DEV::connect();
	if(!res) return(false);
	
	viPrintf(Session,"*RST\n");
	  viPrintf(Session,":FUNC 'FREQ 3'\n");
	Sleep(reset_device_time);
	return(true);
}

bool C53131_DEV::ReadFrequency(double &data,class CTestItem *Tobj)
{
	char char_name[1024] = {0};
	
	if(dev_is_ready() && 
		(viQueryf(Session,"READ:FREQ?\n","%t",char_name)==VI_SUCCESS)) {
		data = atof(char_name);
		//data += foffset;
		return(true);
	}
	
	viClose(Session);
	CString add;
	add.Format("%s", pFCounter->port_name());
    //if(viOpen(DEVRM,add.GetBuffer(0),VI_NULL,VI_NULL,&Session) != VI_SUCCESS) 
	if(viOpen(DEVRM,Address.GetBuffer(0),VI_NULL,VI_NULL,&Session) != VI_SUCCESS) 
		goto __ReadFreqErr;
	viClear(Session);
	if(viPrintf(Session,"*RST\n") != VI_SUCCESS) goto __ReadFreqErr;
	if(viPrintf(Session,"*CLS\n") != VI_SUCCESS) goto __ReadFreqErr;
	   viPrintf(Session,":FUNC 'FREQ 3'\n");
	  Sleep(100);

	if(viQueryf(Session,"READ:FREQ?\n","%t",char_name) != VI_SUCCESS)
		goto __ReadFreqErr;
	data = atof(char_name);
	return(true);

__ReadFreqErr:
	port.key.maping = false;
	return(false);
}
  
//-------------------------------------------------------------------------------------------------
C53131A_DEV::C53131A_DEV(const CString &name):C53131_DEV(name)
{
	GPIB_DEV::port.key.PortName = "";//pCfgf.VCString(Par53131A_Port);
	GPIB_DEV::port.key.connect_probe_name = "";//pCfgf.VCString(Par53131A_Probedev);
}

//-------------------------------------------------------------------------------------------------
C53131B_DEV::C53131B_DEV(const CString &name):C53131_DEV(name)
{
	GPIB_DEV::port.key.PortName = "";//pCfgf.VCString(Par53131B_Port);
	GPIB_DEV::port.key.connect_probe_name = "";//pCfgf.VCString(Par53131B_Probedev);
}

//-------------------------------------------------------------------------------------------------
// SpecAnaly counter
//-------------------------------------------------------------------------------------------------
VSpecAnaly::VSpecAnaly(const CString &name)
{
	dev_statu = 0;
	test_obj_is_bs = 0;
	old_test_obj_is_bs = 125;
	Tobj = NULL;
	dev_paramter_ini();
}

VSpecAnaly::~VSpecAnaly()
{
}

void VSpecAnaly::dev_paramter_ini(void)
{
	ConfigFile &cfg = pCfgf;

	test_obj_is_bs = false;
	if(old_test_obj_is_bs == (int)test_obj_is_bs) return;
	old_test_obj_is_bs = test_obj_is_bs;
	
	reset_device_time = 0;//cfg.Vulong(ParSpecRestTime);
	Span = 0;//cfg.Vulong(ParSpecSpan);
	Mk_Hi_Time = 0;//cfg.Vulong(ParSpecPkHoldTime);
	SweepTime = 0;//cfg.Vulong(ParSpecSweepTime);
	Rband = 0;//cfg.Vulong(ParSpecRband);
	RLv = 0;//cfg.VDouble(ParSpecRlv);
	centerF = 0;//cfg.Vulong(ParSpecCenter);
}

bool VSpecAnaly::creat_dev(const CString &name,class VSpecAnaly *&dev)
{
	if(dev) return(true);
	if(name == "HP8563") dev = new HP8563(name);
/*	else if(name == "HP8590E") dev = new HP8590E(name);
	else if(name == "MT8810C") dev = new MT8810C(name);
	else if(name == "R4131") dev = new R4131(name); 
*/
	else if(name == "R3131") dev = new R3131(name);
	else return(false);
	return(true);
}

void VSpecAnaly::prompt_message(const CString &message,class CTestItem *Tobj)
{
	if(!Tobj) return;
	Tobj->param = message;
	pMainWnd->SendMessage(WM_FLASH_CUR_ITEM_NO_RESUL,(WPARAM)NULL,(LPARAM)Tobj);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
HP8563::HP8563(const CString &name):VSpecAnaly(name),GPIB_DEV(name)
{
	ConfigFile &cfg = pCfgf;

	GPIB_DEV::port.key.PortName = "";//cfg.VCString(ParHP8563_Port);
	GPIB_DEV::port.key.connect_probe_name = "";//cfg.VCString(ParHP8563_Probedev);
	dev_verify_str = GPIB_DEV::port.key.connect_probe_name;
}

HP8563::~HP8563()
{
	
}

void HP8563::dev_paramter_ini(void)
{
	ConfigFile &cfg = pCfgf;

	VSpecAnaly::dev_paramter_ini();
	mark_cf_timer = 0;//cfg.Vulong(ParHP8563_MKCFTime);
}

bool HP8563::connect(void)
{
	dev_paramter_ini();
	bool res = GPIB_DEV::connect();
	if(!res) return(false);
	
	viPrintf(Session,"IP;");
	if(reset_device_time) Sleep(reset_device_time);

	SetCenterF();
	SetSpan(Span);
	SetLv(RLv);

	CString Cmd;
	Cmd.Format("RB %.0lfKHZ\n",Rband);
	viPrintf(Session,Cmd.GetBuffer(0));
	Cmd.Format("ST %.0lfMS\n",SweepTime);
	viPrintf(Session,Cmd.GetBuffer(0));

	Sleep(1000);
	return(true);
}

bool HP8563::SetRB(double Rband)
{
	CString Cmd;
	Cmd.Format("RB %.0lfKHZ\n",Rband);
	viPrintf(Session,Cmd.GetBuffer(0));

	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool HP8563::SetSpan(double Span,char SUnit)
{
	CString Cmd;
	Cmd.Format("SP %.0lf%cZ\n",Span,SUnit);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool HP8563::SetLv(double Lv)
{
	CString Cmd;
	Cmd.Format("RL %.0lfDB\n",Lv);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool HP8563::tryMarkPK2Center(void)
{
	for(int i=0;i<3;i++) 
		if(viPrintf(Session,"MKPK HI;MKCF\n") == VI_SUCCESS) return(true);
	return(false);
}

bool HP8563::SetCenterF(double freq,char fUnit)
{
	CString Cmd;
	Cmd.Format("CF %.0lf%cZ\n",freq,fUnit);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool HP8563::get_measure_value(double &f,double &lv,int freqSampleNum)
{
	bool rValue = true;
#if 0
	if(viPrintf(Session,"MXMH TRA\n") != VI_SUCCESS) return(false);
	if(Mk_Hi_Time) Sleep(Mk_Hi_Time);
#endif
	if(viPrintf(Session,"MKPK HI\n") != VI_SUCCESS) return(false);

	if(viQueryf(Session,"MKA?\n","%lf",&lv) != VI_SUCCESS) {
		lv = 0.0;
		rValue = false;
	}
	if(viQueryf(Session,"MKF?\n","%lf",&f) != VI_SUCCESS) {
		f = 0.0;
		rValue = false;
	}
#if 0
	viPrintf(Session,"CLRW TRA\n");
#endif
	return(rValue);
}

bool HP8563::get_measure_value2(double &f,double &lv,int freqSampleNum)
{
	bool rValue = true;

	lv = 0.0;
	f = 0.0;
	if(!SetSpan(1000,'K')) return(false);
	if(mark_cf_timer) Sleep(mark_cf_timer);
	if(!tryMarkPK2Center()) return(false);
	
	if(!SetSpan(30,'K')) return(false);
	if(mark_cf_timer) Sleep(mark_cf_timer);
	if(!tryMarkPK2Center()) return(false);

	if(mark_cf_timer) Sleep(mark_cf_timer);
	if(viPrintf(Session,"MKPK HI\n") != VI_SUCCESS) return(false);
	if(viQueryf(Session,"MKA?\n","%lf",&lv) != VI_SUCCESS) {
		lv = 0.0;
		rValue = false;
	}
	if(viQueryf(Session,"MKF?\n","%lf",&f) != VI_SUCCESS) {
		f = 0.0;
		rValue = false;
	}

	return(rValue);
}

#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
HP8590E::HP8590E(const CString &name):VSpecAnaly(name),GPIB_DEV(name)
{
	ConfigFile &cfg = pCfgf;

	GPIB_DEV::port.key.PortName = cfg.VCString(ParHP8590E_Port);
	GPIB_DEV::port.key.connect_probe_name = cfg.VCString(ParHP8590E_Probedev);
	dev_verify_str = GPIB_DEV::port.key.connect_probe_name;
}

HP8590E::~HP8590E()
{
}

void HP8590E::dev_paramter_ini(void)
{
	VSpecAnaly::dev_paramter_ini();
}

bool HP8590E::connect(void)
{
	dev_paramter_ini();
	bool res = GPIB_DEV::connect();
	if(!res) return(false);
	
	viPrintf(Session,"IP;");
	if(reset_device_time) Sleep(reset_device_time);

	SetCenterF();
	SetSpan(Span);
	SetLv(RLv);

	CString Cmd;
	Cmd.Format("RB %.0lfKHZ;",Rband);
	viPrintf(Session,Cmd.GetBuffer(0));
	Cmd.Format("ST %.0lfMS;",SweepTime);
	viPrintf(Session,Cmd.GetBuffer(0));
	return(true);
}

bool HP8590E::SetRB(double Rband)
{
	CString Cmd;
	Cmd.Format("RB %.0lfKHZ;",Rband);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool HP8590E::SetSpan(double Span,char SUnit)
{
	CString Cmd;
	Cmd.Format("SP %.0lf%cZ;",Span,SUnit);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool HP8590E::SetLv(double Lv)
{
	CString Cmd;
	Cmd.Format("RL %.0lfDB;",Lv);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool HP8590E::tryMarkPK2Center(void)
{
	for(int i=0;i<3;i++) 
		if(viPrintf(Session,"MKPK HI;MKCF;") == VI_SUCCESS) return(true);
	return(false);
}

bool HP8590E::SetCenterF(double freq,char fUnit)
{
	CString Cmd;
	Cmd.Format("CF %.0lf%cZ;",freq,fUnit);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool HP8590E::get_measure_value(double &f,double &lv,int freqSampleNum)
{
	bool rValue = true;
    
	if(viPrintf(Session,"MXMH TRA;") != VI_SUCCESS) return(false);
	if(Mk_Hi_Time) Sleep(Mk_Hi_Time);
	if(viPrintf(Session,"MKPK HI;") != VI_SUCCESS) return(false);
	Sleep(10);

	if(viQueryf(Session,"MKA?;","%lf",&lv) != VI_SUCCESS) {
		lv = 0.0;
		rValue = false;
	}
	if(viQueryf(Session,"MKF?;","%lf",&f) != VI_SUCCESS) {
		f = 0.0;
		rValue = false;
	}
	viPrintf(Session,"CLRW TRA;");
	return(rValue);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
MT8810C::MT8810C(const CString &name):VSpecAnaly(name),GPIB_DEV(name)
{
	ConfigFile &cfg = pCfgf;

	GPIB_DEV::port.key.PortName = cfg.VCString(ParMT8810C_Port);
	GPIB_DEV::port.key.connect_probe_name = cfg.VCString(ParMT8810C_Probedev);
	dev_verify_str = GPIB_DEV::port.key.connect_probe_name;
}

MT8810C::~MT8810C()
{
}

void MT8810C::dev_paramter_ini(void)
{
	VSpecAnaly::dev_paramter_ini();
}

bool MT8810C::connect(void)
{
	dev_paramter_ini();
	bool res = GPIB_DEV::connect();
	if(!res) return(false);
	
	viPrintf(Session,"INI\n");
	Sleep(reset_device_time);
	viPrintf(Session,"PNLMD SPECT\n");

	CString Cmd;
	Cmd.Format("RB %.0lfKHZ\n",Rband);
	viPrintf(Session,Cmd.GetBuffer(0));
	Cmd.Format("ST %.0lfMS\n",SweepTime);
	viPrintf(Session,Cmd.GetBuffer(0));

	SetCenterF();
	SetSpan(Span);
	SetLv(RLv);
	return(true);
}

bool MT8810C::SetRB(double Rband)
{
	CString Cmd;
	Cmd.Format("RB %.0lfKHZ\n",Rband);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool MT8810C::SetSpan(double Span,char SUnit)
{
	CString Cmd;
	Cmd.Format("SP %lf%cHZ\n",Span,SUnit);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool MT8810C::SetLv(double Lv)
{
	CString Cmd;
	Cmd.Format("RL %lfDBm\n",Lv);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool MT8810C::tryMarkPK2Center(void)
{
	for(int i=0;i<3;i++) {
		if(viPrintf(Session,"MKPK HI\n") != VI_SUCCESS) continue;
		if(viPrintf(Session,"MKCF\n") != VI_SUCCESS) continue;
		return(true);
	}
	return(false);
}

bool MT8810C::SetCenterF(double freq,char fUnit)
{
	CString Cmd;

	freq = freq/1000000;
	fUnit = 'M';
	Cmd.Format("CF %lf%cHZ\n",freq,fUnit);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool MT8810C::get_measure_value(double &f,double &lv,int freqSampleNum)
{
	bool rValue = true;

	SetSpan(100);
	if(viPrintf(Session,"AMD 1\n") != VI_SUCCESS) return(false);
	if(Mk_Hi_Time) Sleep(Mk_Hi_Time);
	if(viPrintf(Session,"MKPK HI\n") != VI_SUCCESS) return(false);

	viPrintf(Session,"BIN 0\n");
	char lvs[100];
	if(viQueryf(Session,"MKL?\n","%s",&lvs) != VI_SUCCESS) {
		lvs[0] = '0';
		lvs[1] = 0;
		rValue = false;
	}
	lv = atof(lvs);

	viPrintf(Session,"BIN 0\n");
	char fs[100];
	if(viQueryf(Session,"MKF?\n","%s",&fs) != VI_SUCCESS) {
		fs[0] = '0';
		fs[1] = 0;
		rValue = false;
	}
	f = atof(fs);

	viPrintf(Session,"AMD 0\n");
	return(rValue);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
R4131::R4131(const CString &name):VSpecAnaly(name),GPIB_DEV(name)
{
	ConfigFile &cfg = pCfgf;

	GPIB_DEV::port.key.PortName = cfg.VCString(ParR4131_Port);
	GPIB_DEV::port.key.connect_probe_name = cfg.VCString(ParR4131_Probedev);
	dev_verify_str = GPIB_DEV::port.key.connect_probe_name;
}

R4131::~R4131()
{
}

void R4131::dev_paramter_ini(void)
{
	VSpecAnaly::dev_paramter_ini();
}

bool R4131::connect(void)
{
	dev_paramter_ini();
	bool res = GPIB_DEV::connect();
	if(!res) return(false);
	
	viPrintf(Session,"IP\n");
	Sleep(reset_device_time);
	viPrintf(Session,"HD0\n");
	viPrintf(Session,"DL0\n");

	CString Cmd;
	Cmd.Format("RB %.0lfKZ\n",Rband);
	viPrintf(Session,Cmd.GetBuffer(0));
	Sleep(1000);
	Cmd.Format("ST %.0lfMS\n",SweepTime);
	viPrintf(Session,Cmd.GetBuffer(0));

	SetCenterF();
	SetSpan(Span);
	SetLv(RLv);
	return(true);
}

bool R4131::SetRB(double Rband)
{
	CString Cmd;
	Cmd.Format("RB %.0lfKZ\n",Rband);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool R4131::SetSpan(double Span,char SUnit)
{
	CString Cmd;
	Cmd.Format("SP %.0lf%cZ\n",Span,SUnit);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool R4131::SetLv(double Lv)
{
	CString Cmd;
	Cmd.Format("RL %.0lfDM\n",Lv);
	if(viPrintf(Session,Cmd.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool R4131::tryMarkPK2Center(void)
{
	for(int i=0;i<3;i++) 
		if(viPrintf(Session,"MKPK HI;MKCF\n") == VI_SUCCESS) return(true);
	return(false);
}

bool R4131::SetCenterF(double freq,char fUnit)
{
	CString CMD;
	CMD.Format("CF %.0lf%cZ\n",freq,fUnit);
	if(viPrintf(Session,CMD.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool R4131::get_measure_value(double &f,double &lv,int freqSampleNum)
{
	bool rValue = true;
    
	if(viPrintf(Session,"MA\n") != VI_SUCCESS) return(false);
	if(Mk_Hi_Time) Sleep(Mk_Hi_Time);
	if(viPrintf(Session,"M4\n") != VI_SUCCESS) return(false);

	if(viQueryf(Session,"OPML\n","%lf",&lv) != VI_SUCCESS) {
		lv = 0.0;
		rValue = false;
	}
	if(viQueryf(Session,"OPMF\n","%lf",&f) != VI_SUCCESS) {
		f = 0.0;
		rValue = false;
	}
	viPrintf(Session,"WR\n");
	Sleep(500);
	viPrintf(Session,"MA\n");
	Sleep(1000);
	return(rValue);
}
#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
R3131::R3131(const CString &name):VSpecAnaly(name),GPIB_DEV(name)
{
	ConfigFile &cfg = pCfgf;

	GPIB_DEV::port.key.PortName = "";//cfg.VCString(ParR3131_Port);
	GPIB_DEV::port.key.connect_probe_name = "";//cfg.VCString(ParR3131_Probedev);
	dev_verify_str = GPIB_DEV::port.key.connect_probe_name;
}

R3131::~R3131()
{
}

void R3131::dev_paramter_ini(void)
{
	VSpecAnaly::dev_paramter_ini();
}

bool R3131::connect(void)
{
	dev_paramter_ini();
	bool res = GPIB_DEV::connect();
	if(!res) return(false);
	
	viPrintf(Session,"IP\n");
	Sleep(reset_device_time);
	viPrintf(Session,"HD0\n");
	viPrintf(Session,"DL0\n");

	CString Cmd;
	Cmd.Format("RB %.0lfKZ\n",Rband);
	viPrintf(Session,Cmd.GetBuffer(0));
	Sleep(1000);
	Cmd.Format("SW %.0lfMS\n",SweepTime);
	viPrintf(Session,Cmd.GetBuffer(0));

	SetCenterF();
	SetSpan(Span);
	SetLv(RLv);
	return(true);
}

bool R3131::SetRB(double Rband)
{
	CString Temp;
	Temp.Format("RB %.0lfKZ\n",Rband);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool R3131::SetSpan(double Span,char SUnit)
{
	CString Temp;
	Temp.Format("SP %.0lf%cZ\n",Span,SUnit);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool R3131::SetLv(double Lv)
{
	CString Temp;
	Temp.Format("RL %.0lfDBM\n",Lv);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool R3131::tryMarkPK2Center(void)
{
	for(int i=0;i<3;i++) 
		if(viPrintf(Session,"MKPK HI;MKCF\n") == VI_SUCCESS) return(true);
	return(false);
}

bool R3131::SetCenterF(double freq,char fUnit)
{
	CString CMD;
	CMD.Format("CF %.0lf%cZ\n",freq,fUnit);
	if(viPrintf(Session,CMD.GetBuffer(0)) != VI_SUCCESS) return(false);
	return(true);
}

bool R3131::get_measure_value(double &f,double &lv,int freqSampleNum)
{
	bool rValue = true;
    
	if(viPrintf(Session,"AM\n") != VI_SUCCESS) return(false);
	if(Mk_Hi_Time) Sleep(Mk_Hi_Time);
	if(viPrintf(Session,"PS\n") != VI_SUCCESS) return(false);
	
	if(viQueryf(Session,"ML?\n","%lf",&lv) != VI_SUCCESS) {
		lv = 0.0;
		rValue = false;
	}
	if(viQueryf(Session,"MF?\n","%lf",&f) != VI_SUCCESS) {
		f = 0.0;
		rValue = false;
	}
	viPrintf(Session,"AB\n");
	viPrintf(Session,"AW\n");
	return(rValue);
}

bool R3131::get_measure_value2(double &f,double &lv,int freqSampleNum)
{
	bool rValue = true;
    
	if(viPrintf(Session,"AM\n") != VI_SUCCESS) return(false);
	if(Mk_Hi_Time) Sleep(Mk_Hi_Time);
	if(viPrintf(Session,"PS\n") != VI_SUCCESS) return(false);
	
	if(viQueryf(Session,"ML?\n","%lf",&lv) != VI_SUCCESS) {
		lv = 0.0;
		rValue = false;
	}
	if(viQueryf(Session,"MF?\n","%lf",&f) != VI_SUCCESS) {
		f = 0.0;
		rValue = false;
	}
	viPrintf(Session,"AB\n");
	viPrintf(Session,"AW\n");
	return(rValue);
}



//--------------------------------------------------------------------------------------------------------------
// bt test set
//--------------------------------------------------------------------------------------------------------------
bt_TSET::bt_TSET(const CString &name)
{
	dev_statu = 0;
	dev_paramter_ini();
}

bt_TSET::~bt_TSET()
{
}

void bt_TSET::dev_paramter_ini(void)
{
	
}

bool bt_TSET::creat_dev(const CString &name,class bt_TSET *&dev)
{
	if(dev) return(true);
//	if(name == "MT8852A") dev = new MT8852A_DEV(name);
	//else if(name == "MT8852B") dev = new MT8852B_DEV(name);
	if(name == "N4010A") dev = new N4010A_DEV(name);
	else return(false);
	return(true);
}

void bt_TSET::prompt_message(const CString &message,class CTestItem *Tobj)
{
	if(!Tobj) return;
	Tobj->param = message;
	Tobj->show_message();
}


int bt_TSET::query(ViSession Session,CString cmd,char *buf,int len)
{
	unsigned long actual = 0;

	memset(buf,0,len);
	if(VI_SUCCESS != viWrite(Session,(ViBuf)cmd.GetBuffer(0),cmd.GetLength(),&actual)) return(_VI_ERROR);

	actual = 0;
	if(VI_SUCCESS != viRead (Session,(ViBuf)buf,len-1,&actual)) return(_VI_ERROR);
	buf[actual] = 0;
	return(VI_SUCCESS);
}



#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
MT8852A_DEV::MT8852A_DEV(const CString &name):bt_TSET(name),GPIB_DEV(name)
{
	ConfigFile &cfg = pCfgf;

	GPIB_DEV::port.key.PortName = cfg.VCString(ParMT8852A_Port);
	GPIB_DEV::port.key.connect_probe_name = cfg.VCString(ParMT8852A_Probedev);
}

MT8852A_DEV::~MT8852A_DEV()
{
}

void MT8852A_DEV::dev_paramter_ini(void)
{
	ConfigFile &cfg = pCfgf;
	
	dut_par = NULL;
	bt_TSET::dev_paramter_ini();
	
	GPIB_DEV::port.key.PortName = "";//cfg.VCString(ParMT8852A_Port);
	GPIB_DEV::port.key.connect_probe_name = "";//cfg.VCString(ParMT8852A_Probedev);

        //discn_dut_time = cfg.Vulong(ParMT8852x_DiCn_time);
        reset_device_time = cfg.Vulong(ParMT8852x_RestTime);   //Sleep(reset_device_time)
        max_test_time = cfg.Vulong(ParMT8852x_TestTime);
        max_test_time *= 1000;

        script = cfg.Vulong(ParMT8852x_Script);
        if(script < 2) script = 3;
        else if(script > 10) script = 10;   //mt8852a cmd contains 'script' parameter

        pathOffset = cfg.VDouble(s_DevA_MCH_pathLoss);   //FIXEDOFF pathOffset
        txPower = cfg.VDouble(ParMT8852x_txPwr);
        freq_offset = cfg.VDouble(ParMT8852x_freqOffset);

        LRxF = cfg.VDouble(ParMT8852x_LRxF);
        LTxF = cfg.VDouble(ParMT8852x_LTxF);
        MRxF = cfg.VDouble(ParMT8852x_MRxF);
        MTxF = cfg.VDouble(ParMT8852x_MTxF);
        HRxF = cfg.VDouble(ParMT8852x_HRxF);
        HTxF = cfg.VDouble(ParMT8852x_HTxF);
#if 0
        op_lf_sel = cfg.ItemSelect(ParMT8852x_OPLFTestSel);
        op_mf_sel = cfg.ItemSelect(ParMT8852x_OPMFTestSel);
        op_hf_sel = cfg.ItemSelect(ParMT8852x_OPHFTestSel);
        op_test_type = cfg.VCString(ParMT8852x_OPTestType);
        op_hopping = cfg.VCString(ParMT8852x_OPHopping);
        op_num_packets = cfg.Vulong(ParMT8852x_OPPacktes);
		pwr_avmin = cfg.VDouble(ParMT8852x_OPAvMin);
        pwr_avmax = cfg.VDouble(ParMT8852x_OPAvMax);
		pwr_peak = cfg.VDouble(ParMT8852x_OPPEAK);
#endif
        if(op_test_type != "LOOPBACK") op_test_type = "TXTEST"; //pwr, loopback,TX
        if(op_hopping == "HOPON") op_is_hopping = true;
        else
        {
           op_is_hopping = false;
           op_hopping = "HOPOFF";
        }
#if 0
        ic_lf_sel = cfg.ItemSelect(ParMT8852x_ICLFTestSel);
        ic_mf_sel = cfg.ItemSelect(ParMT8852x_ICMFTestSel);
        ic_hf_sel = cfg.ItemSelect(ParMT8852x_ICHFTestSel);
        ic_test_type = cfg.VCString(ParMT8852x_ICTestType);
        ic_hopping = cfg.VCString(ParMT8852x_ICHopping);
        ic_num_packets = cfg.Vulong(ParMT8852x_ICPacktes); 
		foffset_pmax = cfg.VDouble(ParMT8852x_ICMaxPos);
        foffset_nmax = cfg.VDouble(ParMT8852x_ICMaxNeg);
#endif
        if(ic_test_type != "LOOPBACK") ic_test_type = "TXTEST"; //ini_carrier, loopback,TX
       
		if(ic_hopping == "HOPON") ic_is_hopping = true;
        else
        {
			ic_is_hopping = false;
			ic_hopping = "HOPOFF";
        }
#if 0
        ss_lf_sel = cfg.ItemSelect(ParMT8852x_SSLFTestSel);
        ss_mf_sel = cfg.ItemSelect(ParMT8852x_SSMFTestSel);
        ss_hf_sel = cfg.ItemSelect(ParMT8852x_SSHFTestSel);
        ss_hopping = cfg.VCString(ParMT8852x_SSHopping);      //single_sens
        ss_num_packets = cfg.Vulong(ParMT8852x_SSPacktes);
        ss_tx_pwr = cfg.VDouble(ParMT8852x_SSTxPwr);
        ss_ber_max = cfg.VDouble(ParMT8852x_SSPERMax);
        ss_fer_max = cfg.VDouble(ParMT8852x_SSFERMax);
#endif
        if(ss_hopping == "HOPON") ss_is_hopping = true;
        else
        {
			ss_is_hopping = false;
			ss_hopping = "HOPOFF";
        }
}

bool MT8852A_DEV::connect(void)
{
	CString Temp;

	dev_paramter_ini();
	bool res = GPIB_DEV::connect();
	if(!res) return(false);
	
	if(viPrintf(Session,"*RST\n") != VI_SUCCESS) goto ___err_connect;
	if(viPrintf(Session,"*CLS\n") != VI_SUCCESS) goto ___err_connect;
	Sleep(reset_device_time);
	if(viPrintf(Session,"*INE 9\n") != VI_SUCCESS) goto ___err_connect;
	
	Temp.Format("SCPTSEL %d\n",script);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	
	Temp.Format("SCRIPTMODE %d,STANDARD\n",script);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;    
	
	Temp.Format("PATHOFF %d,FIXED\n",script);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	Temp.Format("FIXEDOFF %d,%.1lf\n",script,pathOffset);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	
	Temp.Format("TXPWR %d,%.1lf\n",script,txPower);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	
	Temp.Format("SCPTCFG %d,ALLTSTS,OFF\n",script);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	if(op_lf_sel||op_mf_sel||op_hf_sel) Temp.Format("SCPTCFG %d,OP,ON\n",script);
	else Temp.Format("SCPTCFG %d,OP,OFF\n",script);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	if(ic_lf_sel||ic_mf_sel||ic_hf_sel) Temp.Format("SCPTCFG %d,IC,ON\n",script);
	else Temp.Format("SCPTCFG %d,IC,OFF\n",script);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	if(ss_lf_sel||ss_mf_sel||ss_hf_sel) Temp.Format("SCPTCFG %d,SS,ON\n",script);
	else Temp.Format("SCPTCFG %d,SS,OFF\n",script);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	
	if(viPrintf(Session,"SYSCFG EUTSRCE,MANUAL\n") != VI_SUCCESS) goto ___err_connect;
	if(viPrintf(Session,"SYSCFG CONFIG,LKTIMO,5\n") != VI_SUCCESS) goto ___err_connect;
	if(viPrintf(Session,"SYSCFG CONFIG,RANGE,AUTO\n") != VI_SUCCESS) goto ___err_connect;
	
	Temp.Format("OPCFG %d,HOPPING,%s\n",script,op_hopping);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	Temp.Format("OPCFG %d,HOPMODE,DEFINED\n",script);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	Temp.Format("OPCFG %d,TSTCTRL,%s\n",script,op_test_type);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	Temp.Format("OPCFG %d,PKTTYPE,LONG\n",script);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	Temp.Format("OPCFG %d,NUMPKTS,%d\n",script,op_num_packets);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
#if 0	    
	    Temp.Format("OPCFG %d,AVGMNLIM,%.1lf\n",script,pwr_avmin);
        if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
        Temp.Format("OPCFG %d,AVGMXLIM,%.1lf\n",script,pwr_avmax);
        if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
        Temp.Format("OPCFG %d,PEAKLIM,%.1lf\n",script,pwr_peak);
        if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
#endif
	creat_test_f("OPCFG",op_lf_sel,L,"L");
	creat_test_f("OPCFG",op_mf_sel,M,"M");
	creat_test_f("OPCFG",op_hf_sel,H,"H");
	
	Temp.Format("ICCFG %d,HOPPING,%s\n",script,ic_hopping);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	Temp.Format("ICCFG %d,HOPMODE,DEFINED\n",script);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	Temp.Format("ICCFG %d,TSTCTRL,%s\n",script,ic_test_type);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	Temp.Format("ICCFG %d,NUMPKTS,%d\n",script,ic_num_packets);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
#if 0	     
	     Temp.Format("ICCFG %d,MXPOSLIM,%lf\n",script,foffset_pmax*1000);    
	     if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
         Temp.Format("ICCFG %d,MXNEGLIM,%lf\n",script,foffset_nmax*1000);
         if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
#endif
	creat_test_f("ICCFG",ic_lf_sel,L,"L");
	creat_test_f("ICCFG",ic_mf_sel,M,"M");
	creat_test_f("ICCFG",ic_hf_sel,H,"H");
	
	Temp.Format("SSCFG %d,HOPPING,%s\n",script,ss_hopping);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	Temp.Format("SSCFG %d,NUMPKTS,%d\n",script,ss_num_packets);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	Temp.Format("SSCFG %d,TXPWR,%lf\n",script,ss_tx_pwr);
    if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	Temp.Format("SSCFG %d,BERLIM,%lf\n",script,ss_ber_max);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	Temp.Format("SSCFG %d,FERLIM,%lf\n",script,ss_fer_max);
	if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) goto ___err_connect;
	creat_test_f("SSCFG",ss_lf_sel,L,"L");
	creat_test_f("SSCFG",ss_mf_sel,M,"M");
	creat_test_f("SSCFG",ss_hf_sel,H,"H");
	
	return(setDutPar(pMainWnd->dutGloden[DUT_PAR_IDX]));

___err_connect:
        ReleaseSession();
        return(false);
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

bool MT8852A_DEV::setDutPar(CDutGlodenPar *par)
{
    if(dut_par == par) return(true);
    dut_par = par;
    if(!dut_par) return(true);

//#if 0
	//cfg to 8852
	double min_pwr = 1000,  max_pwr = -1000;
    
	if(dut_par->rf_pwr_min     < min_pwr) min_pwr = dut_par->rf_pwr_min;
    if(dut_par->ch2_rf_pwr_min < min_pwr) min_pwr = dut_par->ch2_rf_pwr_min;
	if(dut_par->ch3_rf_pwr_min < min_pwr) min_pwr = dut_par->ch3_rf_pwr_min;
	
	if(dut_par->rf_pwr_max     > max_pwr) max_pwr = dut_par->rf_pwr_max;
	if(dut_par->ch2_rf_pwr_max > max_pwr) max_pwr = dut_par->ch2_rf_pwr_max;
	if(dut_par->ch3_rf_pwr_max > max_pwr) max_pwr = dut_par->ch3_rf_pwr_max;


    CString Temp;
    for(int i=0;i<3;i++)
    {
		Temp.Format("OPCFG %d,AVGMNLIM,%.1lf\n",script,min_pwr);
		if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) continue;
		Temp.Format("OPCFG %d,AVGMXLIM,%.1lf\n",script,max_pwr);
		if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) continue;
		Temp.Format("OPCFG %d,PEAKLIM,%.1lf\n",script,dut_par->rf_pwr_peak);
		if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) continue;
		
		double max_pos = (dut_par->rf_max_pos_foffset + freq_offset)*1000;
		double min_pos = (dut_par->rf_max_neg_foffset - freq_offset)*1000;
		Temp.Format("ICCFG %d,MXPOSLIM,%lf\n",script,max_pos);
		if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) continue;
		Temp.Format("ICCFG %d,MXNEGLIM,%lf\n",script,min_pos);
		if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) continue;
		
		Temp.Format("SSCFG %d,TXPWR,%lf\n",script,dut_par->sens_pwr);
		if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) continue;
		break;
    }
	return(i<3);

//#endif

}
#endif
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
const char *mt8852_res_que_str[][2] = 
{
        //LCH result op
        {"XRESULT OP,HOPOFFL\n",        "XRESULT OP,HOPONL\n"   },
        {"XOP,HOPOFFL,TRUE",            "XOP,HOPONL,TRUE"       },
        {"XOP,HOPOFFL,FALSE",           "XOP,HOPONL,FALSE"      },
        {"XRESULT IC,HOPOFFL\n",        "XRESULT IC,HOPONL\n"   },
        {"XIC,HOPOFFL,TRUE",            "XIC,HOPONL,TRUE"       },
        {"XIC,HOPOFFL,FALSE",           "XIC,HOPONL,FALSE"      },
        {"XRESULT SS,HOPOFFL\n",        "XRESULT SS,HOPONANY\n" },
        {"XSS,HOPOFFL,TRUE",            "XSS,HOPONANY,TRUE"     },
        {"XSS,HOPOFFL,FALSE",           "XSS,HOPONANY,FALSE"    },

        //MCH result op
        {"XRESULT OP,HOPOFFM\n",        "XRESULT OP,HOPONM\n"   },
        {"XOP,HOPOFFM,TRUE",            "XOP,HOPONM,TRUE"       },
        {"XOP,HOPOFFM,FALSE",           "XOP,HOPONM,FALSE"      },
        {"XRESULT IC,HOPOFFM\n",        "XRESULT IC,HOPONM\n"   },
        {"XIC,HOPOFFM,TRUE",            "XIC,HOPONM,TRUE"       },
        {"XIC,HOPOFFM,FALSE",           "XIC,HOPONM,FALSE"      },
        {"XRESULT SS,HOPOFFM\n",        "XRESULT SS,HOPONANY\n" },
        {"XSS,HOPOFFM,TRUE",            "XSS,HOPONANY,TRUE"     },
        {"XSS,HOPOFFM,FALSE",           "XSS,HOPONANY,FALSE"    },

        //HCH result op
        {"XRESULT OP,HOPOFFH\n",        "XRESULT OP,HOPONH\n"   },
        {"XOP,HOPOFFH,TRUE",            "XOP,HOPONH,TRUE"       },
        {"XOP,HOPOFFH,FALSE",           "XOP,HOPONH,FALSE"      },
        {"XRESULT IC,HOPOFFH\n",        "XRESULT IC,HOPONH\n"   },
        {"XIC,HOPOFFH,TRUE",            "XIC,HOPONH,TRUE"       },
        {"XIC,HOPOFFH,FALSE",           "XIC,HOPONH,FALSE"      },
        {"XRESULT SS,HOPOFFH\n",        "XRESULT SS,HOPONANY\n" },
        {"XSS,HOPOFFH,TRUE",            "XSS,HOPONANY,TRUE"     },
        {"XSS,HOPOFFH,FALSE",           "XSS,HOPONANY,FALSE"    },
};


enum {
        OP_RES_QUE_IDX = 0,
        OP_RES_TRUE_IDX,
        OP_RES_FALSE_IDX,
        IC_RES_QUE_IDX,
        IC_RES_TRUE_IDX,
        IC_RES_FALSE_IDX,
        SS_RES_QUE_IDX,
        SS_RES_TRUE_IDX,
        SS_RES_FALSE_IDX,

        LEN_RES_OP,
};


bool MT8852A_DEV::do_test(class CTestItem *Tobj,unsigned long delay)
{
        if(viPrintf(Session,"RUN\n") != VI_SUCCESS) return(false);

        CString Temp;
        unsigned long ttime = 0;
        int statu = 0;
        while(ttime < max_test_time)
        {
                Sleep(1000);
                ttime += 1000;
                if(!(ttime%1000))
                { 
                        Temp.Format("wait test:%d\n",ttime/1000);
                        if(Tobj) prompt_message(Temp, Tobj);
                }
                if(ttime < delay) continue;

                if(viQueryf(Session,"*INS?\n","%d",&statu) != VI_SUCCESS) return(false);
                if(statu & 0x04) break;
        }
        
        if(!(statu & 0x04)) 
        {
                viPrintf(Session,"ABORT");
                return(false);
        }

        if(!(statu & 0x01)) return(false);
        return(true);
}


bool MT8852A_DEV::run(const CString& addr,  struct tSetResult& mt8852_res, class CTestItem *Tobj)
{
	CTObj_mt8852_test *tobj = (CTObj_mt8852_test *)Tobj;
	
	if(!run2(addr,mt8852_res,Tobj)) return(false);
	return(true);
	
}


bool MT8852A_DEV::setPathOffset(double pathoffset)
{
        CString Temp;
        Temp.Format("FIXEDOFF %d,%.1lf\n",script,pathoffset);
        if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) 
        {
            if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) return(false);
        }
        return(true);
}


bool MT8852A_DEV::run2(const CString& addr,struct tSetResult &mt8852_res,class CTestItem *Tobj)
{
	bool rValue = false;

        CString Temp;
        Temp.Format("SYSCFG EUTADDR,%s\n",addr);
        if(viPrintf(Session,Temp.GetBuffer(0)) != VI_SUCCESS) return(rValue);
        Sleep(200);

        if(!do_test(Tobj,1500)) return(false);
        disconnect_dut();

        bool res = get_result_xch(mt8852_res,0);
        if(res) res = get_result_xch(mt8852_res,1);
        if(res) res = get_result_xch(mt8852_res,2);
        
        rValue = false;
        for(int i=0;i<3;i++)
        {
                if(mt8852_res.op[i].valid || mt8852_res.ic[i].valid || mt8852_res.ss[i].valid)
                {
                        rValue = true;
                        break;
                }
        }
        if(!rValue) return(rValue);
        //if(discn_dut_time) Sleep(discn_dut_time);

        return(rValue);
}


bool MT8852A_DEV::get_result_xch(struct tSetResult& tresult,int ch)
{
        struct top_res* pop = &tresult.op[ch];
        struct tic_res* pic = &tresult.ic[ch];
        struct tss_res* pss = &tresult.ss[ch];
        const char *((*que_str)[2]) = &mt8852_res_que_str[ch*LEN_RES_OP];
        const char *temp_str;
        const int len = 256;
        char result[len];
        CString Temp;

        temp_str = que_str[OP_RES_QUE_IDX][op_is_hopping];

        //for rf power
        pop->is_hoping = op_is_hopping;
        memset(result,0,len);
        if(query(Session,que_str[OP_RES_QUE_IDX][op_is_hopping],result,len) != VI_SUCCESS) return(false);
        Temp = result;
        if(Temp.Find(que_str[OP_RES_FALSE_IDX][op_is_hopping]) >= 0) memset(pop,0,sizeof(struct top_res));
        else
        {
                Temp.Replace(que_str[OP_RES_TRUE_IDX][op_is_hopping],"1");
                Temp.Replace(que_str[OP_RES_FALSE_IDX][op_is_hopping],"0");
                Temp.Replace("PASS","1");
                Temp.Replace("FAIL","0");
                if(sscanf(Temp.GetBuffer(0),"%d,%lf,%lf,%lf,%lf,%d,%d,%d",
                          &pop->valid,&pop->av_max,&pop->av_min,&pop->av_pk,
                          &pop->av,&pop->Failed,&pop->Tested,&pop->res) != 8)
                {
                        pop->valid = false;
                        return(false);
                }
        }
        pop->is_hoping = op_is_hopping;

        //for offset
        pic->is_hoping = ic_is_hopping;
        memset(result,0,len);
        if(query(Session,que_str[IC_RES_QUE_IDX][ic_is_hopping],result,len) != VI_SUCCESS) return(false);
        Temp = result;
        if(Temp.Find(que_str[IC_RES_FALSE_IDX][ic_is_hopping]) >= 0) memset(pic,0,sizeof(struct tic_res));
        else
        {
                Temp.Replace(que_str[IC_RES_TRUE_IDX][ic_is_hopping],"1");
                Temp.Replace(que_str[IC_RES_FALSE_IDX][ic_is_hopping],"0");
                Temp.Replace("PASS","1");
                Temp.Replace("FAIL","0");
                if(sscanf(Temp.GetBuffer(0),"%d,%lf,%lf,%lf,%d,%d,%d",
                          &pic->valid,&pic->av,&pic->pos_max,&pic->neg_max,
                          &pic->Failed,&pic->Tested,&pic->res) != 7) 
                {
                        pic->valid = false;
                        return(false);
                }
                // change to kHz unit
				pic->av /= 1000;
                pic->pos_max /= 1000;
                pic->neg_max /= 1000;

                //pic->av -= freq_offset;
                //pic->pos_max -= freq_offset;
                //pic->neg_max -= freq_offset;
        }
        pic->is_hoping = ic_is_hopping;


        //for Single Slot Sensitivity
        pss->is_hoping = ss_is_hopping;
        memset(result,0,len);
        if(query(Session,que_str[SS_RES_QUE_IDX][ss_is_hopping], result, len) != VI_SUCCESS) return(false);
        Temp = result;
        if(Temp.Find(que_str[SS_RES_FALSE_IDX][ss_is_hopping]) >= 0) memset(pss,0,sizeof(struct tss_res));
        else
        {
                Temp.Replace(que_str[SS_RES_TRUE_IDX][ss_is_hopping],"1");
                Temp.Replace(que_str[SS_RES_FALSE_IDX][ss_is_hopping],"0");
                Temp.Replace("PASS","1");
                Temp.Replace("FAIL","0");
                if(sscanf(Temp.GetBuffer(0),"%d,%lf,%lf,%d,%d,%d,%d,%d,%d,%d,%d",
                          &pss->valid,&pss->Ber,&pss->Fer,&pss->res,&pss->fercrc,
                          &pss->ferlength,&pss->ferlost,&pss->packrx,&pss->biterr,
                          &pss->frameerr,&pss->packsend) != 11) 
                {
                        pss->valid = false;
                        return(false);
                }
        }
        pss->is_hoping = ss_is_hopping;

	return(true);
}


void MT8852A_DEV::disconnect_dut(void)
{
        viPrintf(Session, "DISCONNECT\n");
}

extern int hexCharToValue(char ch);

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
N4010A_DEV::N4010A_DEV(const CString &name):bt_TSET(name),GPIB_DEV(name)
{
	ConfigFile &cfg = pCfgf;

	GPIB_DEV::port.key.PortName = cfg.VCString(ParN4010A_Port);
	GPIB_DEV::port.key.connect_probe_name = cfg.VCString(ParN4010A_Probedev);
}

N4010A_DEV::~N4010A_DEV()
{
	if(dev_is_ready())
	{
		viPrintf(Session,"*RST\n");
		viPrintf(Session,"DIAG:SELF:END\n");
		viPrintf(Session,"DIAG:FPAN:MESS:CLE\n");
	}

}

void N4010A_DEV::dev_paramter_ini(void)
{
	ConfigFile &cfg = pCfgf;
	
	dut_par = NULL;
	bt_TSET::dev_paramter_ini();
	
	GPIB_DEV::port.key.PortName = cfg.VCString(ParN4010A_Port);
	GPIB_DEV::port.key.connect_probe_name = cfg.VCString(ParN4010A_Probedev);		//cfg.VCString(ParMT8852A_Probedev);

        //discn_dut_time = cfg.Vulong(ParMT8852x_DiCn_time);
        reset_device_time = 0;//cfg.Vulong(ParMT8852x_RestTime);   //Sleep(reset_device_time)
        max_test_time = 0;//cfg.Vulong(ParMT8852x_TestTime);
        max_test_time *= 1000;


        pathOffset = cfg.VDouble(s_DevA_MCH_pathLoss);   //FIXEDOFF pathOffset



       

}

bool N4010A_DEV::connect(void)
{
	CString Temp;

	dev_paramter_ini();
	bool res = GPIB_DEV::connect();
	if(!res) return(false);
	
	//if(viSetAttribute(Session, VI_ATTR_TMO_VALUE, DEFAULT_TIMEOUT) != VI_SUCCESS) goto ___err_connect;       // 设置超时

	if(viPrintf(Session,"*RST\n") != VI_SUCCESS) goto ___err_connect;
	if(viPrintf(Session,"*CLS\n") != VI_SUCCESS) goto ___err_connect;
	Sleep(reset_device_time);
//	if(viPrintf(Session,"DIAG:FPAN:KEYP:TEST\n") != VI_SUCCESS) goto ___err_connect;
//	if(viPrintf(Session,"DIAG:FPAN:MESS:SET \"Under control by BLE_ATE\"\n") != VI_SUCCESS) goto ___err_connect;
	if(viPrintf(Session,"SENS:ROSC:SOUR INT\n") != VI_SUCCESS) goto ___err_connect;
	if(viPrintf(Session,"INST \"RFA\"\n") != VI_SUCCESS) goto ___err_connect;
//	if(viPrintf(Session,"SENS:CORR:LOSS:STAT ON\n") != VI_SUCCESS) goto ___err_connect;
//	if(viPrintf(Session,"SENS:CORR:LOSS:FIX 0\n") != VI_SUCCESS) goto ___err_connect;
	if(viPrintf(Session,"SEQ:LOOP CONT\n") != VI_SUCCESS) goto ___err_connect;
	return(true);

___err_connect:
        ReleaseSession();
        return(false);
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

bool N4010A_DEV::setDutPar(CDutGlodenPar *par)
{    
	return(true);
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
bool N4010A_DEV::do_test(class CTestItem *Tobj,unsigned long delay)
{
    return(true);
}


bool N4010A_DEV::run(const CString& addr,  struct tSetResult& mt8852_res, class CTestItem *Tobj)
{
	CTObj_mt8852_test *tobj = (CTObj_mt8852_test *)Tobj;
	
	if(viPrintf(Session,"INIT\n") != VI_SUCCESS) return(false);

	return(true);
	
}

bool N4010A_DEV::stop()
{
	if(viPrintf(Session,"ABOR\n") != VI_SUCCESS) return(false);
	
	return(true);
	
}


/*
ViSession control_sesn;
ViSession control_vi;
int get_sesn_and_vi(void)
{
	int result = -1;

	ViSession test_sesn;
	ViSession test_vi;

	ViChar buffer[VI_FIND_BUFLEN];
	ViRsrc matches = buffer;
	//ViUInt32 nmatches;
	//ViFindList list;
	
	
	if(viOpenDefaultRM(&test_sesn) == VI_SUCCESS)
		control_sesn = test_sesn-1;
	else
	{
		viClose(test_sesn);
		return -1;
	}

	if(viOpen(test_sesn,matches,VI_NULL,VI_NULL,&test_vi) == VI_SUCCESS)
		control_vi = test_vi-1;
	else
	{
		viClose(test_vi);
		return -1;
	}
		
	viClose(test_vi);
	viClose(test_sesn);

	return result;
}
*/
bool N4010A_DEV::setPathOffset(double pathoffset)
{
	char buf[256];
	int len = 256;
    CString Temp;
	unsigned long actual = 0;
/*	ViSession test_sesn;

	//viLock（vi，lockType，timeout，requestId，accessKey）：设置资源存取模式。
	//viUnlock（vi）：取消资源存取模式。
	//viWrite（vi，buf，count，retCount）：将数据同步写入到器件中。
	//viWriteAsync（vi，buf，count，jobId）：将数据异步写入到器件中。
	//viUnlock(Session);

	ViChar buffer[VI_FIND_BUFLEN];
	ViRsrc matches = buffer;
	ViUInt32 nmatches;
	ViFindList list;

	//get_sesn_and_vi();

	viOpenDefaultRM(&test_sesn);
	viClose(test_sesn);

	viFindRsrc(DEVRM,"GPIB[0-1]*::?*INSTR",&list,&nmatches,matches);
	viFindRsrc((ViSession)(test_sesn-1),"GPIB[0-1]*::?*INSTR",&list,&nmatches,matches);
*/
/*
//in
{
	Temp.Format("STATus:QUEStionable:INTegrity:COND?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);
	
	Temp.Format("STAT:QUES:FREQ:COND?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);
	
	Temp.Format("STATus:QUEStionable:INTegrity:COND?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
		memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);
	
	Temp.Format("STATus:QUEStionable:INTegrity:COND?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);

	Temp.Format("STATus:QUEStionable:INTegrity:COND?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);

	Temp.Format("STAT:QUES:FREQ:COND?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);	
}
*/
//Sleep(1000);
//Sleep(1000);


//set loss compensation 1
{
	Temp.Format("SOUR:CORR:LCOM OFF");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	Temp.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);

	Temp.Format("SENSe:CORR:LCOM OFF");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	Temp.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);

	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);
}

///*
//set loss compensation 2
{
	Temp.Format("SOUR:CORR:LCOM FIXed");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	Temp.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);

	Temp.Format("SENSe:CORR:LCOM FIXed");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	Temp.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);

	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);

}

//set loss compensation 3
{
	Temp.Format("SOUR:CORR:LCOM:FIXed 0");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	Temp.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);

	Temp.Format("SENSe:CORR:LCOM:FIXed 0");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	Temp.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);

	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);

}
//*/
//Sleep(1000);
//Sleep(1000);

//set loss compensation 4
{

	Temp.Format("SOUR:CORR:LCOM:FIXed %.2lf",pathoffset);
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	Temp.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);


    Temp.Format("SENSe:CORR:LCOM:FIXed %.2lf",pathoffset);
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	Temp.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);

	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);

}


//Sleep(1000);
//Sleep(1000);

/*
//out
{
	Temp.Format("STATus:QUEStionable:INTegrity:COND?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);
	
	Temp.Format("STAT:QUES:FREQ:COND?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);	
	
	Temp.Format("STATus:QUEStionable:INTegrity:COND?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);

	Temp.Format("STATus:QUEStionable:INTegrity:COND?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);

	Temp.Format("STATus:QUEStionable:INTegrity:COND?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);
	
	Temp.Format("STAT:QUES:FREQ:COND?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)Temp.GetBuffer(0),Temp.GetLength(),&actual)) 
		return(false);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(false);
}	
	
Sleep(1000);
Sleep(1000);
	
*/
	
    return(true);
}

bool N4010A_DEV::run2(const CString& addr,struct tSetResult &mt8852_res,class CTestItem *Tobj)
{
	return(true);
}

bool N4010A_DEV::run3()
{
	if(viPrintf(Session,"INIT\n") != VI_SUCCESS) return(false);
	
	return(true);
}
///*
int N4010A_DEV::set_rang(int rang,int &flag)
{
	char buf[256];
	//CString Temp;	
	CString cmd_str;
	CString answer_str;
	unsigned long actual = 0;
	int len = 256;
	int ret = 0;
	
	cmd_str.Format("*CLS");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	cmd_str.Format("DIAG:HW:FEA:RANG %d",rang);
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);
	
	answer_str.Empty();
	answer_str = buf;
	ret = answer_str.Find("+0,\"No error\"");

	if(ret == -1)// 未找到对应字符串
		flag = 0;
	else
		flag = 1;
	return(VI_SUCCESS);
}


int N4010A_DEV::set_rang2(int rang)
{
	char buf[256];
	//CString Temp;	
	CString cmd_str;
	CString answer_str;
	unsigned long actual = 0;
	int len = 256;
	int ret = 0;

	cmd_str.Format("DIAG:HW:DAP:READ:MISC:APOW? 1");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);
	
	cmd_str.Format("DIAG:HW:DAP:ACQ:ADC:OVERrange?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);
	
	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);

	answer_str.Empty();
	answer_str = buf;
	ret = answer_str.Find("+411,\"ADC overrange\"");//answer_str.Find("+0,\"No error\"");
	
	if(ret != -1) return(VI_TRUE);


	
	cmd_str.Format("*CLS");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:BAND 22.0e6");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	if(VI_SUCCESS != viSetAttribute((ViSession)(Session+1),1073676314,5000)) 
		return(_VI_ERROR);
	
	cmd_str.Format("DIAG:HW:FEA:RANG %d",rang);
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);
	

	return(VI_FALSE);
}



int N4010A_DEV::set_rang3(double level,int rang)
{
	char buf[256];
	//CString Temp;	
	CString cmd_str;
	CString answer_str;
	unsigned long actual = 0;
	int len = 256;
	int ret = 0;
	int ret_head,ret_end;
	int temp = 0,i = 0,val=0;
	double temp_ferq; 

//range _ _ _ level
//6 _ _ _ -24.0008420429049
//7 _ _ _ -22.9986181717796
//8 _ _ _ -21.999307342057
//9 _ _ _ -21.0008802678934
//10 _ _ _ -20.0004627314364
//11 _ _ _ -19.0000278201664
//12 _ _ _ -17.9998564916782
//13 _ _ _ -16.9993809939234
//14 _ _ _ -15.9994787969179
//15 _ _ _ -15.0003383731721
//16 _ _ _ -13.9991128584775
//17 _ _ _ -13.0002617967832
//18 _ _ _ -12.0000087923724
//19 _ _ _ -10.9992219128937
//20 _ _ _ -1
//21 _ _ _ -1




	cmd_str.Format("DIAG:HW:DAP:TRIG:LEVel %lf",level);
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);






// 11
	cmd_str.Format("DIAG:HW:DAP:READ:MISC:APOW? 1");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);
	
	
	cmd_str.Format("DIAG:HW:DAP:ACQ:ADC:OVERrange?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);
	
	
	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);

	answer_str.Empty();
	answer_str = buf;
	ret = answer_str.Find("+411,\"ADC overrange\"");//answer_str.Find("+0,\"No error\"");

// 1	
	if(ret != -1) 
	{	
		cmd_str.Format("*CLS");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		cmd_str.Format("DIAG:HW:BAND 22.0e6");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		if(VI_SUCCESS != viSetAttribute((ViSession)(Session+1),1073676314,5000)) 
			return(_VI_ERROR);
		
		cmd_str.Format("DIAG:HW:FEA:RANG %d",(rang+1));
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		cmd_str.Format("SYSTem:ERRor?");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		memset(buf, 0, 256*sizeof(char));
		if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
			return(_VI_ERROR);

		return(VI_FALSE);
	}





	
	cmd_str.Format("*CLS");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:LATC:SEL \"DAP ADC Max Abs Val\"");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	cmd_str.Format("DIAG:HW:LATC:VAL?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);
	
	answer_str.Empty();
	answer_str = buf;
	ret_head = answer_str.Find("+") + 1;// 
	ret_end = answer_str.Find("\n");// 
	//十进制位数
	temp = ret_end - ret_head;
	val = 0;
	for(i=0;i<temp;i++)
	{	
		val = val * 10;
		val += hexCharToValue(buf[ret_head + i]);
	}
	
// 2
	if(val > 7500)
	{	
		cmd_str.Format("*CLS");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		cmd_str.Format("DIAG:HW:BAND 22.0e6");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		if(VI_SUCCESS != viSetAttribute((ViSession)(Session+1),1073676314,5000)) 
			return(_VI_ERROR);
		
		cmd_str.Format("DIAG:HW:FEA:RANG %d",(rang+1));
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		cmd_str.Format("SYSTem:ERRor?");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		memset(buf, 0, 256*sizeof(char));
		if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
			return(_VI_ERROR);

		return(VI_FALSE);
	}	


//CentFreq - 22000000;
	temp_ferq = CentFreq - 22000000;
	cmd_str.Format("DIAG:HW:FEA:FREQ %lf",temp_ferq);
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);












	cmd_str.Format("*CLS");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:BAND 22.0e6");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	if(VI_SUCCESS != viSetAttribute((ViSession)(Session+1),1073676314,5000)) 
		return(_VI_ERROR);
	
	cmd_str.Format("DIAG:HW:FEA:RANG %d",(rang-5));
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);










// 22
	cmd_str.Format("DIAG:HW:DAP:READ:MISC:APOW? 1");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);
	
	
	cmd_str.Format("DIAG:HW:DAP:ACQ:ADC:OVERrange?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);
	
	
	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);

	answer_str.Empty();
	answer_str = buf;
	ret = answer_str.Find("+411,\"ADC overrange\"");//answer_str.Find("+0,\"No error\"");
// 1
	if(ret != -1) 
	{	
		cmd_str.Format("*CLS");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		cmd_str.Format("DIAG:HW:BAND 22.0e6");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		if(VI_SUCCESS != viSetAttribute((ViSession)(Session+1),1073676314,5000)) 
			return(_VI_ERROR);
		
		cmd_str.Format("DIAG:HW:FEA:RANG %d",(rang+1));
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		cmd_str.Format("SYSTem:ERRor?");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		memset(buf, 0, 256*sizeof(char));
		if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
			return(_VI_ERROR);

		return(VI_FALSE);
	}


	cmd_str.Format("*CLS");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:LATC:SEL \"DAP ADC Max Abs Val\"");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	cmd_str.Format("DIAG:HW:LATC:VAL?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);
	
	answer_str.Empty();
	answer_str = buf;
	ret_head = answer_str.Find("+") + 1;// 
	ret_end = answer_str.Find("\n");// 
	//十进制位数
	temp = ret_end - ret_head;
	val = 0;
	for(i=0;i<temp;i++)
	{	
		val = val * 10;
		val += hexCharToValue(buf[ret_head + i]);
	}
	
// 2
	if(val > 7500)
	{	
		cmd_str.Format("*CLS");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		cmd_str.Format("DIAG:HW:BAND 22.0e6");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		if(VI_SUCCESS != viSetAttribute((ViSession)(Session+1),1073676314,5000)) 
			return(_VI_ERROR);
		
		cmd_str.Format("DIAG:HW:FEA:RANG %d",(rang+1));
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		cmd_str.Format("SYSTem:ERRor?");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		memset(buf, 0, 256*sizeof(char));
		if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
			return(_VI_ERROR);

		return(VI_FALSE);
	}	

//CentFreq + 22000000;
	temp_ferq = CentFreq + 22000000;
	cmd_str.Format("DIAG:HW:FEA:FREQ %lf",temp_ferq);
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);







	

	cmd_str.Format("*CLS");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:BAND 22.0e6");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	if(VI_SUCCESS != viSetAttribute((ViSession)(Session+1),1073676314,5000)) 
		return(_VI_ERROR);
	
	cmd_str.Format("DIAG:HW:FEA:RANG %d",(rang-5));
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);







// 33
	cmd_str.Format("DIAG:HW:DAP:READ:MISC:APOW? 1");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);
	
	
	cmd_str.Format("DIAG:HW:DAP:ACQ:ADC:OVERrange?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);
	
	
	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);

	answer_str.Empty();
	answer_str = buf;
	ret = answer_str.Find("+411,\"ADC overrange\"");//answer_str.Find("+0,\"No error\"");
// 1
	if(ret != -1) 
	{	
		cmd_str.Format("*CLS");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		cmd_str.Format("DIAG:HW:BAND 22.0e6");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		if(VI_SUCCESS != viSetAttribute((ViSession)(Session+1),1073676314,5000)) 
			return(_VI_ERROR);
		
		cmd_str.Format("DIAG:HW:FEA:RANG %d",(rang+1));
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		cmd_str.Format("SYSTem:ERRor?");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		memset(buf, 0, 256*sizeof(char));
		if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
			return(_VI_ERROR);

		return(VI_FALSE);
	}


	cmd_str.Format("*CLS");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:LATC:SEL \"DAP ADC Max Abs Val\"");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	cmd_str.Format("DIAG:HW:LATC:VAL?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);
	
	answer_str.Empty();
	answer_str = buf;
	ret_head = answer_str.Find("+") + 1;// 
	ret_end = answer_str.Find("\n");// 
	//十进制位数
	temp = ret_end - ret_head;
	val = 0;
	for(i=0;i<temp;i++)
	{	
		val = val * 10;
		val += hexCharToValue(buf[ret_head + i]);
	}
	
// 2
	if(val > 7500)
	{	
		cmd_str.Format("*CLS");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		cmd_str.Format("DIAG:HW:BAND 22.0e6");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		if(VI_SUCCESS != viSetAttribute((ViSession)(Session+1),1073676314,5000)) 
			return(_VI_ERROR);
		
		cmd_str.Format("DIAG:HW:FEA:RANG %d",(rang+1));
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		cmd_str.Format("SYSTem:ERRor?");
		if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
			return(_VI_ERROR);
		
		memset(buf, 0, 256*sizeof(char));
		if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
			return(_VI_ERROR);

		return(VI_FALSE);
	}	


//CentFreq;
	cmd_str.Format("DIAG:HW:FEA:FREQ %lf",CentFreq);
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char));
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);



	return(VI_TRUE);
}



int N4010A_DEV::run_11a_g(double freq,int &range)
{
	CString cmd_str;
	unsigned long actual = 0;
	//unsigned long attribute = 0;
	//unsigned long attrState = 0;
	int rang_max = 25;
	int rang_min = 25;
	int rang_temp = 0;
	int temp_flag = 0;
	int max_flag = -1;
	int min_flag = -1;
	int i=0;
	char buf[256];
	CString answer_str;
	int len = 256;
	int ret = 0;
	double level = 0;


	CentFreq = freq;
	cmd_str.Format("*CLS");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);	
	
	cmd_str.Format("DIAG:HW:FEA:FREQ %lf",freq);
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
	return(_VI_ERROR);
	
	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:BAND 40.0e6");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("*CLS");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);


	
// 1找到("+0,\"No error\"")和("+120,\"Receiver over range\"")	
	set_rang(rang_min,min_flag);		
	if(min_flag == 1)
	{	
		do{
			rang_max += 20;
			set_rang(rang_max,max_flag);
		}while(max_flag != 0);
	}
	else
	{	rang_max = 25;
		max_flag = 0;
		do{
			rang_min -= 20;
			set_rang(rang_min,min_flag);
		}while(min_flag != 1);
	}		
// 2缩小范围，获得相邻的两值	//获取最大rang_min值和最小rang_max	
	do{
		rang_temp = rang_min + (rang_max - rang_min)/2;
		set_rang(rang_temp,temp_flag);
		if(temp_flag == 1)
		{
			rang_min = rang_temp;
		}
		else
		{
			rang_max = rang_temp;
		}

	}while(rang_max - rang_min > 1);


// 3
	cmd_str.Format("*CLS");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	cmd_str.Format("*CLS");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:BAND 22.0e6");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	cmd_str.Format("DIAG:HW:DAP:DEC 1");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	if(VI_SUCCESS != viSetAttribute((ViSession)(Session+1),1073676314,5000)) 
		return(_VI_ERROR);


	rang_temp = rang_min;
	cmd_str.Format("DIAG:HW:FEA:RANG %d",rang_temp);
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	memset(buf, 0, 256*sizeof(char)); 
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:ACQ:TIME 0.00519218");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:TRIG:SOUR IMMediate");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:TRIG:LEVel -23.9791001300806");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:TRIG:DELay 0");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:TRIG:IDLE 1E-06");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:MEAS:RESULTS 0,0");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:MODE generic,off");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:MEAS:RESULTS 1,1");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

// 4		
	do{
		rang_temp -= 10;
	}while(set_rang2(rang_temp) == VI_FALSE);
	rang_temp += 10;

// 5
	cmd_str.Format("*CLS");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:BAND 22.0e6");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	if(VI_SUCCESS != viSetAttribute((ViSession)(Session+1),1073676314,5000)) 
		return(_VI_ERROR);

	rang_temp ++;

	cmd_str.Format("DIAG:HW:FEA:RANG %d",rang_temp);
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char)); 
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);


	cmd_str.Format(":DIAG:HW:DAP:TRIG:SOUR BURSt");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:TRIG:SLOPe POS");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

// 6
	level = rang_temp-30;
	while(set_rang3(level, rang_temp)==VI_FALSE)
	{
		level ++;
		rang_temp++;
	}
	rang_temp--;


// 7
	cmd_str.Format("*CLS");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:BAND 22.0e6");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	cmd_str.Format("DIAG:HW:DAP:DEC 1");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	if(VI_SUCCESS != viSetAttribute((ViSession)(Session+1),1073676314,5000)) 
		return(_VI_ERROR);

	rang_temp ++;

	cmd_str.Format("DIAG:HW:FEA:RANG %d",rang_temp);
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char)); 
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:ACQ:TIME 0.00519218");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);


	cmd_str.Format("DIAG:HW:DAP:TRIG:SOUR BURSt");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:TRIG:SLOPe POS");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);


	cmd_str.Format("DIAG:HW:DAP:TRIG:LEVel %lf",level);
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:TRIG:DELay 0");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:TRIG:IDLE 1E-06");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);
	
	cmd_str.Format("DIAG:HW:DAP:MODE generic,off");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:FORM:BORD NORM");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:DEC 10");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:MEAS:RESULTS 0,0");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char)); 
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:READ:GEN:BBIQ? 1");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	if(VI_SUCCESS != viSetAttribute((ViSession)(Session+1),1073676344,0)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char)); 
	if(VI_SUCCESS_MAX_CNT != viRead((ViSession)(Session+1),(ViBuf)buf,1,&actual)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char)); 
	if(VI_SUCCESS_MAX_CNT != viRead((ViSession)(Session+1),(ViBuf)buf,1,&actual)) 
		return(_VI_ERROR);	
	
	memset(buf, 0, 256*sizeof(char)); 
	if(VI_SUCCESS_MAX_CNT != viRead((ViSession)(Session+1),(ViBuf)buf,6,&actual)) 
		return(_VI_ERROR);
	
	char testbuf[415369];
	memset(testbuf, 0, 415369*sizeof(char)); 
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)testbuf,415369,&actual)) 
		return(_VI_ERROR);

	if(VI_SUCCESS != viFlush((ViSession)(Session+1),4)) 
		return(_VI_ERROR);


	if(VI_SUCCESS != viSetAttribute((ViSession)(Session+1),1073676344,1)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:ACQ:ADC:OVERrange?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char)); 
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);	

	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char)); 
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:DEC 1");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:HW:DAP:DEC 1");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("*CLS");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("DIAG:FPAN:STAT:CLEAR");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	memset(buf, 0, 256*sizeof(char)); 
	if(VI_SUCCESS != viRead((ViSession)(Session+1),(ViBuf)buf,len,&actual)) 
		return(_VI_ERROR);
	
	range = rang_temp;

	return(VI_SUCCESS);
}
//*/
int N4010A_DEV::setCentFre(double freq)
{
	//if(viPrintf(Session,"SENS:FREQ:CENT %lf\n",freq) != VI_SUCCESS) return(false);
	
	CString cmd_str;
	unsigned long actual = 0;
	
	cmd_str.Format("*CLS");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);	
	
	CentFreq = freq;

	cmd_str.Format("DIAG:HW:FEA:FREQ %lf",freq);
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
	return(_VI_ERROR);
	
	cmd_str.Format("SYSTem:ERRor?");
	if(VI_SUCCESS != viWrite((ViSession)(Session+1),(ViBuf)cmd_str.GetBuffer(0),cmd_str.GetLength(),&actual)) 
		return(_VI_ERROR);

	return(VI_SUCCESS);
}

bool N4010A_DEV::setBand(double band)	//unit	MHz		1.5/3/5
{
	if(viPrintf(Session,"SENS:BAND %lf\n",band) != VI_SUCCESS) return(false);
	return(true);
}

bool N4010A_DEV::setInputPow(double pow)	//unit	dbm
{
	if(viPrintf(Session,"SENS:POW:RANG %lf\n",pow) != VI_SUCCESS) return(false);
	return(true);
}

bool N4010A_DEV::setAcqTime(double time)	//unit	us
{
	if(viPrintf(Session,"SENS:SWE:TIME %lf\n",time) != VI_SUCCESS) return(false);
	return(true);
}

bool N4010A_DEV::setInstMode(unsigned char mode)// 0-Link	1-RF Analyzer	2-RF Generator
{
	if(mode==0)
	{
		if(viPrintf(Session,"INST \"LINKG\"\n") != VI_SUCCESS) return(false);

	}
	else if(mode==1)
	{
		if(viPrintf(Session,"INST \"RFA\"\n") != VI_SUCCESS) return(false);
		
	}
	else if(mode==2)
	{
		if(viPrintf(Session,"INST \"RFG\"\n") != VI_SUCCESS) return(false);
		
	}

	return(true);
}

bool N4010A_DEV::ReadRFFre(double &freqoffset)
{
	bool rValue = true;
	ViStatus status;

	//Sleep(1000);
//viQueryf(GPIB地址，要写的命令，读出来的值的格式，读出来的值放的地方)
//如要读电源的电压电流值：
//status = viQueryf(m_vi[i],"MEAS:VOLT?\n","%lf",&MasVoltage);
//status = viQueryf(m_vi[i],"MEAS:CURR?\n","%lf",&MeasCurrent);
//m_vi[i] 表示仪器的地址，是ViSession型 是VISA自带的参数类型，第二个参数是读电压的命令，可以在NI工具里直接下给仪器的命令，第三个参数是读出来的值的格式，第四个是读出来的值的存放的地方，变量一般定义成double型。	

	if((status=viQueryf(Session,"FETC:FOFF?\n","%lf",&freqoffset)) != VI_SUCCESS) {
		CString par;
		par.Format("%ld",status);
		if(status == VI_ERROR_TMO)
			AfxMessageBox("Time out");
		else
			AfxMessageBox(par);
		freqoffset = 0.0;
		rValue = false;
	}
	return(rValue);
}

bool N4010A_DEV::ReadRFAPower(double &pow)
{
	bool rValue = true;
	ViStatus status;

	//Sleep(1000);
	if((status=viQueryf(Session,"FETC:APOW?\n","%lf",&pow)) != VI_SUCCESS) {
		CString par;
		par.Format("%ld",status);
		if(status == VI_ERROR_TMO)
			AfxMessageBox("Time out");
		else
			AfxMessageBox(par);
		pow = 0.0;
		rValue = false;
	}
	return(rValue);
}

bool N4010A_DEV::StartRFTX(int ch,double power)
{
	bool rValue = true;
	int32 error = -1;

	setInstMode(2);

	if(viPrintf(Session,"MMEM:DEL:NAME \"BT4_STD.WF1\",\"WFM 1:\"\n") != VI_SUCCESS) {
		rValue = false;
	}
	if(viPrintf(Session,"MMEM:DATA \"WFM1:BT4_STD.WF1\", #526260\n") != VI_SUCCESS) {
		rValue = false;
	}
	if(viPrintf(Session,"MMEM:DEL:NAME \"BT4_Seq\",\"SEQ:\"\n") != VI_SUCCESS) {
		rValue = false;
	}
	if(viPrintf(Session,"MMEM:DATA \"SEQ:BT4_Seq\", #3364\n") != VI_SUCCESS) {
		rValue = false;
	}
	if(viPrintf(Session,"WFM1:BT4_STD.wf1 1000\n") != VI_SUCCESS) {
		rValue = false;
	}
	if(viPrintf(Session,"OUTP OFF\n") != VI_SUCCESS) {
		rValue = false;
	}
	if(viPrintf(Session,"SOUR:POW:LEV:IMM:AMPL %lf\n",power) != VI_SUCCESS) {
		rValue = false;
	}
	if(viPrintf(Session,"SOUR:FREQ:FIX %d\n",2402000000+ch*2000000) != VI_SUCCESS) {
		rValue = false;
	}
	if(viPrintf(Session,"SOUR:RAD:ARB:CLOCK:SRAT 10000000\n") != VI_SUCCESS) {
		rValue = false;
	}
	if(viPrintf(Session,"SOUR:RAD:ARB:WAVE \"SEQ:BT4_Seq\"\n") != VI_SUCCESS) {
		rValue = false;
	}
	if(viPrintf(Session,"SOUR:RAD:ARB:STAT ON\n") != VI_SUCCESS) {
		rValue = false;
	}
	if(viPrintf(Session,"SOUR:POW:LEV:IMM:AMPL -60\n") != VI_SUCCESS) {
		rValue = false;
	}
	if(viPrintf(Session,"SOUR:RAD:ARB:TRIG:TYPE SING\n") != VI_SUCCESS) {
		rValue = false;
	}
	if(viPrintf(Session,"OUTP ON\n") != VI_SUCCESS) {
		rValue = false;
	}
	if(viPrintf(Session,"SOUR:RAD:ARB:TRIG:SOFT\n") != VI_SUCCESS) {
		rValue = false;
	}
	Sleep(100);
	while(1)
	{
		if(viQueryf(Session,"STAT:OPER:COND?\n","%d",&error) != VI_SUCCESS) {
			rValue = false;
		}
		if(error==0)
			break;
	}
	
	if(viPrintf(Session,"OUTP OFF\n") != VI_SUCCESS) {
		rValue = false;
	}
	if(viPrintf(Session,"SOUR:RAD:ARB:WAVE \"WFM1:SINE_TEST_WFM\"\n") != VI_SUCCESS) {
		rValue = false;
	}
	if(viPrintf(Session,"SOUR:RAD:ARB:TRIG:TYPE CONT\n") != VI_SUCCESS) {
		rValue = false;
	}
	if(viPrintf(Session,"DIAG:FPAN:STAT:CLEAR\n") != VI_SUCCESS) {
		rValue = false;
	}
	return(rValue);
}

//=================================================================================================
//=================================================================================================
RTL87XX_COM::RTL87XX_COM(const CString &Com):CComDevice(Com,"","rtl8711")
{
	//max_time = 1000;
	#if 0//yhl
	port.key.Baud = 38400;
	#else
	port.key.Baud = 115200;
	#endif
	//uint8 state = 0;//tx=1;rx=2;idle=0;
	//com_baud = 9600;
	//com_stop_bit = ONESTOPBIT;
	//com_parity = NOPARITY;////EVENPARITY 偶校验 NOPARITY 无校验//MARKPARITY 标记校验 ODDPARITY 奇校验
	//com_byte_size = 8;
	//UM0155_page9
	//测试完成后更新的数据，需写入rtl8711
	uint8 x_tal = 32;
	//0x2B地址值
	uint8 tx_pwr_index_difference = 2;
	
	uint8 tx_pwr_l = 42;
	uint8 tx_pwr_m = 43;
	uint8 tx_pwr_h = 44;
	
	uint8 channel_plan = 0x20;
	uint8 thermal_meter = 0x1A;
}

//yhl 2018/10/28
bool RTL87XX_COM::DevValid(int high_low)
{
  	CString cmd_str;
	char rx_buf[200] = {0};
	//CString mac_str="78da07c2de87	vendorId:7241	 secret:92d3e6899f6c5fbb3b3801c1df148274	pk:200";
	#if 0//yhl
	cmd_str.Format("help\r\n");
	if(!send_and_search(cmd_str, 0, "WLAN AT COMMAND SET", 0))
	{
		return(false);	
	}
	else		
	{
		return (true);
	}
	#else
	//parse_mac_txt();
	//testtime++;
	//cmd_str.Format(&mac_send[0]);
	//cmd_str.Format("+R\r");
		//esp_send_mac();
	//cmd_str.Format(&mac_send[0]);
	//cmd_str.Format("+R\r");
	cmd_str.Format("print_ver\r");
	if(!send_and_search(cmd_str, 0, "v", 0))//可用
	{
		return(false);	
	}
	else		
	{
		return (true);
	}
	#endif
}
//烧录时和AT命令控制时候的波特率为38400
//-------------------------------------------------------------------------------------------------
/*
bool RTL87XX_COM::connect(const CString &dName, int port_baud, int Parity)
{
	if(!port_maped()) {
		CString com_temp;
		for(int i=0;i<COM_SOURCEMAX;i++) {
			if(!is_com_index_can_used(i)) continue;
			if(port.key.is_probe_name_null() &&
			   port.key.is_port_name_null() ) continue;
			if(port.key.is_probe_name_null() && 
			   (port.key.PortName != COM_Port_Name[i])) continue;
			
			com_temp = COM_Port_Name[i];
			if(!openCom(com_temp,port_baud,Parity)) continue;
			if(port.key.is_probe_name_null() ) {
				port.key.PortName = COM_Port_Name[i];
				port.key.maping = true;
				port.addToList(list_head);
				break;
			}
			CloseHandle(hCom);
			hCom = INVALID_HANDLE_VALUE;
		}
		return(port.key.maping);
	}

	if((hCom == INVALID_HANDLE_VALUE) &&
	   !openCom(port_name(),port_baud,Parity)) return(false);
	return(true);
}
*/
/*
bool RTL87XX_COM::connect(const CString &dName, int high_low, int port_baud,int Parity)
{
	if(!port_maped()) {
		CString com_temp;
		for(int i=0;i<COM_SOURCEMAX;i++) {
			if(!is_com_index_can_used(i)) continue;
			if(port.key.is_probe_name_null() &&
			   port.key.is_port_name_null() ) continue;
			if(port.key.is_probe_name_null() && 
			   (port.key.PortName != COM_Port_Name[i])) continue;
			
			com_temp = COM_Port_Name[i];
			if(!openCom(com_temp,port_baud,port_parity())) continue;
			if(port.key.is_probe_name_null() || DevValid(high_low)) {
				port.key.PortName = COM_Port_Name[i];
				port.key.maping = true;
				port.addToList(list_head);
				break;
			}
			CloseHandle(hCom);
			hCom = INVALID_HANDLE_VALUE;
		}
		return(port.key.maping);
	}

	if((hCom == INVALID_HANDLE_VALUE) &&
	   !openCom(port_name(),port_baud,port_parity())) return(false);
	return(DevValid(high_low));
}
*/
//-------------------------------------------------------------------------------------------------


//=================================================================================================
//=================================================================================================
CTBOX::CTBOX(const CString &Com):CComDevice(Com,"","test box")
{
        max_time = pCfgf.Vulong(ParTestBoxMaxTime);
		//CComDevice::DevSourceMap();
}

bool CTBOX::DevValid(int)
{
        //bool st;

        //if(get_st(st)) return(true);
        //return(false);
       char rx_buf[32] = {0};
       CString temp;
	   if(!send_and_get("STATUS\r\n",0,rx_buf,0,max_time)) return(false);
       temp = rx_buf;
       if((temp.Find("TOK XOK") >= 0) 
		   || (temp.Find("TCLOSE XCLOSE") >= 0) 
		   || (temp.Find("TOK XCLOSE") >= 0) 
		   || (temp.Find("TCLOSE XOK") >= 0))  return(true);
	   return(false);
}

bool CTBOX::connect(void)
{
        //char rx_buf[32] = {0};
        //CString temp;

        if(!CComDevice::connect("", 1)) return(false);
        //if(!send_and_get("STATUS",0,rx_buf,0,max_time)) return(false);
        //temp = rx_buf;
        //if((temp.Find("OPEN") >= 0)  return(true);
			//&& !send_and_get("Handler Mode Off",0,rx_buf,0,max_time)) return(false);
        //temp = rx_buf;
        //if((temp.Find("OK") >= 0)) return(true);
	return(true);
}



bool CTBOX::get_st(bool &is_open)
{
        char rx_buf[32] = {0};
        CString temp;

        if(!send_and_get("STATUS\r\n",0,rx_buf,0,max_time)) return(false);
        temp = rx_buf;
        //if((temp.Find("ERR") >= 0) && !send_and_get("STATUS",0,rx_buf,0,max_time)) return(false);
        //temp = rx_buf;

        if(temp.Find("TOK XOK") >= 0) is_open = true;
        else if(temp.Find("TCLOSE XCLOSE") >= 0) is_open = false;
        else return(false);
        return(true);
}


bool CTBOX::open_box(void)
{  
        bool st = 0;
        if(!get_st(st)) return(false);
        if(st == true) return(true);

        CString temp;
        char rx_buf[32] = {0};
        if(!send_and_get("TXOPEN\r\n",0,rx_buf,0,max_time)) return(false);
        temp = rx_buf;
        if(!(temp.Find("TOK XOK") >= 0)) return(false);

        if(!get_st(st) || !st) return(false);
	return(true);
}

bool CTBOX::close_box(void)
{
        bool st = 0;
        if(!get_st(st)) return(false);
        if(st == false) return(true);

        CString temp;
        char rx_buf[32] = {0};
        if(!send_and_get("TXCLOSE\r\n",0,rx_buf,0,max_time)) return(false);
        temp = rx_buf;
        if(!(temp.Find("TCLOSE XCLOSE") >= 0)) return(false);

        if(!get_st(st) || st) return(false);
	return(true);
}

//=================================================================================================
//=================================================================================================
static DWORD WINAPI MultimeterPro(LPVOID PMultimeter);

CMultimeter::CMultimeter(int Com):port("","Multimeter","")
{
        com = Com;
        com_allocation = false;
        hCom = false;
        pWinThread = NULL;
        measure_type = Multimeter_invalid;
        id_is_ok = false;

        if(com < COM_SOURCEMAX)
        { 
                port.key.PortName = COM_Port_Name[Com];
                com_allocation = true;
        }
        port.key.Baud = 19200;
	    port.key.Parity = ODDPARITY;

        InitializeCriticalSection(&g_cs);
}

CMultimeter::~CMultimeter()
{
        if(pWinThread)
        {
                DWORD lpExitCode = 0;

                thread_running = 0;
                for(int i=0;i<20;i++)
                {
                        if(!pWinThread || !GetExitCodeThread(pWinThread->m_hThread, &lpExitCode)) break;
                        if(lpExitCode != STILL_ACTIVE) break;
                        Sleep(100);
                }
        }
        disconnect();
        DeleteCriticalSection(&g_cs);
}

//-------------------------------------------------------------------------------------------------
bool CMultimeter::openCom(void)
{
	CString use_com = port.key.PortName;
        if(!com_allocation) return(false);
	if(use_com.GetLength() > 4) use_com = "\\\\.\\" + use_com;

	hCom = CreateFile(use_com,				// 串口名称或设备路径
			  GENERIC_READ|GENERIC_WRITE,		// 读写方式
			  0,					// 共享方式：独占
			  NULL,					// 默认的安全描述符
			  OPEN_EXISTING,			// 创建方式
			  0,					// 不需设置文件属性
			  NULL					// 不需参照模板文件
			  );
	if(hCom == INVALID_HANDLE_VALUE) return(false);		// 打开串口失败

	DCB dcb;						// 串口控制块
	COMMTIMEOUTS timeouts = {				// 串口超时控制参数
		0,						// 读字符间隔超时时间:不启用
		4,						// 读操作时每字符的时间: 4 ms (n个字符总共为n ms)
		3000,						// 基本的(额外的)读超时时间: 3000 ms
		2,						// 写操作时每字符的时间: 2 ms (n个字符总共为n ms)
		1000						// 基本的(额外的)写超时时间: 1000ms
	};
	
	GetCommState(hCom,&dcb);				// 取DCB

	dcb.fOutxCtsFlow = FALSE;				// 禁止硬流控, 因为本系统不需要 
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE; 
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fAbortOnError = false;

	dcb.BaudRate = port.key.Baud;
	dcb.ByteSize = 7;
	dcb.Parity = port.key.Parity;
	dcb.StopBits = ONESTOPBIT;
	dcb.fBinary = true; 
	SetCommState(hCom,&dcb);				// 设置DCB

	SetupComm(hCom,128,128);				// 设置输入输出缓冲区大小
	SetCommTimeouts(hCom,&timeouts);			// 设置超时

	DWORD dwError;						// ie:CE_RXOVER
   	COMSTAT cs;
	ClearCommError(hCom,&dwError,&cs);
	PurgeComm(hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);
	return(true);
}

//-------------------------------------------------------------------------------------------------
bool CMultimeter::recive_by_length(char *str,DWORD &len)
{
	if(hCom == INVALID_HANDLE_VALUE) return(false);
	if(!str) return(false);

	DWORD dwError;
	COMSTAT ComStat;
	if(::ClearCommError(hCom,&dwError,&ComStat) && (dwError >0)) 
        {
		if(ComStat.cbInQue >= 100) 
                {
			::PurgeComm(hCom,PURGE_RXCLEAR);
			return(false);
		}
	}

	DWORD dwNumRead = 0;	// 串口已接收的数据长度
        if(!ReadFile(hCom,str,len,&dwNumRead,NULL))
        {
                len = 0;
                return(false);
        }
        if(dwNumRead) 
        {
                len = dwNumRead;
                return(true);
        }
        return(false);
}

bool CMultimeter::connect(void)
{
        if(!openCom()) return(false);

        thread_running = true;
        pWinThread = AfxBeginThread((AFX_THREADPROC)MultimeterPro,(LPVOID)this);
        return(true);
}

void CMultimeter::disconnect(void)
{
        //port.dependFromList(CComDevice::list_head);
        if(INVALID_HANDLE_VALUE != hCom)
		{
			port.dependFromList(CComDevice::list_head);
			CloseHandle(hCom);
		}
	    hCom = INVALID_HANDLE_VALUE;
}



int CMultimeter::analyze(char *srt)
{
        if((srt[12] != 0x0D) || (srt[13] != 0x0A)) return(Multimeter_invalid);

        char fu = srt[6];
        char range = srt[0];
        char stau = srt[7];
        char option3 = srt[10];
        char option4 = srt[11];

        // DC & AUTO
        if(!(option3 & 0x08)) return(Multimeter_invalid);
        unsigned long ltemp = srt[1]&0x0f;
        ltemp = (ltemp*10) + (srt[2]&0x0f);
        ltemp = (ltemp*10) + (srt[3]&0x0f);
        ltemp = (ltemp*10) + (srt[4]&0x0f);
        ltemp = (ltemp*10) + (srt[5]&0x0f);

        double data = ltemp;
        int temp = Multimeter_invalid;
        // voltage sw to mv
        if(fu == 0x3B)
        {
                temp = Multimeter_voltage;
                if(stau & 0x01) data = MULMETER_OVERFLOW;                        //overflow, ;
                else if(range == 0x30) data /= 10;
                else if(range == 0x31) data = data;
                else if(range == 0x32) data = data*10;
                else if(range == 0x33) data = data*100;
                else if(range == 0x34) data = data/100;
                else temp = Multimeter_invalid;
        }
        // auto uA current --> sw to uA
        else if((fu == 0x3D) && (option3 & 0x02))                               //only auto allow, =
        {
                temp = Multimeter_current;
                if(option4 & 0x04) temp = Multimeter_invalid;                   //A not ok now
                else if(stau & 0x01) data = MULMETER_OVERFLOW;                  //overflow
                else if(range == 0x30) data /= 1000;
                else if(range == 0x31) data /= 100;
                else temp = Multimeter_invalid;
        }
        // auto mA current --> sw to mA
        else if((fu == 0x3F) && (option3 & 0x02))                               //only auto allow, ?
        {
                temp = Multimeter_current;
                if(option4 & 0x04) temp = Multimeter_invalid;                   //A not ok now
                else if(stau & 0x01) data = MULMETER_OVERFLOW;                  //overflow
                else if(range == 0x30) data = data;
                else if(range == 0x31) data = data*10;
                else temp = Multimeter_invalid;
        }
		

        if(stau & 0x04) data = -data;
        if(temp != Multimeter_invalid)
        {
                EnterCriticalSection(&g_cs);
                {
                        tvalue = data;
                        measure_type = temp;
                }
                LeaveCriticalSection(&g_cs);
        }
        return(temp);
}

//bool CMultimeter::get_voltage(double &data)
bool CMultimeter::get_voltage(double *data)
{
        if(!pWinThread || !thread_running) return(false);
        EnterCriticalSection(&g_cs);
        measure_type = Multimeter_invalid;
        LeaveCriticalSection(&g_cs);

        for(int i=0;i<20;i++)
        {
                if(!thread_running) return(false);

                EnterCriticalSection(&g_cs);
                if(Multimeter_voltage == measure_type)
                { 
                        LeaveCriticalSection(&g_cs);
                        *data = tvalue;
                        return(true);
                }
                LeaveCriticalSection(&g_cs);

                Sleep(100);
                continue;
        }
        return(false);
}

bool CMultimeter::get_current(double &data)
{
        if(!pWinThread || !thread_running) return(false);
        EnterCriticalSection(&g_cs);
        measure_type = Multimeter_invalid;
        LeaveCriticalSection(&g_cs);

        for(int i=0;i<20;i++)
        {
                if(!thread_running) return(false);

                EnterCriticalSection(&g_cs);
                if(Multimeter_current == measure_type)
                { 
                        LeaveCriticalSection(&g_cs);
                        data = tvalue;
                        return(true);
                }
                LeaveCriticalSection(&g_cs);

                Sleep(100);
                continue;
        }
        return(false);
}

static DWORD WINAPI MultimeterPro(LPVOID PMultimeter)
{
        CMultimeter *Multimeter = (CMultimeter*)PMultimeter;
        DWORD len = 0,rlen;
        char data[32] = {0};
        CString temp;

        len = 14;
        bool res = Multimeter->recive_by_length(data,len);
        if(res)
        { 
                res = false;
                for(DWORD i=0;i<len-1;i++)
                {
                        if((data[i] == 0x0D) && (data[i+1] == 0x0A)) 
                        {
                                res = true;
                                break;
                        }
                }
        }
        if(!res)
        { 
                Multimeter->disconnect();
                Multimeter->thread_running = false;
                Multimeter->pWinThread = NULL;
                return(0);
        }
        Multimeter->id_is_ok = true;

        while(Multimeter->thread_running)
        {
                if(!Multimeter->thread_running) break;
                rlen = 14;
                if(!Multimeter->recive_by_length(data,rlen)) 
                {
                        if(pMainWnd) pMainWnd->GetDlgItem(IDC_MultimeterVal)->SetWindowText("万用表未连接");
                        Multimeter->disconnect();
                        Multimeter->thread_running = 0;
                        break;
                }
                if(!Multimeter->thread_running) break;
                if(rlen < 14) continue;

                data[14] = 0;
                int type = Multimeter->analyze(data);
                if(Multimeter_invalid != type)
                {
                        temp.Format("万用表:%.3lf %s", Multimeter->tvalue,(type == Multimeter_voltage)?"mv":"uA");
                        if(pMainWnd) pMainWnd->GetDlgItem(IDC_MultimeterVal)->SetWindowText(temp);
                }
        }

        Multimeter->pWinThread = NULL;
        return(0);
}

CMultimeter *getMultimeterDev(void)
{
        int try_com = 0;
        int max_try_com = pCfgf.Vint(ParMulTryCOMMum);
        if(max_try_com > 16) max_try_com = 16;
        CMultimeter *dev[16] = { 0 };

        CComDevice::DevSourceMap();
        for(int i=0;i<COM_SOURCEMAX;i++)
        {
            if(!CComDevice::is_com_index_can_used(i)) continue;
            dev[try_com] = new CMultimeter(i);
            dev[try_com]->connect();

            try_com += 1;
            if(try_com >= max_try_com) break;
        }

        bool have_com_run;
        CMultimeter *id_dev = NULL;
        for(int j=0;j<50;j++)
        {
                have_com_run = false;
                for(i=0;i<try_com;i++)
                {
                        if(dev[i])
                        { 
                                if(!dev[i]->pWinThread) 
                                {
                                        delete dev[i];
                                        dev[i] = NULL;        
                                }
                                else if(!dev[i]->id_is_ok || id_dev) have_com_run = true;
                                else
                                {
                                        id_dev = dev[i];
                                        dev[i] = NULL;
                                        for(int k = 0; k < try_com; k++) if(dev[k]) dev[k]->thread_running = false;
                                }
                        }
                }
                if(!have_com_run) break;
                Sleep(100);
        }

        if(id_dev)
        { 
                id_dev->port.key.maping = true;
                id_dev->port.addToList(CComDevice::list_head);
        }
        return(id_dev);
}


#if 0

//=================================================================================================
// CSR use this to communictaion with the MCU to set IO for audio test/PIO test
//=================================================================================================
#include"ac6652_lib.h"
#pragma comment (lib,"ac6652.lib")

#define PIO_RF_A_B_MASK                 (1<<0)
#define PIO_RF_A                        0
#define PIO_RF_B                        PIO_RF_A_B_MASK

#define PIO_RF_MT8852_FOFFSET_MASK      (1<<1)
#define PIO_RF_FOR_MT8852               PIO_RF_MT8852_FOFFSET_MASK
#define PIO_RF_FOR_FOFFSET              0

#define PIO_AUDIO_A_B_MASK              (1<<2)
#define PIO_AUDIO_FOR_A                 PIO_AUDIO_A_B_MASK
#define PIO_AUDIO_FOR_B                 0

#define PIO_PWR_VBUS_BAT_MASK           (1<<3)
#define PIO_PWR_FOR_VBUS                PIO_PWR_VBUS_BAT_MASK
#define PIO_PWR_FOR_BAT                 0

#define PIO_MCUR_A_B_MASK               (1<<4)
#define PIO_MCUR_FOR_A                  PIO_MCUR_A_B_MASK
#define PIO_MCUR_FOR_B                  0

#define PIO_PWR_AG_MASK                 (1<<6)
#define PIO_PWR_AG_ON                   PIO_PWR_AG_MASK
#define PIO_PWR_AG_OFF                  0

#define PIO_PWR_BG_MASK                 (1<<7)
#define PIO_PWR_BG_ON                   PIO_PWR_BG_MASK
#define PIO_PWR_BG_OFF                  0

#define SET_PIO(m,d) do {               \
        if(d) DO_L0_7 = DO_L0_7|m;      \
        else DO_L0_7 = DO_L0_7 & (~m);  \
}while(0)

CIOCARD::CIOCARD(void)
{
        hd6652 = NULL;

        reset_golden = true;
        pwr_is_vbus = true;
        pwr_source_is_vbus = true;

        //A RF offset test,audio for A,VBus power,A current measure,A gloden power off,B gloden power off
        DO_L0_7 = PIO_RF_A|PIO_RF_FOR_FOFFSET|PIO_PWR_FOR_VBUS|PIO_MCUR_FOR_A|PIO_PWR_AG_OFF|PIO_PWR_BG_OFF;
        DO_H8_15 = 0;
}

CIOCARD::~CIOCARD()
{
        if(hd6652) AC6652_CloseDevice(hd6652);
        hd6652 = NULL;
}

int CIOCARD::connect(void)
{
        pwr_source_is_vbus = pCfgf.ItemSelect(parPwrByVBus);
        if(hd6652) return(2);

        hd6652 = AC6652_OpenDevice(0);
	if(hd6652 == INVALID_HANDLE_VALUE)
	{
                hd6652 = NULL;
                return(0);
        }
        shutdown_dut_pwr();
        return(1);
}

bool CIOCARD::set_route_vbus(void)
{
        if(!pMainWnd->have_iocard_cfj) return(true);
        if(!pMainWnd->glbResource_get(GRESOURCE_GPIO_CARD)) return(false);
        if(!pwr_is_vbus) 
        {
                SET_PIO(PIO_PWR_VBUS_BAT_MASK,PIO_PWR_FOR_VBUS);
                AC6652_DO(hd6652,0,DO_L0_7);
                pwr_is_vbus = true;
        }
        pMainWnd->glbResource_put(GRESOURCE_GPIO_CARD);
        return(true);
}

bool CIOCARD::set_route_bat(void)
{
        if(!pMainWnd->have_iocard_cfj) return(true);
        if(!pMainWnd->glbResource_get(GRESOURCE_GPIO_CARD)) return(false);
        if(pwr_is_vbus) 
        {
                SET_PIO(PIO_PWR_VBUS_BAT_MASK,PIO_PWR_FOR_BAT);
                AC6652_DO(hd6652,0,DO_L0_7);
                pwr_is_vbus = false;
        }
        pMainWnd->glbResource_put(GRESOURCE_GPIO_CARD);
        return(true);
}

bool CIOCARD::open_dut_pwr(void)
{
        if(pwr_source_is_vbus) return(set_route_vbus());
        else return(set_route_bat());
}
bool CIOCARD::shutdown_dut_pwr(void)
{
        if(pwr_source_is_vbus) return(set_route_bat());
        else return(set_route_vbus());
}

bool CIOCARD::set_route_mt8852(int dev_id)
{
        if(!pMainWnd->have_iocard_cfj) return(true);
        if(!hd6652) return(false);

        if(!pMainWnd->glbResource_get(GRESOURCE_GPIO_CARD)) return(false);
        if(TEST_DEV_ID_B == dev_id)
        { 
                SET_PIO(PIO_RF_MT8852_FOFFSET_MASK,PIO_RF_FOR_MT8852);
                SET_PIO(PIO_RF_A_B_MASK,PIO_RF_B);
                AC6652_DO(hd6652,0,DO_L0_7);
        }
        else
        {
                SET_PIO(PIO_RF_MT8852_FOFFSET_MASK,PIO_RF_FOR_MT8852);
                SET_PIO(PIO_RF_A_B_MASK,PIO_RF_A);
                AC6652_DO(hd6652,0,DO_L0_7);
        }
        pMainWnd->glbResource_put(GRESOURCE_GPIO_CARD);
        return(true);
}

bool CIOCARD::set_route_foffset(int dev_id)
{
        if(!pMainWnd->have_iocard_cfj) return(true);
        if(!hd6652) return(false);

        if(!pMainWnd->glbResource_get(GRESOURCE_GPIO_CARD)) return(false);
        if(TEST_DEV_ID_B == dev_id)
        { 
                SET_PIO(PIO_RF_MT8852_FOFFSET_MASK,PIO_RF_FOR_FOFFSET);
                SET_PIO(PIO_RF_A_B_MASK,PIO_RF_B);
                AC6652_DO(hd6652,0,DO_L0_7);
        }
        else
        {
                SET_PIO(PIO_RF_MT8852_FOFFSET_MASK,PIO_RF_FOR_FOFFSET);
                SET_PIO(PIO_RF_A_B_MASK,PIO_RF_A);
                AC6652_DO(hd6652,0,DO_L0_7);
        }
        pMainWnd->glbResource_put(GRESOURCE_GPIO_CARD);
        return(true);
}

bool CIOCARD::set_route_audio(int dev_id)
{
        if(!pMainWnd->have_iocard_cfj) return(true);
        if(!hd6652) return(false);

        if(!pMainWnd->glbResource_get(GRESOURCE_GPIO_CARD)) return(false);
        if(TEST_DEV_ID_B == dev_id)
        { 
                SET_PIO(PIO_AUDIO_A_B_MASK,PIO_AUDIO_FOR_B);
                AC6652_DO(hd6652,0,DO_L0_7);
        }
        else
        {
                SET_PIO(PIO_AUDIO_A_B_MASK,PIO_AUDIO_FOR_A);
                AC6652_DO(hd6652,0,DO_L0_7);
        }
        pMainWnd->glbResource_put(GRESOURCE_GPIO_CARD);
        return(true);
}

bool CIOCARD::set_route_current(int dev_id)
{
        if(!pMainWnd->have_iocard_cfj) return(true);
        if(!hd6652) return(false);

        if(!pMainWnd->glbResource_get(GRESOURCE_GPIO_CARD)) return(false);
        if(TEST_DEV_ID_B == dev_id)
        { 
                SET_PIO(PIO_MCUR_A_B_MASK,PIO_MCUR_FOR_B);
                AC6652_DO(hd6652,0,DO_L0_7);
        }
        else
        {
                SET_PIO(PIO_MCUR_A_B_MASK,PIO_MCUR_FOR_A);
                AC6652_DO(hd6652,0,DO_L0_7);
        }
        pMainWnd->glbResource_put(GRESOURCE_GPIO_CARD);
        return(true);
}

bool CIOCARD::set_route_ag_pwr(int dev_id,bool on_off)
{
        if(!pMainWnd->have_iocard_cfj) return(true);
        if(!hd6652) return(false);

        if(!pMainWnd->glbResource_get(GRESOURCE_GPIO_CARD)) return(false);
        if(TEST_DEV_ID_B == dev_id)
        {
                if(on_off) SET_PIO(PIO_PWR_BG_MASK,PIO_PWR_BG_ON);
                else SET_PIO(PIO_PWR_BG_MASK,PIO_PWR_BG_OFF);
                AC6652_DO(hd6652,0,DO_L0_7);
        }
        else
        {
                if(on_off) SET_PIO(PIO_PWR_AG_MASK,PIO_PWR_AG_ON);
                else SET_PIO(PIO_PWR_AG_MASK,PIO_PWR_AG_OFF);
                AC6652_DO(hd6652,0,DO_L0_7);
        }
        pMainWnd->glbResource_put(GRESOURCE_GPIO_CARD);
        return(true);
}

bool CIOCARD::get_value(long &data)
{
        data = 0;
        if(!pMainWnd->have_iocard_cfj) return(true);
        if(!hd6652) return(false);

        data = AC6652_DI(hd6652,0);
        long hdata = AC6652_DI(hd6652,1);
        data += (hdata<<8);
        return(true);
}

#endif
