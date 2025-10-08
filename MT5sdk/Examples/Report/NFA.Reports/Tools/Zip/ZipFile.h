//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "Zlib.h"
//+------------------------------------------------------------------+
//| ZIP file                                                         |
//+------------------------------------------------------------------+
class CZipFile
  {
private:
   enum constants
     {
      ZIP_READ_CHUNK       =4*1024*1024,
      SIZEDATA_INDATABLOCK =4080,
      //--- magic numbers
      CENTRALHEADERMAGIC   =0x02014b50,
      LOCALHEADERMAGIC     =0x04034b50,
      ENDHEADERMAGIC       =0x06054b50,
      //--- versions
      VERSIONMADEBY        =0x0,
      EXTRACTVERSION       =20,
      EXTRACTVERSION_ZIP64 =45,
      //--- size of headers
      SIZECENTRALHEADER    =0x2e,
      SIZEEXTRAFIELD_CD    =20,
      SIZEZIP64ENDOFCENTDIR=44,
     };
   //---
   struct ZipFileInfo
     {
      SYSTEMTIME        tmzDate;
      uint32_t          dosDate;                // if dos_date==0, tmu_date is used
      uint32_t          internal_fa;            // internal file attributes
      uint32_t          external_fa;            // external file attributes
      wchar_t           file_path[512];
      uint64_t          file_size;              // size of current file for init zip64
     };
   //---
   struct ZipFileDataBlock
     {
      uint32_t          avail_in_this_block;
      uint32_t          filled_in_this_block;
      uint32_t          unused;                 // for future use and alignement
      BYTE             *data;
      ZipFileDataBlock *next;
     };
   //---
   struct ZipCurFileInfo
     {
      SZlib::z_stream   stream;                 // zLib stream structure for inflate
      int32_t           stream_initialised;     // 1 is stream is initialised
      uint32_t          pos_in_buffered_data;   // last written byte in buffered_data
      uint32_t          pos_local_header;       // offset of the local header of the file currenty writing
      char             *central_header;         // central header data for the current file
      uint32_t          size_centralheader;     // size of the central header for cur file
      uint32_t          flag;                   // flag of the file currently writing
      int32_t           method;                 // compression method of file currenty wr.
      BYTE             *buffered_data;          // buffer contain compressed data to be write
      uint32_t          dosDate;
      uint32_t          crc32;
      uint64_t          file_size;              //
      uint32_t          filename_len;           //
      bool              using_zip64;            //
     };
   //---
   struct ZipFileInfoInt
     {
      ZipFileDataBlock *first;
      ZipFileDataBlock *last;
      bool              in_opened_file_in_zip;  // 1 if a file in the metazip is currently write
      ZipCurFileInfo    ci;                     // info on the file curretly writing
      uint32_t          begin_pos;              // position of the beginning of the metazipfile
      uint32_t          number_entry;
     };

private:
   CMTStrPath        m_path;     // path
   CMTFile           m_file;     // file
   ZipFileInfoInt    m_info;     // information
   char*             m_buffer;   // buffer

public:
                     CZipFile(void);
                    ~CZipFile(void);
   //--- open/close
   bool              Open(LPCWSTR path);
   bool              Close(void);
   LPCWSTR           Path(void) const { return(m_path.Str()); }
   //--- adding
   bool              AddFile(LPCWSTR path,LPCWSTR zip_path);
   bool              AddDirectory(LPCWSTR zip_path);
   bool              AddDirectory(LPCWSTR zip_path,LPCWSTR full_path,LPCWSTR mask);
   uint64_t          GetLastOriginalFileLength(void) { return(m_info.ci.stream.total_in);  }
   uint64_t          GetLastZippedFileLength(void)   { return(m_info.ci.stream.total_out); }

private:
   //---
   void              PutValueInMemory(BYTE dest,uint32_t x,int size);
   bool              PutValueInMemory(char *buff,const uint32_t buff_size,uint64_t x,const uint32_t size);
   bool              PutValue(uint32_t x,int size);
   bool              PutValue64(uint64_t x,const int32_t size);
   void              UpdateZipInfo(ZipFileInfo& info);
   //--- write zip header
   bool              WriteEndOfCentralDirectory(uint64_t& pos_eocd_inzip,const uint64_t pos_centdir_inzip);
   bool              WriteEndOfCentralDirectoryLocator(const uint64_t pos_eocd_inzip);
   //---
   int32_t           BlockWrite(void);
   void              BlockAddData(char *buffer,uint32_t size);
   void              BlockFreeAll(void);
   ZipFileDataBlock *BlockAlloc(void);
   void              BlockFree(ZipFileDataBlock *block);
   //---
   uint32_t          GetDosDate(ZipFileInfo& info);
   //---
   bool              FileInZipOpenNew(ZipFileInfo& info);
   bool              FileInZipWrite(const void *buffer,uint32_t size);
   bool              FileInZipClose(void);
  };
//+------------------------------------------------------------------+
  