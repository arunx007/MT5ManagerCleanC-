<%@ Control Language="C#" AutoEventWireup="true" Inherits="System.Web.Mvc.ViewUserControl<WebTrader.Models.Paginator>" %>
<%@ Import Namespace="WebTrader.Helpers" %>
<%  
    if(Model.PageCount==1)
        return;
    if(Model.CurrentPage>Model.PageCount)
        Model.CurrentPage=Model.PageCount;
    uint fromPage=Model.CurrentPage>7 ? Math.Max(1,Model.CurrentPage-7):1;
    uint toPage = Math.Min(Model.PageCount,Model.CurrentPage + 7);
    if(fromPage>1)
      {
       Response.Write(Html.RouteLinkAbsolut("1", Model.RouteName, new { page = 1 },new { }));
       Response.Write(" ");
       Response.Write(Html.RouteLinkAbsolut("...",Model.RouteName,new { page = fromPage },new { }));
       Response.Write(" ");
      }
    for(uint i=fromPage;i<=toPage;i++)
        {
        if(i==Model.CurrentPage)
            {%> <strong><%= Html.Encode(i)%></strong> <%}
       else
           {%> <%= Html.RouteLinkAbsolut(i.ToString(),Model.RouteName,new { page = i},new { })%> <%}
       }
    if(toPage<Model.PageCount)
      {
       Response.Write(" ");
       Response.Write(Html.RouteLinkAbsolut("...",Model.RouteName,new { page = toPage },new { }));
       Response.Write(" ");
       Response.Write(Html.RouteLinkAbsolut(Model.PageCount.ToString(),Model.RouteName,new { page = Model.PageCount },new { }));
      }%>
