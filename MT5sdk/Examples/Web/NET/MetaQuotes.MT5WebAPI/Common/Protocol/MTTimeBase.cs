//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System;
using System.Collections;
using System.Collections.Generic;
using System.Web.Script.Serialization;
using MetaQuotes.MT5WebAPI.Common.Utils;
//---
namespace MetaQuotes.MT5WebAPI.Common.Protocol
  {
  /// <summary>
  /// class for work with time
  /// </summary>
  class MTTimeBase : MTAPIBase
    {
    public MTTimeBase(MTAsyncConnect connect) : base(connect) { }
    /// <summary>
    /// Get current server time
    /// </summary>
    public int TimeServer()
      {
      //--- get answer
      byte[] answer;
      //--- send request
      if((answer = Send(MTProtocolConsts.WEB_CMD_TIME_SERVER,null)) == null)
        {
        MTLog.Write(MTLogType.Error,"send time server failed");
        return 0;
        }
      //---
      string answerStr = MTUtils.GetString(answer);
      if(MTLog.IsWriteDebugLog) MTLog.Write(MTLogType.Debug,string.Format("result answer time: {0}",answerStr));
      //---
      MTRetCode errorCode;
      MTTimeServerAnswer timeInfo;
      //--- parse answer
      if((errorCode = ParseTimeServer(answerStr,out timeInfo)) != MTRetCode.MT_RET_OK)
        {
        MTLog.Write(MTLogType.Error,string.Format("parse time server failed: [{0}] {1}",errorCode,MTFormat.GetError(errorCode)));
        return 0;
        }
      //---
      return timeInfo.GetUnixTime();
      }
    /// <summary>
    /// Get time config
    /// </summary>
    public MTRetCode TimeGet(out MTConTime timeCon)
      {
      timeCon = null;
      //--- get answer
      byte[] answer;
      //--- send request
      if((answer = Send(MTProtocolConsts.WEB_CMD_TIME_GET,null)) == null)
        {
        if(MTLog.IsWriteDebugLog) MTLog.Write(MTLogType.Error,"send time get failed");
        return MTRetCode.MT_RET_ERR_NETWORK;
        }
      //---
      string answerStr = MTUtils.GetString(answer);
      if(MTLog.IsWriteDebugLog) MTLog.Write(MTLogType.Debug,string.Format("result answer: {0}",answerStr));
      //---
      MTRetCode errorCode;
      //--- parse answer
      if((errorCode = ParseTimeGet(answerStr,out timeCon)) != MTRetCode.MT_RET_OK)
        {
        MTLog.Write(MTLogType.Error,string.Format("parse time get failed: {0}",MTFormat.GetErrorStandart(errorCode)));
        return errorCode;
        }
      //---
      return MTRetCode.MT_RET_OK;
      }
    /// <summary>
    /// check answer from MetaTrader 5 server
    /// </summary>
    /// <param name="answer">answer from MT5 sever</param>
    /// <param name="timeAnswer">result parsing</param>
    private static MTRetCode ParseTimeServer(string answer,out MTTimeServerAnswer timeAnswer)
      {
      int pos = 0;
      timeAnswer = null;
      //--- get command answer
      string command = MTParseProtocol.GetCommand(answer,ref pos);
      if(command != MTProtocolConsts.WEB_CMD_TIME_SERVER)
        {
        MTLog.Write(MTLogType.Error,string.Format("answer command '{0}' is incorrect, wait {1}",command,MTProtocolConsts.WEB_CMD_TIME_SERVER));
        return MTRetCode.MT_RET_ERR_DATA;
        }
      //---
      timeAnswer = new MTTimeServerAnswer();
      //--- get param
      int posEnd = -1;
      MTAnswerParam param;
      while((param = MTParseProtocol.GetNextParam(answer,ref pos,ref posEnd)) != null)
        {
        switch(param.Name)
          {
          case MTProtocolConsts.WEB_PARAM_RETCODE:
          timeAnswer.RetCode = param.Value;
          break;
          case MTProtocolConsts.WEB_PARAM_TIME:
          timeAnswer.Time = param.Value;
          break;
          }
        }
      //--- check ret code
      MTRetCode errorCode;
      if((errorCode = MTParseProtocol.GetRetCode(timeAnswer.RetCode)) != MTRetCode.MT_RET_OK) return errorCode;
      //--- get time
      if(!timeAnswer.CheckTime()) return MTRetCode.MT_RET_ERR_PARAMS;
      //---
      return MTRetCode.MT_RET_OK;
      }
    /// <summary>
    /// check answer from MetaTrader 5 server
    /// </summary>
    /// <param name="answer">answer from MT server</param>
    /// <param name="timeCon">result parsing</param>
    private static MTRetCode ParseTimeGet(string answer,out MTConTime timeCon)
      {
      timeCon = null;
      //---
      int pos = 0;
      //--- get command answer
      string command = MTParseProtocol.GetCommand(answer,ref pos);
      if(command != MTProtocolConsts.WEB_CMD_TIME_GET)
        {
        MTLog.Write(MTLogType.Error,string.Format("answer command '{0}' is incorrect, wait {1}",command,MTProtocolConsts.WEB_CMD_TIME_GET));
        return MTRetCode.MT_RET_ERR_DATA;
        }
      //---
      MTTimeGetAnswer timeAnswer = new MTTimeGetAnswer();
      //--- get param
      int posEnd = -1;
      MTAnswerParam param;
      while((param = MTParseProtocol.GetNextParam(answer,ref pos,ref posEnd)) != null)
        {
        switch(param.Name)
          {
          case MTProtocolConsts.WEB_PARAM_RETCODE:
          timeAnswer.RetCode = param.Value;
          break;
          }
        }
      //---
      MTRetCode errorCode;
      //--- check ret code
      if((errorCode = MTParseProtocol.GetRetCode(timeAnswer.RetCode)) != MTRetCode.MT_RET_OK) return errorCode;
      //--- get json
      if((timeAnswer.ConfigJson = MTParseProtocol.GetJson(answer,posEnd)) == null) return MTRetCode.MT_RET_REPORT_NODATA;
      //--- теперь парсим Json
      timeCon = timeAnswer.GetFromJson();
      if(timeCon == null) return MTRetCode.MT_RET_REPORT_NODATA;
      //---
      return MTRetCode.MT_RET_OK;
      }
    }
  /// <summary>
  /// answer on request time_server
  /// </summary>
  public class MTTimeServerAnswer
    {
    public string RetCode { get; set; }
    public string Time { get; set; }
    /// <summary>
    /// 
    /// </summary>
    public MTTimeServerAnswer()
      {
      RetCode = "-1";
      Time = "none";
      }
    /// <summary>
    /// Get time in unix format
    /// </summary>

    public int GetUnixTime()
      {
      if(string.IsNullOrEmpty(Time) || Time == "none") return 0;
      //---
      string[] temp = Time.Split(' ');
      int unixTime;
      if(int.TryParse(temp[0],out unixTime)) return unixTime;
      return 0;
      }
    public bool CheckTime()
      {
      return !string.IsNullOrEmpty(Time) && Time != "none";
      }
    }
  /// <summary>
  /// answer on request time_get
  /// </summary>
  internal class MTTimeGetAnswer : MTBaseAnswerJson
    {
    /// <summary>
    /// From json get class MT5ConTime
    /// </summary>
    public MTConTime GetFromJson()
      {
      try
        {
        JavaScriptSerializer serializer = new JavaScriptSerializer { MaxJsonLength = MAX_LENGHT_JSON };
        serializer.RegisterConverters(new List<JavaScriptConverter> { new MTConTimeConverter() });
        return serializer.Deserialize<MTConTime>(ConfigJson);
        }
      catch(Exception e)
        {
        MTLog.Write(MTLogType.Error,string.Format("parsing time config from json failed, {0}",e));
        }
      return null;
      }
    }
  /// <summary>
  /// class parsin from json to MTConTime
  /// </summary>
  internal class MTConTimeConverter : JavaScriptConverter
    {
    private static readonly Type[] m_SupportedTypes = new[]
    {
        typeof( MTConTime )
    };
    public override IEnumerable<Type> SupportedTypes
      {
      get { return m_SupportedTypes; }
      }
    /// <summary>
    /// Deserialize from MT server json
    /// </summary>
    /// <param name="dictionary">data from json</param>
    /// <param name="type"></param>
    /// <param name="serializer"></param>
    public override object Deserialize(IDictionary<string,object> dictionary,Type type,JavaScriptSerializer serializer)
      {
      if(dictionary == null) return null;
      //---
      MTConTime obj = new MTConTime();
      //--- 
      if(dictionary.ContainsKey("Daylight"))
        obj.Daylight = MTDataHelper.GetInt32(dictionary["Daylight"]);
      //--- 
      if(dictionary.ContainsKey("DaylightState"))
        obj.DaylightState = MTDataHelper.GetInt32(dictionary["DaylightState"]);
      //---
      if(dictionary.ContainsKey("TimeZone"))
        obj.TimeZone = MTDataHelper.GetInt32(dictionary["TimeZone"]);
      //--- 
      if(dictionary.ContainsKey("TimeServer"))
        obj.TimeServer = MTDataHelper.GetString(dictionary["TimeServer"]);
      //---
      if(dictionary.ContainsKey("Days"))
        obj.Days = ParsingDays(dictionary["Days"] as ArrayList);
      //---
      return obj;
      }
    /// <summary>
    /// parsing data fro days
    /// </summary>
    /// <param name="arrayList"></param>
    private static MTConTime.EnTimeTableMode[][] ParsingDays(ArrayList arrayList)
      {
      if(arrayList == null) return null;
      try
        {
        //--- get array of days
        MTConTime.EnTimeTableMode[][] result = new MTConTime.EnTimeTableMode[arrayList.Count][];
        int i = 0;
        foreach(ArrayList values in arrayList)
          {
          if(values == null) continue;
          result[i] = new MTConTime.EnTimeTableMode[values.Count];
          int j = 0;
          foreach(string value in values)
            {
            result[i][j] = (MTConTime.EnTimeTableMode)MTDataHelper.GetInt32(value);
            j++;
            }
          //---
          i++;
          }
        //---
        return result;
        }
      catch(Exception e)
        {
        MTLog.Write(MTLogType.Error,string.Format("error pasing days in json, {0}",e));
        }
      return null;
      }
    public override IDictionary<string,object> Serialize(object obj,JavaScriptSerializer serializer)
      {
      return null;
      }
    }
  }
