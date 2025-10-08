//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
namespace WebTrader.Models.Core.Validator
  {
  /// <summary>
  /// Validate the length of string
  /// </summary>
  public class ValidatorLength:ValidatorBase
    {
    private readonly int m_MinLength;
    public int MinLength { get { return m_MinLength; } }
    //---
    private readonly int m_MaxLength;
    public int MaxLength { get { return m_MaxLength; } }
    /// <summary>
    /// Validate the length of string
    /// </summary>
    /// <param name="text">input string</param>
    /// <param name="minlength">min length for string</param>
    static public bool Validate(string text,int minlength)
      {
      return text!=null&&text.Length>=minlength;
      }
    /// <summary>
    /// Validate the length of string
    /// </summary>
    /// <param name="text">input string</param>
    /// <param name="minlength">min length for string</param>
    /// <param name="maxlength">max length for string</param>
    static public bool Validate(string text,int minlength,int maxlength)
      {
      return text!=null&&text.Length>=minlength&&text.Length<=maxlength;
      }
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="message">string</param>
    /// <param name="minlength">min length</param>
    public ValidatorLength(string message,int minlength)
      : base(ValidatorType.Length,message)
      {
      m_MinLength=minlength;
      }
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="message">string</param>
    /// <param name="minlength">min length</param>
    /// <param name="maxlength">max length</param>
    public ValidatorLength(string message,int minlength,int maxlength)
      : base(ValidatorType.Length,message)
      {
      m_MinLength=minlength;
      m_MaxLength=maxlength;
      }
    }
  }
