//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Book item transaction structure                                  |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct MTBookItem
  {
   //--- book item type
   enum EnBookItem
     {
      ItemReset      =0,                                    // reset book item
      ItemSell       =1,                                    // sell item
      ItemBuy        =2,                                    // buy item
      ItemSellMarket =3,                                    // sell item by market
      ItemBuyMarket  =4                                     // buy item by market
     };
   uint32_t          type;                                  // EnBookItem
   double            price;                                 // deal price
   int64_t           volume;                                // deal volume - only integer values
   int64_t           volume_ext;                            // deal volume with extended accuracy - 8 digits
   uint32_t          reserved[6];                           // reserved
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Book structure                                                   |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct MTBook
  {
   //--- book flags
   enum EnBookFlags
     {
      FLAG_PRE_AUCTION=1,                                   // pre-auction book state
      FLAG_SNAPSHOT   =2,                                   // snapshot of book
      //--- enumeration borders
      FLAG_NONE       =0,                                   // none
      FLAG_ALL        =FLAG_PRE_AUCTION|FLAG_SNAPSHOT       // all flags
     };
   wchar_t           symbol[32];                            // symbol
   MTBookItem        items[32*4];                           // book transactions
   uint32_t          items_total;                           // book transactions count
   uint64_t          flags;                                 // flags
   int64_t           datetime;                              // datetime
   int64_t           datetime_msc;                          // datetime
   uint32_t          reserved[58];                          // reserved
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Book difference structure type                                   |
//+------------------------------------------------------------------+
typedef MTBook MTBookDiff;
//+------------------------------------------------------------------+
//| Book events notification interface                               |
//+------------------------------------------------------------------+
class IMTBookSink
  {
public:
   virtual void      OnBook(const MTBook& /*book*/) {}
  };
//+------------------------------------------------------------------+
