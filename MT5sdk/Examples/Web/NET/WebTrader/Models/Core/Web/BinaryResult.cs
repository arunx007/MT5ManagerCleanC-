//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System.Net.Mime;
using System.Web.Mvc;
//---   
namespace WebTrader.Models.Core.Web
  {
  /// <summary>
  /// Binary HTTP-response
  /// </summary>
  public class BinaryResult : ContentResult
    {
    public int FromByte { get; set; }
    public string FileName { get; set; }
    public byte[] ContentBytes { get; set; }
    //---
    public BinaryResult(byte[] input,string contentType)
      : this(input,0,contentType,null) { }
    //---
    public BinaryResult(byte[] input,int fromByte,string contentType,string fileName)
      {
      FromByte = fromByte;
      FileName = fileName;
      ContentBytes = input;
      ContentType = contentType;
      }
    //---
    public override void ExecuteResult(ControllerContext context)
      {
      if(FromByte == 0 && (ContentBytes.Length - FromByte) <= 0) { return; }
      //---
      if(!string.IsNullOrEmpty(FileName)) { context.HttpContext.Response.AddHeader("Content-Disposition",new ContentDisposition { FileName = FileName }.ToString()); }
      if(!string.IsNullOrEmpty(ContentType)) { context.HttpContext.Response.ContentType = ContentType; }
      //---
      context.HttpContext.Response.OutputStream.Write(ContentBytes,FromByte,ContentBytes.Length - FromByte);
      }
    }
  }
