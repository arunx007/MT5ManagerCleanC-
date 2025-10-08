//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Symbol trade and quotes sessions config                          |
//+------------------------------------------------------------------+
class IMTConSymbolSession
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConSymbolSession* symbol)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- session start in minutes (60 = 01:00)
   virtual uint32_t  Open(void) const=0;
   virtual MTAPIRES  Open(const uint32_t open)=0;
   //--- session start hours and minutes
   virtual uint32_t  OpenHours(void) const=0;
   virtual uint32_t  OpenMinutes(void) const=0;
   //--- session end in minutes (60 = 01:00)
   virtual uint32_t  Close(void) const=0;
   virtual MTAPIRES  Close(const uint32_t close)=0;
   //--- session end hours and minutes
   virtual uint32_t  CloseHours(void) const=0;
   virtual uint32_t  CloseMinutes(void) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConSymbolSession(void) {}
  };
//+------------------------------------------------------------------+
//| Symbol config                                                    |
//+------------------------------------------------------------------+
class IMTConSymbol
  {
public:
   //--- economical sectors
   enum EnSectors
     {
      SECTOR_UNDEFINED              =0,
      SECTOR_BASIC_MATERIALS        =1,
      SECTOR_COMMUNICATION_SERVICES =2,
      SECTOR_CONSUMER_CYCLICAL      =3,
      SECTOR_CONSUMER_DEFENSIVE     =4,
      SECTOR_ENERGY                 =5,
      SECTOR_FINANCIAL              =6,
      SECTOR_HEALTHCARE             =7,
      SECTOR_INDUSTRIALS            =8,
      SECTOR_REAL_ESTATE            =9,
      SECTOR_TECHNOLOGY             =10,
      SECTOR_UTILITIES              =11,
      SECTOR_CURRENCY               =12,
      SECTOR_CURRENCY_CRYPTO        =13,
      SECTOR_INDEXES                =14,
      SECTOR_COMMODITIES            =15,
      //--- enumeration borders
      SECTOR_FIRST                  =SECTOR_UNDEFINED,
      SECTOR_LAST                   =SECTOR_COMMODITIES
     };
   //--- economical industries
   enum EnIndustries
     {
      INDUSTRY_UNDEFINED            =0,
      //--- 
      //--- Basic Materials
      //--- 
      INDUSTRY_AGRICULTURAL_INPUTS  =1,
      INDUSTRY_ALUMINIUM            =2,
      INDUSTRY_BUILDING_MATERIALS   =3,
      INDUSTRY_CHEMICALS            =4,
      INDUSTRY_COKING_COAL          =5,
      INDUSTRY_COPPER               =6,
      INDUSTRY_GOLD                 =7,
      INDUSTRY_LUMBER_WOOD          =8,
      INDUSTRY_INDUSTRIAL_METALS    =9,
      INDUSTRY_PRECIOUS_METALS      =10,
      INDUSTRY_PAPER                =11,
      INDUSTRY_SILVER               =12,
      INDUSTRY_SPECIALTY_CHEMICALS  =13,
      INDUSTRY_STEEL                =14,
      //--- enumeration borders
      INDUSTRY_BASIC_MATERIALS_FIRST=1,
      INDUSTRY_BASIC_MATERIALS_LAST =14,
      INDUSTRY_BASIC_MATERIALS_END  =50,
      //--- 
      //--- Communication Services
      //--- 
      INDUSTRY_ADVERTISING          =51,
      INDUSTRY_BROADCASTING         =52,
      INDUSTRY_GAMING_MULTIMEDIA    =53,
      INDUSTRY_ENTERTAINMENT        =54,
      INDUSTRY_INTERNET_CONTENT     =55,
      INDUSTRY_PUBLISHING           =56,
      INDUSTRY_TELECOM              =57,
      //--- enumeration borders
      INDUSTRY_COMMUNICATION_FIRST  =51,
      INDUSTRY_COMMUNICATION_LAST   =57,
      INDUSTRY_COMMUNICATION_END    =100,
      //--- 
      //--- Consumer Cyclical
      //--- 
      INDUSTRY_APPAREL_MANUFACTURING=101,
      INDUSTRY_APPAREL_RETAIL       =102,
      INDUSTRY_AUTO_MANUFACTURERS   =103,
      INDUSTRY_AUTO_PARTS           =104,
      INDUSTRY_AUTO_DEALERSHIP      =105,
      INDUSTRY_DEPARTMENT_STORES    =106,
      INDUSTRY_FOOTWEAR_ACCESSORIES =107,
      INDUSTRY_FURNISHINGS          =108,
      INDUSTRY_GAMBLING             =109,
      INDUSTRY_HOME_IMPROV_RETAIL   =110,
      INDUSTRY_INTERNET_RETAIL      =111,
      INDUSTRY_LEISURE              =112,
      INDUSTRY_LODGING              =113,
      INDUSTRY_LUXURY_GOODS         =114,
      INDUSTRY_PACKAGING_CONTAINERS =115,
      INDUSTRY_PERSONAL_SERVICES    =116,
      INDUSTRY_RECREATIONAL_VEHICLES=117,
      INDUSTRY_RESIDENT_CONSTRUCTION=118,
      INDUSTRY_RESORTS_CASINOS      =119,
      INDUSTRY_RESTAURANTS          =120,
      INDUSTRY_SPECIALTY_RETAIL     =121,
      INDUSTRY_TEXTILE_MANUFACTURING=122,
      INDUSTRY_TRAVEL_SERVICES      =123,
      //--- enumeration borders
      INDUSTRY_CONSUMER_CYCL_FIRST  =101,
      INDUSTRY_CONSUMER_CYCL_LAST   =123,
      INDUSTRY_CONSUMER_CYCL_END    =150,
      //--- 
      //--- Consumer Defensive
      //--- 
      INDUSTRY_BEVERAGES_BREWERS    =151,
      INDUSTRY_BEVERAGES_NON_ALCO   =152,
      INDUSTRY_BEVERAGES_WINERIES   =153,
      INDUSTRY_CONFECTIONERS        =154,
      INDUSTRY_DISCOUNT_STORES      =155,
      INDUSTRY_EDUCATION_TRAINIG    =156,
      INDUSTRY_FARM_PRODUCTS        =157,
      INDUSTRY_FOOD_DISTRIBUTION    =158,
      INDUSTRY_GROCERY_STORES       =159,
      INDUSTRY_HOUSEHOLD_PRODUCTS   =160,
      INDUSTRY_PACKAGED_FOODS       =161,
      INDUSTRY_TOBACCO              =162,
      //--- enumeration borders
      INDUSTRY_CONSUMER_DEF_FIRST   =151,
      INDUSTRY_CONSUMER_DEF_LAST    =162,
      INDUSTRY_CONSUMER_DEF_END     =200,
      //--- 
      //--- Energy
      //--- 
      INDUSTRY_OIL_GAS_DRILLING     =201,
      INDUSTRY_OIL_GAS_EP           =202,
      INDUSTRY_OIL_GAS_EQUIPMENT    =203,
      INDUSTRY_OIL_GAS_INTEGRATED   =204,
      INDUSTRY_OIL_GAS_MIDSTREAM    =205,
      INDUSTRY_OIL_GAS_REFINING     =206,
      INDUSTRY_THERMAL_COAL         =207,
      INDUSTRY_URANIUM              =208,
      //--- enumeration borders
      INDUSTRY_ENERGY_FIRST         =201,
      INDUSTRY_ENERGY_LAST          =208,
      INDUSTRY_ENERGY_END           =250,
      //--- 
      //--- Financial
      //--- 
      INDUSTRY_EXCHANGE_TRADED_FUND   =251,
      INDUSTRY_ASSETS_MANAGEMENT      =252,
      INDUSTRY_BANKS_DIVERSIFIED      =253,
      INDUSTRY_BANKS_REGIONAL         =254,
      INDUSTRY_CAPITAL_MARKETS        =255,
      INDUSTRY_CLOSE_END_FUND_DEBT    =256,
      INDUSTRY_CLOSE_END_FUND_EQUITY  =257,
      INDUSTRY_CLOSE_END_FUND_FOREIGN =258,
      INDUSTRY_CREDIT_SERVICES        =259,
      INDUSTRY_FINANCIAL_CONGLOMERATE =260,
      INDUSTRY_FINANCIAL_DATA_EXCHANGE=261,
      INDUSTRY_INSURANCE_BROKERS      =262,
      INDUSTRY_INSURANCE_DIVERSIFIED  =263,
      INDUSTRY_INSURANCE_LIFE         =264,
      INDUSTRY_INSURANCE_PROPERTY     =265,
      INDUSTRY_INSURANCE_REINSURANCE  =266,
      INDUSTRY_INSURANCE_SPECIALTY    =267,
      INDUSTRY_MORTGAGE_FINANCE       =268,
      INDUSTRY_SHELL_COMPANIES        =269,
      //--- enumeration borders
      INDUSTRY_FINANCIAL_FIRST        =251,
      INDUSTRY_FINANCIAL_LAST         =269,
      INDUSTRY_FINANCIAL_END          =300,
      //--- 
      //--- Healthcare
      //--- 
      INDUSTRY_BIOTECHNOLOGY        =301,
      INDUSTRY_DIAGNOSTICS_RESEARCH =302,
      INDUSTRY_DRUGS_MANUFACTURERS  =303,
      INDUSTRY_DRUGS_MANUFACTURERS_SPEC=304,
      INDUSTRY_HEALTHCARE_PLANS        =305,
      INDUSTRY_HEALTH_INFORMATION   =306,
      INDUSTRY_MEDICAL_FACILITIES   =307,
      INDUSTRY_MEDICAL_DEVICES      =308,
      INDUSTRY_MEDICAL_DISTRIBUTION =309,
      INDUSTRY_MEDICAL_INSTRUMENTS  =310,
      INDUSTRY_PHARM_RETAILERS      =311,
      //--- enumeration borders
      INDUSTRY_HEALTHCARE_FIRST     =301,
      INDUSTRY_HEALTHCARE_LAST      =311,
      INDUSTRY_HEALTHCARE_END       =350,
      //--- 
      //--- Industrials
      //--- 
      INDUSTRY_AEROSPACE_DEFENSE    =351,
      INDUSTRY_AIRLINES             =352,
      INDUSTRY_AIRPORTS_SERVICES    =353,
      INDUSTRY_BUILDING_PRODUCTS    =354,
      INDUSTRY_BUSINESS_EQUIPMENT   =355,
      INDUSTRY_CONGLOMERATES        =356,
      INDUSTRY_CONSULTING_SERVICES  =357,
      INDUSTRY_ELECTRICAL_EQUIPMENT =358,
      INDUSTRY_ENGINEERING_CONSTRUCTION  =359,
      INDUSTRY_FARM_HEAVY_MACHINERY      =360,
      INDUSTRY_INDUSTRIAL_DISTRIBUTION   =361,
      INDUSTRY_INFRASTRUCTURE_OPERATIONS =362,
      INDUSTRY_FREIGHT_LOGISTICS    =363,
      INDUSTRY_MARINE_SHIPPING      =364,
      INDUSTRY_METAL_FABRICATION    =365,
      INDUSTRY_POLLUTION_CONTROL    =366,
      INDUSTRY_RAILROADS            =367,
      INDUSTRY_RENTAL_LEASING       =368,
      INDUSTRY_SECURITY_PROTECTION  =369,
      INDUSTRY_SPEALITY_BUSINESS_SERVICES=370,
      INDUSTRY_SPEALITY_MACHINERY   =371,
      INDUSTRY_STUFFING_EMPLOYMENT  =372,
      INDUSTRY_TOOLS_ACCESSORIES    =373,
      INDUSTRY_TRUCKING             =374,
      INDUSTRY_WASTE_MANAGEMENT     =375,
      //--- enumeration borders
      INDUSTRY_INDUSTRIALS_FIRST    =351,
      INDUSTRY_INDUSTRIALS_LAST     =375,
      INDUSTRY_INDUSTRIALS_END      =400,
      //--- 
      //--- Real Estate
      //--- 
      INDUSTRY_REAL_ESTATE_DEVELOPMENT=401,
      INDUSTRY_REAL_ESTATE_DIVERSIFIED=402,
      INDUSTRY_REAL_ESTATE_SERVICES   =403,
      INDUSTRY_REIT_DIVERSIFIED     =404,
      INDUSTRY_REIT_HEALTCARE       =405,
      INDUSTRY_REIT_HOTEL_MOTEL     =406,
      INDUSTRY_REIT_INDUSTRIAL      =407,
      INDUSTRY_REIT_MORTAGE         =408,
      INDUSTRY_REIT_OFFICE          =409,
      INDUSTRY_REIT_RESIDENTAL      =410,
      INDUSTRY_REIT_RETAIL          =411,
      INDUSTRY_REIT_SPECIALITY      =412,
      //--- enumeration borders
      INDUSTRY_REAL_ESTATE_FIRST    =401,
      INDUSTRY_REAL_ESTATE_LAST     =412,
      INDUSTRY_REAL_ESTATE_END      =450,
      //--- 
      //--- Technology
      //--- 
      INDUSTRY_COMMUNICATION_EQUIPMENT=451,
      INDUSTRY_COMPUTER_HARDWARE      =452,
      INDUSTRY_CONSUMER_ELECTRONICS   =453,
      INDUSTRY_ELECTRONIC_COMPONENTS  =454,
      INDUSTRY_ELECTRONIC_DISTRIBUTION=455,
      INDUSTRY_IT_SERVICES            =456,
      INDUSTRY_SCIENTIFIC_INSTRUMENTS =457,
      INDUSTRY_SEMICONDUCTOR_EQUIPMENT=458,
      INDUSTRY_SEMICONDUCTORS         =459,
      INDUSTRY_SOFTWARE_APPLICATION   =460,
      INDUSTRY_SOFTWARE_INFRASTRUCTURE=461,
      INDUSTRY_SOLAR                  =462,
      //--- enumeration borders
      INDUSTRY_TECHNOLOGY_FIRST       =451,
      INDUSTRY_TECHNOLOGY_LAST        =462,
      INDUSTRY_TECHNOLOGY_END         =500,
      //--- 
      //--- Utilities
      //--- 
      INDUSTRY_UTILITIES_DIVERSIFIED       =501,
      INDUSTRY_UTILITIES_POWERPRODUCERS    =502,
      INDUSTRY_UTILITIES_RENEWABLE         =503,
      INDUSTRY_UTILITIES_REGULATED_ELECTRIC=504,
      INDUSTRY_UTILITIES_REGULATED_GAS     =505,
      INDUSTRY_UTILITIES_REGULATED_WATER   =506,
      //--- enumeration borders
      INDUSTRY_UTILITIES_FIRST        =501,
      INDUSTRY_UTILITIES_LAST         =506,
      INDUSTRY_UTILITIES_END          =550,
      //--- 
      //--- Commodities
      //--- 
      INDUSTRY_COMMODITIES_AGRICULTURAL=551,
      INDUSTRY_COMMODITIES_ENERGY     =552,
      INDUSTRY_COMMODITIES_METALS     =553,
      INDUSTRY_COMMODITIES_PRECIOUS   =554,
      //--- enumeration borders
      INDUSTRY_COMMODITIES_FIRST      =551,
      INDUSTRY_COMMODITIES_LAST       =554,
      INDUSTRY_COMMODITIES_END        =600,
      //--- enumeration borders
      INDUSTRY_FIRST                  =0,
      INDUSTRY_LAST                   =INDUSTRY_COMMODITIES_LAST
     };
   //--- allowed filling modes flags
   enum EnFillingFlags
     {
      FILL_FLAGS_NONE         =0, // none
      FILL_FLAGS_FOK          =1, // allowed FOK
      FILL_FLAGS_IOC          =2, // allowed IOC
      FILL_FLAGS_BOC          =4, // book or cancel
      //--- flags borders
      FILL_FLAGS_FIRST        =FILL_FLAGS_FOK,
      FILL_FLAGS_ALL          =FILL_FLAGS_FOK|FILL_FLAGS_IOC|FILL_FLAGS_BOC
     };
   //--- allowed order expiration modes flags
   enum EnExpirationFlags
     {
      TIME_FLAGS_NONE         =0, // none
      TIME_FLAGS_GTC          =1, // allowed Good Till Cancel
      TIME_FLAGS_DAY          =2, // allowed Good Till Day
      TIME_FLAGS_SPECIFIED    =4, // allowed specified expiration date
      TIME_FLAGS_SPECIFIED_DAY=8, // allowed specified expiration date as day
      //--- flags borders
      TIME_FLAGS_FIRST        =TIME_FLAGS_GTC,
      TIME_FLAGS_ALL          =TIME_FLAGS_GTC|TIME_FLAGS_DAY|TIME_FLAGS_SPECIFIED|TIME_FLAGS_SPECIFIED_DAY
     };
   //--- allowed order flags
   enum EnOrderFlags
     {
      ORDER_FLAGS_NONE        =0,  // none
      ORDER_FLAGS_MARKET      =1,  // market orders
      ORDER_FLAGS_LIMIT       =2,  // limit orders
      ORDER_FLAGS_STOP        =4,  // stop orders
      ORDER_FLAGS_STOP_LIMIT  =8,  // stop limit orders
      ORDER_FLAGS_SL          =16, // sl orders
      ORDER_FLAGS_TP          =32, // tp orders
      ORDER_FLAGS_CLOSEBY     =64, // close-by orders
      //--- flags borders
      ORDER_FLAGS_FIRST       =ORDER_FLAGS_MARKET,
      ORDER_FLAGS_ALL         =ORDER_FLAGS_MARKET|ORDER_FLAGS_LIMIT|ORDER_FLAGS_STOP|ORDER_FLAGS_STOP_LIMIT|ORDER_FLAGS_SL|ORDER_FLAGS_TP|ORDER_FLAGS_CLOSEBY
     };
   //--- allowed trade modes
   enum EnTradeMode
     {
      TRADE_DISABLED          =0, // trade disabled
      TRADE_LONGONLY          =1, // only long positions allowed
      TRADE_SHORTONLY         =2, // only short positions allowed
      TRADE_CLOSEONLY         =3, // only position closure
      TRADE_FULL              =4, // all trade operations are allowed
      //--- enumeration borders
      TRADE_FIRST             =TRADE_DISABLED,
      TRADE_LAST              =TRADE_FULL
     };
   //--- order execution modes
   enum EnExecutionMode
     {
      EXECUTION_REQUEST       =0, // Request Execution
      EXECUTION_INSTANT       =1, // Instant Execution
      EXECUTION_MARKET        =2, // Market Execution
      EXECUTION_EXCHANGE      =3, // Exchange Execution
      //--- enumeration borders
      EXECUTION_FIRST         =EXECUTION_REQUEST,
      EXECUTION_LAST          =EXECUTION_EXCHANGE
     };
   //--- profit and margin calculation modes
   enum EnCalcMode
     {
      //--- market maker modes
      TRADE_MODE_FOREX              =0,
      TRADE_MODE_FUTURES            =1,
      TRADE_MODE_CFD                =2,
      TRADE_MODE_CFDINDEX           =3,
      TRADE_MODE_CFDLEVERAGE        =4,
      TRADE_MODE_FOREX_NO_LEVERAGE  =5,
      //--- market makers enumerations
      TRADE_MODE_MM_FIRST           =TRADE_MODE_FOREX,
      TRADE_MODE_MM_LAST            =TRADE_MODE_FOREX_NO_LEVERAGE,
      //--- exchange modes
      TRADE_MODE_EXCH_STOCKS        =32,
      TRADE_MODE_EXCH_FUTURES       =33,
      TRADE_MODE_EXCH_FUTURES_FORTS =34,
      TRADE_MODE_EXCH_OPTIONS       =35,
      TRADE_MODE_EXCH_OPTIONS_MARGIN=36,
      TRADE_MODE_EXCH_BONDS         =37,
      TRADE_MODE_EXCH_STOCKS_MOEX   =38,
      TRADE_MODE_EXCH_BONDS_MOEX    =39,
      //--- exchange enumerations
      TRADE_MODE_EXCH_FIRST         =TRADE_MODE_EXCH_STOCKS,
      TRADE_MODE_EXCH_LAST          =TRADE_MODE_EXCH_BONDS_MOEX,
      //--- service modes
      TRADE_MODE_SERV_COLLATERAL    =64,
      //--- service enumerations
      TRADE_MODE_SERV_FIRST         =TRADE_MODE_SERV_COLLATERAL,
      TRADE_MODE_SERV_LAST          =TRADE_MODE_SERV_COLLATERAL,
      //--- enumeration borders
      TRADE_MODE_FIRST              =TRADE_MODE_FOREX,
      TRADE_MODE_LAST               =TRADE_MODE_SERV_COLLATERAL
     };
   //--- orders expiration modes
   enum EnGTCMode
     {
      ORDERS_GTC              =0,
      ORDERS_DAILY            =1,
      ORDERS_DAILY_NO_STOPS   =2,
      //--- enumeration borders
      ORDERS_FIRST            =ORDERS_GTC,
      ORDERS_LAST             =ORDERS_DAILY_NO_STOPS
     };
   //--- tick collection flags
   enum EnTickFlags
     {
      TICK_REALTIME           =1,  // allow realtime tick apply
      TICK_COLLECTRAW         =2,  // allow to collect raw ticks
      TICK_FEED_STATS         =4,  // allow to receive price statisticks from datafeeds
      TICK_NEGATIVE_PRICES    =8,  // allow to receive negative prices
      //--- flags borders
      TICK_NONE               =0,
      TICK_ALL                =TICK_REALTIME|TICK_COLLECTRAW|TICK_FEED_STATS|TICK_NEGATIVE_PRICES
     };
   //--- chart mode
   enum EnChartMode
     {
      CHART_MODE_BID_PRICE    =0,
      CHART_MODE_LAST_PRICE   =1,
      CHART_MODE_OLD          =255,
      //--- enumeration borders
      CHART_MODE_FIRST        =CHART_MODE_BID_PRICE,
      CHART_MODE_LAST         =CHART_MODE_OLD
     };
   //--- margin check modes
   enum EnMarginFlags
     {
      MARGIN_FLAGS_NONE             =0x0000000,  // none
      MARGIN_FLAGS_CHECK_PROCESS    =0x0000001,  // check margin after dealer confirmation
      MARGIN_FLAGS_CHECK_SLTP       =0x0000002,  // check margin on SL-TP trigger
      MARGIN_FLAGS_HEDGE_LARGE_LEG  =0x0000004,  // check margin for hedged positions using large leg
      MARGIN_FLAGS_EXCLUDE_PL       =0x0000008,  // exclude buy positions PL from free margin and margin level calculation
      MARGIN_FLAGS_RECALC_RATES     =0x0000010,  // recalculate margin exchange rate on the End of Day
      //--- flags borders
      MARGIN_FLAGS_ALL              =MARGIN_FLAGS_CHECK_PROCESS|MARGIN_FLAGS_CHECK_SLTP|MARGIN_FLAGS_HEDGE_LARGE_LEG|MARGIN_FLAGS_EXCLUDE_PL|MARGIN_FLAGS_RECALC_RATES
     };
   //--- swaps calculation modes
   enum EnSwapMode
     {
      SWAP_DISABLED           =0,
      SWAP_BY_POINTS          =1,
      SWAP_BY_SYMBOL_CURRENCY =2,
      SWAP_BY_MARGIN_CURRENCY =3,
      SWAP_BY_GROUP_CURRENCY  =4,
      SWAP_BY_INTEREST_CURRENT=5,
      SWAP_BY_INTEREST_OPEN   =6,
      SWAP_REOPEN_BY_CLOSE_PRICE=7,
      SWAP_REOPEN_BY_BID        =8,
      SWAP_BY_PROFIT_CURRENCY   =9,
      //--- enumeration borders
      SWAP_FIRST              =SWAP_DISABLED,
      SWAP_LAST               =SWAP_BY_PROFIT_CURRENCY
     };
   //--- swaps days
   enum EnSwapDays
     {
      SWAP_DAY_SUNDAY         =0,
      SWAP_DAY_MONDAY         =1,
      SWAP_DAY_TUESDAY        =2,
      SWAP_DAY_WEDNESDAY      =3,
      SWAP_DAY_THURSDAY       =4,
      SWAP_DAY_FRIDAY         =5,
      SWAP_DAY_SATURDAY       =6,
      SWAP_DAY_DISABLED       =7,
      //--- enumeration borders
      SWAP_DAY_FIRST          =SWAP_DAY_SUNDAY,
      SWAP_DAY_LAST           =SWAP_DAY_DISABLED
     };
   //--- swap flags
   enum EnSwapFlags
     {
      SWAP_FLAGS_NONE             =0x00000000,
      SWAP_FLAGS_CONSIDER_HOLIDAYS=0x00000001,
      //--- enumeration borders
      SWAP_FLAGS_DEFAULT          =SWAP_FLAGS_NONE,
      SWAP_FLAGS_ALL              =SWAP_FLAGS_CONSIDER_HOLIDAYS
     };
   //--- Instant Execution Flags
   enum EnInstantFlags
     {
      INSTANT_FLAGS_NONE             =0x00000000,
      INSTANT_FLAGS_FAST_CONFIRMATION=0x00000001,
      //--- enumeration borders
      INSTANT_FLAGS_ALL              =INSTANT_FLAGS_FAST_CONFIRMATION
     };
   //--- Instant Execution Modes
   enum EnInstantMode
     {
      INSTANT_CHECK_NORMAL    =0,
      //--- enumeration borders
      INSTANT_CHECK_FIRST     =INSTANT_CHECK_NORMAL,
      INSTANT_CHECK_LAST      =INSTANT_CHECK_NORMAL
     };
   //--- Request Execution Flags
   enum EnRequestFlags
     {
      REQUEST_FLAGS_NONE      =0,  // none
      REQUEST_FLAGS_ORDER     =1,  // trade orders should be additional confirmed after quotation
      //--- flags borders
      REQUEST_FLAGS_ALL       =REQUEST_FLAGS_ORDER
     };
   //--- Common Trade Flags
   enum EnTradeFlags
     {
      TRADE_FLAGS_NONE            =0,   // none
      TRADE_FLAGS_PROFIT_BY_MARKET=1,   // convert fx profit using market prices
      TRADE_FLAGS_ALLOW_SIGNALS   =2,   // allow trade signals for symbol
      //--- flags borders
      TRADE_FLAGS_ALL             =TRADE_FLAGS_PROFIT_BY_MARKET|TRADE_FLAGS_ALLOW_SIGNALS,
      TRADE_FLAGS_DEFAULT         =TRADE_FLAGS_ALLOW_SIGNALS
     };
   //--- Margin Rate Types
   enum EnMarginRateTypes
     {
      MARGIN_RATE_BUY         =0,
      MARGIN_RATE_SELL        =1,
      MARGIN_RATE_BUY_LIMIT   =2,
      MARGIN_RATE_SELL_LIMIT  =3,
      MARGIN_RATE_BUY_STOP    =4,
      MARGIN_RATE_SELL_STOP   =5,
      MARGIN_RATE_BUY_STOP_LIMIT =6,
      MARGIN_RATE_SELL_STOP_LIMIT=7,
      //--- enumeration borders
      MARGIN_RATE_FIRST       =MARGIN_RATE_BUY,
      MARGIN_RATE_LAST        =MARGIN_RATE_SELL_STOP_LIMIT
     };
   //--- Options Mode
   enum EnOptionMode
     {
      OPTION_MODE_EUROPEAN_CALL=0,
      OPTION_MODE_EUROPEAN_PUT =1,
      OPTION_MODE_AMERICAN_CALL=2,
      OPTION_MODE_AMERICAN_PUT =3,
      //--- enumeration borders
      OPTION_MODE_FIRST        =OPTION_MODE_EUROPEAN_CALL,
      OPTION_MODE_LAST         =OPTION_MODE_AMERICAN_PUT
     };
   //--- Splice Type
   enum EnSpliceType
     {
      SPLICE_NONE              =0,
      SPLICE_UNADJUSTED        =1,
      SPLICE_ADJUSTED          =2,
      //--- enumeration borders
      SPLICE_FIRST             =SPLICE_NONE,
      SPLICE_LAST              =SPLICE_ADJUSTED
     };
   //--- Splice Time Type
   enum EnSpliceTimeType
     {
      SPLICE_TIME_EXPIRATION   =0,
      //--- enumeration borders
      SPLICE_TIME_FIRST        =SPLICE_TIME_EXPIRATION,
      SPLICE_TIME_LAST         =SPLICE_TIME_EXPIRATION
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConSymbol* symbol)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- name
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- hierarchical symbol path (including symbol name)
   virtual LPCWSTR   Path(void) const=0;
   virtual MTAPIRES  Path(LPCWSTR path)=0;
   //--- ISIN
   virtual LPCWSTR   ISIN(void) const=0;
   virtual MTAPIRES  ISIN(LPCWSTR isin)=0;
   //--- local description
   virtual LPCWSTR   Description(void) const=0;
   virtual MTAPIRES  Description(LPCWSTR descr)=0;
   //--- internation description
   virtual LPCWSTR   International(void) const=0;
   virtual MTAPIRES  International(LPCWSTR intern)=0;
   //--- basic symbol name
   virtual LPCWSTR   Basis(void) const=0;
   virtual MTAPIRES  Basis(LPCWSTR basis)=0;
   //--- source symbol name
   virtual LPCWSTR   Source(void) const=0;
   virtual MTAPIRES  Source(LPCWSTR source)=0;
   //--- symbol specification page URL
   virtual LPCWSTR   Page(void) const=0;
   virtual MTAPIRES  Page(LPCWSTR page)=0;
   //--- symbol base currency
   virtual LPCWSTR   CurrencyBase(void) const=0;
   virtual MTAPIRES  CurrencyBase(LPCWSTR currency)=0;
   virtual uint32_t  CurrencyBaseDigits(void) const=0;
   //--- symbol profit currency
   virtual LPCWSTR   CurrencyProfit(void) const=0;
   virtual MTAPIRES  CurrencyProfit(LPCWSTR currency)=0;
   virtual uint32_t  CurrencyProfitDigits(void) const=0;
   //--- symbol margin currency
   virtual LPCWSTR   CurrencyMargin(void) const=0;
   virtual MTAPIRES  CurrencyMargin(LPCWSTR currency)=0;
   virtual uint32_t  CurrencyMarginDigits(void) const=0;
   //--- symbol color
   virtual COLORREF  Color(void) const=0;
   virtual MTAPIRES  Color(const COLORREF color)=0;
   //--- symbol background color
   virtual COLORREF  ColorBackground(void) const=0;
   virtual MTAPIRES  ColorBackground(const COLORREF color)=0;
   //--- symbol digits
   virtual uint32_t  Digits(void) const=0;
   virtual MTAPIRES  Digits(const uint32_t digits)=0;
   //--- symbol digits derivation (1/10^digits & 10^digits)
   virtual double    Point(void) const=0;
   virtual double    Multiply(void) const=0;
   //--- EnTickFlags
   virtual uint64_t    TickFlags(void) const=0;
   virtual MTAPIRES  TickFlags(const uint64_t flags)=0;
   //--- Depth of Market depth (both legs)
   virtual uint32_t  TickBookDepth(void) const=0;
   virtual MTAPIRES  TickBookDepth(const uint32_t depth)=0;
   //--- filtration soft level
   virtual uint32_t  FilterSoft(void) const=0;
   virtual MTAPIRES  FilterSoft(const uint32_t filter)=0;
   //--- filtration soft level counter
   virtual uint32_t  FilterSoftTicks(void) const=0;
   virtual MTAPIRES  FilterSoftTicks(const uint32_t ticks)=0;
   //--- filtration hard level
   virtual uint32_t  FilterHard(void) const=0;
   virtual MTAPIRES  FilterHard(const uint32_t filter)=0;
   //--- filtration hard level counter
   virtual uint32_t  FilterHardTicks(void) const=0;
   virtual MTAPIRES  FilterHardTicks(const uint32_t ticks)=0;
   //--- filtration discard level
   virtual uint32_t  FilterDiscard(void) const=0;
   virtual MTAPIRES  FilterDiscard(const uint32_t ticks)=0;
   //--- spread max value
   virtual uint32_t  FilterSpreadMax(void) const=0;
   virtual MTAPIRES  FilterSpreadMax(const uint32_t spread)=0;
   //--- spread min value
   virtual uint32_t  FilterSpreadMin(void) const=0;
   virtual MTAPIRES  FilterSpreadMin(const uint32_t spread)=0;
   //--- EnTradeMode
   virtual uint32_t  TradeMode(void) const=0;
   virtual MTAPIRES  TradeMode(const uint32_t mode)=0;
   //--- EnCalcMode
   virtual uint32_t  CalcMode(void) const=0;
   virtual MTAPIRES  CalcMode(const uint32_t mode)=0;
   //--- EnExecutionMode
   virtual uint32_t  ExecMode(void) const=0;
   virtual MTAPIRES  ExecMode(const uint32_t mode)=0;
   //--- EnGTCMode
   virtual uint32_t  GTCMode(void) const=0;
   virtual MTAPIRES  GTCMode(const uint32_t mode)=0;
   //--- EnFillingFlags
   virtual uint32_t  FillFlags(void) const=0;
   virtual MTAPIRES  FillFlags(const uint32_t flags)=0;
   //--- EnExpirationFlags
   virtual uint32_t  ExpirFlags(void) const=0;
   virtual MTAPIRES  ExpirFlags(const uint32_t flags)=0;
   //--- symbol spread (0-floating)
   virtual uint32_t  Spread(void) const=0;
   virtual MTAPIRES  Spread(const uint32_t spread)=0;
   //--- spread balance
   virtual INT       SpreadBalance(void) const=0;
   virtual MTAPIRES  SpreadBalance(const INT spread)=0;
   //--- spread difference
   virtual INT       SpreadDiff(void) const=0;
   virtual MTAPIRES  SpreadDiff(const INT diff)=0;
   //--- spread difference balance
   virtual INT       SpreadDiffBalance(void) const=0;
   virtual MTAPIRES  SpreadDiffBalance(const INT spread)=0;
   //--- tick value
   virtual double    TickValue(void) const=0;
   virtual MTAPIRES  TickValue(const double value)=0;
   //--- tick size
   virtual double    TickSize(void) const=0;
   virtual MTAPIRES  TickSize(const double size)=0;
   //--- contract size
   virtual double    ContractSize(void) const=0;
   virtual MTAPIRES  ContractSize(const double size)=0;
   //--- stops level
   virtual INT       StopsLevel(void) const=0;
   virtual MTAPIRES  StopsLevel(const INT level)=0;
   //--- freeze level
   virtual INT       FreezeLevel(void) const=0;
   virtual MTAPIRES  FreezeLevel(const INT level)=0;
   //--- quotes timeout 
   virtual uint32_t  QuotesTimeout(void) const=0;
   virtual MTAPIRES  QuotesTimeout(const uint32_t timeout)=0;
   //--- minimal volume
   virtual uint64_t    VolumeMin(void) const=0;
   virtual MTAPIRES  VolumeMin(const uint64_t volume)=0;
   //--- maximal volume
   virtual uint64_t    VolumeMax(void) const=0;
   virtual MTAPIRES  VolumeMax(const uint64_t volume)=0;
   //--- volume step
   virtual uint64_t    VolumeStep(void) const=0;
   virtual MTAPIRES  VolumeStep(const uint64_t volume)=0;
   //--- cumulative positions and orders limit
   virtual uint64_t    VolumeLimit(void) const=0;
   virtual MTAPIRES  VolumeLimit(const uint64_t volume)=0;
   //--- EnMarginFlags
   virtual uint32_t  MarginFlags(void) const=0;
   virtual MTAPIRES  MarginFlags(const uint32_t mode)=0;
   //--- initial margin
   virtual double    MarginInitial(void) const=0;
   virtual MTAPIRES  MarginInitial(const double margin)=0;
   //--- maintenance margin
   virtual double    MarginMaintenance(void) const=0;
   virtual MTAPIRES  MarginMaintenance(const double margin)=0;
   //--- long orders and positions margin rate
   virtual double    MarginLong(void) const=0;
   virtual MTAPIRES  MarginLong(const double margin)=0;
   //--- short orders and positions margin rate
   virtual double    MarginShort(void) const=0;
   virtual MTAPIRES  MarginShort(const double margin)=0;
   //--- limit orders and positions margin rate
   virtual double    MarginLimit(void) const=0;
   virtual MTAPIRES  MarginLimit(const double margin)=0;
   //--- stop orders and positions margin rate
   virtual double    MarginStop(void) const=0;
   virtual MTAPIRES  MarginStop(const double margin)=0;
   //--- stop-limit orders and positions margin rate
   virtual double    MarginStopLimit(void) const=0;
   virtual MTAPIRES  MarginStopLimit(const double margin)=0;
   //--- EnSwapMode
   virtual uint32_t  SwapMode(void) const=0;
   virtual MTAPIRES  SwapMode(const uint32_t mode)=0;
   //--- long positions swaps rate
   virtual double    SwapLong(void) const=0;
   virtual MTAPIRES  SwapLong(const double swap)=0;
   //--- short positions swaps rate
   virtual double    SwapShort(void) const=0;
   virtual MTAPIRES  SwapShort(const double swap)=0;
   //--- 3 time swaps day, EnSwapDay
   virtual uint32_t  Swap3Day(void) const=0;
   virtual MTAPIRES  Swap3Day(const uint32_t day)=0;
   //--- trade start date
   virtual int64_t     TimeStart(void) const=0;
   virtual MTAPIRES  TimeStart(const int64_t start)=0;
   //--- trade end date
   virtual int64_t     TimeExpiration(void) const=0;
   virtual MTAPIRES  TimeExpiration(const int64_t expiration)=0;
   //--- quote sessions
   virtual MTAPIRES  SessionQuoteAdd(const uint32_t wday,IMTConSymbolSession* symbol)=0;
   virtual MTAPIRES  SessionQuoteUpdate(const uint32_t wday,const uint32_t pos,const IMTConSymbolSession* session)=0;
   virtual MTAPIRES  SessionQuoteDelete(const uint32_t wday,const uint32_t pos)=0;
   virtual MTAPIRES  SessionQuoteClear(const uint32_t wday)=0;
   virtual MTAPIRES  SessionQuoteShift(const uint32_t wday,const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  SessionQuoteTotal(const uint32_t wday) const=0;
   virtual MTAPIRES  SessionQuoteNext(const uint32_t wday,const uint32_t pos,IMTConSymbolSession* session) const=0;
   //--- trade sessions
   virtual MTAPIRES  SessionTradeAdd(const uint32_t wday,IMTConSymbolSession* symbol)=0;
   virtual MTAPIRES  SessionTradeUpdate(const uint32_t wday,const uint32_t pos,const IMTConSymbolSession* session)=0;
   virtual MTAPIRES  SessionTradeDelete(const uint32_t wday,const uint32_t pos)=0;
   virtual MTAPIRES  SessionTradeClear(const uint32_t wday)=0;
   virtual MTAPIRES  SessionTradeShift(const uint32_t wday,const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  SessionTradeTotal(const uint32_t wday) const=0;
   virtual MTAPIRES  SessionTradeNext(const uint32_t wday,const uint32_t pos,IMTConSymbolSession* session) const=0;
   //--- request execution flags
   virtual uint32_t  REFlags(void) const=0;
   virtual MTAPIRES  REFlags(const uint32_t flags)=0;
   //--- request execution timeout
   virtual uint32_t  RETimeout(void) const=0;
   virtual MTAPIRES  RETimeout(const uint32_t timeout)=0;
   //--- instant execution check mode 
   virtual uint32_t  IECheckMode(void) const=0;
   virtual MTAPIRES  IECheckMode(const uint32_t mode)=0;
   //--- instant execution timeout
   virtual uint32_t  IETimeout(void) const=0;
   virtual MTAPIRES  IETimeout(const uint32_t timeout)=0;
   //--- instant execution profit slippage
   virtual uint32_t  IESlipProfit(void) const=0;
   virtual MTAPIRES  IESlipProfit(const uint32_t slippage)=0;
   //--- instant execution losing slippage
   virtual uint32_t  IESlipLosing(void) const=0;
   virtual MTAPIRES  IESlipLosing(const uint32_t slippage)=0;
   //--- instant execution max volume
   virtual uint64_t    IEVolumeMax(void) const=0;
   virtual MTAPIRES  IEVolumeMax(const uint64_t volume)=0;
   //--- settle price (for futures)
   virtual double    PriceSettle(void) const=0;
   virtual MTAPIRES  PriceSettle(const double price)=0;
   //--- price limit max (for futures)
   virtual double    PriceLimitMax(void) const=0;
   virtual MTAPIRES  PriceLimitMax(const double price)=0;
   //--- price limit min (for futures)
   virtual double    PriceLimitMin(void) const=0;
   virtual MTAPIRES  PriceLimitMin(const double price)=0;
   //--- EnTradeFlags
   virtual uint64_t    TradeFlags(void) const=0;
   virtual MTAPIRES  TradeFlags(const uint64_t flags)=0;
   //--- EnOrderFlags
   virtual uint32_t  OrderFlags(void) const=0;
   virtual MTAPIRES  OrderFlags(const uint32_t flags)=0;
   //--- orders and positions margin rates
   virtual double    MarginRateInitial(const uint32_t type) const=0;
   virtual MTAPIRES  MarginRateInitial(const uint32_t type,const double margin_rate)=0;
   //--- orders and positions margin rates
   virtual double    MarginRateMaintenance(const uint32_t type) const=0;
   virtual MTAPIRES  MarginRateMaintenance(const uint32_t type,const double margin_rate)=0;
   //--- options mode EnOptionMode
   virtual uint32_t  OptionsMode(void) const=0;
   virtual MTAPIRES  OptionsMode(const uint32_t mode)=0;
   //--- option strike price value
   virtual double    PriceStrike(void) const=0;
   virtual MTAPIRES  PriceStrike(const double price)=0;
   //--- liquidity rate
   virtual double    MarginRateLiquidity(void) const=0;
   virtual MTAPIRES  MarginRateLiquidity(const double margin_rate)=0;
   //--- bond face value
   virtual double    FaceValue(void) const=0;
   virtual MTAPIRES  FaceValue(const double value)=0;
   //--- bond accrued interest
   virtual double    AccruedInterest(void) const=0;
   virtual MTAPIRES  AccruedInterest(const double interest)=0;
   //--- futures splice type EnSpliceType
   virtual uint32_t  SpliceType(void) const=0;
   virtual MTAPIRES  SpliceType(const uint32_t type)=0;
   //--- futures splice time type EnSpliceType
   virtual uint32_t  SpliceTimeType(void) const=0;
   virtual MTAPIRES  SpliceTimeType(const uint32_t time_type)=0;
   //--- options splice point in days
   virtual uint32_t  SpliceTimeDays(void) const=0;
   virtual MTAPIRES  SpliceTimeDays(const uint32_t days)=0;
   //--- hedged positions margin rate
   virtual double    MarginHedged(void) const=0;
   virtual MTAPIRES  MarginHedged(const double margin)=0;
   //--- currency rate
   virtual double    MarginRateCurrency(void) const=0;
   virtual MTAPIRES  MarginRateCurrency(const double margin_rate)=0;
   //--- gap level
   virtual uint32_t  FilterGap(void) const=0;
   virtual MTAPIRES  FilterGap(const uint32_t gap)=0;
   //--- gap level ticks
   virtual uint32_t  FilterGapTicks(void) const=0;
   virtual MTAPIRES  FilterGapTicks(const uint32_t ticks)=0;
   //--- chart mode
   virtual uint32_t  ChartMode(void) const=0;
   virtual MTAPIRES  ChartMode(const uint32_t mode)=0;
   //---  currency digits
   virtual MTAPIRES  CurrencyBaseDigitsSet(const uint32_t digits)=0;
   virtual MTAPIRES  CurrencyProfitDigitsSet(const uint32_t digits)=0;
   virtual MTAPIRES  CurrencyMarginDigitsSet(const uint32_t digits)=0;
   //--- instant execution flags with extended accuracy
   virtual uint32_t  IEFlags(void) const=0;
   virtual MTAPIRES  IEFlags(const uint32_t flags)=0;
   //--- minimal volume with extended accuracy
   virtual uint64_t    VolumeMinExt(void) const=0;
   virtual MTAPIRES  VolumeMinExt(const uint64_t volume)=0;
   //--- maximal volume with extended accuracy
   virtual uint64_t    VolumeMaxExt(void) const=0;
   virtual MTAPIRES  VolumeMaxExt(const uint64_t volume)=0;
   //--- volume step with extended accuracy
   virtual uint64_t    VolumeStepExt(void) const=0;
   virtual MTAPIRES  VolumeStepExt(const uint64_t volume)=0;
   //--- cumulative positions and orders limit with extended accuracy
   virtual uint64_t    VolumeLimitExt(void) const=0;
   virtual MTAPIRES  VolumeLimitExt(const uint64_t volume)=0;
   //--- instant execution max volume with extended accuracy
   virtual uint64_t    IEVolumeMaxExt(void) const=0;
   virtual MTAPIRES  IEVolumeMaxExt(const uint64_t volume)=0;
   //--- category
   virtual LPCWSTR   Category(void) const=0;
   virtual MTAPIRES  Category(LPCWSTR category)=0;
   //--- exchange
   virtual LPCWSTR   Exchange(void) const=0;
   virtual MTAPIRES  Exchange(LPCWSTR exchange)=0;
   //--- CFI
   virtual LPCWSTR   CFI(void) const=0;
   virtual MTAPIRES  CFI(LPCWSTR cfi)=0;
   //--- Sector
   virtual uint32_t  Sector(void) const=0;
   virtual MTAPIRES  Sector(const uint32_t sector)=0;
   //--- Industry
   virtual uint32_t  Industry(void) const=0;
   virtual MTAPIRES  Industry(const uint32_t industry)=0;
   //--- Country - ISO 3166-1 alpha-3 code
   virtual LPCWSTR   Country(void) const=0;
   virtual MTAPIRES  Country(LPCWSTR country)=0;
   //--- Delay for subscriptions
   virtual uint32_t  SubscriptionsDelay(void) const=0;
   virtual MTAPIRES  SubscriptionsDelay(const uint32_t delay)=0;
   //--- Days in year
   virtual uint32_t  SwapYearDays(void) const=0;
   virtual MTAPIRES  SwapYearDays(const uint32_t days)=0;
   //--- swap flags
   virtual uint32_t  SwapFlags(void) const=0;
   virtual MTAPIRES  SwapFlags(const uint32_t flags)=0;
   //--- swap rate for Sunday
   virtual double    SwapRateSunday(void) const=0;
   virtual MTAPIRES  SwapRateSunday(const double rate)=0;
   //--- swap rate for Monday
   virtual double    SwapRateMonday(void) const=0;
   virtual MTAPIRES  SwapRateMonday(const double rate)=0;
   //--- swap rate for Tuesday
   virtual double    SwapRateTuesday(void) const=0;
   virtual MTAPIRES  SwapRateTuesday(const double rate)=0;
   //--- swap rate for Wednesday
   virtual double    SwapRateWednesday(void) const=0;
   virtual MTAPIRES  SwapRateWednesday(const double rate)=0;
   //--- swap rate for Thursday
   virtual double    SwapRateThursday(void) const=0;
   virtual MTAPIRES  SwapRateThursday(const double rate)=0;
   //--- swap rate for Friday
   virtual double    SwapRateFriday(void) const=0;
   virtual MTAPIRES  SwapRateFriday(const double rate)=0;
   //--- swap rate for Saturday
   virtual double    SwapRateSaturday(void) const=0;
   virtual MTAPIRES  SwapRateSaturday(const double rate)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConSymbol(void) {}
  };
//+------------------------------------------------------------------+
//| Symbol  configuration array interface                            |
//+------------------------------------------------------------------+
class IMTConSymbolArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConSymbolArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTConSymbol* record)=0;
   virtual MTAPIRES  AddCopy(const IMTConSymbol* record)=0;
   virtual MTAPIRES  Add(IMTConSymbolArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTConSymbolArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTConSymbol* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTConSymbol* record)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTConSymbol* record)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTConSymbol*  Next(const uint32_t index) const=0;
   //--- sorting and search
   virtual MTAPIRES  Sort(MTSortFunctionPtr sort_function)=0;
   virtual int32_t   Search(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchGreatOrEq(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchGreater(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchLessOrEq(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchLess(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchLeft(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchRight(const void *key,MTSortFunctionPtr sort_function) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConSymbolArray(void) {}
  };
//+------------------------------------------------------------------+
//| Symbol config events notification interface                      |
//+------------------------------------------------------------------+
class IMTConSymbolSink
  {
public:
   virtual void      OnSymbolAdd(const IMTConSymbol*    /*config*/) {  }
   virtual void      OnSymbolUpdate(const IMTConSymbol* /*config*/) {  }
   virtual void      OnSymbolDelete(const IMTConSymbol* /*config*/) {  }
   virtual void      OnSymbolSync(void)                             {  }
   //--- config modification hooks (only for Server API)
   virtual MTAPIRES  HookSymbolAdd(const uint64_t /*login*/,IMTConSymbol* /*new_cfg*/)                                { return(MT_RET_OK); }
   virtual MTAPIRES  HookSymbolUpdate(const uint64_t /*login*/,const IMTConSymbol* /*cfg*/,IMTConSymbol* /*new_cfg*/) { return(MT_RET_OK); }
   virtual MTAPIRES  HookSymbolDelete(const uint64_t /*login*/,const IMTConSymbol* /*cfg*/)                           { return(MT_RET_OK); }
  };
//+------------------------------------------------------------------+
