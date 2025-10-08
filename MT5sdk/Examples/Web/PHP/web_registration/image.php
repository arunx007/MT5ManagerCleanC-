<?php
//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                   Copyright 2000-2021, MetaQuotes Software Corp. |
//|                                        http://www.metaquotes.net |
//+------------------------------------------------------------------+
  /**
   * Class captcha
   */
  class CImage
    {
    function __construct()
      {
      session_start();
      //---
      header("Content-type: image/gif");
      header("Expires: Mon, 1 Jun 1999 01:00:00 GMT");
      //---
      if (!function_exists('imagecreate'))
        {
        readfile('i/0.gif');
        exit;
        }
      //---
      $im = imagecreate(59, 22);
      //---
      imagecolorallocate($im, 0xff, 0xff, 0xff);
      $black = imagecolorallocate($im, 0x62, 0x63, 0x63);
      //---
      $verify_code = trim($_SESSION['verify_code']);
      $len = strlen($verify_code);
      //---
      $x = 3;
      $y = 3;
      $w = imagefontwidth(5);
      mt_srand($this->MakeSeed());
      //---
      for ($i = 0; $i < $len; ++$i)
        {
        imagestring($im, 5, $x, $y + mt_rand(0, 4) - 2, $verify_code[$i], $black);
        //---
        $x += $w;
        }
      //--- output the image
      imagegif($im);
      //---
      exit;
      }
    function MakeSeed()
      {
      list($usec, $sec) = explode(' ', microtime());
      return ((float)$sec + ((float)$usec * 100000));
      }
    }

  //--- show captcha
  $image = new CImage();
?>