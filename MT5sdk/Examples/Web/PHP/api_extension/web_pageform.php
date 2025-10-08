<?php
//+------------------------------------------------------------------+
//|                           MetaTrader 5 Web API Extension Example |
//|                   Copyright 2000-2021, MetaQuotes Software Corp. |
//|                                        http://www.metaquotes.net |
//+------------------------------------------------------------------+
class WebPageForm
  {
  private $root_url;
  private $info;
  private $api;
  /**
   * Construct
   */
  function __construct()
    {
    //---- start session
    session_start();
    //--- Init
    $info = array('server' => '', 'port' => '', 'login' => '', 'password' => '', 'command' => '', 'src_cur' => '', 'dst_cur' => '', 'group' => '');
    //---
    $this->root_url = $_SERVER['HTTP_HOST'] . rtrim(dirname($_SERVER['PHP_SELF']), '/\\');
    //---
    $this->api = new MTWebAPI(AGENT, PATH_TO_LOGS, false);
    $this->api->SetLoggerWriteDebug(IS_WRITE_DEBUG_LOG);
    }
  /**
   * Create web page
   * @return void
   */
  public function CreatePage()
    {
    //--- check request
    if (isset($_REQUEST['a']) && is_array($_REQUEST['a']))
      {
      //--- extract method name
      $methodName = 'On' . key($_REQUEST['a']);
      //--- call the method
      if (method_exists($this, $methodName)) $this->$methodName();
      return;
      }
	//--- show page
    $this->Show('main');
    }
  /**
   * View page
   * @param $page string
   * @return void
   */
  private function Show($page)
    {
    switch($page)
    {
      case 'main':
        include("authorization.phtml");
        break;
      case 'command':
        include("command.phtml");
        break;
      default: 
        include("authorization.phtml");
    }
    }
  /**
   * View error page
   * @param $error string
   * @param $page string
   * @return void
   */	
  private function ShowError($error, $page)
    {
    switch($page)
    {
      case 'main':
        include("authorization.phtml");
        break;
      case 'command':
        include("command.phtml");
        break;
      default: 
        include("authorization.phtml");
    }
    exit;
    }
  /**
   * Send Command to MT5 server
   * @return void
   */
  private function OnSendCommand()
    {
    if (!isset($_REQUEST['info']) || !is_array($_REQUEST['info']))
       $this->ShowError('Invalid data parameters<br>', 'command');
    //--- receive input
    $this->info = $_REQUEST['info'];
    //--- check auth-params
    if(!isset($_SESSION['info']) || !is_array($_SESSION['info']))
       $this->ShowError("Invalid connection parameters<br>",'main');
    //--- pull back server info
    $this->info['server']   = $_SESSION['info']['server'];
    $this->info['port']     = $_SESSION['info']['port'];
    $this->info['login']    = $_SESSION['info']['login'];
    $this->info['password'] = $_SESSION['info']['password'];
    //--- connect to server
    if(($retcode=$this->api->Connect($this->info['server'], $this->info['port'], MT5_CONNECTION_TIMEOUT, $this->info['login'], $this->info['password']))!=MTRetCode::MT_RET_OK)
       $this->ShowError("Connect failed [$retcode]<br>", 'main');
    $error = '';
    switch($this->info['command'])
      {
      //--- get total users
      case 'APIEXT_TOTAL_USERS':
        //--- check input params
        if (empty($this->info['group']))
          {
           $error .= 'Empty group<br>';
           break;
          }
        //--- do command
        $count = $this->UsersGetTotal();
        //--- push out result
        $this->info['result'] = "USERS TOTAL = $count";
        break;
      //--- get total orders
      case 'APIEXT_TOTAL_ORDERS':
        //--- check input params
        if (empty($this->info['group']))
          {
          $error .= 'Empty group<br>';
          break;
          }
        //--- do command
        $count = $this->OrdersGetTotal();
        //--- push out result
        $this->info['result'] = "ORDERS TOTAL = $count";
        break;
      //--- get total positions
      case 'APIEXT_TOTAL_POSITIONS':
        //--- check input params
        if (empty($this->info['group']))
          {
          $error .= 'Empty group<br>';
          break;
          }
        //--- do command
        $count = $this->PositionsGetTotal();
        //--- push out result
        $this->info['result'] = "POSITIONS TOTAL = $count";
        break;
       //--- get buy rate
      case 'APIEXT_RATE_BUY':
        //--- check input params
        if (empty($this->info['src_cur'])) $error .= 'Empty Source Currency<br>';
        if (empty($this->info['dst_cur'])) $error .= 'Empty Destination Currency<br>';
        if ($error!='') break;
        //--- do command
        $rate = $this->RateGet(true);
        //--- push out result
        $this->info['result'] = "BUY RATE = $rate";
        break;
      //--- get sell rate	
      case 'APIEXT_RATE_SELL':
        //--- check input params
        if (empty($this->info['src_cur']))     $error .= 'Empty Source Currency<br>';
        if (empty($this->info['dst_cur'])) $error .= 'Empty Destination Currency<br>';
        if ($error) break;
        //--- do command
        $rate = $this->RateGet(false);
        //--- push out result
        $this->info['result'] = "SELL RATE = $rate";
        break;
      default:
        //--- bad errror command
        $this->ShowError('Bad error command: "'.$this->info['command'].'"<br>', 'command');
      }
    //---
    if($error!='')
      $this->ShowError($error, 'command');
    $this->Show('command');
  }
  /**
   * Get buy or sell rate
   * @param $is_buy bool
   * @return rate
   */  
  private function RateGet($is_buy)
    {
    //--- send custom command
    $params       = array();
    $answer       = array();
    $answer_boddy = array();
    //--- check command parameters
    $error='';
    if (!isset($this->info['src_cur'])     || empty($this->info['src_cur']))     $error .= 'Empty Source Currency<br>';
    if (!isset($this->info['dst_cur']) || empty($this->info['dst_cur'])) $error .= 'Empty Destination Currency<br>';
    if ($error!='') $this->ShowError($error,'command');
    //--- set param's array
    $params['base']    =$this->info['src_cur'];
    $params['currency']=$this->info['dst_cur'];
    //-- send command
    if(($retcode=$this->api->CustomSend($is_buy ? 'APIEXT_RATE_BUY' : 'APIEXT_RATE_SELL', $params, '', $answer, $answer_boddy))!=MTRetCode::MT_RET_OK)
      $this->ShowError("CustomSend failed [$retcode]<br>",'command');
    //--- check answer
    if(!isset($answer['RATE']) || empty($answer['RATE']))
      $this->ShowError("Bad answer of CustomSend<br>",'command');
    //--- show answer
    return $answer['RATE'];
    }
  /**
   * Get total open positions in the group-mask
   * @return total
   */	
  private function PositionsGetTotal()
    {
    //--- send custom command
    $params       = array();
    $answer       = array();
    $answer_boddy = array();
    //--- check command parameters
    if(!isset($this->info['group']) || empty($this->info['group']))
    {
      $this->ShowError('Empty group<br>', 'command');
    }
    //--- set param's array
    $params['group']=$this->info['group'];
    if(($retcode=$this->api->CustomSend('APIEXT_TOTAL_POSITIONS', $params, '', $answer, $answer_boddy))!=MTRetCode::MT_RET_OK)
      $this->ShowError("CustomSend failed [$retcode]<br>", 'command');
    //--- check answer
    if(!isset($answer['TOTAL']) || empty($answer['TOTAL']))
      $this->ShowError("Bad answer of CustomSend<br>", 'command');
    //--- show answer
    return $answer['TOTAL'];
    }
  /**
   * Get total users in the group-mask
   * @return total users
   */
  private function UsersGetTotal()
    {
      //--- send custom command
      $params       = array();
      $answer       = array();
      $answer_boddy = array();
      //--- check command parameters
      if(!isset($this->info['group']) || empty($this->info['group']))
        {
        $this->ShowError('Empty group<br>', 'command');
        }
      //--- set param's array
      $params['group']=$this->info['group'];
      if(($retcode=$this->api->CustomSend('APIEXT_TOTAL_USERS', $params, '', $answer, $answer_boddy))!=MTRetCode::MT_RET_OK)
        $this->ShowError("CustomSend failed [$retcode]<br>", 'command');
      //--- check answer
      if(!isset($answer['TOTAL']) || empty($answer['TOTAL']))
        $this->ShowError("Bad answer of CustomSend<br>", 'command');
      //--- show answer
      return $answer['TOTAL'];
    }
  /**
   * Get total open orders in the group-mask
   * @return total orders
   */
  private function OrdersGetTotal()
    {
      //--- send custom command
      $params       = array();
      $answer       = array();
      $answer_boddy = array();
      //--- check command parameters
      if(!isset($this->info['group']) || empty($this->info['group']))
        {
        $this->ShowError('Empty group<br>', 'command');
        }
      //--- set param's array
      $params['group']=$this->info['group'];
      if(($retcode=$this->api->CustomSend('APIEXT_TOTAL_ORDERS', $params, '', $answer, $answer_boddy))!=MTRetCode::MT_RET_OK)
        $this->ShowError("CustomSend failed [$retcode]<br>", 'command');
      //--- check answer
      if(!isset($answer['TOTAL']) || empty($answer['TOTAL']))
        $this->ShowError("Bad answer of CustomSend<br>", 'command');
      //--- show answer
      return $answer['TOTAL'];
    }
  /**
   * Connect to server
   * @return void
   */	
  private function OnConnect()
    {
    //--- check request parametes
    if (!isset($_REQUEST['info']) || !is_array($_REQUEST['info']))
    {
      $error = 'Invalid data parameters<br>';
      $this->ShowError($error, 'main');
    }
    //--- receive input
    $this->info = $_REQUEST['info'];
    //----
    foreach ($this->info as $key => $value) $value = trim($value);
    //--- validation input
    $error = '';
    if (empty($this->info['server']))   $error .= 'Empty server name<br>';
    if (empty($this->info['port']))     $error .= 'Empty server\'s port<br>';
    if (empty($this->info['login']))    $error .= 'Empty login<br>';
    if (empty($this->info['password'])) $error .= 'Empty password<br>';
    //---
    if($error!='')
      $this->ShowError($error, 'main');
    //--- all parameters are ok. connect
    if(($retcode=$this->api->Connect($this->info['server'], $this->info['port'], MT5_CONNECTION_TIMEOUT, $this->info['login'], $this->info['password']))!=MTRetCode::MT_RET_OK)
      $this->ShowError("Connect failed [$retcode]<br>", 'main');
    //--- connect ok
    //-- clean other params
    $this->info['group'] = '';
    $this->info['src_cur']='';
    $this->info['dst_cur']='';
    //--- seve all to session array
    $_SESSION['info'] = $this->info;
    //--- show command page
    $this->Show('command');
    }
  }
?>