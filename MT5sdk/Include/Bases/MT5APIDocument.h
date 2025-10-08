//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "MT5APIAttachment.h"
//+------------------------------------------------------------------+
//| Document record interface                                        |
//+------------------------------------------------------------------+
class IMTDocument
  {
public:
   //--- document types
   enum EnDocumentTypes
     {
      DOCUMENT_TYPE_OTHER                       =0,      // other
      //--- client
      DOCUMENT_TYPE_PERSONAL_IDENTITY           =1,      // proof of identity
      DOCUMENT_TYPE_PERSONAL_ADDRESS            =2,      // proof of address
      //--- company
      DOCUMENT_TYPE_REGISTERED_ADDRESS          =1000,   // registered address
      DOCUMENT_TYPE_DIRECTORS_PASSPORT          =1001,   // directors passport
      DOCUMENT_TYPE_CERTIFICATE_OF_INCORPORATION=1002,   // certificate of incorporation
      DOCUMENT_TYPE_CERTIFICATE_OF_DIRECTORS    =1003,   // certificate of directors
      DOCUMENT_TYPE_CERTIFICATE_OF_GOOD_STANDING=1004,   // certificate of good standing
      //--- enumeration borders
      DOCUMENT_TYPE_FIRST                       =DOCUMENT_TYPE_OTHER,
      DOCUMENT_TYPE_LAST                        =DOCUMENT_TYPE_CERTIFICATE_OF_GOOD_STANDING
     };
   //--- document subtypes
   enum EnDocumentSubtype
     {
      DOCUMENT_SUBTYPE_OTHER                    =0,      // other
      DOCUMENT_SUBTYPE_ID_CARD                  =1,      // id card
      DOCUMENT_SUBTYPE_PASSPORT                 =2,      // passport
      DOCUMENT_SUBTYPE_DRIVERS                  =3,      // driving license
      DOCUMENT_SUBTYPE_BANK_CARD                =4,      // bank card
      DOCUMENT_SUBTYPE_UTILITY_BILL             =5,      // utility bill
      DOCUMENT_SUBTYPE_BANK_STATEMENT           =6,      // bank statement
      DOCUMENT_SUBTYPE_TAX_STATEMENT            =7,      // tax statement
      DOCUMENT_SUBTYPE_SELFIE                   =8,      // selfie
      DOCUMENT_SUBTYPE_PROFILE_IMAGE            =9,      // profile image
      DOCUMENT_SUBTYPE_ID_DOC_PHOTO             =10,     // id documents photo
      DOCUMENT_SUBTYPE_AGREEMENT                =11,     // agreement of some sort, e.g. for processing personal info
      DOCUMENT_SUBTYPE_CONTRACT                 =12,     // some sort of contract
      DOCUMENT_SUBTYPE_RESIDENCE_PERMIT         =13,     // residence permit or registration document in the foreign city/country
      DOCUMENT_SUBTYPE_EMPLOYMENT_CERTIFICATE   =14,     // document from an employer, e.g. proof that a user works there
      DOCUMENT_SUBTYPE_DRIVERS_TRANSLATION      =15,     // translation of the driving license required in the target country
      DOCUMENT_SUBTYPE_INVESTOR_DOC             =16,     // document from an investor, e.g. documents which disclose assets of the investor
      DOCUMENT_SUBTYPE_VEHICLE_REG_CERTIFICATE  =17,     // certificate of vehicle registration
      DOCUMENT_SUBTYPE_INCOME_SOURCE            =18,     // proof of income
      DOCUMENT_SUBTYPE_PAYMENT_METHOD           =19,     // entity confirming payment (like bank card, crypto wallet, etc)
      //--- enumeration borders
      DOCUMENT_SUBTYPE_FIRST                    =DOCUMENT_SUBTYPE_OTHER,
      DOCUMENT_SUBTYPE_LAST                     =DOCUMENT_SUBTYPE_PAYMENT_METHOD
     };
   //--- document statuses
   enum EnDocumentStatus
     {
      DOCUMENT_STATUS_NEW                       =0,        // new
      DOCUMENT_STATUS_APPROVED                  =100,      // approved
      DOCUMENT_STATUS_REJECTED                  =200,      // rejected
      DOCUMENT_STATUS_ARCHIVED                  =300,      // archived
      DOCUMENT_STATUS_DELETED                   =400,      // deleted
      //--- enumeration borders
      DOCUMENT_STATUS_FIRST                     =DOCUMENT_STATUS_NEW,
      DOCUMENT_STATUS_LAST                      =DOCUMENT_STATUS_DELETED,
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTDocument* document)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- record id
   virtual uint64_t    RecordID(void) const=0;
   virtual MTAPIRES  RecordID(const uint64_t record_id)=0;
   //--- related client
   virtual uint64_t    RelatedClient(void) const=0;
   virtual MTAPIRES  RelatedClient(const uint64_t record_id)=0;
   //--- approved date
   virtual int64_t     ApprovedDate(void) const=0;
   virtual MTAPIRES  ApprovedDate(const int64_t date)=0;
   //--- approved by
   virtual uint64_t    ApprovedBy(void) const=0;
   virtual MTAPIRES  ApprovedBy(const uint64_t manager)=0;
   //--- date issue
   virtual int64_t     DateIssue(void) const=0;
   virtual MTAPIRES  DateIssue(const int64_t date)=0;
   //--- date expiration
   virtual int64_t     DateExpiration(void) const=0;
   virtual MTAPIRES  DateExpiration(const int64_t date)=0;
   //--- document type
   virtual uint32_t  DocumentType(void) const=0;
   virtual MTAPIRES  DocumentType(const uint32_t type)=0;
   //--- document name
   virtual LPCWSTR   DocumentName(void) const=0;
   virtual MTAPIRES  DocumentName(LPCWSTR name)=0;
   //--- document comment
   virtual LPCWSTR   DocumentComment(void) const=0;
   virtual MTAPIRES  DocumentComment(LPCWSTR comment)=0;
   //--- document status
   virtual uint32_t  DocumentStatus(void) const=0;
   virtual MTAPIRES  DocumentStatus(const uint32_t status)=0;
   //--- attachments
   virtual MTAPIRES  AttachmentsAdd(const IMTAttachment *attachment)=0;
   virtual MTAPIRES  AttachmentsClear(void)=0;
   virtual uint32_t  AttachmentsTotal(void) const=0;
   virtual MTAPIRES  AttachmentsNext(const uint32_t pos,uint64_t& attachment_id,MTAPISTR& attachment_name,uint32_t& attachment_size) const=0;
   //--- document subtype
   virtual uint32_t  DocumentSubtype(void) const=0;
   virtual MTAPIRES  DocumentSubtype(const uint32_t subtype)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTDocument(void) {}
  };
//+------------------------------------------------------------------+
//| Document array interface                                         |
//+------------------------------------------------------------------+
class IMTDocumentArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTDocumentArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTDocument* document)=0;
   virtual MTAPIRES  AddCopy(const IMTDocument* document)=0;
   virtual MTAPIRES  Add(IMTDocumentArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTDocumentArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTDocument *Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTDocument* document)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTDocument* document)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTDocument*Next(const uint32_t index) const=0;
   //--- sorting and search
   virtual MTAPIRES  Sort(MTSortFunctionPtr sort_function)=0;
   virtual int32_t   Search(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchGreatOrEq(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchGreater(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchLessOrEq(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchLess(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchLeft(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchRight(const void *key,MTSortFunctionPtr sort_function) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTDocumentArray(void) {}
  };
//+------------------------------------------------------------------+
//| Document records notification interface                          |
//+------------------------------------------------------------------+
class IMTDocumentSink
  {
public:
   //--- events
   virtual void      OnDocumentAdd(const IMTDocument*    /*document*/)           {  }
   virtual void      OnDocumentUpdate(const IMTDocument* /*document*/)           {  }
   virtual void      OnDocumentDelete(const IMTDocument* /*document*/)           {  }
  };
//+------------------------------------------------------------------+
