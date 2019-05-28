class CTObj_wtriad:public CTestItem
{
private:
	CTObj_wtriad& operator = (const CTObj_wtriad &x);
	CTObj_wtriad(CTObj_wtriad &x);

protected:

	CString file;
	CStringList * TriadList;

public:
	CTObj_wtriad(const char* pname, testSet *Tset, int ID, ConfigFile &cfg = pCfgf);
	virtual ~CTObj_wtriad() {};

	bool getline(const char* file,const char* mac,char *content);
	int TestOp(ConfigFile &cfg = pCfgf);
};

CTObj_wtriad::CTObj_wtriad(const char* pname,testSet *Tset, int ID, 
								   ConfigFile &cfg):CTestItem(pname, Tset,ID)
{
	//man_input_type = cfg.ItemSelect(s_InputAddress_manual);
	//default_address_write_only = cfg.ItemSelect(slaveDefAddrWOnly);
//	file="triad.txt";
	file=cfg.VCString(s_triad_file);
	TriadList = new CStringList();
	GetAllFile(TriadList,"TriadFile\\*.txt");
}

bool CTObj_wtriad::getline(const char* file,const char* mac,char *content)
{
	ifstream inFile;
	char buff[256]={0};
	char* p;

	inFile.open(file);
	while( !inFile.eof())
	{
		inFile.getline(buff,255);
		p= strstr(buff,mac);
		if (p)
		{
			strncpy(content,buff,strlen(buff));
			break;
		}

	}
	inFile.close();
	if(strlen(content))
		return true;
	else
		return false;
}


int CTObj_wtriad::TestOp(ConfigFile &cfg)
{
		CDutDevice *pobj=tset->pObj;
        struct bt_address_s* paddr = &tset->addr;
        result = false;
        is_tested = true;
		static const CString BD_ADDR_CS_NAME = "bdaddr";

		
		uint16 bdAddrValue[3];
		bdAddrValue[0] = paddr->lap & 0xffff;
		bdAddrValue[1] = (paddr->uap<<8) | (paddr->lap>>16) ;
		bdAddrValue[2] = paddr->nap;
		uint16 length = 3;
		
		CString strMAC;
		strMAC.Format("%.4x%.2x%.6x",paddr->nap,paddr->uap,paddr->lap);

		int gloden = tset->gloden_idx();
        if(gloden)
        { 
                result = true;
                param = tset->gloden_idx_name();
                return(NO_ERR);
        }

		char *mac;
		mac=(char*)(LPCTSTR)strMAC;

		
#if 1
		file="";
		for(int i=0;i<TriadList->GetCount();i++)
		{
			CString temp=TriadList->GetAt(TriadList->FindIndex(i));
			if(temp.GetLength()==29)
			{
				CString start=temp.Mid(0,12);
				CString end=temp.Mid(13,12);
				long lstart=strtol(start,NULL,16);
				if(start.Mid(0,6)==end.Mid(0,6) && start.Mid(0,6)==strMAC.Mid(0,6) && strtol(strMAC.Mid(6,6),NULL,16)>=strtol(start.Mid(6,6),NULL,16) &&  strtol(strMAC.Mid(6,6),NULL,16)<= strtol(end.Mid(6,6),NULL,16))
				{
					file = GetPathOrFile("TriadFile\\%s",temp);
					break;
				}
			}
		}


	if(file=="")
	{	param = "get file err";
            return(ERR_NOMAL);
	}

#endif

		char buff[256]={0};
		if(!getline(file,mac,buff))
		{
			param = "get triad err";
            return(ERR_NOMAL);
		}
		
		char pid[20]={0};
		char mac_r[20]={0};
		char secret[33]={0};
		char temp[255]={0};
		
		char *iindex=strchr(buff,'\t');
		strncpy(pid,buff,iindex-buff);
		
		strncpy(secret,iindex+1,32);
/*
		if(pobj->ResetToTestMode ()!=0)
		{
			 param = "enter test mode err";
             return(ERR_NOMAL);
		}
*/
		if(pobj->WriteTriad (pid,mac,secret)!=0)
		{
			param="write triad err";
			return(ERR_NOMAL);
		}
/*
		if(pobj->WriteCalResultToFlash (false)!=0)
		{
			param="save err";
			return(ERR_NOMAL);
		}
*/
		//verify
		char rpid[MAX_ARRAY_SIZE]={0};
		char rmac[MAX_ARRAY_SIZE]={0};
		char rsecret[MAX_ARRAY_SIZE]={0};

		if(pobj->ReadTriad (rpid,rmac,rsecret)!=0)
		{
			param="write triad err";
			return(ERR_NOMAL);
		}

		if(strcmp(rpid,pid)!=0)
		{
			param="read pid err";
			return(ERR_NOMAL);
		}

		CString cRsecret=rsecret;
		cRsecret.MakeLower();

		if(strcmp((char*)(LPCTSTR)cRsecret,secret)!=0)
		{
			param="read secret err";
			return(ERR_NOMAL);
		}

        param = "";
        param.Format("%s:%s",pid,secret);
        result = true;

		return(NO_ERR);
}

