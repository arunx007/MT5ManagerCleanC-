<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<WebTrader.Models.Views.ViewPositions>" %>
<%@ Import Namespace="MetaQuotes.MT5WebAPI.Common" %>
<%@ Import Namespace="WebTrader.Controllers" %>
<%@ Import Namespace="WebTrader.Models" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">Positions</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">
    <h1>Positions</h1>
<%
    if(Model.Positions!=null)
      {
%>
        <table class="base_table" width="100%" cellspacing="0" cellpadding="0" border="0">
        <tr>
            <th style="width: 45px;">Symbol</th>
            <th style="width: 120px;">Date</th>
            <th style="width: 70px;">Type</th>
            <th style="width: 60px;">Volume</th>
            <th style="width: 60px;">Price</th>
            <th style="width: 60px;">S / L</th>
            <th style="width: 60px;">T / P</th>
            <th style="width: 60px;">Price current</th>
            <th style="width: 60px;">Profit</th>
            <th style="text-align:left">Comment</th>
        </tr>
<%
      int count = 0;
          foreach (MTPosition mtPosition in Model.Positions)
            {
            count++;
  %>
    <tr>
        <td><%=mtPosition.Symbol %></td>
        <td><%=Date.ConvertFromUnixTime(mtPosition.TimeCreate).ToString("yyyy.MM.dd hh:mm") %></td>
        <td><%=mtPosition.Action == MTPosition.EnPositionAction.POSITION_BUY ? "buy":"sell"%></td>
        <td><%=mtPosition.ContractSize%></td>
        <td><%=mtPosition.PriceOpen%></td>
        <td><%=mtPosition.PriceSL%></td>
        <td><%=mtPosition.PriceTP%></td>
        <td><%=mtPosition.PriceCurrent%></td>
        <td><%=mtPosition.Profit%></td>
        <td style="text-align:left"><%=mtPosition.Comment%></td>
    </tr>
  <%
}  
%>
        </table>
<%
        if(Model.CurrentPage>1 || count>=PositionsController.POSITIONS_PAGE)
          {
%>
<div style="padding: 10px 0 5px 0;">
         <span class="paginator">Page: <% 
                 
                Html.RenderPartial("~/Views/Shared/Paginator.ascx",
                new Paginator
                {
                    PageCount = count >= PositionsController.POSITIONS_PAGE ? Model.CurrentPage + 1 : Model.CurrentPage,
                    RouteName = "PositionsPage",
                    CurrentPage = Model.CurrentPage,
                    
                }); %>
         </span>
      </div>
<%
          }
      }
     %>
</asp:Content>
