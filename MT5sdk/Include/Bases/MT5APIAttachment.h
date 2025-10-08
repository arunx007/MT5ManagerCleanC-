//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Attachment record interface                                      |
//+------------------------------------------------------------------+
class IMTAttachment
  {
public:
   //--- file types
   enum EnFileType
     {
      FILE_TYPE_OTHER                       =0,          // other
      FILE_TYPE_TXT                         =1,          // txt
      FILE_TYPE_DOC                         =2,          // doc
      FILE_TYPE_PDF                         =3,          // pdf
      FILE_TYPE_JPG                         =4,          // jpg
      FILE_TYPE_PNG                         =5,          // png
      FILE_TYPE_BMP                         =6,          // bmp
      FILE_TYPE_ZIP                         =7,          // zip
      //--- enumeration borders
      FILE_TYPE_FIRST                       =FILE_TYPE_OTHER,
      FILE_TYPE_LAST                        =FILE_TYPE_ZIP,
     };
   //--- file flags
   enum EnFileFlags
     {
      FILE_FLAG_EMBEDDED                    =0x1,        // embedded for content (images in HTML)
      //--- enumeration borders
      FILE_FLAG_NONE                        =0,
      FILE_FLAG_ALL                         =FILE_FLAG_EMBEDDED,
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTAttachment* file)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- record id
   virtual uint64_t    RecordID(void) const=0;
   virtual MTAPIRES  RecordID(const uint64_t record_id)=0;
   //--- related client
   virtual uint64_t    RelatedClient(void) const=0;
   virtual MTAPIRES  RelatedClient(const uint64_t record_id)=0;
   //--- file type
   virtual uint32_t  FileType(void) const=0;
   virtual MTAPIRES  FileType(const uint32_t type)=0;
   //--- file name
   virtual LPCWSTR   FileName(void) const=0;
   virtual MTAPIRES  FileName(LPCWSTR name)=0;
   //--- file content
   virtual const void* FileContent(uint32_t& content_size) const=0;
   virtual MTAPIRES  FileContent(const void* content,const uint32_t content_size)=0;
   //--- file size
   virtual uint32_t  FileSize(void)=0;
   //--- file flags
   virtual uint32_t  FileFlags(void) const=0;
   virtual MTAPIRES  FileFlags(const uint32_t flags)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTAttachment(void) {}
  };
//+------------------------------------------------------------------+
//| Comment array interface                                          |
//+------------------------------------------------------------------+
class IMTAttachmentArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTAttachmentArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTAttachment* attachment)=0;
   virtual MTAPIRES  AddCopy(const IMTAttachment* attachment)=0;
   virtual MTAPIRES  Add(IMTAttachmentArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTAttachmentArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTAttachment *Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTAttachment* attachment)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTAttachment* attachment)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTAttachment *Next(const uint32_t index) const=0;
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
                    ~IMTAttachmentArray(void) {}
  };
//+------------------------------------------------------------------+
