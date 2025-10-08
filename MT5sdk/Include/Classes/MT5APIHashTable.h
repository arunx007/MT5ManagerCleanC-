//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include <windows.h>
#include <utility>
//+------------------------------------------------------------------+
//| The default allocator for the hash                               |
//| anything less than or equal to TSize is allocated via blocks     |
//| in the list,  everything else via VirtualAlloc                   |
//+------------------------------------------------------------------+
template <uint32_t TSize>
class TMTHashAllocator final
  {
private:
   constexpr static uint32_t C_ALIGNED_SIZE=(TSize + 7) & ~7;
   //--- check the sizes
                     static_assert(C_ALIGNED_SIZE < 32 * 1024, "Too big allocator item sizes");
   //--- block header
   struct alignas(8) BlockHeader
     {
      BlockHeader      *next;
     };
   //--- free element
   struct ListItem
     {
      ListItem         *next;
     };

   BlockHeader      *m_block=nullptr;         // list of blocks
   ListItem         *m_first=nullptr;         // the first free item in the list (this is one of the items in the block)
   uint32_t          m_granularity=0;         // alignment

public:
   //+------------------------------------------------------------------+
   //| Constructor                                                      |
   //+------------------------------------------------------------------+
   TMTHashAllocator(void)
     {
      SYSTEM_INFO si={};
      GetSystemInfo(&si);
      m_granularity=si.dwAllocationGranularity;
     }
   //+------------------------------------------------------------------+
   //| Destructor                                                       |
   //+------------------------------------------------------------------+
   ~TMTHashAllocator(void)
     {
      Shutdown();
     }
   //+------------------------------------------------------------------+
   //| Allocation                                                       |
   //+------------------------------------------------------------------+
   void* Allocate(size_t size)
     {
      //--- IMPORTANT, you cannot replace VirtualAlloc with anything else here, you need m_granularity alignment
      //--- anything larger than TSize directly
      if(size > C_ALIGNED_SIZE)
         return(VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
      //--- If there are no free ones, we allocate
      if(m_first==nullptr)
        {
         BlockHeader *new_block=(BlockHeader*)VirtualAlloc(nullptr, m_granularity, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
         if(new_block==nullptr)
            return(nullptr);
         //--- add to the list of blocks
         new_block->next=m_block;
         m_block=new_block;

         BYTE *mem=(BYTE*)new_block + sizeof(BlockHeader);
         BYTE *mem_max=(BYTE*)new_block + m_granularity;
         //--- build a list of free ones in a new block
         for(; mem + C_ALIGNED_SIZE<=mem_max; mem+=C_ALIGNED_SIZE)
           {
            ((ListItem*)mem)->next=m_first;
            m_first=((ListItem*)mem);
           }
        }
      //--- take 1 item
      void *temp=m_first;
      m_first=m_first->next;
      //---
      return(temp);
     }
   //+------------------------------------------------------------------+
   //| Free                                                             |
   //+------------------------------------------------------------------+
   void Free(void* ptr)
     {
      if(ptr==nullptr)
         return;
      //--- checking what we are freeing
      if((reinterpret_cast<uintptr_t>(ptr)&(m_granularity-1))!=0)
        {
         ((ListItem*)ptr)->next=m_first;
         m_first=(ListItem*)ptr;
        }
      else
         VirtualFree(ptr, 0, MEM_RELEASE);
     }
   //+------------------------------------------------------------------+
   //| Clearing, it is assumed that all blocks have already been freed  |
   //+------------------------------------------------------------------+
   void Shutdown(void)
     {
      m_first=nullptr;
      //--- go through all the blocks and release
      while(m_block)
        {
         void *free_mem=m_block;
         m_block=m_block->next;
         VirtualFree(free_mem, 0, MEM_RELEASE);
        }
     }
  };
//+------------------------------------------------------------------+
//| Functions for accumulative hashing                               |
//+------------------------------------------------------------------+
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
namespace MTFowlerNollVo
  {
   struct FNV1
     {
      uint64_t          hash=14695981039346656037ULL;
      //---
      void Add(const void* data, size_t data_size)
        {
         for(size_t idx=0; idx < data_size; idx++)
            hash=(hash * 1099511628211ULL) ^ reinterpret_cast<const uint8_t*>(data)[idx];
        }
      //---
      template<typename T> FNV1& Add(const T &value)
        {
         Add(&value,sizeof(T));
         return *this;
        }
      //---
      template<typename T> FNV1& AddValues(const T *values,const size_t total)
        {
         if(values)
            Add(values,total*sizeof(T));
         return *this;
        }
      //---
      template<typename TCHAR> void AddStr(const TCHAR* str)
        {
         for(const TCHAR* cp=str;*cp!=0;cp++)
            Add(cp,sizeof(*cp));
        }
      uint64_t          Hash(void) const     { return(hash); }
      operator          uint64_t(void) const   { return(hash); }
     };
//--- alternative method
   struct FNV1a
     {
      uint64_t          hash=14695981039346656037ULL;
      //---
      void Add(const void* data, size_t data_size)
        {
         for(size_t idx=0; idx < data_size; idx++)
            hash=(hash ^ reinterpret_cast<const uint8_t*>(data)[idx]) * 1099511628211ULL;
        }
      //---
      template<typename TCHAR> void AddStr(const TCHAR* str)
        {
         for(const TCHAR* cp=str;*cp!=0;cp++)
            Add(cp,sizeof(*cp));
        }
      uint64_t          Hash(void) const     { return(hash); }
      operator          uint64_t(void) const   { return(hash); }
     };
   //+------------------------------------------------------------------+
   //| Auxiliary wraps (completion)                                     |
   //+------------------------------------------------------------------+
   inline uint64_t InnerCalculateFNV1(uint64_t hash)
     {
      return(hash);
     }
   //+------------------------------------------------------------------+
   //| Auxiliary wraps                                                  |
   //+------------------------------------------------------------------+
   template<typename... TArgs>
   uint64_t InnerCalculateFNV1(uint64_t hash, const void* data, size_t data_size, TArgs&&... args)
     {
      for(size_t idx=0; idx < data_size; idx++)
         hash=(hash * 1099511628211ULL) ^ reinterpret_cast<const uint8_t*>(data)[idx];
      //--- variadic recursion
      return(InnerCalculateFNV1(hash, std::forward<TArgs>(args)...));
     }
   //+------------------------------------------------------------------+
   //| Auxiliary wraps (completion)                                     |
   //+------------------------------------------------------------------+
   inline uint64_t InnerCalculateFNV1a(uint64_t hash)
     {
      return(hash);
     }
   //+------------------------------------------------------------------+
   //| Auxiliary wraps                                                  |
   //+------------------------------------------------------------------+
   template<typename... TArgs>
   uint64_t InnerCalculateFNV1a(uint64_t hash, const void* data, size_t data_size, TArgs&&... args)
     {
      for(size_t idx=0; idx < data_size; idx++)
         hash=(hash ^ reinterpret_cast<const uint8_t*>(data)[idx]) * 1099511628211ULL;
      //--- variadic recursion
      return(InnerCalculateFNV1(hash, std::forward<TArgs>(args)...));
     }
  }
//+------------------------------------------------------------------+
//| Calculate hash in one-line hash via variadic template            |
//+------------------------------------------------------------------+
template<typename... TArgs>
uint64_t MTCalculateFNV1(TArgs&&... args)
  {
   return(MTFowlerNollVo::InnerCalculateFNV1(14695981039346656037ULL, std::forward<TArgs>(args)...));
  }
//+------------------------------------------------------------------+
//| Calculate hash in one-line hash via variadic template            |
//+------------------------------------------------------------------+
template<typename... TArgs>
uint64_t MTCalculateFNV1a(TArgs&&... args)
  {
   return(MTFowlerNollVo::InnerCalculateFNV1a(14695981039346656037ULL, std::forward<TArgs>(args)...));
  }
//+------------------------------------------------------------------+
//| Hash table with collision resolution via list                    |
//| no duplicates                                                    |
//| all previous references are valid after addition/deletion        |
//| no duplicates, checks for duplicates                             |
//+------------------------------------------------------------------+
template <typename TValue, typename THash, typename TEqual, typename TAllocator=TMTHashAllocator<sizeof(TValue)+16>>
class TMTHashTable final
  {
   //--- constants
   static constexpr uint64_t COUNT_INITIAL=512;   // default initial table size

   //--- this structure is followed by TValue
   struct HashItem
     {
      HashItem         *next;             // the next element with the same hash (to resolve collisions)
      uint64_t          hash;             // "raw" hash
     };

public:
   //--- iterator template class
   template<typename T>
   class TIterator final
     {
      friend            TMTHashTable;                // table class is friend

   private:
      HashItem        **m_table{};                 // table pointer
      const HashItem *const *const m_table_end{};  // table end pointer
      HashItem         *m_item{};                  // current item pointer

   public:
                        TIterator(void)=default;
      //--- constructor to iterate from the beginning of the table, moves to the first existing element
                        TIterator(HashItem **const table,const uint64_t size) : m_table{table},m_table_end{table+size} { operator++(); }
      //--- constructor to iterate from the found element in the table
                        TIterator(HashItem **const table,const HashItem *const *const table_end) : m_table{table},m_table_end{table_end},m_item{*table} {}
      //--- value
      T&                operator *(void) &                     { return *(T*)(m_item+1); }
      T&&               operator *(void) &&                    { return *(T*)(m_item+1); }
      //--- pointer to value
      T*                operator->(void)                       { return(T*)(m_item+1); }
      //--- comparation
      bool              operator==(const TIterator &r) const   { return m_item==r.m_item; }
      bool              operator!=(const TIterator &r) const   { return m_item!=r.m_item; }
      operator          bool(void) const                       { return m_item!=nullptr; }
      //--- iteration
      TIterator&        operator++(void)
        {
         //--- next item in the collision list
         if(m_item)
           {
            m_item=m_item->next;
            //--- if we have reached the end of the collision list, we move to the next element in the hash table
            if(!m_item)
               m_table++;
           }
         //--- if we have reached the end of the collision list, we search for the next element in the hash table
         if(!m_item)
            for(;m_table!=m_table_end;m_table++)
              {
               m_item=*m_table;
               //--- found the following non-empty entry in the hash table
               if(m_item)
                  break;
              }
         return *this;
        }
     };
   //--- iterators classes
   using             Iterator=TIterator<TValue>;
   using             ConstIterator=TIterator<const TValue>;

private:
   TAllocator        m_allocator;         // allocator
   const THash       m_hash;              // hash function
   const TEqual      m_equal;             // equivalence function

   HashItem        **m_table =nullptr;    // our hash table
   uint64_t          m_count =0;          // current hash table size
   uint64_t          m_filled=0;          // number of elements (all including collisions)
   uint64_t          m_mask  =0;          // current mask calculated in advance

public:
   //+------------------------------------------------------------------+
   //| Constructor                                                      |
   //+------------------------------------------------------------------+
   explicit TMTHashTable(void)
     {
     }
   //+------------------------------------------------------------------+
   //| Constructor                                                      |
   //+------------------------------------------------------------------+
   explicit TMTHashTable(const TAllocator &allocator) : m_allocator(allocator)
     {
     }
   //+------------------------------------------------------------------+
   //| Move constructor                                                 |
   //+------------------------------------------------------------------+
   TMTHashTable(TMTHashTable &&r) : m_allocator(std::move(r.m_allocator)),m_hash(std::move(r.m_hash)),m_equal(std::move(r.m_equal)),m_table(r.m_table),m_count(r.m_count),m_filled(r.m_filled),m_mask(r.m_mask)
     {
      r.m_table =nullptr;
      r.m_count =0;
      r.m_filled=0;
      r.m_mask  =0;
     }
   //+------------------------------------------------------------------+
   //| Destructor                                                       |
   //+------------------------------------------------------------------+
   ~TMTHashTable(void)
     {
      Shutdown();
     }
   //+------------------------------------------------------------------+
   //| Move operator                                                    |
   //+------------------------------------------------------------------+
   TMTHashTable& operator=(TMTHashTable &&r)
     {
      std::swap(m_allocator,r.m_allocator);
      std::swap(m_hash     ,r.m_hash     );
      std::swap(m_equal    ,r.m_equal    );
      std::swap(m_table    ,r.m_table    );
      std::swap(m_count    ,r.m_count    );
      std::swap(m_filled   ,r.m_filled   );
      std::swap(m_mask     ,r.m_mask     );
      return *this;
     }
   //+------------------------------------------------------------------+
   //| Allocator                                                        |
   //+------------------------------------------------------------------+
   TAllocator& Allocator(void)
     {
      return(m_allocator);
     }
   //+------------------------------------------------------------------+
   //| Inserting an element in the hash and creating an instance of     |
   //| the TOther class                                                 |
   //+------------------------------------------------------------------+
   template <typename TOther=TValue, typename... TArgs>
   TOther* Insert(TArgs&&... args)
     {
      return(InsertEx<TOther>(0, std::forward<TArgs>(args)...));
     }
   //+------------------------------------------------------------------+
   //| Inserting an element in the hash creating an instance of         |
   //| the TValue class                                                 |
   //+------------------------------------------------------------------+
   template <typename TOther=TValue, typename... TArgs>
   TOther* InsertEx(uint32_t extra_size, TArgs&&... args)
     {
      //--- if rebuild is required
      if(m_filled==m_count)
         if(!RebuildTable(m_count<=0 ? COUNT_INITIAL : m_count * 2))
            return(nullptr);
      //--- allocate
      HashItem *item=(HashItem*)m_allocator.Allocate(sizeof(HashItem) + sizeof(TOther) + extra_size);
      if(item==nullptr)
         return(nullptr);
      TOther* obj=(TOther*)(item + 1);
      //--- construct
      new (obj) TOther{std::forward<TArgs>(args)...};
      //--- now look for where to put
      item->hash=m_hash(*obj);
      uint64_t idx=item->hash & m_mask;
      //--- check for duplicates
      HashItem *temp=m_table[idx];
      while(temp)
        {
         if(m_equal(*(TValue*)(temp + 1), *obj))
           {
            obj->~TValue();
            //--- we have a duplicate
            m_allocator.Free(item);
            return(nullptr);
           }
         temp=temp->next;
        }
      //--- no duplicates, insert
      item->next=m_table[idx];
      m_table[idx]=item;
      m_filled++;
      //---
      return(obj);
     }
   //+------------------------------------------------------------------+
   //| Find or insert new item                                          |
   //+------------------------------------------------------------------+
   template <typename... TArgs>
   TValue* FindOrInsert(TArgs&&... args)
     {
      //--- calculate the hash and index in the table
      const uint64_t hash=m_hash(std::forward<TArgs>(args)...);
      uint64_t       idx=hash & m_mask;
      //--- table check
      if(m_count)
        {
         //--- find item in collision linked list
         for(HashItem *temp=m_table[idx];temp;temp=temp->next)
           {
            //--- check equality
            if(m_equal(*(TValue*)(temp + 1), std::forward<TArgs>(args)...))
              {
               //--- item found
               return((TValue*)(temp + 1));
              }
           }
        }
      //--- item not found
      //--- check rebuild is required
      if(m_filled==m_count)
        {
         //--- rebuild the table
         if(!RebuildTable(m_count ? m_count * 2 : COUNT_INITIAL))
            return(nullptr);
         //--- new index in the table
         idx=hash & m_mask;
        }
      //--- allocate
      HashItem *item=(HashItem*)m_allocator.Allocate(sizeof(HashItem) + sizeof(TValue));

      if(!item)
         return(nullptr);
      //--- construct
      TValue* obj=(TValue*)(item + 1);

      new (obj) TValue{std::forward<TArgs>(args)...};
      //--- insert
      item->hash=hash;
      item->next=m_table[idx];
      m_table[idx]=item;
      m_filled++;
      return(obj);
     }
   //+------------------------------------------------------------------+
   //| Search for the element                                           |
   //+------------------------------------------------------------------+
   template <typename... TArgs>
   TValue* Find(TArgs&&... args) const
     {
      if(m_count<=0)
         return(nullptr);
      //--- calculate hash
      uint64_t hash=m_hash(std::forward<TArgs>(args)...);
      //--- now the equivalence
      HashItem *temp=m_table[hash & m_mask];
      while(temp)
        {
         if(m_equal(*(TValue*)(temp + 1), std::forward<TArgs>(args)...))
           {
            //--- found
            return((TValue*)(temp + 1));
           }
         temp=temp->next;
        }
      //--- nothing found
      return(nullptr);
     }
   //+------------------------------------------------------------------+
   //| Delete the 1st and only the 1st element                          |
   //+------------------------------------------------------------------+
   template <typename... TArgs>
   bool Remove(TArgs&&... args)
     {
      if(m_count<=0)
         return(false);
      //--- calculate hash
      uint64_t hash=m_hash(std::forward<TArgs>(args)...);
      //--- now the equivalence
      HashItem **temp=&m_table[hash & m_mask];
      while(*temp)
        {
         if(m_equal(*(TValue*)(*temp + 1), std::forward<TArgs>(args)...))
           {
            //--- correct the list
            HashItem *to_delete=*temp;
            *temp=to_delete->next;
            //--- found deleted
            ((TValue*)(to_delete + 1))->~TValue();
            m_allocator.Free(to_delete);
            m_filled--;
            //---
            return(true);
           }
         temp=&(*temp)->next;
        }
      //--- nothing found
      return(false);
     }
   //+------------------------------------------------------------------+
   //| Preparing a table of a predetermined size                        |
   //+------------------------------------------------------------------+
   bool Reserve(uint64_t count)
     {
      uint64_t mult=1;
      //--- calculate the power of two
      while(count > mult)
         mult*=2;
      //--- rebuild the table
      return(RebuildTable(mult));
     }
   //+------------------------------------------------------------------+
   //| Clear the table and free all allocated memory                    |
   //+------------------------------------------------------------------+
   void Shutdown(void)
     {
      //--- free memory
      if(m_table)
        {
         //--- pass through all the elements
         for(uint64_t tt=0; tt < m_count; tt++)
           {
            HashItem *temp=m_table[tt];
            while(temp)
              {
               HashItem *next=temp->next;
               //--- delete
               ((TValue*)(temp + 1))->~TValue();
               m_allocator.Free(temp);
               temp=next;
              }
           }
         //---
         m_allocator.Free(m_table);
         m_table=nullptr;
        }
      //---
      m_count =0;
      m_filled=0;
      m_mask  =0;
     }
   //+------------------------------------------------------------------+
   //| Iterate over each element in the hash,                           |
   //| through the passed functor                                       |
   //+------------------------------------------------------------------+
   template <typename TIterator, typename... TArgs>
   void Iterate(TIterator &&iterator, TArgs&&... args)
     {
      for(uint64_t tt=0; tt < m_count; tt++)
        {
         HashItem *temp=m_table[tt];
         while(temp)
           {
            HashItem* next=temp->next;
            //--- pass the call to the functor with additional parameters
            if(!iterator(*(TValue*)(temp + 1), std::forward<TArgs>(args)...))
               return;
            //--- to the next
            temp=next;
           }
        }
     }
   //+------------------------------------------------------------------+
   //| Number of elements                                               |
   //+------------------------------------------------------------------+
   uint64_t Size(void) const
     {
      return(m_filled);
     }
   //+------------------------------------------------------------------+
   //| Iteration                                                        |
   //+------------------------------------------------------------------+
   Iterator          begin(void)             { return{m_table,m_count}; }
   ConstIterator     begin(void) const       { return{m_table,m_count}; }
   constexpr Iterator end(void)              { return{}; }
   constexpr ConstIterator end(void) const   { return{}; }

private:
   //--- prohibit copy constructor and assignment
                     TMTHashTable(const TMTHashTable&)=delete;
   const TMTHashTable& operator=(const TMTHashTable&)=delete;
   //+------------------------------------------------------------------+
   //| Rebuild the table                                                |
   //+------------------------------------------------------------------+
   bool RebuildTable(uint64_t new_count)
     {
      if(new_count<=m_count)
         return(false);
      //--- allocate memory for the new table
      HashItem **new_table=(HashItem**)m_allocator.Allocate(sizeof(HashItem*) * new_count);
      if(new_table==nullptr)
         return(false);
      //--- be sure to clean up
      ZeroMemory(new_table, sizeof(HashItem*) * new_count);

      uint64_t new_mask=new_count - 1;
      //--- if there is something to copy
      if(m_table)
        {
         //--- now copy and paste the old stuff
         for(uint64_t tt=0; tt < m_count; tt++)
           {
            HashItem* temp=m_table[tt];
            while(temp)
              {
               //--- save the next
               HashItem* next=temp->next;
               //--- insert into the new table
               uint64_t idx=temp->hash & new_mask;
               temp->next=new_table[idx];
               new_table[idx]=temp;
               //--- to the next
               temp=next;
              }
           }
         //--- release old
         m_allocator.Free(m_table);
        }
      //--- swap tables
      m_table=new_table;
      m_mask =new_mask;
      m_count=new_count;
      //---
      return(true);
     }
  };
//+------------------------------------------------------------------+
