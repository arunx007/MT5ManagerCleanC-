//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Server |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "MIMEConverter.h"
//+------------------------------------------------------------------+
//| MIME Header format                                               |
//+------------------------------------------------------------------+
const wchar_t CMIMEConverter::s_header_mime_fmt[]=L"From: \r\n"
                                                  L"Subject: \r\n"
                                                  L"Date: \r\n"
                                                  L"MIME-Version: 1.0\r\n"
                                                  L"Content-Type: multipart/related;\r\n"
                                                  L"  type=\"text/html\";\r\n"
                                                  L"  boundary=\"%s\"\r\n"
                                                  L"X-MimeOLE: Produced By Microsoft MimeOLE V6.0.6002.18005\r\n"
                                                  L"\r\n"
                                                  L"This is a multi-part message in MIME format.\r\n\r\n";
//+------------------------------------------------------------------+
//| MIME part header format                                          |
//+------------------------------------------------------------------+
const wchar_t CMIMEConverter::s_header_part_fmt[]=L"--%s\r\n"
                                                  L"Content-Type: %s\r\n"
                                                  L"Content-Transfer-Encoding: base64\r\n"
                                                  L"Content-Location: %s\r\n\r\n";
//+------------------------------------------------------------------+
//| MIME footer format                                               |
//+------------------------------------------------------------------+
const wchar_t CMIMEConverter::s_footer_mime_fmt[]=L"--%s--\r\n";
//+------------------------------------------------------------------+
//| MIME strings delimeter                                           |
//+------------------------------------------------------------------+
const wchar_t CMIMEConverter::s_string_delimeter[]=L"\r\n\r\n";
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CMIMEConverter::CMIMEConverter(void)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CMIMEConverter::~CMIMEConverter(void)
  {
  }
