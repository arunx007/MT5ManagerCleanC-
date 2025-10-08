//+------------------------------------------------------------------+
//|                                           support.metaquotes.net |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Web;
//---  
namespace WebTrader.Models
  {
  /// <summary>
  /// log message type
  /// </summary>
  public enum LogType
  { Info = 1, Error = 2 };
  /// <summary>
  /// Class write logs
  /// </summary>
  static public class Log
    {
    static private Thread m_Thread;
    private static string m_Path = "";
    static private readonly StringBuilder m_Buffer = new StringBuilder();
    private const string PATH_NAME_FORMAT = "yyyy_MM";
    private const string FILE_NAME_FORMAT = "dd";
    private const string FILE_NAME_EXT = "log";
    static private string m_PathNow = "";
    static private int m_FileDayNow;
    static private StreamWriter m_StreamWriter;
    static private bool m_Do = true;
    /// <summary>
    /// Path for logs in application
    /// </summary>
    public static string LogPath { get { return m_Path; } }
    /// <summary>
    /// Start logs
    /// </summary>
    /// <param name="path">path to write log</param>
    /// <returns></returns>
    static public bool Start(string path)
      {
      try
        {
        if(!Directory.Exists(path)) Directory.CreateDirectory(path);
        m_Path = path;
        }
      catch(Exception e)
        {
        File.WriteAllText("c:\\temp\\error.txt",e.ToString());
        }
      //--- start new thread
      try
        {
        m_Thread = new Thread(DoWriteToFile) { IsBackground = true };
        m_Thread.Start();
        }
      catch(Exception) { return false; }
      //---  
      return true;
      }
    /// <summary>
    /// Periodicly write to file
    /// </summary>
    static private void DoWriteToFile()
      {
      while(m_Do)
        {
        WriteToFile();
        Thread.Sleep(2000);
        }
      }
    /// <summary>
    /// Write in logs
    /// </summary>    
    static public void Write(int status,string message)
      {
      StringBuilder sb = new StringBuilder(DateTime.Now.ToString("HH:mm:ss\t"));
      sb.Append(status);
      sb.Append("\t");
      //--- get UserHostAddress
      sb.Append("None");
      sb.Append("\t");
      //--- 
      sb.Append("\t");
      sb.Append("fromWebAPI");
      sb.Append("\t");
      string url = "";
      //---          
      //sb.Append("\t");
      if(!string.IsNullOrEmpty(message))
        {
        foreach(string line in message.Split(new[] { "\r\n","\n" },StringSplitOptions.RemoveEmptyEntries))
          {
          StringBuilder lineBuilder = new StringBuilder(sb.ToString());
          lineBuilder.Append(url);
          url = string.Empty;
          lineBuilder.Append("\t");
          lineBuilder.Append(line);
          //recordBuilder.Append(message.Replace("\r\n","\n").Replace("\t"," "));
          lineBuilder.Append("\r\n");
          //--- This lock is used to prevent changes in buffer length during comparison
          lock(m_Buffer)
            {
            m_Buffer.Append(lineBuilder);
            }
          }
        }
      else
        {
        //---
        sb.Append("\r\n");
        lock(m_Buffer)
          {
          m_Buffer.Append(sb.ToString());
          }
        }
      //--- This lock is used to prevent changes in buffer length during comparison
      if(m_Buffer.Length > 64000)
        WriteToFile();
      }
    /// <summary>
    /// write log
    /// </summary>    
    static public void Write(LogType status,string functionName,string message)
      {
      StringBuilder sb = new StringBuilder(DateTime.Now.ToString("HH:mm:ss\t"));
      sb.Append((int)status);
      sb.Append("\t");
      //--- get UserHostAddress
      string ip;
      try
        {
        if(HttpContext.Current != null && HttpContext.Current.Request != null)
          ip = HttpContext.Current.Request.UserHostAddress;
        else ip = "None";
        }
      catch(Exception)
        {
        ip = "None";
        }
      sb.Append(ip);
      //---          
      sb.Append("\t");
      string url = "";
      try
        {
        if(HttpContext.Current != null)
          url = HttpContext.Current.Request.RawUrl;
        }
      catch(Exception)
        {
        url = "None";
        }
      sb.Append(functionName);
      sb.Append("\t");
      if(!string.IsNullOrEmpty(message))
        {
        foreach(string line in message.Split(new[] { "\r\n","\n" },StringSplitOptions.RemoveEmptyEntries))
          {
          StringBuilder lineBuilder = new StringBuilder(sb.ToString());
          lineBuilder.Append(url);
          url = string.Empty;
          lineBuilder.Append("\t");
          lineBuilder.Append(line);
          lineBuilder.Append("\r\n");
          //--- This lock is used to prevent changes in buffer length during comparison
          lock(m_Buffer)
            {
            m_Buffer.Append(lineBuilder);
            }
          }
        }
      else
        {
        //---
        sb.Append("\r\n");
        lock(m_Buffer)
          {
          m_Buffer.Append(sb.ToString());
          }
        }
      //--- This lock is used to prevent changes in buffer length during comparison
      if(m_Buffer.Length > 64000)
        WriteToFile();
      }
    /// <summary>
    /// Запись в файл
    /// </summary>
    static private void WriteToFile()
      {
      lock(m_Buffer)
        {
        if(m_Thread == null)
          return;
        //---
        if(m_Buffer.Length > 0)
          {
          DateTime time = DateTime.Now;
          string path = time.ToString(PATH_NAME_FORMAT);
          int fileday = time.Day;
          //--- change the day and change path
          if(path != m_PathNow || fileday != m_FileDayNow)
            {
            if(m_StreamWriter != null)
              m_StreamWriter.Close();
            //---
            m_FileDayNow = fileday;
            m_PathNow = path;
            }
          //--- get new log file name
          StringBuilder filename = new StringBuilder();
          filename.Append(Path.Combine(LogPath,m_PathNow));
          //--- create path if it need
          string fname = filename.ToString();
          if(!Directory.Exists(fname))
            Directory.CreateDirectory(fname);
          filename.AppendFormat("\\{0}.{1}",time.ToString(FILE_NAME_FORMAT),FILE_NAME_EXT);
          fname = filename.ToString();
          //--- create new file or write in exists file
          if(File.Exists(fname))
            try
              {
              if(m_StreamWriter == null)
                m_StreamWriter = new StreamWriter(new FileStream(fname,FileMode.Append,FileAccess.Write,FileShare.Read));
              }
            catch(Exception)
              {
              return;
              }
          else
            m_StreamWriter = new StreamWriter(new FileStream(fname,FileMode.CreateNew,FileAccess.Write,FileShare.Read));
          //--- write buffer in file
          if(m_StreamWriter != null)
            {
            try
              {
              m_StreamWriter.Write(m_Buffer.ToString());
              m_StreamWriter.Flush();
              }
            catch(Exception)
              {
              return;
              }
            }
          //--- remove only after success write
          m_Buffer.Remove(0,m_Buffer.Length);
          }
        }
      }
    /// <summary>
    /// Log stop
    /// </summary>
    static public void Stop()
      {
      WriteToFile();
      if(m_StreamWriter != null)
        {
        m_StreamWriter.Close();
        //--- close file stream
        if(m_StreamWriter.BaseStream != null)
          m_StreamWriter.BaseStream.Dispose();
        m_StreamWriter.Dispose();
        }
      m_Do = false;
      //--- stop thread
      if(m_Thread != null)
        m_Thread.Join();
      }
    }
  }