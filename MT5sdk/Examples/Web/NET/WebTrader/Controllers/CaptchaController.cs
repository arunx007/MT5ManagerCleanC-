//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;
using System.Web.Mvc;
using WebTrader.Models.Core.Controllers;
using WebTrader.Models.Core.Web;
//---
namespace WebTrader.Controllers
  {
  /// <summary>
  /// Getting captha for page
  /// </summary>
  public class CaptchaController : BaseController
    {
    public ActionResult Index()
      {
      string text = GetTextFromSession();
      //---  
      if(string.IsNullOrEmpty(text))
        return new EmptyResult();
      //---  
      return GetCaptchaResult(text);
      }
    /// <summary>
    /// Get text from session
    /// </summary>
    /// <returns></returns>
    private string GetTextFromSession() { return (string)HttpContext.Session["captcha"]; }
    /// <summary>
    /// Get image with text
    /// </summary>
    /// <param name="text">text in image</param>
    /// <returns></returns>
    private ActionResult GetCaptchaResult(string text)
      {
      const int WIDTH = 100;
      const int HEIGHT = 40;
      //--- rand
      Random rnd = new Random(DateTime.Now.Millisecond);
      //--- new image
      Image im = new Bitmap(WIDTH,HEIGHT,PixelFormat.Format32bppArgb);
      Graphics g = Graphics.FromImage(im);
      g.SmoothingMode = SmoothingMode.AntiAlias;
      //--- get fonts
      FontFamily[] famArray = new[] { new FontFamily("Courier New") };
      //--- font
      int famIndex = rnd.Next(0,famArray.Length);
      const int FONT_SIZE = 24;
      //--- rect images
      Rectangle rect = new Rectangle(0,0,WIDTH,HEIGHT);
      //--- background
      SolidBrush solidBrush = new SolidBrush(Color.White);
      g.FillRectangle(solidBrush,rect);
      solidBrush.Dispose();
      //--- path
      GraphicsPath path = new GraphicsPath();
      const int Y_POS = 5;
      Point pointStart = new Point(2,Y_POS);
      //--- draw word
      for(int i = 0; i < text.Length; i++)
        {
        string substr = text.Substring(i,1);
        Font font = new Font(famArray[famIndex],FONT_SIZE + rnd.Next(-1,4),FontStyle.Regular);
        path.AddString(substr,font.FontFamily,(int)font.Style,font.Size,pointStart,StringFormat.GenericDefault);
        pointStart.X += 16;
        pointStart.Y = Y_POS + rnd.Next(-3,3);
        font.Dispose();
        }
      //--- fill word
      SolidBrush fillBrush = new SolidBrush(Color.Black);
      g.FillPath(fillBrush,path);
      fillBrush.Dispose();
      //---
      path.Dispose();
      //--- Save to array
      byte[] ret;
      using(MemoryStream ms = new MemoryStream())
        {
        im.Save(ms,ImageFormat.Gif);
        ret = ms.ToArray();
        }
      im.Dispose();
      g.Dispose();
      //--- delete cache in brawser
      Response.AddHeader("Expires",DateTime.Now.ToString("R"));
      //---  
      return new BinaryResult(ret,"image/gif");
      }
    }
  }