//+------------------------------------------------------------------+
//| Process data                                                     |
//+------------------------------------------------------------------+
bool CMIMEConverter::Process(const uint32_t codepage,const char *data,const uint32_t data_size,const MIMEImageArray &images,CMTMemPack &mime)
  {
//--- clear buffer
   mime.Clear();
//--- format MIME parts delimeter
   wchar_t boundary[MAX_PATH]={0};
   bool res=GetBoundary(boundary,_countof(boundary));
//--- process MIME header
   res=res && ProcessHeader(boundary,mime);
//--- process MIME body part
   res=res && ProcessPartBody(codepage,data,data_size,boundary,images,mime);
//--- process MIME images
   for(uint32_t i=0;i<images.Total() && res;i++)
      res=ProcessPartImage(boundary,images[i],mime);
//--- process MIME footer
   res=res && ProcessFooter(boundary,mime);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process MIME header                                              |
//+------------------------------------------------------------------+
bool CMIMEConverter::ProcessHeader(LPCWSTR boundary,CMTMemPack &mime) const
  {
//---check
   if(!boundary)
      return(false);
//--- format header
   CMTStr4096 str;
   str.Format(s_header_mime_fmt,boundary);
//--- add header
   return(mime.Add(str.Str(),str.Len()*sizeof(wchar_t)));
  }
//+------------------------------------------------------------------+
//| Process MIME body part                                           |
//+------------------------------------------------------------------+
bool CMIMEConverter::ProcessPartBody(const uint32_t codepage,const char *data,const uint32_t data_size,LPCWSTR boundary,const MIMEImageArray &images,CMTMemPack &mime)
  {
//---check
   if(!data || data_size<1)
      return(false);
//--- process MIME body header
   bool res=ProcessPartBodyHeader(codepage,boundary,images,mime);
//--- allocate buffer for data base64 encoding
   m_base64_buf.Clear();
   res=res && m_base64_buf.Reallocate(data_size*2*sizeof(wchar_t));
//--- encode source data with base 64
   if(res)
     {
      int32_t len;
      if((len=Base64Encode(data,data_size,(LPWSTR)m_base64_buf.Buffer(),m_base64_buf.Max()/sizeof(wchar_t)))>0)
         m_base64_buf.Len((len-1)*sizeof(wchar_t));
      else
         res=false;
     }
//--- add base64 data
   res=res && mime.Add(m_base64_buf.Buffer(),m_base64_buf.Len());
//--- add strings delimeter
   res=res && mime.Add(s_string_delimeter,sizeof(s_string_delimeter)-sizeof(wchar_t));
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process MIME header of body part                                 |
//+------------------------------------------------------------------+
bool CMIMEConverter::ProcessPartBodyHeader(const uint32_t codepage,LPCWSTR boundary,const MIMEImageArray &images,CMTMemPack &mime) const
  {
   wchar_t content_location[MAX_PATH]={0};
   wchar_t content_type[MAX_PATH]={0};
   wchar_t data_codepage[64]={0};
//--- format filename
   CMTStr::Copy(content_location,L"News.html");
//--- get string representation of codepage
   if(!CodepageGetName(codepage,data_codepage,_countof(data_codepage)))
      CMTStr::Copy(data_codepage,L"utf-8");
//--- format Content-type line
   CMTStr::FormatStr(content_type,L"text/html;\n\tcharset=\"%s\"",data_codepage);
//--- format header
   CMTStr4096 str;
   str.Format(s_header_part_fmt,boundary,content_type,content_location);
//--- add header
   return(mime.Add(str.Str(),str.Len()*sizeof(wchar_t)));
  }
//+------------------------------------------------------------------+
//| Process MIME image part                                          |
//+------------------------------------------------------------------+
bool CMIMEConverter::ProcessPartImage(LPCWSTR boundary,const MIMEImage &image,CMTMemPack &mime)
  {
//---check
   if(!image.data || image.data_size<1)
      return(false);
//--- process MIME header for image part
   bool res=ProcessPartImageHeader(boundary,image,mime);
//--- allocate buffer for base64 encoded data
   m_base64_buf.Clear();
   res=res && m_base64_buf.Reallocate(image.data_size*2*sizeof(wchar_t));
//--- encode image with base64
   if(res)
     {
      int32_t len;
      if((len=Base64Encode(image.data,image.data_size,(LPWSTR)m_base64_buf.Buffer(),m_base64_buf.Max()/sizeof(wchar_t)))>0)
         m_base64_buf.Len((len-1)*sizeof(wchar_t));
      else
         res=false;
     }
//--- add base64 data
   res=res && mime.Add(m_base64_buf.Buffer(),m_base64_buf.Len());
//--- add string delimeter
   res=res && mime.Add(s_string_delimeter,sizeof(s_string_delimeter)-sizeof(wchar_t));
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process MIME header for image part                               |
//+------------------------------------------------------------------+
bool CMIMEConverter::ProcessPartImageHeader(LPCWSTR boundary,const MIMEImage &image,CMTMemPack &mime) const
  {
//--- copy image name
   CMTStrPath image_name;
   image_name.Assign(image.name);
//--- format Content-Type header
   wchar_t content_type[64]={0};
   if(image_name.FindNoCase(L".gif")>=0)
      CMTStr::Copy(content_type,L"image/gif");
   else
     {
      if(image_name.FindNoCase(L".png")>=0)
         CMTStr::Copy(content_type,L"image/png");
      else
         CMTStr::Copy(content_type,L"image/jpeg");
     }
//--- format header
   CMTStr4096 str;
   str.Format(s_header_part_fmt,boundary,content_type,image.name);
//--- add header
   return(mime.Add(str.Str(),str.Len()*sizeof(wchar_t)));
  }
//+------------------------------------------------------------------+
//| Process MIME footer                                              |
//+------------------------------------------------------------------+
bool CMIMEConverter::ProcessFooter(LPCWSTR boundary,CMTMemPack &mime) const
  {
//---check
   if(!boundary)
      return(false);
//--- format footer
   CMTStr4096 str;
   str.Format(s_footer_mime_fmt,boundary);
//--- add footer
   return(mime.Add(str.Str(),str.Len()*sizeof(wchar_t)));
  }
//+------------------------------------------------------------------+
//| Get MIME parts delimeter                                         |
//+------------------------------------------------------------------+
bool CMIMEConverter::GetBoundary(LPWSTR boundary,const uint32_t boundary_size) const
  {
//---check
   if(!boundary || boundary_size<1)
      return(false);
//--- get current time
   int64_t ctm=_time64(nullptr);
   tm    ttm={};
   if(_gmtime64_s(&ttm,&ctm)!=0)
      return(false);
//--- format new delimeter
   CMTStr::FormatStr(boundary,boundary_size,L"------=_NextPart_%08x_%04d%02d%02d%02d%02d%02d",
                                            ttm.tm_sec+rand(),
                                            ttm.tm_year+1900,
                                            ttm.tm_mon+1,
                                            ttm.tm_mday,
                                            ttm.tm_hour,
                                            ttm.tm_min,
                                            ttm.tm_sec);
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Encode data with base64                                          |
//+------------------------------------------------------------------+
int32_t CMIMEConverter::Base64Encode(const char *buffer,const uint32_t buffer_size,LPWSTR result,const uint32_t result_max,const uint32_t line_break_period/*=100*/) const
  {
   static const wchar_t base64array[]=L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   uint32_t             i=0,j=0,pad=0;
   uint32_t             line_length=0;
//---check
   if(!buffer || !result)
      return(0);
//--- terminate string
   result[0]=0;
//--- go through data and encode it
   while(i<buffer_size && j<(result_max-8))
     {
      //--- break line
      if(line_break_period>0 && line_length>=line_break_period)
        {
         result[j++]=L'\r';
         result[j++]=L'\n';
         line_length=0;
        }
      //--- get data bytes
      BYTE b3,b2,b1=buffer[i++];
      if(i>=buffer_size)
        {
         b2=b3=0;
         pad=2;
        }
      else
        {
         b2=buffer[i++];
         if(i<buffer_size)
            b3=buffer[i++];
         else
           {
            b3=0;
            pad=1;
           }
        }
      //--- encode
      BYTE c1=(BYTE)(b1 >> 2);
      BYTE c2=(BYTE)(((b1 & 0x3) << 4) | (b2 >> 4));
      BYTE c3=(BYTE)(((b2 & 0xf) << 2) | (b3 >> 6));
      BYTE c4=(BYTE)(b3 & 0x3f);
      result[j++]=base64array[c1];
      result[j++]=base64array[c2];
      switch(pad)
        {
         case 0:
            result[j++]=base64array[c3];
            result[j++]=base64array[c4];
            break;
         case 1:
            result[j++]=base64array[c3];
            result[j++]='=';
            break;
         case 2:
            result[j++]='=';
            result[j++]='=';
            break;
         default:
            break;
        }
      line_length+=4;
     }
//--- terminate string
   result[j++]=0;
//--- return size
   return(j);
  }
//+------------------------------------------------------------------+
//| Get codepage name                                                |
//+------------------------------------------------------------------+
bool CMIMEConverter::CodepageGetName(uint32_t codepage_code,LPWSTR codepage_name,uint32_t codepage_name_len) const
  {
//--- check
   if(codepage_name==NULL || codepage_name_len<1)
      return(false);
//--- search for codepage and return it's name
   switch(codepage_code)
     {
      case CP_ACP:   CMTStr::Copy(codepage_name,codepage_name_len,L"ANSI");         return(true);
      case CP_OEMCP: CMTStr::Copy(codepage_name,codepage_name_len,L"OEM");          return(true);
      case CP_MACCP: CMTStr::Copy(codepage_name,codepage_name_len,L"MAC");          return(true);
      case CP_UTF7:  CMTStr::Copy(codepage_name,codepage_name_len,L"utf-7");        return(true);
      case CP_UTF8:  CMTStr::Copy(codepage_name,codepage_name_len,L"utf-8");        return(true);
      case 950:      CMTStr::Copy(codepage_name,codepage_name_len,L"Big5");         return(true);
      case 1252:     CMTStr::Copy(codepage_name,codepage_name_len,L"ISO-8859-1");   return(true);
      case 1254:     CMTStr::Copy(codepage_name,codepage_name_len,L"ISO-8859-9");   return(true);
      case 1251:     CMTStr::Copy(codepage_name,codepage_name_len,L"windows-1251"); return(true);
      case 936:      CMTStr::Copy(codepage_name,codepage_name_len,L"gb2312");       return(true);
      case 932:      CMTStr::Copy(codepage_name,codepage_name_len,L"shift_jis");    return(true);
      case 949:      CMTStr::Copy(codepage_name,codepage_name_len,L"euc-kr");       return(true);
     }
//--- unknown codepage
   return(false);
  }
//+------------------------------------------------------------------+
