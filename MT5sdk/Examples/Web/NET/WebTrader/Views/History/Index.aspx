<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<WebTrader.Models.Views.ViewOrders>" %>
<%@ Import Namespace="MetaQuotes.MT5WebAPI.Common" %>
<%@ Import Namespace="WebTrader.Controllers" %>
<%@ Import Namespace="WebTrader.Models" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">History orders</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">
<h1>History orders</h1>
<%
    if(Model.Orders!=null)
      {
        
%>
        <table class="base_table" width="100%" cellspacing="0" cellpadding="0" border="0">
        <tr>
            <th style="width: 45px;">Symbol</th>
            <th style="width: 45px;">Order</th>
            <th style="width: 120px;">Date</th>
            <th style="width: 70px;">Type</th>
            <th style="width: 60px;">Volume</th>
            <th style="width: 60px;">Price</th>
            <th style="width: 60px;">S / L</th>
            <th style="width: 60px;">T / P</th>
            <th style="width: 60px;">Price current</th>
            <th style="width: 60px;">State</th>
            <th style="text-align:left">Comment</th>
        </tr>
<%
      int count = 0;
          foreach (MTOrder mtOrder in Model.Orders)
            {
            count++;
  %>
    <tr>
        <td><%=mtOrder.Symbol %></td>
        <td><%=mtOrder.Order %></td>
        <td><%=Date.ConvertFromUnixTime(mtOrder.TimeSetup).ToString("yyyy.MM.dd hh:mm") %></td>
        <td><%= mtOrder.Type%></td>
        <td><%=mtOrder.ContractSize%></td>
        <td><%=mtOrder.PriceOrder%></td>
        <td><%=mtOrder.PriceSL%></td>
        <td><%=mtOrder.PriceTP%></td>
        <td><%=mtOrder.PriceCurrent%></td>
        <td><%=mtOrder.State%></td>
        <td style="text-align:left"><%=mtOrder.Comment%></td>
    </tr>
  <%
}  
%>
        </table>
<%
    if(Model.CurrentPage > 1 || count >= HistoryController.ORDERS_PAGE)
          {
%>
<div style="padding: 10px 0 5px 0;">
         <span class="paginator">Page: <% 
                 
                Html.RenderPartial("~/Views/Shared/Paginator.ascx",
                new Paginator
                {
                    PageCount = count >= HistoryController.ORDERS_PAGE ? Model.CurrentPage + 1 : Model.CurrentPage,
                    RouteName = "HistoryPage",
                    CurrentPage = Model.CurrentPage,
                    
                }); %>
         </span>
      </div>
<%
          }
      }
     %>
</asp:Content>
