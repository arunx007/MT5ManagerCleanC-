//+------------------------------------------------------------------+
//|                                         MetaTrader 5 API Server  |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Mail recipients range                                            |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct MTMailRange
  {
   uint64_t          first_login;
   uint64_t          last_login;
   uint32_t          reserved[4];
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Mail Message interface                                           |
//+------------------------------------------------------------------+
class IMTMail
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTMail* mail)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- mail ID and parent mail ID
   virtual uint64_t    ID(void) const=0;
   virtual uint64_t    Parent(void) const=0;
   //--- subject
   virtual LPCWSTR   Subject(void) const=0;
   virtual MTAPIRES  Subject(LPCWSTR subject)=0;
   //--- from ID
   virtual uint64_t    From(void) const=0;
   virtual MTAPIRES  From(const uint64_t id)=0;
   //--- from name
   virtual LPCWSTR   FromName(void) const=0;
   virtual MTAPIRES  FromName(LPCWSTR name)=0;
   //--- to ID
   virtual uint64_t    To(void) const=0;
   virtual MTAPIRES  To(const uint64_t id)=0;
   //--- to name
   virtual LPCWSTR   ToName(void) const=0;
   virtual MTAPIRES  ToName(LPCWSTR name)=0;
   //--- to ID ranges
   virtual MTAPIRES  ToRangesAdd(MTMailRange& range)=0;
   virtual MTAPIRES  ToRangesDelete(const uint32_t pos)=0;
   virtual MTAPIRES  ToRangesClear(void)=0;
   virtual uint32_t  ToRangesTotal(void) const=0;
   virtual MTAPIRES  ToRangesNext(const uint32_t pos,MTMailRange& range) const=0;
   //--- time
   virtual int64_t     Time(void) const=0;
   //--- body
   virtual LPCVOID   Body(void) const=0;
   virtual uint32_t  BodySize(void) const=0;
   virtual MTAPIRES  Body(LPCVOID body,const uint32_t body_size)=0;
   //--- attachments
   virtual MTAPIRES  AttachmentsAdd(LPCWSTR filename,LPCVOID attachment,const uint32_t attachment_size)=0;
   virtual MTAPIRES  AttachmentsClear(void)=0;
   virtual uint32_t  AttachmentsTotal(void) const=0;
   virtual LPVOID    AttachmentsBody(const uint32_t pos) const=0;
   virtual uint32_t  AttachmentsSize(const uint32_t pos) const=0;
   virtual LPCWSTR   AttachmentsName(const uint32_t pos) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTMail(void) {}
  };
//+------------------------------------------------------------------+
//| Mail events notification interface                               |
//+------------------------------------------------------------------+
class IMTMailSink
  {
public:
   virtual void      OnMail(const IMTMail* /*mail*/) {                    };
   virtual MTAPIRES  HookMail(IMTMail* /*mail*/)     { return(MT_RET_OK); }
  };
//+------------------------------------------------------------------+

