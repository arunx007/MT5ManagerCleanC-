//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System.Text.RegularExpressions;
//---
namespace WebTrader.Models.Core.Validator
  {
  /// <summary>
  /// Validate
  /// string by regular expression 
  /// </summary>
  public class ValidatorRegexp : ValidatorBase
    {
    private readonly string m_RegExp;
    public string RegExp
      {
      get
        {
        return m_RegExp;
        }
      }
    //---
    private readonly string m_Flags;
    public string Flags
      {
      get
        {
        return m_Flags;
        }
      }
    /// <summary>
    /// Validate with regular expression 
    /// </summary>
    /// <param name="text">input string</param>
    /// <param name="regexp">regular expression</param>
    /// <param name="options">regular options</param>
    /// <returns></returns>
    public static bool Validate(string text,string regexp,RegexOptions options)
      {
      return Regex.IsMatch(text,regexp,options);
      }
    /// <summary>
    /// Validate with regular expression 
    /// </summary>
    /// <param name="text">input string</param>
    /// <param name="regexp">regular expression</param>
    /// <param name="flags">flags</param>
    /// <returns></returns>
    public ValidatorRegexp(string text,string regexp,string flags)
      : base(ValidatorType.Regexp,text)
      {
      m_RegExp = regexp;
      m_Flags = flags;
      }
    }
  }
