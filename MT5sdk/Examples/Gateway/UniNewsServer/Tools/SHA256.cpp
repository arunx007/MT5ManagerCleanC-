//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Server |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "SHA256.h"
//+------------------------------------------------------------------+
//| Transform functions                                              |
//+------------------------------------------------------------------+
#define Ch(x,y,z)  ((x & y) ^ (~x & z))
#define Maj(x,y,z) ((x & y) ^ (x & z) ^ (y & z))
#define S(x, n)    (((x)>>((n)&31))|((x)<<(32-((n)&31))))
#define R(x, n)    ((x)>>(n))
#define Sigma0(x)  (S(x, 2) ^ S(x, 13) ^ S(x, 22))
#define Sigma1(x)  (S(x, 6) ^ S(x, 11) ^ S(x, 25))
#define Gamma0(x)  (S(x, 7) ^ S(x, 18) ^ R(x, 3))
#define Gamma1(x)  (S(x, 17) ^ S(x, 19) ^ R(x, 10))
//+------------------------------------------------------------------+
//| Furst 32 bites of fractional parts of cubic roots of             |
//| first 64 simple numbers [from 2 to 311]                          |
//+------------------------------------------------------------------+
const ULONG CSHA256::s_keys[64]={
   0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL,
   0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL,
   0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL,
   0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
   0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL,
   0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL,
   0x06ca6351UL, 0x14292967UL, 0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL,
   0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
   0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL,
   0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL,
   0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,
   0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
   0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
   };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CSHA256::CSHA256(void)
  {
   ZeroMemory(&m_state,sizeof(m_state));
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
__declspec(noinline) CSHA256::~CSHA256(void)
  {
   SecureZeroMemory(m_state.state,sizeof(m_state.state));
  }
//+------------------------------------------------------------------+
//| Hash calculation                                                 |
//+------------------------------------------------------------------+
void CSHA256::CalculateHash(const PUCHAR buf,uint32_t len,PUCHAR hash,const uint32_t hash_len)
  {
//--- check
   if(buf && len>0 && hash)
     {
      CSHA256 sha256;
      //--- 
      sha256.Init();               // initialization
      sha256.Process(buf,len);     // process data
      sha256.Done(hash,hash_len);  // finalize data
     }
//---
  }
//+------------------------------------------------------------------+
//| Data compression                                                 |
//+------------------------------------------------------------------+
void CSHA256::Compress(void)
  {
   ULONG S[8]={0},W[64]={0},t0=0,t1=0;
   uint32_t  i;
//--- copy state into S
   for(i=0;i<8;i++)
      S[i]=m_state.state[i];
//--- copy the state into 512-bits into W[0..15]
   for(i=0;i<16;i++)
      W[i]=(((unsigned long) m_state.buf[(4*i)+0])<<24)|
      (((unsigned long) m_state.buf[(4*i)+1])<<16)     |
      (((unsigned long) m_state.buf[(4*i)+2])<<8)      |
      (((unsigned long) m_state.buf[(4*i)+3]));
//--- fill W[16..63]
   for(i=16;i<64;i++)
      W[i]=Gamma1(W[i-2])+W[i-7]+Gamma0(W[i-15])+W[i-16];
//--- compress
   for(i=0;i<64;i++)
     {
      t0  =S[7]+Sigma1(S[4])+Ch(S[4],S[5],S[6])+s_keys[i]+W[i];
      t1  =Sigma0(S[0])+Maj(S[0],S[1],S[2]);
      S[7]=S[6];
      S[6]=S[5];
      S[5]=S[4];
      S[4]=S[3]+t0;
      S[3]=S[2];
      S[2]=S[1];
      S[1]=S[0];
      S[0]=t0+t1;
     }
//--- feedback
   for(i=0;i<8;i++)
      m_state.state[i]+=S[i];
  }
//+------------------------------------------------------------------+
//| Initialization                                                    |
//+------------------------------------------------------------------+
void CSHA256::Init(void)
  {
   m_state.curlen=m_state.length=0;
   m_state.state[0]=0x6A09E667UL;
   m_state.state[1]=0xBB67AE85UL;
   m_state.state[2]=0x3C6EF372UL;
   m_state.state[3]=0xA54FF53AUL;
   m_state.state[4]=0x510E527FUL;
   m_state.state[5]=0x9B05688CUL;
   m_state.state[6]=0x1F83D9ABUL;
   m_state.state[7]=0x5BE0CD19UL;
  }
//+------------------------------------------------------------------+
//| Process data                                                     |
//+------------------------------------------------------------------+
void CSHA256::Process(uint8_t const *buf,const uint32_t len)
  {
   uint32_t i=len;
//--- check
   if(buf==NULL || len==0)
      return;
//--- convert buffer
   while(i--)
     {
      //--- copy byte
      m_state.buf[m_state.curlen++]=*buf++;
      //--- is 64 bytes full?
      if(m_state.curlen==64)
        {
         Compress();
         m_state.length+=512;
         m_state.curlen=0;
        }
     }
  }
//+------------------------------------------------------------------+
//| Hash finalization                                                |
//+------------------------------------------------------------------+
void CSHA256::Done(PUCHAR hash,const uint32_t hash_len)
  {
   uint32_t i;
   uint32_t len=std::min(uint32_t(MAX_HASH),hash_len);
//--- check
   if(hash==NULL || len==0)
      return;
//--- increase the length of the message
   m_state.length+=m_state.curlen*8;
//--- if the length is currenlly above 56 bytes we append zeros then compress.
//--- Then we can fall back to padding zeros and length encoding like normal.
   if(m_state.curlen>=56)
     {
      //--- append the '1' bit
      m_state.buf[m_state.curlen++]=(uint8_t)0x80;
      for(;m_state.curlen<64;)
         m_state.buf[m_state.curlen++]=(uint8_t)0x00;
      Compress();
      m_state.curlen=0;
     }
   else
     {
      //--- append the '1' bit
      m_state.buf[m_state.curlen++]=(uint8_t)0x80;
     }
//--- pad upto 56 bytes of zeroes
   for(;m_state.curlen<56;)
      m_state.buf[m_state.curlen++]=(uint8_t)0x00;
//--- since all messages are under 2^32 bits we mark the top bits zero
   for(i=56;i<60;i++)
      m_state.buf[i]=(uint8_t)0x00;
//--- append length
   for(i=60;i<64;i++)
      m_state.buf[i]=(m_state.length>>((63-i)*8))&255;
   Compress();
//--- copy output
   for(i=0;i<len;i++)
      hash[i]=(m_state.state[i>>2]>>(((3-i)&3)<<3))&255;
  }
//+------------------------------------------------------------------+
