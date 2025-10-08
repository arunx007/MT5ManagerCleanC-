//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "MT5APIAttachment.h"
//+------------------------------------------------------------------+
//| Comment record interface                                         |
//+------------------------------------------------------------------+
class IMTComment
  {
public:
   //--- comment flags
   enum EnCommentFlags
     {
      COMMENT_FLAG_DELETED    =0x1,                   // deleted
      COMMENT_FLAG_IMPORTANT  =0x2,                   // important
      //--- enumeration borders
      COMMENT_FLAG_NONE       =0,
      COMMENT_FLAG_ALL        =COMMENT_FLAG_DELETED|COMMENT_FLAG_IMPORTANT,
     };
   //--- comment types
   enum EnCommentType
     {
      COMMENT_TYPE_UNDEFINED                =0,       // undefined
      COMMENT_TYPE_LOGRECORD                =1,       // log record
      COMMENT_TYPE_CALLRECORD               =2,       // call record
      COMMENT_TYPE_ROBOTRECORD              =3,       // robot record
      //--- enumeration borders
      COMMENT_TYPE_FIRST                    =COMMENT_TYPE_UNDEFINED,
      COMMENT_TYPE_LAST                     =COMMENT_TYPE_ROBOTRECORD,
     };
   //--- comment results
   enum EnCommentResult
     {
      COMMENT_RESULT_UNDEFINED              =0,       // undefined
      COMMENT_RESULT_CALL_NO_ANSWER         =1,       // call no answer
      COMMENT_RESULT_CALL_WRONG_NUMBER      =2,       // call wrong number
      COMMENT_RESULT_CALL_NOT_INTERESTED    =3,       // call not interested
      COMMENT_RESULT_CALL_SUCCESSFUL        =4,       // call successful
      //--- enumeration borders
      COMMENT_RESULT_FIRST                  =COMMENT_RESULT_UNDEFINED,
      COMMENT_RESULT_LAST                   =COMMENT_RESULT_CALL_SUCCESSFUL,
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTComment* comment)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- record id
   virtual uint64_t    RecordID(void) const=0;
   virtual MTAPIRES  RecordID(const uint64_t record_id)=0;
   //--- related client
   virtual uint64_t    RelatedClient(void) const=0;
   virtual MTAPIRES  RelatedClient(const uint64_t record_id)=0;
   //--- related document
   virtual uint64_t    RelatedDocument(void) const=0;
   virtual MTAPIRES  RelatedDocument(const uint64_t record_id)=0;
   //--- flags
   virtual uint32_t  Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint32_t flags)=0;
   //--- extra
   virtual LPCWSTR   Extra(void) const=0;
   virtual MTAPIRES  Extra(LPCWSTR extra)=0;
   //--- text
   virtual LPCWSTR   Text(void) const=0;
   virtual MTAPIRES  Text(LPCWSTR text)=0;
   //--- comment type
   virtual uint32_t  CommentType(void) const=0;
   virtual MTAPIRES  CommentType(const uint32_t type)=0;
   //--- comment result
   virtual uint32_t  CommentResult(void) const=0;
   virtual MTAPIRES  CommentResult(const uint32_t result)=0;
   //--- attachments
   virtual MTAPIRES  AttachmentsAdd(const IMTAttachment *attachment)=0;
   virtual MTAPIRES  AttachmentsClear(void)=0;
   virtual uint32_t  AttachmentsTotal(void) const=0;
   virtual MTAPIRES  AttachmentsNext(const uint32_t pos,uint64_t& attachment_id,MTAPISTR& attachment_name,uint32_t& attachment_size) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTComment(void) {}
  };
//+------------------------------------------------------------------+
//| Comment array interface                                          |
//+------------------------------------------------------------------+
class IMTCommentArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTCommentArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTComment* comment)=0;
   virtual MTAPIRES  AddCopy(const IMTComment* comment)=0;
   virtual MTAPIRES  Add(IMTCommentArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTCommentArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTComment *Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTComment* comment)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTComment* comment)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTComment*Next(const uint32_t index) const=0;
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
                    ~IMTCommentArray(void) {}
  };
//+------------------------------------------------------------------+
//| Comment records notification interface                           |
//+------------------------------------------------------------------+
class IMTCommentSink
  {
public:
   //--- events
   virtual void      OnCommentAdd(const IMTComment*    /*comment*/)           {  }
   virtual void      OnCommentUpdate(const IMTComment* /*comment*/)           {  }
   virtual void      OnCommentDelete(const IMTComment* /*comment*/)           {  }
  };
//+------------------------------------------------------------------+
