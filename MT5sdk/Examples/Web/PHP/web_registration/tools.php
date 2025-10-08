<?
//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                   Copyright 2000-2021, MetaQuotes Software Corp. |
//|                                        http://www.metaquotes.net |
//+------------------------------------------------------------------+
  /**
   * Tools class, can send email and crypt
   */
  class CTools
    {

    function Mail($to, $to_name, $from, $from_name, $subject, $text, $type = 'text')
      {
      //----
      include_once('lib/PEAR/Net/SMTP.php');
      //----
      $to_name = $this->MailEsc($to_name);
      $from_name = $this->MailEsc($from_name);
      $subject = $this->MailEsc($subject);
      //----
      $headers = "MIME-Version: 1.0\r\n" . "Return-Path: $from\r\n" . "X-Mailer: MQL4 site\r\n" . "Date: " . date('r') . "\r\n" . "Message-Id: " . time() . '.' . preg_replace('/[^0-9a-z-]+/iu', '-', $to) . "@wr.com\r\n";
      //----
      if ($type == 'text') $headers .= "Content-type: text/plain; charset=UTF-8\r\n";
      else              $headers .= "Content-type: text/html; charset=UTF-8\r\n";
      //----
      if ($to_name != '') $headers .= "To: $to_name <$to>\r\n";
      else               $headers .= "To: $to\r\n";
      if ($from_name != '') $headers .= "From: $from_name <$from>\r\n";
      else               $headers .= "From: $from\r\n";
      //----
      $smtp = new Net_SMTP(SMTP_SERVER, 25, SMTP_LOGIN);
      $smtp->connect();
      //----
      if (PEAR::isError($e = $smtp->auth(SMTP_LOGIN, SMTP_PASSWORD)))
        {
        $smtp->disconnect();
        return (false);
        }
      //----
      if (PEAR::isError($smtp->mailFrom($from)))
        {
        $smtp->disconnect();
        return (false);
        }
      //----
      if (PEAR::isError($smtp->rcptTo($to)))
        {
        $smtp->disconnect();
        return (false);
        }
      //----
      if (PEAR::isError($smtp->data("Subject: $subject\r\n" . $headers . "\r\n" . $text)))
        {
        $smtp->disconnect();
        return (false);
        }
      //----
      $smtp->disconnect();
      //----
      return (true);
      }

    function MailEsc($str)
      {
      if ($str == '') return ('');
      //--- remove dangerous chars
      $str = preg_replace('/[\x00\r\n]+/', ' ', $str);
      //---
      if (preg_match('/[^\x00-\x7f]/', $str))
        {
        preg_match_all('/[^\x21-\x3C\x3E-\x7E\x09\x20]/', $str, $m);
        //---
        $_8 = sizeof($m[0]);
        $len = strlen($str);
        //---
        if (($len * 1.33) < ($_8 * 3)) return ('=?UTF-8?B?' . base64_encode($str) . '?=');
        //---
        return ('=?UTF-8?Q?' . $this->QPrintableEncode($str) . '?=');
        }
      //---
      return $str;
      }
    /**
     * Crypt data
     * @param $data string
     * @param $ckey string
     * @return string
     */
    function Crypt($data, $ckey)
      {
      $key = $box = array();
      $keylen = strlen($ckey);
      //----
      for ($i = 0; $i <= 255; ++$i)
        {
        $key[$i] = ord($ckey{$i % $keylen});
        $box[$i] = $i;
        }
      //---
      for ($x = $i = 0; $i <= 255; $i++)
        {
        $x = ($x + $box[$i] + $key[$i]) % 256;
        list($box[$i], $box[$x]) = array($box[$x], $box[$i]);
        }
      //----
      $k = $cipherby = $cipher = '';
      $datalen = strlen($data);
      //----
      for ($a = $j = $i = 0; $i < $datalen; $i++)
        {
        $a = ($a + 1) % 256;
        $j = ($j + $box[$a]) % 256;
        //----
        list ($box[$a], $box[$j]) = array($box[$j], $box[$a]);
        $k = $box[($box[$a] + $box[$j]) % 256];
        $cipherby = ord($data{$i}) ^ $k;
        $cipher .= chr($cipherby);
        }
      //----
      return $cipher;
      }
    }
?>