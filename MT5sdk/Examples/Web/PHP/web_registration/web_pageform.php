<?php
//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                   Copyright 2000-2021, MetaQuotes Software Corp. |
//|                                        http://www.metaquotes.net |
//+------------------------------------------------------------------+
class WebPageForm
  {
  const VERIFY_CODE_LENGTH = 5;
  private $crypt_key = '682d95a2009e19fb3570ccb4d98b820f+dfgerth';
  private $user;
  private $verify_code;
  private $root_url;
  private $api;
  /**
   * Construct
   */
  function __construct()
    {
    //---- start session
    session_start();
    //---- initialize
    $this->user = array('email' => '', 'name' => '', 'password' => '', 'country' => '', 'state' => '', 'city' => '', 'zipcode' => '', 'phone' => '', 'phone_password' => '', 'address' => '', 'password' => '', 'confirm_password' => '', 'invest_password' => '');
    //---
    $this->root_url = $_SERVER['HTTP_HOST'] . rtrim(dirname($_SERVER['PHP_SELF']), '/\\');
    //---
    $this->api = new MTWebAPI(AGENT, PATH_TO_LOGS);
    $this->api->SetLoggerWriteDebug(IS_WRITE_DEBUG_LOG);
    }
  /**
   * Create web page
   * @return void
   */
  public function CreatePage()
    {
    //---
    if (isset($_REQUEST['a']) && is_array($_REQUEST['a']))
      {
      //--- extract method name
      $methodName = 'On' . key($_REQUEST['a']);
      //--- call the method
      if (method_exists($this, $methodName)) $this->$methodName();
      return;
      }
    $this->Show();
    }
  /**
   * Send email
   * @return void
   */
  private function OnRegister()
    {
    //--- validation request
    if (!isset($_REQUEST['user']) || !is_array($_REQUEST['user']) || !isset($_REQUEST['verify_code']) || empty($_REQUEST['verify_code']) || !isset($_SESSION['verify_code']) || empty($_SESSION['verify_code']))
      {
      include("errors/invalid_data.html");
      exit;
      }
    //--- receive input
    $this->user = $_REQUEST['user'];
    $this->verify_code = $_REQUEST['verify_code'];
    //----
    foreach ($this->user as $key => $value) $value = trim($value);
    //--- validation input
    if ($_SESSION['verify_code'] != $this->verify_code || $this->user['password'] != $this->user['confirm_password'] || empty($this->user['email']) || empty($this->user['name']) || empty($this->user['password']) || empty($this->user['country']) || empty($this->user['state']) || empty($this->user['city']) || empty($this->user['zipcode']) || empty($this->user['phone']) || empty($this->user['address']) || !$this->CheckPassword($this->user['password']))
      {
      $error = '';
      if ($_SESSION['verify_code'] != $this->verify_code) $error .= 'Invalid verify code<br>';
      if ($this->user['password'] != $this->user['confirm_password']) $error .= 'Password not confirm<br>';
      if (empty($this->user['email'])) $error .= 'Empty email<br>';
      if (empty($this->user['name'])) $error .= 'Empty name<br>';
      if (empty($this->user['password'])) $error .= 'Empty password<br>';
      if (empty($this->user['country'])) $error .= 'Empty country<br>';
      if (empty($this->user['state'])) $error .= 'Empty state<br>';
      if (empty($this->user['city'])) $error .= 'Empty city<br>';
      if (empty($this->user['zipcode'])) $error .= 'Empty zipcode<br>';
      if (empty($this->user['phone'])) $error .= 'Empty phone<br>';
      if (!$this->CheckPassword($this->user['password'])) $error .= 'Password is invalid <br>';
      //---
      $this->GenerateVerifyCode();
      include("register.phtml");
      exit;
      }
    //--- prepare line
    $line = $this->user['email'] . "\n" . $this->user['password'] . "\n" . $this->user['group'] . "\n" . $this->user['leverage'] . "\n" . $this->user['zipcode'] . "\n" . $this->user['country'] . "\n" . $this->user['state'] . "\n" . $this->user['city'] . "\n" . $this->user['address'] . "\n" . $this->user['phone'] . "\n" . $this->user['name'] . "\n" . $this->user['phone_password'] . "\n" . $this->user['invest_password'] . "\n" . time();
    $line .= "\n" . base_convert(crc32($line), 10, 36);
    //--- create new tools
    $tools = new CTools();
    //--- compress line
    $line = gzcompress($line);
    //--- prepare url and key
    $url = "http://" . $this->root_url . "/index.php?a[activate]=";
    $key = str_replace(array('+', '/'), array('&', ','), rtrim(base64_encode($tools->Crypt($line, $this->crypt_key)), '='));
    $url = $url . str_replace('&', '_', $key);
    //--- mail subject
    $mail_subject = 'Confirmation email from MetaTrader 5 Server';
    //--- body
    $mail_text = file_get_contents("templates/email.html");
    $mail_text = str_replace('{username}', $this->user['name'], $mail_text);
    $mail_text = str_replace('{url}', $url, $mail_text);
    //--- send mail
    if (!$tools->Mail($this->user['email'], $this->user['name'], SMTP_FROM, SMTP_FROM_NAME, $mail_subject, $mail_text, 'html'))
      {
      include("errors/email.html");
      exit;
      }
    //--- add activate
    $this->AddActivation($key);
    //--- redirect
    header("Location: http://" . $this->root_url . "/register_mail.html");
    }
  /**
   * User click on activate url
   * @return void
   */
  function OnActivate()
    {
    //--- validation request
    if (!isset($_REQUEST['a']['activate']))
      {
      include("errors/empty_key.html");
      exit;
      }
    //--- receive key
    $key = str_replace('_', '&', $_REQUEST['a']['activate']);
    //--- check activation
    $this->CheckActivation($key);
    //---
    $tools = new CTools();
    //--- extract data and crc32
    $data = base64_decode(str_replace(array('&', ','), array('+', '/'), $key));
    $data = explode("\n", gzuncompress($tools->Crypt($data, $this->crypt_key)));
    $crc32 = base_convert(crc32(implode("\n", array_slice($data, 0, -1))), 10, 36);
    //--- check crc
    if ($crc32 == $data[14])
      {
      //--- put parameters in our structure
      $new_user = $this->api->UserCreate();
      $new_user->Email = $data[0];
      $new_user->MainPassword = $data[1];
      $new_user->Group = $data[2];
      $new_user->Leverage = $data[3];
      $new_user->ZipCode = $data[4];
      $new_user->Country = $data[5];
      $new_user->State = $data[6];
      $new_user->City = $data[7];
      $new_user->Address = $data[8];
      $new_user->Phone = $data[9];
      $new_user->Name = $data[10];
      $new_user->PhonePassword = $data[11];
      $new_user->InvestPassword = $data[12];
      //--- create new account
      $this->CreateAccount($new_user, $user_server);
      //--- see new login on server
      $new_user->Login = $user_server->Login;
      //--- set activation
      $this->SetActivation($key);
      //--- goto to complete page
      include("complete.phtml");
      }
    else
    include("errors/invalid_key.html");
    }
  /**
   * Check password
   * @param $password string
   * @return bool
   */
  function CheckPassword($password)
    {
    $digit = 0;
    $upper = 0;
    $lower = 0;
    //--- check password size
    if (strlen($password) < 5) return (false);
    //--- check password
    for ($i = 0; $i < strlen($password); $i++)
      {
      if (ctype_digit($password[$i])) $digit = 1;
      if (ctype_lower($password[$i])) $lower = 1;
      if (ctype_upper($password[$i])) $upper = 1;
      }
    //--- final check
    return (($digit + $upper + $lower) >= 2);
    }
  /**
   * Create user on MT server
   * @param $user MTUser
   * @param $user_server MTUser
   * @return bool
   */
  function CreateAccount($user, &$user_server)
    {
    //--- check connection
    if (!$this->api->IsConnected())
      {
      if (($error_code = $this->api->Connect(MT5_SERVER_IP, MT5_SERVER_PORT, 300, MT5_SERVER_WEB_LOGIN, MT5_SERVER_WEB_PASSWORD)) != MTRetCode::MT_RET_OK)
        {
        echo "authentication on MetaTrader Server failed [", $error_code, "] ", MTRetCode::GetError($error_code);
        exit;
        }
      }
    //--- add user
    if (($error_code = $this->api->UserAdd($user, $user_server)) != MTRetCode::MT_RET_OK)
      {
      echo "add user on MetaTrader Server failed [", $error_code, "] ", MTRetCode::GetError($error_code);
      exit;
      }
    }

  function AddActivation($key)
    {
    if (!CMysql::query("INSERT INTO activation(create_time,activation_key) VALUES(now(),\"$key\")"))
      {
      include("errors/invalid_sql.html");
      exit;
      }
    }
  /**
   * Check activation
   * @param $key string
   * @return void
   */
  function CheckActivation($key)
    {
    //---
    CMysql::query("DELETE FROM activation WHERE create_time <= ADDDATE(now(),-5)");
    //--- get activation record
    $result = CMysql::query("SELECT activated FROM activation WHERE activation_key='" . $key . "' LIMIT 1");
    if (!$result)
      {
      include("errors/invalid_sql.html");
      exit;
      }
    //--- get row
    $row = mysql_fetch_row($result);
    if (!$row)
      {
      include("errors/invalid_key.html");
      exit;
      }
    elseif ($row[0] == true)
      {
      include("errors/already.html");
      exit;
      }
    }
  /**
   * Update activation record into database
   * @param string $key
   * @return void
   */
  function SetActivation($key)
    {
    if (!CMysql::query("UPDATE activation SET activated=true WHERE activation_key=\"$key\""))
      {
      include("errors/invalid_sql.html");
      exit;
      }
    }
  /**
   * Generate new verify code
   * @return void
   */
  private function GenerateVerifyCode()
    {
    $random_string = '';
    for ($i = 0; $i < self::VERIFY_CODE_LENGTH; $i++)
      {
      $random_string .= rand(0, 9);
      }
    $_SESSION['verify_code'] = $random_string;
    }
  /**
   * View page
   * @return void
   */
  private function Show()
    {
    $this->GenerateVerifyCode();
    //---
    include("register.phtml");
    }
  }

?>