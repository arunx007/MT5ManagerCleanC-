//+------------------------------------------------------------------+
//|                                          MetaTrader 5 TextFeeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
namespace TextFeeder.NET.DataSource
  {
   using MetaQuotes.MT5CommonAPI;
   using MetaQuotes.MT5GatewayAPI;
   using System;
   using System.Collections.Generic;
   using System.Xml.Serialization;   
   //+------------------------------------------------------------------+
   //| News text xml format descriptor                                  |
   //+------------------------------------------------------------------+
   [Serializable]
   [XmlRoot("MQNews")]
   public class MQNews
     {
      //+------------------------------------------------------------------+
      //| Subject attribute accessor                                       |
      //+------------------------------------------------------------------+
      [XmlAttribute]
      public string Subject { get; set; }
      //+------------------------------------------------------------------+
      //| Category attribute accessor                                      |
      //+------------------------------------------------------------------+
      [XmlAttribute]
      public string Category { get; set; }
      //+------------------------------------------------------------------+
      //| Language attribute accessor                                      |
      //+------------------------------------------------------------------+
      [XmlAttribute]
      public uint Lang { get; set; }
      //+------------------------------------------------------------------+
      //| News body tag accessor                                           |
      //+------------------------------------------------------------------+
      [XmlElement("MQNewsBody")]
      public string MQNewsBody { get; set; }
      //+------------------------------------------------------------------+
      //| Constructor                                                      |
      //+------------------------------------------------------------------+
      public MQNews()
        {
         Subject   =string.Empty;
         Category  =string.Empty;
         MQNewsBody=string.Empty;
         Lang      =0;
        }
      //+------------------------------------------------------------------+
      //| Converting to MTNews struct                                      |
      //+------------------------------------------------------------------+
      public MTNews ToMTNews()
        {
         MTNews news=new MTNews();
         //---
         news.language=Lang;
         news.subject =Subject;
         news.category=Category;
         news.body    =MQNewsBody;
         news.body_len=(uint)MQNewsBody.Length;
         //--- all ok
         return(news);
        }
     }
  }
//+------------------------------------------------------------------+

