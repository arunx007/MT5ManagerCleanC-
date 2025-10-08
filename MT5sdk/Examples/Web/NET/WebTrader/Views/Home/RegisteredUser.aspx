<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<WebTrader.Models.Views.ViewRegister>" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">Registration on MetaTreder 5</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">
    <div style="text-align: center">
        <h1 class="interface">Registration on MetaTreder 5</h1>
    </div>
    <div class="formRegister" style="width:440px;margin: 0px auto;">
        <div>You registred on MetaTrader 5 Server</div>
        <div>Login: <%=Model.User.Login%></div>
        <div>Password: *******</div>
    </div>
</asp:Content>

