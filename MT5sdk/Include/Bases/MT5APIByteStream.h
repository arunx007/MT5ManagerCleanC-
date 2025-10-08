//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Byte stream interface                                            |
//+------------------------------------------------------------------+
class IMTByteStream
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTByteStream* stream)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- common properties
   virtual uint32_t  Len(void)=0;
   virtual uint32_t  ReadLen(void)=0;
   //--- data add
   virtual MTAPIRES  Add(const void *buf,const uint32_t len)=0;
   virtual MTAPIRES  AddChar(const char data)=0;
   virtual MTAPIRES  AddUChar(const uint8_t data)=0;
   virtual MTAPIRES  AddShort(const short data)=0;
   virtual MTAPIRES  AddUShort(const uint16_t data)=0;
   virtual MTAPIRES  AddInt(const int32_t data)=0;
   virtual MTAPIRES  AddUInt(const uint32_t data)=0;
   virtual MTAPIRES  AddInt64(const int64_t data)=0;
   virtual MTAPIRES  AddUInt64(const uint64_t data)=0;
   virtual MTAPIRES  AddFloat(const float data)=0;
   virtual MTAPIRES  AddDouble(const double data)=0;
   virtual MTAPIRES  AddResult(const MTAPIRES data)=0;
   virtual MTAPIRES  AddStr(LPCWSTR buf)=0;
   //--- data read
   virtual void      ReadReset(void)=0;
   virtual MTAPIRES  Read(void* buf,const uint32_t len)=0;
   virtual MTAPIRES  ReadSkip(const uint32_t len)=0;
   virtual MTAPIRES  ReadChar(char& data)=0;
   virtual MTAPIRES  ReadUChar(uint8_t& data)=0;
   virtual MTAPIRES  ReadShort(SHORT& data)=0;
   virtual MTAPIRES  ReadUShort(uint16_t& data)=0;
   virtual MTAPIRES  ReadInt(int32_t& data)=0;
   virtual MTAPIRES  ReadUInt(uint32_t& data)=0;
   virtual MTAPIRES  ReadInt64(int64_t& data)=0;
   virtual MTAPIRES  ReadUInt64(uint64_t& data)=0;
   virtual MTAPIRES  ReadFloat(float& data)=0;
   virtual MTAPIRES  ReadDouble(double& data)=0;
   virtual MTAPIRES  ReadResult(MTAPIRES& data)=0;
   virtual MTAPIRES  ReadStr(MTAPISTR& buf)=0;
   //--- web api data add
   virtual MTAPIRES  WebAddParamStr(LPCWSTR name,LPCWSTR value)=0;
   virtual MTAPIRES  WebAddParamChar(LPCWSTR name,const char value)=0;
   virtual MTAPIRES  WebAddParamUChar(LPCWSTR name,const uint8_t value)=0;
   virtual MTAPIRES  WebAddParamShort(LPCWSTR name,const short value)=0;
   virtual MTAPIRES  WebAddParamUShort(LPCWSTR name,const uint16_t value)=0;
   virtual MTAPIRES  WebAddParamInt(LPCWSTR name,const int32_t value)=0;
   virtual MTAPIRES  WebAddParamUInt(LPCWSTR name,const uint32_t value)=0;
   virtual MTAPIRES  WebAddParamInt64(LPCWSTR name,const int64_t value)=0;
   virtual MTAPIRES  WebAddParamUInt64(LPCWSTR name,const uint64_t value)=0;
   virtual MTAPIRES  WebAddParamDouble(LPCWSTR name,const double value,const uint32_t digits)=0;
   virtual MTAPIRES  WebAddParamFinalize(void)=0;
   //--- web api data read
   virtual MTAPIRES  WebReadCommand(MTAPISTR& cmd)=0;
   virtual MTAPIRES  WebReadParamName(MTAPISTR& name)=0;
   virtual MTAPIRES  WebReadParamStr(MTAPISTR& str)=0;
   virtual MTAPIRES  WebReadParamStr(LPWSTR value,const uint32_t size)=0;
   virtual MTAPIRES  WebReadParamSkip(void)=0;
   virtual MTAPIRES  WebReadParamChar(char& data)=0;
   virtual MTAPIRES  WebReadParamUChar(uint8_t& data)=0;
   virtual MTAPIRES  WebReadParamShort(SHORT& data)=0;
   virtual MTAPIRES  WebReadParamUShort(uint16_t& data)=0;
   virtual MTAPIRES  WebReadParamInt(int32_t& value)=0;
   virtual MTAPIRES  WebReadParamUInt(uint32_t& value)=0;
   virtual MTAPIRES  WebReadParamInt64(int64_t& value)=0;
   virtual MTAPIRES  WebReadParamUInt64(uint64_t& value)=0;
   virtual MTAPIRES  WebReadParamDouble(double& value)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTByteStream(void) {}
  };
//+------------------------------------------------------------------+
  