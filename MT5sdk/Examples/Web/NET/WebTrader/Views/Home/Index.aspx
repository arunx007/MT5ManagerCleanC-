<%@ Page Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage" %>
<%@ Import Namespace="WebTrader.Models.Core.Validator" %>
<%@ Import Namespace="WebTrader.Models.Users" %>
<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">Home Page</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">
    <% 
  if(!UserState.IsLoggedIn)
    {
    using(Html.BeginFormValidated("","Home",FormMethod.Post, new {style="margin: 0px;"}))
           { %>
   <div>	 
      <div style="text-align: center">
          <h1 class="interface">Welcome to MetaQuotes Software Web Trader!</h1>
      </div>
      <div class="formLogin" style="width:400px;margin: 0px auto;">
        <div style="margin: 5px auto;text-align: center;"><%= Html.Validator("common",new ValidatorBase())%></div>
        <div class="line" style="margin-top: 10px;">
            <label for="login">Login:</label>
            <div class="offset">
                <a href="/register" style="float: right;margin-right: 7px;">Registration</a>
                <div class="inputWrapper"><%= Html.TextBox("login", null, new { style = "width: 180px", tabindex = "1", title = "Enter the login please, only numbers" })%></div>
            </div>
        </div>
        <div class="offset">
            <%= Html.Validator("login",new ValidatorNumeric("Login is incorrect"))%>
        </div>
        <div class="line">
            <label for="password">Password:</label>
            <div class="offset"><div class="inputWrapper"><%= Html.Password("password", null, new { style = "width: 180px", tabindex = "2", title = "Enter the password please", autocomplete = "off", })%></div></div>
        </div>
        <div class="offset">
            <%= Html.Validator("password",new ValidatorEmpty("Password is empty"))%>
        </div>
        <div class="offset" style="line-height:20px; vertical-align:top;">
            <input type="submit" tabindex="4" class="buttonActive" style="width: 100px;margin:10px 0px;" value="Login" title="Login">
        </div>
     </div>
   </div>
      <script language="javascript">
         var l = document.getElementById('login');
         if (l) l.focus();
      </script>
<%  } 
  }
  else
    {
%>
    You are log in.
<%
    }
%>
</asp:Content>
