//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System;
using System.Collections.Generic;
using System.Web.Script.Serialization;
using MetaQuotes.MT5WebAPI.Common.Utils;
//---
namespace MetaQuotes.MT5WebAPI.Common.Protocol
  {
  /// <summary>
  /// Send common_get to MetaTrader 5 Server
  /// </summary>
  class MTCommonBase : MTAPIBase
    {
    public MTCommonBase(MTAsyncConnect connect) : base(connect) { }
    /// <summary>
    /// send request common_get
    /// </summary>
    /// <param name="common">config from MT5 server</param>
    /// <returns></returns>
    public MTRetCode CommonGet(out MTConCommon common)
      {
      common = null;
      //--- get answer
      byte[] answer;
      //--- send request
      if((answer = Send(MTProtocolConsts.WEB_CMD_COMMON_GET,null)) == null)
        {
        MTLog.Write(MTLogType.Error,"send common get failed");
        return MTRetCode.MT_RET_ERR_NETWORK;
        }
      //---
      string answerStr = MTUtils.GetString(answer);
      if(MTLog.IsWriteDebugLog) MTLog.Write(MTLogType.Debug,string.Format("result answer: {0}",answerStr));
      //--- parse answer
      MTRetCode errorCode;
      if((errorCode = ParseCommonGet(answerStr,out common)) != MTRetCode.MT_RET_OK)
        {
        MTLog.Write(MTLogType.Error,string.Format("parse common get failed: {0}",MTFormat.GetErrorStandart(errorCode)));
        return errorCode;
        }
      //---
      return MTRetCode.MT_RET_OK;
      }
    /// <summary>
    /// check answer from MetaTrader 5 server
    /// </summary>
    /// <param name="answer">answer from MT5 sever</param>
    /// <param name="conCommon">result parsing</param>
    private static MTRetCode ParseCommonGet(string answer,out MTConCommon conCommon)
      {
      int pos = 0;
      conCommon = null;
      //--- get command answer
      string command = MTParseProtocol.GetCommand(answer,ref pos);
      if(command != MTProtocolConsts.WEB_CMD_COMMON_GET)
        {
        MTLog.Write(MTLogType.Error,string.Format("answer command '{0}' is incorrect, wait {1}",command,MTProtocolConsts.WEB_CMD_COMMON_GET));
        return MTRetCode.MT_RET_ERR_DATA;
        }
      //---
      MTCommonGetAnswer commonAnswer = new MTCommonGetAnswer();
      //--- get param
      int posEnd = -1;
      MTAnswerParam param;
      while((param = MTParseProtocol.GetNextParam(answer,ref pos,ref posEnd)) != null)
        {
        switch(param.Name)
          {
          case MTProtocolConsts.WEB_PARAM_RETCODE:
          commonAnswer.RetCode = param.Value;
          break;
          }
        }
      //---
      MTRetCode errorCode;
      //--- check ret code
      if((errorCode = MTParseProtocol.GetRetCode(commonAnswer.RetCode)) != MTRetCode.MT_RET_OK) return errorCode;
      //--- get json
      if((commonAnswer.ConfigJson = MTParseProtocol.GetJson(answer,posEnd)) == null) return MTRetCode.MT_RET_REPORT_NODATA;
      //--- parsing Json
      conCommon = commonAnswer.GetFromJson();
      //--- parsing empty
      if(conCommon == null) return MTRetCode.MT_RET_REPORT_NODATA;
      //---
      return MTRetCode.MT_RET_OK;
      }
    }
  /// <summary>
  /// answer on request common_get
  /// </summary>
  internal class MTCommonGetAnswer : MTBaseAnswerJson
    {
    /// <summary>
    /// From json get class MT5ConTime
    /// </summary>
    public MTConCommon GetFromJson()
      {
      try
        {
        JavaScriptSerializer serializer = new JavaScriptSerializer { MaxJsonLength = MAX_LENGHT_JSON };
        serializer.RegisterConverters(new List<JavaScriptConverter> { new MTConCommonConverter() });
        return serializer.Deserialize<MTConCommon>(ConfigJson);
        }
      catch(Exception e)
        {
        MTLog.Write(MTLogType.Error,string.Format("parsing common config from json failed, {0}",e));
        }
      return null;
      }
    }
  /// <summary>
  /// class parsin from json to MTConCommon
  /// </summary>
  internal class MTConCommonConverter : JavaScriptConverter
    {
    private static readonly Type[] m_SupportedTypes = new[] { typeof(MTConCommon) };
    public override IEnumerable<Type> SupportedTypes { get { return m_SupportedTypes; } }
    /// <summary>
    /// Deserialize from MT server json
    /// </summary>
    /// <param name="dictionary">data from json</param>
    /// <param name="type">type</param>
    /// <param name="serializer">class serializator</param>
    public override object Deserialize(IDictionary<string,object> dictionary,Type type,JavaScriptSerializer serializer)
      {
      if(dictionary == null) return null;
      //---
      MTConCommon obj = new MTConCommon();
      //--- 
      if(dictionary.ContainsKey("Name"))
        obj.Name = MTDataHelper.GetString(dictionary["Name"]);
      //--- 
      if(dictionary.ContainsKey("Owner"))
        obj.Owner = MTDataHelper.GetString(dictionary["Owner"]);
      //---
      if(dictionary.ContainsKey("OwnerID"))
        obj.OwnerID = MTDataHelper.GetString(dictionary["OwnerID"]);
      //---
      if(dictionary.ContainsKey("OwnerHost"))
        obj.OwnerHost = MTDataHelper.GetString(dictionary["OwnerHost"]);
      //---
      if(dictionary.ContainsKey("OwnerEmail"))
        obj.OwnerEmail = MTDataHelper.GetString(dictionary["OwnerEmail"]);
      //---
      if(dictionary.ContainsKey("Product"))
        obj.Product = MTDataHelper.GetString(dictionary["Product"]);
      //---
      if(dictionary.ContainsKey("ExpirationLicense"))
        obj.ExpirationLicense = MTDataHelper.GetInt64(dictionary["ExpirationLicense"]);
      //---
      if(dictionary.ContainsKey("ExpirationSupport"))
        obj.ExpirationSupport = MTDataHelper.GetInt64(dictionary["ExpirationSupport"]);
      //---
      if(dictionary.ContainsKey("LimitTradeServers"))
        obj.LimitTradeServers = MTDataHelper.GetUInt32(dictionary["LimitTradeServers"]);
      //---
      if(dictionary.ContainsKey("LimitWebServers"))
        obj.LimitWebServers = MTDataHelper.GetUInt32(dictionary["LimitWebServers"]);
      //---
      if(dictionary.ContainsKey("LimitAccounts"))
        obj.LimitAccounts = MTDataHelper.GetUInt32(dictionary["LimitAccounts"]);
      //---
      if(dictionary.ContainsKey("LimitDeals"))
        obj.LimitDeals = MTDataHelper.GetUInt32(dictionary["LimitDeals"]);
      //---
      if(dictionary.ContainsKey("LimitSymbols"))
        obj.LimitSymbols = MTDataHelper.GetUInt32(dictionary["LimitSymbols"]);
      //---
      if(dictionary.ContainsKey("LimitGroups"))
        obj.LimitGroups = MTDataHelper.GetUInt32(dictionary["LimitGroups"]);
      //---
      if(dictionary.ContainsKey("LiveUpdateMode"))
        obj.LiveUpdateMode = (EnUpdateMode)MTDataHelper.GetUInt32(dictionary["LiveUpdateMode"]);
      //---
      if(dictionary.ContainsKey("TotalUsers"))
        obj.TotalUsers = MTDataHelper.GetUInt32(dictionary["TotalUsers"]);
      //---
      if(dictionary.ContainsKey("TotalUsersReal"))
        obj.TotalUsersReal = MTDataHelper.GetUInt32(dictionary["TotalUsersReal"]);
      //---
      if(dictionary.ContainsKey("TotalDeals"))
        obj.TotalDeals = MTDataHelper.GetUInt32(dictionary["TotalDeals"]);
      //---
      if(dictionary.ContainsKey("TotalOrders"))
        obj.TotalOrders = MTDataHelper.GetUInt32(dictionary["TotalOrders"]);
      //---
      if(dictionary.ContainsKey("TotalOrdersHistory"))
        obj.TotalOrdersHistory = MTDataHelper.GetUInt32(dictionary["TotalOrdersHistory"]);
      //---
      if(dictionary.ContainsKey("TotalPositions"))
        obj.TotalPositions = MTDataHelper.GetUInt32(dictionary["TotalPositions"]);
      //---
      if (dictionary.ContainsKey("AccountUrl"))
        obj.AccountURL = MTDataHelper.GetString(dictionary["AccountUrl"]);
      //---
      if (dictionary.ContainsKey("AccountAuto"))
        obj.AccountAuto = MTDataHelper.GetUInt32(dictionary["AccountAuto"]);
      //---
      return obj;
      }
    public override IDictionary<string,object> Serialize(object obj,JavaScriptSerializer serializer)
      {
      return null;
      }

    }

  }
