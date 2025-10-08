//+------------------------------------------------------------------+
//|                                                     MetaTrader 5 |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Class of string-wise file reading                                |
//+------------------------------------------------------------------+
class CStringFile
  {
private:
   HANDLE            m_file;
   int32_t           m_file_size;
   BYTE             *m_buffer;
   int32_t           m_buffer_size;                // in bytes
   int32_t           m_index;                      // in symbols
   int32_t           m_readed;                     // in bytes
   int32_t           m_line;
   char             *m_tmpstr;
   int32_t           m_tmpstr_len;

public:
                     CStringFile(const int32_t nBufSize=65536);
                    ~CStringFile(void);
   bool              Open(const LPCWSTR szFile);
   void              Close(void);
   //---
   int32_t           GetNextLine(wchar_t *line,int line_maxsize);
   void              Reset(void);
   inline int32_t    FileLen(void) const  { return(m_file_size); }
   bool              IsOpen()      const  { return(m_file!=NULL && m_file!=INVALID_HANDLE_VALUE); }

private:
   int32_t           GetNextLineA(char *line,const int32_t maxsize);
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
inline CStringFile::CStringFile(const int32_t nBufSize) : m_file(NULL),m_file_size(0),
             m_buffer(new BYTE[nBufSize]),m_buffer_size(nBufSize),
             m_index(0),m_readed(0),m_line(0),m_tmpstr(NULL),m_tmpstr_len(0)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
inline CStringFile::~CStringFile(void)
  {
//--- close
   Close();
//--- clear
   if(m_buffer!=NULL) { delete[] m_buffer; m_buffer=NULL; }
   if(m_tmpstr!=NULL) { delete[] m_tmpstr; m_tmpstr=NULL; }
  }
//+------------------------------------------------------------------+
//| Open the file                                                    |
//+------------------------------------------------------------------+
inline bool CStringFile::Open(const LPCWSTR szFile)
  {
   DWORD dwFileSizeHigh;
//--- checking
   if(szFile==NULL) return(false);
   Close();
//--- open the file
   m_file=CreateFileW(szFile,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
//--- check
   if(m_file==NULL || m_file==INVALID_HANDLE_VALUE) { m_file=NULL; return(false); }
//--- determin its size
   m_file_size=(int)GetFileSize(m_file,&dwFileSizeHigh);
//--- reset
   Reset();
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Close the file                                                   |
//+------------------------------------------------------------------+
inline void CStringFile::Close(void)
  {
//--- hasn't the file been closed yet?
   if(m_file!=NULL) { ::CloseHandle(m_file); m_file=NULL; }
//--- reset counters
   m_index =0;
   m_readed=0;
   m_line  =0;
  }
//+------------------------------------------------------------------+
//| Reset reading - go to the file beginning                         |
//+------------------------------------------------------------------+
inline void CStringFile::Reset(void)
  {
   m_index=0;
   m_line =0;
   if(m_file!=NULL) SetFilePointer(m_file,0,NULL,FILE_BEGIN);
  }
//+------------------------------------------------------------------+
//| Read the next string from the file                               |
//| Return value : 0 - error or end of the file                      |
//|            non 0 - string index in the file (from 1)             |
//+------------------------------------------------------------------+
inline int32_t CStringFile::GetNextLine(wchar_t *line,int line_maxsize)
  {
   int32_t res;
//--- check
   if(!line || line_maxsize<1)
      return(0);
//--- prepare the string
   line[0]=L'\0';
//--- check the buffer size
   if(m_tmpstr_len<line_maxsize)
     {
      //--- buffer no enough - allocate more memory
      m_tmpstr_len+=line_maxsize;
      if(m_tmpstr) delete[] m_tmpstr;
      m_tmpstr=new char[m_tmpstr_len];
     }
//--- if there are no problems with the buffer, read the ASCII string
   if(m_tmpstr)
     {
      res=GetNextLineA(m_tmpstr,line_maxsize);
      //--- convert to unicode
      if(::MultiByteToWideChar(CP_ACP,0,m_tmpstr,-1,line,line_maxsize)==0)
        {
         line[0]=L'\0';
         res=0;
        }
      //--- return the string number
      return(res);
     }
//--- everything is bad
   return(0);
  }
//+------------------------------------------------------------------+
//| Read the next ASCII string from the file                         |
//+------------------------------------------------------------------+
inline int32_t CStringFile::GetNextLineA(char* line,const int32_t maxsize)
  {
   char *currsym=line,*lastsym=line+maxsize-1;
   BYTE *curpos=m_buffer+m_index;
//--- checking
   if(m_file==NULL || m_buffer==NULL) return(0);
//--- it is a loop
   for(;;)
     {
      //--- the first string or we've read the entire buffer
      if(m_line==0 || m_index==m_readed)
        {
         m_index=0;
         //--- read to buffer
         if(::ReadFile(m_file,m_buffer,m_buffer_size,(DWORD*)&m_readed,NULL)==FALSE) return(0);
         //--- have read 0 bytes? end of file
         if(m_readed<1) { *currsym=0; return(currsym!=line ? m_line:0); }
         curpos=m_buffer;
        }
      //--- analyze the buffer
      while(m_index<m_readed)
        {
         //--- reached the end?
         if(currsym>=lastsym) { *currsym=0; return(m_line); }
         //--- analyze the symbol (found the string end)
         if(*curpos=='\n')
           {
            //--- was there a carriage return before that?
            if(currsym>line && currsym[-1]=='\r') currsym--; // was-remove it
            *currsym=0;
            //--- return the string number
            m_line++; m_index++;
            return(m_line);
           }
         //--- common symbol - copy it
         *currsym++=*curpos++; m_index++;
        }
     }
//---
   return(0); // this is impossible...
  }
//+------------------------------------------------------------------+
