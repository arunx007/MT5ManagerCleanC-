//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
namespace WebTrader.Models.Core.Validator
  {
  /// <summary>
  /// Check equals between two string
  /// </summary>
   public class ValidatorEqual : ValidatorBase
     {
      private readonly string m_Example;
      public string Example { get {return m_Example;} } 
      //---
      private readonly bool m_IgnoreCase;
      public bool IgnoreCase { get {return m_IgnoreCase;} }      
      /// <summary>
      /// validate two input
      /// </summary>
      /// <param name="message">mesage error</param>
      /// <param name="example">name of input</param>
      /// <param name="ignoreCase">name of input</param>
      public ValidatorEqual(string message, string example, bool ignoreCase) : base (ValidatorType.Equal,message)
        {
         m_Example = example;
         m_IgnoreCase = ignoreCase;
        }
      /// <summary>
      /// validate two input
      /// </summary>
      /// <param name="message">mesage error</param>
      /// <param name="example">name of input</param>
      public ValidatorEqual(string message, string example) : base (ValidatorType.Equal,message)
        {
         m_Example = example;
         m_IgnoreCase = false;
        }
      /// <summary>
      /// CompareTo string
      /// </summary>
      /// <param name="text">first string</param>
      /// <param name="example">second string</param>
      /// <returns>is compare this strings</returns>
      static public bool Validate(string text, string example)
        {
         return text.CompareTo(example)==0;
        }
     }
  }
