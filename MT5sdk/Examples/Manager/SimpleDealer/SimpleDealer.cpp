//+------------------------------------------------------------------+
//|                                                     SimpleDealer |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "Dealer.h"
//+------------------------------------------------------------------+
//| Example usage                                                    |
//+------------------------------------------------------------------+
void Usage(void)
  {
   wchar_t name[_MAX_PATH]=L"",*ptr=NULL;
//--- get module file name
   GetModuleFileNameW(NULL,name,_countof(name));
   ptr=wcsrchr(name,L'\\');
   if(ptr!=NULL) ptr++;
   else          ptr=name;
//--- show usage
   wprintf(L"\nUsage:\n"
           L"  %s /server:address:port /login:login /password:password\n",ptr);
  }
//+------------------------------------------------------------------+
//| Main entry point                                                 |
//+------------------------------------------------------------------+
int32_t wmain(int32_t argc,wchar_t* argv[])
  {
   wchar_t server[256]=L"";
   uint64_t  login=0;
   wchar_t password[64]=L"";
   wchar_t tmp[256]=L"";
//--- show startup banner
#ifdef _WIN64
   wprintf_s(L"SimpleDealer 64 bit\nCopyright 2000-2025, MetaQuotes Ltd.\n\n");
#else
   wprintf_s(L"SimpleDealer 32 bit\nCopyright 2000-2025, MetaQuotes Ltd.\n\n");
#endif
//--- get parameters
   for(int32_t i=1;i<argc;i++)
     {
      if(_wcsnicmp(argv[i],L"/server:",_countof(L"/server:")-1)==0)
        {
         wcscpy_s(server,argv[i]+_countof(L"/server:")-1);
         continue;
        }
      if(_wcsnicmp(argv[i],L"/login:",_countof(L"/login:")-1)==0)
        {
         login=_wcstoui64(argv[i]+_countof(L"/login:")-1,NULL,10);
         continue;
        }
      if(_wcsnicmp(argv[i],L"/password:",_countof(L"/password:")-1)==0)
        {
         wcscpy_s(password,argv[i]+_countof(L"/password:")-1);
         continue;
        }
     }
//--- check and request parameters
   if(server[0]==L'\0')
     {
      wprintf_s(L"Server: ");
      wscanf_s(L"%255s",server,(int)_countof(server));
     }
   if(login==0)
     {
      wprintf_s(L"Login: ");
      wscanf_s(L"%255s",tmp,(int)_countof(tmp));
      login=_wcstoui64(tmp,NULL,10);
     }
   if(password[0]==L'\0')
     {
      wprintf_s(L"Password: ");
      wscanf_s(L"%63s",password,(int)_countof(password));
     }
//--- check parameters
   if(server[0]==L'\0' || login==0 || password[0]==L'\0')
     {
      Usage();
      return(0);
     }
//--- dealer starting
   CDealer dealer;
   if(!dealer.Start(server,login,password))
      return(-1);
//--- waiting for ESC press
   wprintf_s(L"Press Escape to stop...\n");
   while(_getwch()!=27);
//--- dealer stopping
   dealer.Stop();
//--- done
   return(0);
  }
//+------------------------------------------------------------------+
