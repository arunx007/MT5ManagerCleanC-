<?php
//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                   Copyright 2000-2021, MetaQuotes Software Corp. |
//|                                        http://www.metaquotes.net |
//+------------------------------------------------------------------+
  /**
 * CMysql::query(SQL);
 *
 */
  class CMysql
    {
    private static $link;
    /**
     * SQL query
     * @param string $sql
     * @return Resource
     */
    public static function query($sql)
      {
      if (!CMysql::$link)
        {
        CMysql::connect();
        }
      //---
      if (CMysql::$link)
        {
        $res = mysql_query($sql, CMysql::$link);
        //---
        if (mysql_errno(CMysql::$link)) trigger_error("MySQL error: " . mysql_error(CMysql::$link) . ' query: ' . $sql, E_USER_WARNING);
        //---
        return $res;
        }
      //---
      return null;
      }

    /**
     * Create connection to database
     */
    public static function connect()
      {
      //---
      if (CMysql::$link = mysql_pconnect(MYSQL_SERVER, MYSQL_LOGIN, MYSQL_PASSWORD))
        {
        mysql_select_db(MYSQL_DB_NAME, CMysql::$link);
        mysql_query('SET NAMES \'utf8\'', CMysql::$link);
        }
      else
        {
        header("HTTP/1.1 500 Server busy");
        //---
        include("errors/database.html");
        //---
        exit;
        }
      }
    /**
     * Close connection to database
     *
     */
    public static function close()
      {
      if (CMysql::$link) mysql_close(CMysql::$link);
      }
    /**
     * Get mysql error
     *
     * @return string
     */
    public static function error()
      {
      if (CMysql::$link) return mysql_error(CMysql::$link);
      return "";
      }
    /**
     * Get mysql error number
     * @return int
     */
    public static function errno()
      {
      if (CMysql::$link) return mysql_errno(CMysql::$link);
      return 0;
      }
    /**
     * Get last ID after insert
     * @return int
     */
    public static function lastID()
      {
      if (CMysql::$link) return mysql_insert_id(CMysql::$link);
      return -1;
      }
    /**
     * mysql_real_escape_string
     *
     * @param string $str
     * @return string
     */
    public static function escape_string($str)
      {
      if (!CMysql::$link) CMysql::connect();
      return mysql_real_escape_string($str, CMysql::$link);
      }
    }

?>