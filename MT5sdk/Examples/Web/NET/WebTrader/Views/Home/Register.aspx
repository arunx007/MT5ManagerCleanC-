<%@ Page Title="" Language="C#" MasterPageFile="~/Views/Shared/Site.Master" Inherits="System.Web.Mvc.ViewPage<WebTrader.Models.Views.ViewRegister>" %>
<%@ Import Namespace="WebTrader.Models.Core.Validator" %>
<%@ Import Namespace="WebTrader.Helpers" %>

<asp:Content ID="Content1" ContentPlaceHolderID="TitleContent" runat="server">Registration on MetaTreder 5</asp:Content>

<asp:Content ID="Content2" ContentPlaceHolderID="MainContent" runat="server">
    <div style="text-align: center">
        <h1 class="interface">Registration on MetaTreder 5</h1>
    </div>
<%
    using(Html.BeginFormValidated("Register","Home",FormMethod.Post, new {style="margin: 0px;"}))
      {
%>
    <div class="formRegister" style="width:440px;margin: 0px auto;">
        <div style="margin: 5px auto;text-align: center;"><%=Html.Validator("common", new ValidatorBase())%></div>
        <div class="line" style="margin-top: 10px;">
            <label for="login">Name:</label>
            <div class="offset"><div class="inputWrapper"><%=Html.TextBox("name", Model.User.Name,
                                     new {style = "width: 240px", tabindex = "1", title = "Enter the name please"})%></div></div>
        </div>
        <div class="offset">
            <%=Html.Validator("name",new ValidatorEmpty("Name is empty"))%>
        </div>
        <div class="line" style="margin-top: 10px;">
            <label for="login">Email:</label>
            <div class="offset"><div class="inputWrapper"><%=Html.TextBox("email", Model.User.Email,
                                     new {style = "width: 240px", tabindex = "2", title = "Enter the email please"})%></div></div>
        </div>
        <div class="offset">
            <%=Html.Validator("email", new ValidatorEmpty("Email is empty"),new ValidatorEmail("Invalid email"))%>
        </div>
        <div class="line">
            <label for="password">Main password:</label>
            <div class="offset"><div class="inputWrapper"><%=Html.Password("mainpassword", null,
                                      new
                                        {
                                                style = "width: 240px",
                                                tabindex = "3",
                                                title = "Enter the password please",
                                                autocomplete = "off",
                                        })%></div></div>
        </div>
        <div class="offset">
            <%=Html.Validator("mainpassword",new ValidatorEmpty("Main password is empty"),new ValidatorLength("Too small password",5))%>
        </div>
        <div class="line">
            <label for="password">Invest password:</label>
            <div class="offset"><div class="inputWrapper"><%=Html.Password("investpassword",Model.User.InvestPassword,
                                     new
                                       {
                                               style = "width: 240px",
                                               tabindex = "4",
                                               title = "Enter the password please",
                                               autocomplete = "off",
                                       })%></div></div>
        </div>
        <div class="offset">
            <%=Html.Validator("investpassword",new ValidatorEmpty("password is empty"), new ValidatorLength("Too small password",5))%>
        </div>
       <div class="line">
            <label for="login">Group:</label>
            <div class="offset">
                <select name="group" style="width: 240px;">
                    <option value="demo\demoforex"<%=string.Compare("demo\\demoforex", Model.User.Group, true) == 0 ? " selected='seleced'" : ""%>>demo\demoforex</option>
                    <option value="demoforex-usd"<%=string.Compare("demoforex-usd", Model.User.Group, true) == 0 ? " selected='seleced'" : ""%>>demoforex-usd</option>
                    <option value="demoforex-eur"<%=string.Compare("demoforex-eur", Model.User.Group, true) == 0 ? " selected='seleced'" : ""%>>demoforex-eur</option>
                    <option value="demoforex-jpy"<%=string.Compare("demoforex-jpy", Model.User.Group, true) == 0 ? " selected='seleced'" : ""%>>demoforex-jpy</option>
                </select>
            </div>
        </div>
        <div class="line">
            <label for="leverage">Leverage:</label>
            <div class="offset"><div class="inputWrapper"><%=Html.TextBox("leverage", Model.User.Leverage,
                                     new {style = "width: 240px", tabindex = "6", title = "Enter the leverage please"})%></div></div>
        </div>
        <div class="line">
            <label for="login">City:</label>
            <div class="offset"><div class="inputWrapper"><%=Html.TextBox("city", Model.User.City,
                                     new {style = "width: 240px", tabindex = "6", title = "Enter the city please"})%></div></div>
        </div>
        <div class="line">
            <label for="login">Zip:</label>
            <div class="offset"><div class="inputWrapper"><%=Html.TextBox("zipcode", Model.User.ZIPCode,
                                     new {style = "width: 240px", tabindex = "7", title = "Enter the zip code please"})%></div></div>
        </div>
        <div class="line">
            <label for="login">State:</label>
            <div class="offset"><div class="inputWrapper"><%=Html.TextBox("state", Model.User.State,
                                     new {style = "width: 240px", tabindex = "8", title = "Enter the state please"})%></div></div>
        </div>
        
         <div class="line">
            <label for="login">Country:</label>
            <div class="offset">
            <select name="country" style="width: 240px;">
<%
      if (Model.Countries != null)
        {
        foreach (string country in Model.Countries)
          {
%>
            <option name="<%=country%>"<%=string.Compare(country, Model.User.Country, true) == 0 ? " selected='selected'" : ""%>><%=country%></option>
  <%
          }
        }
%>>
            </select>
            </div>
        </div>
        <div class="line">
            <label for="login">Address:</label>
            <div class="offset"><div class="inputWrapper"><%=Html.TextBox("address", Model.User.Address,
                                     new {style = "width: 240px", tabindex = "10", title = "Enter the address please"})%></div></div>
        </div>
        <div class="line">
            <label for="login">Phone:</label>
            <div class="offset"><div class="inputWrapper"><%=Html.TextBox("phone", Model.User.Phone,
                                     new {style = "width: 240px", tabindex = "11", title = "Enter the phone please"})%></div></div>
        </div>
        <div class="line">
            <label for="login">Phone password:</label>
            <div class="offset"><div class="inputWrapper"><%=Html.TextBox("address", Model.User.PhonePassword,
                                     new
                                       {
                                               style = "width: 240px",
                                               tabindex = "12",
                                               title = "Enter the phone password please"
                                       })%></div></div>
        </div>
         <div class="line">
            <label for="login">Verification code:</label>
            <div class="offset">
                <%=Html.Captcha(Model.VerificationCode,"Enter verification code")%>
                <div class="inputWrapper" style="float: left;width: 136px;margin: 5px 0px 0px 10px;"><%=Html.TextBox("captcha", null,
                                     new
                                       {
                                               style = "width: 130px",
                                               tabindex = "12",
                                               title = "Enter the verification code please"
                                       })%></div>
            </div>
        </div>
        <div class="offset" style="line-height:20px; vertical-align:top;">
            <input type="submit" tabindex="4" class="buttonActive" style="width: 100px;margin:10px 0px;" value="Register" title="Register">
        </div>
     </div>
<%
      }%>

</asp:Content>

