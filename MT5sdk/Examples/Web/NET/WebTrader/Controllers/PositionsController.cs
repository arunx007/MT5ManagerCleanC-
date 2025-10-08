//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System.Collections.Generic;
using System.Web.Mvc;
using MetaQuotes.MT5WebAPI;
using MetaQuotes.MT5WebAPI.Common;
using WebTrader.Models;
using WebTrader.Models.Core.Controllers;
using WebTrader.Models.Users;
using WebTrader.Models.Views;
//---
namespace WebTrader.Controllers
  {
  public class PositionsController : BaseAsyncController
    {
    public const int POSITIONS_PAGE = 20;
    private List<MTPosition> m_Positions;
    private uint m_Page;
    /// <summary>
    /// Index
    /// </summary>
    /// <param name="page"></param>
    public void IndexAsync(uint? page)
      {
      if(!UserState.IsLoggedIn)
        {
        Http404NotFound("");
        return;
        }
      //---
      AsyncManager.OutstandingOperations.Increment();
      //---
      m_Page = page ?? 1;
      //---
      QueueData data = new QueueData { Callback = OnPositionGetPage };
      //---
      data.Data = new DataPage
                    {
                      Login = UserState.User.Login,
                      Offset = (m_Page-1) * POSITIONS_PAGE,
                      Total = POSITIONS_PAGE
                    };
      data.Name = "PositionGetPage";
      //---
      QueueRequest.Add(data);
      }
    /// <summary>
    /// View
    /// </summary>
    /// <returns></returns>
    public ActionResult IndexCompleted()
      {
      if(!UserState.IsLoggedIn)
        {
        return Http404NotFound("");
        }
      //---
      ViewPositions viewPositions = new ViewPositions { Positions = m_Positions,CurrentPage = m_Page <= 0 ? 1 : m_Page };
      return View("Index",viewPositions);
      }
    /// <summary>
    /// Get data of positions
    /// </summary>
    /// <param name="result"></param>
    /// <param name="data"></param>
    public void OnPositionGetPage(MTRetCode result,object data)
      {
      if(result != MTRetCode.MT_RET_OK)
        {
        Log.Write(LogType.Error,"OnPositionGetPage","positions did not get from MetaTrader 5 server");
        }
      else
        {
        m_Positions = data as List<MTPosition>;
        if(m_Positions == null)
          {
          ViewData.ModelState.AddModelError("common","positions did not get from MetaTrader 5 server");
          return;
          }
        //---
        Log.Write(LogType.Error,"OnCheckUser",string.Format("positions get total: {0}",m_Positions.Count));
        }
      //---
      AsyncManager.OutstandingOperations.Decrement();
      }
    }
  }
