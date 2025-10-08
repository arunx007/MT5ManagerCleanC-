//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Server |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Class of convertation html page with images into mime format     |
//+------------------------------------------------------------------+
class CMIMEConverter
  {

public:
   //--- image structure
   struct MIMEImage
     {
      wchar_t           name[MAX_PATH];               // image name
      char             *data;                         // image data
      uint32_t          data_size;                    // image data size
     };
   //--- images array type
   typedef TMTArray<MIMEImage> MIMEImageArray;

private:
   //--- static members
   static const wchar_t s_header_mime_fmt[];          // MIME header format
   static const wchar_t s_header_part_fmt[];          // MIME part header format
   static const wchar_t s_footer_mime_fmt[];          // MIME footer format
   static const wchar_t s_string_delimeter[];         // MIME string delimeter
   //--- buffer for base 64 encoded data
   CMTMemPack        m_base64_buf;

public:
   //--- constructor/destructor
                     CMIMEConverter();
                    ~CMIMEConverter(void);
   //--- process data
   bool              Process(const uint32_t codepage,const char *data,const uint32_t data_size,const MIMEImageArray &images,CMTMemPack &mime);

private:
   //--- process data
   bool              ProcessHeader(LPCWSTR boundary,CMTMemPack &mime) const;
   bool              ProcessPartBody(const uint32_t codepage,const char *data,const uint32_t data_size,LPCWSTR boundary,const MIMEImageArray &images,CMTMemPack &mime);
   bool              ProcessPartBodyHeader(const uint32_t codepage,LPCWSTR boundary,const MIMEImageArray &images,CMTMemPack &mime) const;
   bool              ProcessPartImage(LPCWSTR boundary,const MIMEImage &image,CMTMemPack &mime);
   bool              ProcessPartImageHeader(LPCWSTR boundary,const MIMEImage &image,CMTMemPack &mime) const;
   bool              ProcessFooter(LPCWSTR boundary,CMTMemPack &mime) const;
   //--- get MIME parts separator
   bool              GetBoundary(LPWSTR boundary,const uint32_t boundary_size) const;
   //--- base64 encoding
   int32_t           Base64Encode(const char *buffer,const uint32_t buffer_size,LPWSTR result,const uint32_t result_max,const uint32_t line_break_period=100) const;
   //--- get codepage name
   bool              CodepageGetName(uint32_t codepage_code,LPWSTR codepage_name,uint32_t codepage_name_len) const;
  };
//+------------------------------------------------------------------+
