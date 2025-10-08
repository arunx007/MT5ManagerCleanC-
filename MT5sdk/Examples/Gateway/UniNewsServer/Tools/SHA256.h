//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Server |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| SHA-256 realization                                              |
//| https://en.wikipedia.org/wiki/SHA-2                              |
//+------------------------------------------------------------------+
class CSHA256
  {
private:
   enum constants
     {
      MAX_HASH=32,                  // maximum hash size
     };
   //--- state structure
   struct ShaState
     {
      ULONG             state[8];   // state
      ULONG             length;     // total size
      ULONG             curlen;     // current size
      uint8_t           buf[64];    // data buffer
     };

private:
   static const ULONG s_keys[64];

private:
   ShaState          m_state;

public:
                     CSHA256(void);
                    ~CSHA256(void);
   //--- hash calculation
   static void       CalculateHash(const PUCHAR buf,const uint32_t len,PUCHAR hash,const uint32_t hash_len);

private:
   void              Compress(void);
   void              Init(void);
   void              Process(uint8_t const *buf,const uint32_t len);
   void              Done(PUCHAR hash,const uint32_t hash_len);
  };
//+------------------------------------------------------------------+
