//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Dataset request field description                                |
//+------------------------------------------------------------------+
struct DatasetField
  {
   IMTDatasetField::EnFieldId id;      // field id
   bool              select;           // select field
   uint32_t          offset;           // field offset in bytes
   uint32_t          size;             // size in bytes for strings
   //--- fill dataset request field
   MTAPIRES          FieldFill(IMTDatasetField &field) const;
  };
//+------------------------------------------------------------------+
//| Dataset request helper class                                     |
//+------------------------------------------------------------------+
class CDatasetRequest
  {
private:
   const DatasetField *m_fields;       // fields array
   const uint32_t    m_fields_total;   // fields array count
   IMTDatasetRequest &m_request;       // request interface
   IMTDatasetField  *m_field;          // field interface
   IMTDatasetField  *m_reference;      // field reference interface
   uint32_t          m_pos;            // current field position

public:
                     CDatasetRequest(const DatasetField *fields,uint32_t fields_total,IMTDatasetRequest &request);
   virtual          ~CDatasetRequest(void);
   //--- next field
   uint32_t          Next(MTAPIRES res);
   //--- add field to dataset request
   MTAPIRES          FieldAdd(void);
   //--- add field to dataset request with where condition
   MTAPIRES          FieldAddWhereUInt(uint32_t value);
   MTAPIRES          FieldAddWhereUIntArray(const uint64_t *values,uint32_t total);
   MTAPIRES          FieldAddWhereKeys(const IMTReportCacheKeySet &keys);
   MTAPIRES          FieldAddWhereString(LPCWSTR value);
   MTAPIRES          FieldAddWhereStringParam(const IMTConParam *param,LPCWSTR value_ignore);
   //--- add field to dataset request with between condition
   MTAPIRES          FieldAddBetweenInt(int64_t from,int64_t to);
   MTAPIRES          FieldAddBetweenUInt(uint64_t from,uint64_t to);
   MTAPIRES          FieldAddBetweenDouble(double from,double to);
   //--- update field of dataset request with between condition
   MTAPIRES          FieldSetBetweenInt(int64_t from,int64_t to);
   MTAPIRES          FieldSetBetweenUInt(uint64_t from,uint64_t to);
   MTAPIRES          FieldSetFromUInt(uint64_t from)    { return(FieldSetBetweenUInt(from,UINT64_MAX)); }

private:
   //--- fill current field
   MTAPIRES          FieldFill(void);
   //--- get field reference from request
   MTAPIRES          FieldReference(void);
  };
//+------------------------------------------------------------------+
