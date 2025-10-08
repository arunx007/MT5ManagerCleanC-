//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
//---
using System;
using System.Collections.Generic;
using System.Threading;
using MetaQuotes.MT5WebAPI;
using MetaQuotes.MT5WebAPI.Common;
//---
namespace WebTrader.Models
  {
  /// <summary>
  /// Queue with data and callback functions
  /// </summary>
  public class QueueData
    {
    /// <summary>
    /// Name command
    /// </summary>
    public string Name { get; set; }
    /// <summary>
    /// Data
    /// </summary>
    public object Data { get; set; }
    /// <summary>
    /// Callback function
    /// </summary>
    public Action<MTRetCode,object> Callback { get; set; }
    /// <summary>
    /// Id command
    /// </summary>
    public long ID { get; set; }
    }
  /// <summary>
  /// Check user
  /// </summary>
  public class CheckLogin
    {
    public ulong Login { get; set; }
    public string Password { get; set; }
    }
  /// <summary>
  /// Get positions
  /// </summary>
  public class DataPage
    {
    public ulong Login { get; set; }
    public uint Offset { get; set; }
    public uint Total { get; set; }
    }
  /// <summary>
  /// Get orders
  /// </summary>
  public class DataOrderPage
    {
    public ulong Login { get; set; }
    public uint Offset { get; set; }
    public uint Total { get; set; }
    public long From { get; set; }
    public long To { get; set; }
    }
  /// <summary>
  /// Queue for request to metatrader server
  /// </summary>
  public class QueueRequest
    {
    private const int WAIT_LOG_TIMEOUT = 500;
    //--- blocked element
    private static readonly ReaderWriterLockSlim m_Locker = new ReaderWriterLockSlim();
    //--- max time blocked
    private const int LOCKER_TIME_OUT = 5000;
    static private readonly ManualResetEvent m_StopSignal = new ManualResetEvent(false);
    static private readonly AutoResetEvent m_FlushSignal = new AutoResetEvent(false);
    /// <summary>
    /// List of data for send to MT server
    /// </summary>
    private static readonly List<QueueData> m_Queue = new List<QueueData>();
    private static long m_MaxID;
    /// <summary>
    /// WebAPI class
    /// </summary>
    private static MT5WebAPI m_WebApi;
    static private Thread m_Thread;
    /// <summary>
    /// Start queues
    /// </summary>
    /// <returns></returns>
    static public bool Start()
      {
      //--- start new thread
      try
        {
        m_WebApi = new MT5WebAPI("WebTrader",Log.Write);
        m_WebApi.IsWriteDebugLog = true;
        //--- connect t MT server
        if(!Connect()) return false;
        //---
        m_Queue.Clear();
        m_Thread = new Thread(Work) { IsBackground = true };
        m_Thread.Start();
        //---
        Log.Write(LogType.Info,"queueRequest.Start","queue request started");
        }
      catch(Exception e)
        {
        Log.Write(LogType.Error,"queueRequest.Start",string.Format("not start queue request: {0}",e));
        return false;
        }
      //---  
      return true;
      }
    /// <summary>
    /// Create connection to MT server
    /// </summary>
    private static bool Connect()
      {
      MTRetCode code;
      if((code = m_WebApi.Connect(Settings.MTServer,Settings.MTServerPort,Settings.MTLogin,Settings.MTPassword,
                       Settings.MTPumps,Settings.MTCrypt,5000)) != MTRetCode.MT_RET_OK)
        {
        Log.Write(LogType.Error,"queueRequest.Start",string.Format("not start queue request: {0}",MTFormat.GetError(code)));
        return false;
        }
      Log.Write(LogType.Error,"queueRequest.Start","connected to MT");

      return true;
      }
    /// <summary>
    /// check queue and send request to metatrader server
    /// </summary>
    static private void Work()
      {
      bool needBreak = false;
      //---
      try
        {
        while(true)
          {
          int waitIndex = WaitHandle.WaitAny(new EventWaitHandle[] { m_StopSignal,m_FlushSignal },WAIT_LOG_TIMEOUT);
          //---
          if(waitIndex == 0)
            {
            needBreak = true;
            }
          //---
          SendData();
          //---
          if(needBreak)
            break;
          }
        }
      catch(ThreadAbortException ex)
        {
        Log.Write(LogType.Error,"queueRequest",string.Format("the queue thread is aborted: {0}",ex.Message));
        Thread.ResetAbort();
        }
      }
    /// <summary>
    /// Send datat to MT server
    /// </summary>
    private static void SendData()
      {
      QueueData data;
      while((data = GetFirst()) != null)
        {
        Log.Write(LogType.Info,"SendData","get data and try do it");
        //--- check connect, and exit
        if(!m_WebApi.IsConnected) if(!Connect()) return;
        //---
        MTRetCode code;
        switch(data.Name)
          {
          case "UserPasswordCheck":
            {
            //---
            CheckLogin loginData = data.Data as CheckLogin;
            if(loginData == null)
              {
              Log.Write(LogType.Error,"SendData","UserPasswordCheck set invalid data");
              Remove(data);
              continue;
              }
            //---
            code = m_WebApi.UserPasswordCheck(loginData.Login,loginData.Password);
            //---
            Log.Write(LogType.Error,"SendData",string.Format("UserPasswordCheck {0}",MTFormat.GetError(code)));
            //---
            data.Callback(code,null);
            //---
            break;
            }
          //--- add user
          case "UserAdd":
              {
              MTUser mtUser = data.Data as MTUser;
              if(mtUser == null)
                {
                Log.Write(LogType.Error,"SendData","UserAdd set invalid data");
                Remove(data);
                continue;
                }
              //---
              MTUser newUser;
              code = m_WebApi.UserAdd(mtUser,out newUser);
              //---
              if(code != MTRetCode.MT_RET_OK)
                {
                Log.Write(LogType.Error,"SendData",string.Format("UserAdd {0}",MTFormat.GetError(code)));
                //---
                data.Callback(code,newUser);
                break;
                }
              //--- change deposit, only for demo
              if(mtUser.Group.IndexOf("demo") > -1)
                code = m_WebApi.UserDepositChange(newUser.Login,Settings.MTDefaultDeposit,"default deposit new  user",
                                                  MTDeal.EnDealAction.DEAL_BALANCE);
              Log.Write(LogType.Error,"SendData",string.Format("UserAdd set new balance {0}",MTFormat.GetError(code)));
              //---
              data.Callback(code,newUser);
              //---
              break;
              }
          //--- get position by page
          case "PositionGetPage":
              DataPage dataPositions = data.Data as DataPage;
              if(dataPositions == null)
                {
                Log.Write(LogType.Error,"SendData","PositionGetPage set invalid data");
                Remove(data);
                continue;
                }
              //---
              List<MTPosition> positions;
              code = m_WebApi.PositionGetPage(dataPositions.Login,dataPositions.Offset,dataPositions.Total,
                                              out positions);
              //---
              Log.Write(LogType.Error,"SendData",string.Format("PositionGetPage {0}",MTFormat.GetError(code)));
              //---
              data.Callback(code,positions);
              //---
              break;
          //--- get history by page
          case "HistoryGetPage":
              DataOrderPage dataHistoryOrder = data.Data as DataOrderPage;
              if(dataHistoryOrder == null)
                {
                Log.Write(LogType.Error,"SendData","HistoryGetPage set invalid data");
                Remove(data);
                continue;
                }
              //---
              List<MTOrder> ordersHistory;
              code = m_WebApi.HistoryGetPage(dataHistoryOrder.Login,dataHistoryOrder.From,dataHistoryOrder.To,dataHistoryOrder.Offset,dataHistoryOrder.Total,
                                              out ordersHistory);
              //---
              Log.Write(LogType.Error,"SendData",string.Format("HistoryGetPage {0}",MTFormat.GetError(code)));
              //---
              data.Callback(code,ordersHistory);
              //---
              break;
          //--- get orders by page
          case "OrdersGetPage":
              DataOrderPage dataOrder = data.Data as DataOrderPage;
              if(dataOrder == null)
                {
                Log.Write(LogType.Error,"SendData","OrdersGetPage set invalid data");
                Remove(data);
                continue;
                }
              //---
              List<MTOrder> orders;
              code = m_WebApi.HistoryGetPage(dataOrder.Login,dataOrder.From,dataOrder.To,dataOrder.Offset,dataOrder.Total,
                                              out orders);
              //---
              Log.Write(LogType.Error,"SendData",string.Format("OrdersGetPage {0}",MTFormat.GetError(code)));
              //---
              data.Callback(code,orders);
              //---
              break;

          }
        //---
        Remove(data);
        }
      }
    /// <summary>
    /// Get first in queue
    /// </summary>
    /// <returns></returns>
    private static QueueData GetFirst()
      {
      try
        {
        if(m_Locker != null && m_Locker.TryEnterReadLock(LOCKER_TIME_OUT))
          {
          if(m_Queue.Count > 0) return m_Queue[0];
          }
        else
          Log.Write(LogType.Error,"GetFirst","can not get first element");
        }
      catch(Exception e)
        {
        Log.Write(LogType.Error,"GetFirst",string.Format("can not get first element: {0}",e));
        }
      finally
        {
        if(m_Locker != null && m_Locker.IsReadLockHeld)
          m_Locker.ExitReadLock();
        }
      //--- 
      return null;
      }
    /// <summary>
    /// Remove
    /// </summary>
    /// <param name="queue">element of queue</param>
    private static void Remove(QueueData queue)
      {
      try
        {
        if(m_Locker != null && m_Locker.TryEnterWriteLock(LOCKER_TIME_OUT))
          {
          if(m_Queue.Count > 0)
            {
            m_Queue.Remove(queue);
            Log.Write(LogType.Info,"Remove",string.Format("remove element id: {0}",queue.ID));
            }
          }
        else
          Log.Write(LogType.Error,"Remove","can not get first element");
        }
      catch(Exception e)
        {
        Log.Write(LogType.Error,"Remove",string.Format("can not get first element: {0}",e));
        }
      finally
        {
        if(m_Locker != null && m_Locker.IsWriteLockHeld)
          m_Locker.ExitWriteLock();
        }
      }
    /// <summary>
    /// Remove
    /// </summary>
    /// <param name="queue">elemet of queue</param>
    public static bool Add(QueueData queue)
      {
      if(queue == null) return false;
      try
        {
        long number = Interlocked.Increment(ref m_MaxID);
        queue.ID = number;
        //---
        if(m_Locker != null && m_Locker.TryEnterWriteLock(LOCKER_TIME_OUT))
          {
          m_Queue.Add(queue);
          Log.Write(LogType.Info,"Add",string.Format("add element in queue: {0}, {1}",queue.ID,queue.Name));
          return true;
          }
        else
          Log.Write(LogType.Error,"Add","can not get first element");
        }
      catch(Exception e)
        {
        Log.Write(LogType.Error,"Add",string.Format("can not get first element: {0}",e));
        }
      finally
        {
        if(m_Locker != null && m_Locker.IsWriteLockHeld)
          m_Locker.ExitWriteLock();
        }
      return false;
      }
    /// <summary>
    /// stop logging
    /// </summary>
    static public void Stop()
      {
      //--- Signal to stop the flushing thread
      m_StopSignal.Set();
      //--- Wait for correct writing of last data
      if((m_Thread != null) && (m_Thread.ThreadState != ThreadState.Unstarted))
        {
        if(!m_Thread.Join(4 * WAIT_LOG_TIMEOUT))
          {
          m_Thread.Abort();
          }
        }
      //--- close connection
      m_WebApi.Disconnect();
      }
    }
  }