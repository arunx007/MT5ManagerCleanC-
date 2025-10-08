//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "Zlib.h"
//+------------------------------------------------------------------+
//| ZIP file extraction                                              |
//+------------------------------------------------------------------+
class CUnzipFile
  {
public:
   enum constants
     {
      BUFREADCOMMENT    =0x400,
      SIZECENTRALDIRITEM=0x2e,
      SIZEZIPLOCALHEADER=0x1e
     };
   //---
   struct UnzipInfo
     {
      uint32_t          entries;             // total number of entries in the central dir on this disk
      uint32_t          size_comment;        // size of the global comment of the zipfile
     };
   //---
   struct UnzipFileInfo
     {
      uint32_t          version;             // version made by                 2 bytes
      uint32_t          version_needed;      // version needed to extract       2 bytes
      uint32_t          flag;                // general purpose bit flag        2 bytes
      uint32_t          method;              // compression method              2 bytes
      uint32_t          dosDate;             // last mod file date in Dos fmt   4 bytes
      uint32_t          crc;                 // crc-32                          4 bytes
      uint64_t          comp_size;           // compressed size, zip64 - 8b     4 bytes
      uint64_t          uncomp_size;         // uncompressed size, zip64 - 8b   4 bytes
      uint32_t          size_filename;       // filename length                 2 bytes
      uint32_t          size_extra;          // extra field length              2 bytes
      uint32_t          size_comment;        // file comment length             2 bytes
      uint32_t          disk_start;          // disk number start               2 bytes
      uint32_t          int_fa;              // internal file attributes        2 bytes
      uint32_t          ext_fa;              // external file attributes        4 bytes
      SYSTEMTIME        tmu_date;
     };
private:
   //---
   struct FileInZip
     {
      char             *buffer;              // internal buffer for compressed data
      SZlib::z_stream   stream;              // stream structure for inflate
      uint64_t          position;            // position in byte on the zipfile, for fseek
      uint32_t          initialised;         // flag set if stream structure is initialised
      uint32_t          loc_extra_offset;    // offset of the local extra field
      uint32_t          loc_extra_size;      // size of the local extra field
      uint32_t          loc_extra_pos;       // position in the local extra field in read
      uint32_t          crc32;               // crc32 of all data uncompressed
      uint32_t          crc32_wait;          // crc32 we must obtain after decompress all
      uint64_t          read_comp;           // number of byte to be decompressed
      uint64_t          read_uncomp;         // number of byte to be obtained after decomp
      uint32_t          method;              // compression method (0==store)
      uint64_t          byte_before;         // byte before the zipfile, (>0 for sfx)
     };
   //---
   struct UnzipFileInfoInt
     {
      UnzipInfo         gi;                  // public global information
      uint64_t          byte_before;         // byte before the zipfile, (>0 for sfx)
      uint32_t          num_file;            // number of the current file in the zipfile
      uint64_t          pos_in_cdir;         // pos of the current file in the central dir
      bool              file_ok;             // flag about the usability of the current file
      uint64_t          central_pos;         // position of the beginning of the central dir
      uint32_t          size_cdir;           // size of the central directory
      uint64_t          offset_cdir;         // offset of start of central directory with respect to the starting disk number
      UnzipFileInfo     info;                // public info about the current file in zip
      uint32_t          offset_curfile;      // relative offset of local header 4 bytes
      FileInZip*        pzfile;              // structure about the current file if we are decompressing it
     };
private:
   CMTFile           m_file;
   UnzipFileInfoInt  m_info;

public:
                     CUnzipFile(void);
                    ~CUnzipFile(void);
   //--- file open\close
   bool              Open(LPCWSTR path);
   void              Close(void);
   bool              IsOpen(void) const { return(m_info.central_pos!=0); }
   //--- information
   bool              Info(UnzipInfo& info);
   //--- file navigation
   bool              FileFirst(void);
   bool              FileNext(void);
   bool              FileOpen(void);
   bool              FileClose(void);
   uint32_t          FileRead(void *buffer,uint32_t size);
   bool              FileInfo(UnzipFileInfo& info,LPWSTR name,uint32_t name_size);

private:
   //---
   uint64_t          FindByMagic(const uint32_t magic_number);
   bool              CurFileInfoInt(UnzipFileInfo& info,uint32_t& offset,LPWSTR name,uint32_t name_size);
   bool              CheckHeader(uint32_t& size_var,uint32_t& extra_offset,uint32_t& extra_size);
   //---
   bool              GetByte(uint32_t& value);
   bool              GetShort(uint32_t& value);
   bool              GetLong(uint32_t& value);
   bool              GetUInt64(uint64_t& value);
   //---
   bool              FileInZipAlloc(void);
   void              FileInZipFree(void);
  };
//+------------------------------------------------------------------+
