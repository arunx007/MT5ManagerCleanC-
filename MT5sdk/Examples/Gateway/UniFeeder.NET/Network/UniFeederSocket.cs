//+------------------------------------------------------------------+
//|                                           MetaTrader 5 UniFeeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
namespace UniFeeder.NET
  {
   using MetaQuotes.MT5CommonAPI;
   using MetaQuotes.MT5GatewayAPI;
   using System;
   using System.Collections.Generic;
   using System.Globalization;
   using System.Net.Sockets;
   using System.Text;
   using System.Threading;
   //+------------------------------------------------------------------+
   //| Class of socket of connection to UniDDE Connector                |
   //+------------------------------------------------------------------+
   class CUniFeederSocket: IDisposable
     {
      //--- constants
      const int THREAD_TIMEOUT =60;       // timeout of thread completion, s
      const int LOGIN_COUNT_MAX=3;        // maximal number of authorization attempts
      const int MAX_BUFFER_LEN =1024;     // maximal number of bytes to receive in temporary buffer
      //--- reference to parent
      CDataSource       m_parent;
      //--- access to Universal DDE Connector
      Socket            m_socket;         // socket
      //--- connection timeouts
      Int64             m_pingtime;       // server ping time
      Int64             m_closetime;      // time of closing by timeout
      //--- buffer for the read string
      string            m_string;
      //--- for valid double from string conversion
      NumberStyles      m_double_style    =NumberStyles.Any;
      IFormatProvider   m_double_format   =new NumberFormatInfo() {NumberDecimalDigits='.'};
      //+------------------------------------------------------------------+
      //| Constructor                                                      |
      //+------------------------------------------------------------------+
      public CUniFeederSocket(CDataSource parent)
        {
         m_parent     =parent;
         m_socket     =null;
         m_pingtime   =0;
         m_closetime  =0;
         m_string     =string.Empty;
        }
      //+------------------------------------------------------------------+
      //| Destructor                                                       |
      //+------------------------------------------------------------------+
      public void Dispose()
        {
         //--- close the socket
         Close();
        }
      //+------------------------------------------------------------------+
      //|                                                                  |
      //+------------------------------------------------------------------+
      void UpdateTime()
        {
         m_closetime=SMTTime.Time64()+THREAD_TIMEOUT;
        }
      //+------------------------------------------------------------------+
      //| Checking the connection                                          |
      //+------------------------------------------------------------------+
      public bool Check()
        {
         //--- socket not closed yet?
         if(m_socket==null)
            return(false);
         //--- check the time
         if(SMTTime.Time64()>=m_closetime)
           {
            //--- close the socket
            Close();
            return(false);
           }
         //--- everything is ok
         return(true);
        }
      //+------------------------------------------------------------------+
      //| Check connection to the server and connect, if necessary         |
      //+------------------------------------------------------------------+
      public bool Connect(string server,string login,string password,string symbols)
        {
         //--- checking
         if(server==null || login==null || password==null)
            return(false);
         //--- if already connected, fine, quit
         if(m_socket!=null)
            return(true);
         //--- return the connection result
         return(ConnectDDE(server) && Login(login,password,symbols));
        }
      //+------------------------------------------------------------------+
      //| Closing the socket                                               |
      //+------------------------------------------------------------------+
      public void Close()
        {
         //--- if the socket is open, close it
         if(m_socket!=null)
           {
            try
              {
               m_socket.Shutdown(SocketShutdown.Both);
               m_socket.Close();
              }
            catch
              {
               //--- ignore this exception info
              }
            //---
            m_socket=null;
           }
        }
      //+------------------------------------------------------------------+
      //| Receipt of ticks                                                 |
      //+------------------------------------------------------------------+
      public bool ReadTicks(ref List<MTTick> ticks)
        {
         //--- check target & read data from socket
         if(ticks==null || !DataCheck())
            return(false);
         //---
         string tick_string=string.Empty;
         //--- process all received strings
         while(DataGetString(ref tick_string))
           {
            //--- reset tick
            MTTick tick=new MTTick();
            //--- read one tick and store it in the buffer
            if(ReadTick(tick_string,ref tick))
               ticks.Add(tick);
           }
         //--- successful
         return(true);
        }
      //+------------------------------------------------------------------+
      //| Connection to UniDDE server                                      |
      //+------------------------------------------------------------------+
      bool ConnectDDE(string server)
        {
         string ip  =string.Empty;
         int    port=0;
         int    cp  =0;
         //--- checking
         if(server==null || server.Length<1)
            return(false);
         try
           {
            //--- unparse the server and port
            ip=server;
            if((cp=ip.IndexOf(':'))!=-1)
              {
               if(!int.TryParse(ip.Substring(cp+1), out port))
                 {
                  ExtLogger.Out(EnMTLogCode.MTLogErr,"UniFeeder can't get port from {0}",ip);
                  return(false);
                 }
               ip=ip.Substring(0,cp);
              }
            //--- close the old connection
            Close();
            //--- refresh the time
            UpdateTime();
            //--- create a socket
            m_socket=new Socket(AddressFamily.InterNetwork,SocketType.Stream,ProtocolType.IP);
            m_socket.Connect(ip,port);
            //--- refresh the time
            UpdateTime();
            //--- everything is ok
            return(true);
           }
         catch(Exception ex)
           {
            Close();
            ExtLogger.Out(EnMTLogCode.MTLogErr,"UniFeeder socket connect to '{0}' failed [{1}]",server,ex.Message);
           }
         //---
         return(false);
        }
      //+------------------------------------------------------------------+
      //| Authorization                                                    |
      //+------------------------------------------------------------------+
      bool Login(string login,string password,string symbols)
        {
         bool   found=false;
         string resp  =string.Empty;
         //--- checking
         if(login==null || password==null || symbols==null)
            return(false);
         //--- send a command
         if(!SendString(login+"\r\n"+password+"\r\n"))
           {
            ExtLogger.Out(EnMTLogCode.MTLogErr,"UniFeeder socket login failed [{0}]",login);
            return(false);
           }
         //--- read the next string and check inclusion of the sub-string "Access granted"
         ReadStringCheck(ref resp,"Access granted",ref found);
         //--- if we couldn't find the string, error
         if(!found)
           {
            ExtLogger.Out(EnMTLogCode.MTLogErr,"UniFeeder socket invalid login [{0}]",login);
            return(false);
           }
         //--- login is confirmed
         m_pingtime=SMTTime.Time64();
         //--- send symbols
         if(symbols!=null && symbols.Length>0)
           {
            if(!SendString("> Symbols:") || !SendString(symbols) || !SendString("\r\n"))
              {
               ExtLogger.OutString(EnMTLogCode.MTLogErr,"UniFeeder socket symbols initialization failed");
               return(false);
              }
           }
         //--- everything is ok
         return (true);
        }
      //+------------------------------------------------------------------+ 
      //| Reading a tick from a string                                     | 
      //| Format: USDJPY 0 1.0106 1.0099                                   | 
      //+------------------------------------------------------------------+
      bool ReadTick(string str, ref MTTick tick)
        {
         //--- checking
         if(str==null)
            return(false);
         //---
         try
           {
            //--- remove useless characters
            str=str.Replace('\r',' ');
            //--- split string 
            string[] temp=str.Split(' ');
            //--- skip ping response
            if(temp.Length>1 && temp[0]==">" && temp[1].ToUpper()=="PING")
               return(false);
            //--- check it
            if(temp.Length<3)
              {
               ExtLogger.Out(EnMTLogCode.MTLogErr,"invalid tick input string '{0}'",str);
               return(false);
              }
            //--- form the tick symbol
            tick.symbol=temp[0];
            //--- bid price & ask price
            if(!double.TryParse(temp[1],m_double_style,m_double_format,out tick.bid) ||
               !double.TryParse(temp[2],m_double_style,m_double_format,out tick.ask))
              {
               ExtLogger.Out(EnMTLogCode.MTLogErr,"failed to parse tick, invalid bid/ask string '{0}'",str);
               return(false);
              }
           }
         catch(Exception ex)
           {
            ExtLogger.Out(EnMTLogCode.MTLogErr,"read tick failed [{0}]",ex.Message);
            return(false);
           }
         //--- everything is ok
         return(true);
        }
      //+------------------------------------------------------------------+
      //| Obtain the size of data in the socket                            |
      //+------------------------------------------------------------------+
      int IsReadable()
        {
         int size=0;
         try
           {
            //--- checking the socket validity
            if(m_socket!=null)
              {
               byte[] temp=BitConverter.GetBytes(0);
               int os=m_socket.IOControl(IOControlCode.DataToRead,null,temp);
               size=BitConverter.ToInt32(temp,0);
              }
           }
         catch
           {  
            Close();
            size=0;
           }
         //--- return the size
         return(size);
        }
      //+------------------------------------------------------------------+
      //| Read data from the socket                                        |
      //+------------------------------------------------------------------+
      bool DataUpdate()
        {
         int    len     =0;
         int    received=0;
         byte[] buffer  =null;
         string temp    =null;
         //---
         if(m_socket==null)
            return(false);
         //--- check if there are data in the socket
         if((len=IsReadable())<1)
            return(true);
         //--- 
         try
           {
            buffer  =new byte[len];
            received=m_socket.Receive(buffer);
            //--- lets check it
            if(received!=len)
              {
               ExtLogger.Out(EnMTLogCode.MTLogErr,"UniFeeder receive data failed - received({0}) != expected({1})",received,len);
               return(false);
              }
            //---
            temp     =Encoding.ASCII.GetString(buffer);
            m_string+=temp;
            //--- update incoming traffic
            m_parent.StateTraffic((uint)received,0);
            //--- refresh the time
            UpdateTime();
            //--- everything is ok
            return(true);
           }
         catch(Exception ex)
           {
            ExtLogger.Out(EnMTLogCode.MTLogErr,"UniFeeder update data failed [{0}]",ex.Message);
           }
         //---
         return(false);
        }
      //+------------------------------------------------------------------+
      //| Parse the line obtained from the socket                          |
      //+------------------------------------------------------------------+
      bool DataGetString(ref string buf)
        {
         int cp=0;
         //---
         if(m_string==null || m_string.Length<1)
            return(false);
         //---
         try
           {
            if((cp=m_string.IndexOf('\n'))!=-1)
              {
               buf     =m_string.Substring(0,cp);
               m_string=m_string.Substring(cp+1);
              }
            else
              {
               buf     =m_string;
               m_string=null;
              }
            //--- everything is ok
            return(true);
           }
         catch(Exception ex)
           {
            ExtLogger.Out(EnMTLogCode.MTLogErr,"UniFeeder socket parse input failed [{0}]",ex.Message);
           }
         //---
         return(false);
        }
      //+------------------------------------------------------------------+
      //| Data check                                                       |
      //+------------------------------------------------------------------+
      bool DataCheck()
        {
         //--- check connection
         if(m_socket==null)
            return(false);
         //--- send ping to DDE Connector server if necessary
         if(!SendPing())
           {
            ExtLogger.OutString(EnMTLogCode.MTLogErr,"UniFeeder socket ping failed");
            return(false);
           }
         //--- get all data from the socket to buffer m_data
         if(!DataUpdate())
           {
            ExtLogger.OutString(EnMTLogCode.MTLogErr,"UniFeeder socket connection lost");
            return(false);
           }
         //--- successful
         return(true);
        }
      //+------------------------------------------------------------------+
      //| Read data from the socket and compare to the preset substring    |
      //+------------------------------------------------------------------+
      bool ReadStringCheck(ref string buf,string str,ref bool found)
        {
         bool res=false;
         //--- checking
         if(buf==null || str==null)
            return(false);
         //--- reset the flag that we've found the line
         found=false;
         //--- reading data
         if(res=ReadString(ref buf))
            found=buf.IndexOf(str)!=-1;
         //--- return the result
         return(res);
        }
      //+------------------------------------------------------------------+
      //| Read the string from the socket                                  |
      //+------------------------------------------------------------------+
      bool ReadString(ref string result)
        {
         int         len  =MAX_BUFFER_LEN;
         int         start=0;
         int         count=0;
         byte[]      buf  =null;
         SocketError serr =SocketError.Fault;
         //--- checking
         if(m_socket==null)
            return(false);
         //---
         try
           {
            buf=new byte[len];
            //--- read data
            while(len>0)
              {
               int received=m_socket.Receive(buf,start,len,SocketFlags.None,out serr);
               if(received<1)
                 {
                  //--- checking errors of the socket
                  if(serr!=SocketError.WouldBlock || count>10)
                    {
                     Close();
                     return(false);
                    }
                  //--- read data
                  count++;
                  Thread.Sleep(50);
                  continue;
                 }
               //---
               len  -=received;
               start+=received;
               //--- reached the end of the string?
               if(buf[received-1]==13)
                  continue;
               if(buf[received-1]==10)
                  break;
              }
            //---
            result=Encoding.ASCII.GetString(buf);
            //--- update incoming traffic
            m_parent.StateTraffic((uint)start,0);
            //--- raise the time
            UpdateTime();
            //--- everything is ok
            return(true);
           }
         catch(Exception ex)
           {
            ExtLogger.Out(EnMTLogCode.MTLogErr,"UniFeeder ReadString failed [{0}]",ex.Message);
           }
         //---
         return(false);
        }
      //+------------------------------------------------------------------+
      //| Sending ping to DDE Connector                                    |
      //+------------------------------------------------------------------+
      bool SendPing()
        {
         Int64 ctm=SMTTime.Time64();
         //--- every minute send a ping signal to DDE Connector
         if((ctm-m_pingtime)>60)
           {
            //--- send the ping
            if(SendString("> Ping\r\n")==false)
              {
               ExtLogger.OutString(EnMTLogCode.MTLogErr,"UniFeeder ping failed");
               return(false);
              }
            //--- save the time of the last ping
            m_pingtime=ctm;
           }
         //--- everything is ok
         return(true);
        }
      //+------------------------------------------------------------------+
      //| Send the string                                                  |
      //+------------------------------------------------------------------+
      bool SendString(string buf)
        {
         int         count=0,res,len;
         int         start=0;
         byte[]      raw  =null;
         SocketError serr =SocketError.Fault;
         //--- checking
         if(buf==null || m_socket==null)
            return (false);
         try
           {
            //--- send data
            raw=Encoding.ASCII.GetBytes(buf);
            len=raw.Length;
            //---
            while(len>0)
              {
               if((res=m_socket.Send(raw,start,len,SocketFlags.None,out serr))<1)
                 {
                  //--- checking for the socket error
                  if(serr!=SocketError.WouldBlock || count>10)
                    {
                     Close();
                     return (false);
                    }
                  //--- write the rest of data
                  count++;
                  Thread.Sleep(50);
                  continue;
                 }
               //--- change positions
               start+=res;
               len  -=res;
              }
            //--- raise the time
            UpdateTime();
            //--- update outgoing traffic
            m_parent.StateTraffic(0,(uint)start);
            //--- everything is ok
            return(true);
           }
         catch(Exception ex)
           {
            ExtLogger.Out(EnMTLogCode.MTLogErr,"UniFeeder send string failed [{0}]",ex.Message);
           }
         //---
         return(false);
        }
     }
  }
//+------------------------------------------------------------------+
