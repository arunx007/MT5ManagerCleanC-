<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">404: Page not found</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">
<div style="height: 250px; padding: 80px 0 0 0;">
        <div style="text-align: left; float: left;">
            <h1 style="margin: 0 0 30px 0; font-size: 40px;">404</h1>
            <p><strong>The page you have requested does not exist</strong><br />
            Please check the page address and try again</p>
        </div>
    </div>
</asp:Content>
