//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
namespace WebTrader.Models.Core.Validator
  {
   public enum ValidatorType : int { Empty = 1, Length = 2, Numeric = 3, Email = 4, Equal = 5, Regexp = 6, Inn = 7, Url = 8, Radio = 9, Custom = 10, IP = 11, IsNotEqual=12}
  
   public class ValidatorBase
     {
      protected ValidatorType m_Type;
      protected string m_Message;
      public ValidatorType Type {get {return m_Type;}}
      public string Message {get {return m_Message;}}
     
      public ValidatorBase(ValidatorType type, string message)
        {
         m_Type = type;
         m_Message = message;
        }
      
      public ValidatorBase()
        {
         m_Type = 0;
         m_Message = "";
        }
     }
  }
