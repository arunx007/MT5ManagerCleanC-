//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Client record interface                                          |
//+------------------------------------------------------------------+
class IMTClient
  {
public:
   //--- genders
   enum EnGender
     {
      GENDER_UNSPECIFIED                  =0,            // unspecified
      GENDER_MALE                         =1,            // male
      GENDER_FEMALE                       =2,            // female
      //--- enumeration borders
      GENDER_FIRST                        =GENDER_UNSPECIFIED,
      GENDER_LAST                         =GENDER_FEMALE
     };
   //--- client types
   enum EnClientType
     {
      CLIENT_TYPE_UNDEFINED               =0,            // undefined
      CLIENT_TYPE_INDIVIDUAL              =1,            // individual person
      CLIENT_TYPE_CORPORATE               =2,            // corporate
      CLIENT_TYPE_FUND                    =3,            // fund
      //--- enumeration borders
      CLIENT_TYPE_FIRST                   =CLIENT_TYPE_UNDEFINED,
      CLIENT_TYPE_LAST                    =CLIENT_TYPE_FUND
     };
   //--- client statuses
   enum EnClientStatus
     {
      //--- demo account registered
      CLIENT_STATUS_UNREGISTERED          =0,           // anonymous client (demo account without any data)
      CLIENT_STATUS_REGISTERED            =100,         // client that opened demo account and filled contact data
      CLIENT_STATUS_NOTINTERESTED         =200,         // client that opened demo account and filled contact data, but doesn't have interest to open real account
      //--- preliminary account registered
      CLIENT_STATUS_APPLICATION_INCOMPLETED=300,        // client that filled data for real account
      CLIENT_STATUS_APPLICATION_COMPLETED =400,         // client that filled data for real account and attached all documents
      CLIENT_STATUS_APPLICATION_INFORMATION=500,        // need more information for next processing to open real account
      CLIENT_STATUS_APPLICATION_REJECTED  =600,         // client was rejected
      //--- client
      CLIENT_STATUS_APPROVED              =700,         // client with real account
      CLIENT_STATUS_FUNDED                =800,         // client that was funded his real account
      CLIENT_STATUS_ACTIVE                =900,         // client that have trading activity  for some period
      CLIENT_STATUS_INACTIVE              =1000,        // client that haven't trading activity  for some period
      CLIENT_STATUS_SUSPENDED             =1100,        // client that was suspended
      //--- closed 
      CLIENT_STATUS_CLOSED                =1200,        // closed by himself
      CLIENT_STATUS_TERMINATED            =1300,        // closed by company
      //--- enumeration borders
      CLIENT_STATUS_FIRST                 =CLIENT_STATUS_UNREGISTERED,
      CLIENT_STATUS_LAST                  =CLIENT_STATUS_TERMINATED
     };
   //--- preferred communications
   enum EnPreferredCommunication
     {
      PREFERRED_COMMUNICATION_UNDEFINED   =0,            // undefined
      PREFERRED_COMMUNICATION_EMAIL       =1,            // email
      PREFERRED_COMMUNICATION_PHONE       =2,            // phone
      PREFERRED_COMMUNICATION_PHONE_SMS   =3,            // SMS
      PREFERRED_COMMUNICATION_MESSENGER   =4,            // messenger
      //--- enumeration borders
      PREFERRED_COMMUNICATION_FIRST       =PREFERRED_COMMUNICATION_UNDEFINED,
      PREFERRED_COMMUNICATION_LAST        =PREFERRED_COMMUNICATION_MESSENGER,
     };
   //--- experiences
   enum EnTradingExperience
     {
      EXPERIENCE_LESS_1_YEAR              =0,            // less than one year
      EXPERIENCE_1_3_YEAR                 =1,            // from one to three years
      EXPERIENCE_ABOVE_3_YEAR             =2,            // above three years
      //--- enumeration borders
      EXPERIENCE_FIRST                    =EXPERIENCE_LESS_1_YEAR,
      EXPERIENCE_LAST                     =EXPERIENCE_ABOVE_3_YEAR,
     };
   //--- employments
   enum EnEmployment
     {
      EMPLOYMENT_UNEMPLOYED               =0,            // unemployed
      EMPLOYMENT_EMPLOYED                 =1,            // employed
      EMPLOYMENT_SELF_EMPLOYED            =2,            // self employed
      EMPLOYMENT_RETIRED                  =3,            // retired
      EMPLOYMENT_STUDENT                  =4,            // student
      EMPLOYMENT_OTHER                    =5,            // other
      //--- enumeration borders           
      EMPLOYMENT_FIRST                    =EMPLOYMENT_UNEMPLOYED,
      EMPLOYMENT_LAST                     =EMPLOYMENT_OTHER
     };
   //--- employment industries
   enum EnEmploymentIndustry
     {
      INDUSTRY_NONE                       =0,            // none
      INDUSTRY_AGRICULTURE                =1,            // agriculture
      INDUSTRY_CONSTRUCTION               =2,            // construction
      INDUSTRY_MANAGEMENT                 =3,            // management
      INDUSTRY_COMMUNICATION              =4,            // communication
      INDUSTRY_EDUCATION                  =5,            // education
      INDUSTRY_GOVERNMENT                 =6,            // government
      INDUSTRY_HEALTHCARE                 =7,            // health care
      INDUSTRY_TOURISM                    =8,            // tourism
      INDUSTRY_IT                         =9,            // IT
      INDUSTRY_SECURITY                   =10,           // security
      INDUSTRY_MANUFACTURING              =11,           // manufacturing
      INDUSTRY_MARKETING                  =12,           // marketing
      INDUSTRY_SCIENCE                    =13,           // science
      INDUSTRY_ENGINEERING                =14,           // engineering
      INDUSTRY_TRANSPORT                  =15,           // transport
      INDUSTRY_OTHER                      =16,           // other
      //--- enumeration borders
      INDUSTRY_FIRST                      =INDUSTRY_AGRICULTURE,
      INDUSTRY_LAST                       =INDUSTRY_OTHER
     };
   //--- education levels
   enum EnEducationLevel
     {
      EDUCATION_LEVEL_NONE                =0,            // none
      EDUCATION_LEVEL_HIGH_SCHOOL         =1,            // high school
      EDUCATION_LEVEL_BACHELOR            =2,            // bachelor
      EDUCATION_LEVEL_MASTER              =3,            // master
      EDUCATION_LEVEL_PHD                 =4,            // PHD
      EDUCATION_LEVEL_OTHER               =5,            // other
      //--- enumeration borders
      EDUCATION_LEVEL_FIRST               =EDUCATION_LEVEL_NONE,
      EDUCATION_LEVEL_LAST                =EDUCATION_LEVEL_OTHER
     };
   //--- wealth sources
   enum EnWealthSource
     {
      WEALTH_SOURCE_EMPLOYMENT            =0,            // employment
      WEALTH_SOURCE_SAVINGS               =1,            // savings
      WEALTH_SOURCE_INHERITANCE           =2,            // inheritance
      WEALTH_SOURCE_OTHER                 =3,            // other
      //--- enumeration borders
      WEALTH_SOURCE_FIRST                 =WEALTH_SOURCE_EMPLOYMENT,
      WEALTH_SOURCE_LAST                  =WEALTH_SOURCE_OTHER
     };
   //--- client origins
   enum EnClientOrigin
     {
      CLIENT_ORIGIN_MANUAL                =0,           // manual created
      CLIENT_ORIGIN_DEMO                  =1,           // automatic was created from demo account
      CLIENT_ORIGIN_CONTEST               =2,           // automatic was created from contest account
      CLIENT_ORIGIN_PRELIMINARY           =3,           // automatic was created from preliminary account
      CLIENT_ORIGIN_REAL                  =4,           // automatic was created from real account
      //--- enumeration borders
      CLIENT_ORIGIN_FIRST                 =CLIENT_ORIGIN_MANUAL,
      CLIENT_ORIGIN_LAST                  =CLIENT_ORIGIN_REAL,
     };
   //--- KYC status
   enum EnKYCStatus
     {
      KYC_STATUS_UNDEFINED                =0,            // undefined
      KYC_STATUS_APPROVED                 =1,            // approved
      KYC_STATUS_DECLINED                 =2,            // declined
      //--- enumeration borders
      KYC_STATUS_FIRST                    =KYC_STATUS_UNDEFINED,
      KYC_STATUS_LAST                     =KYC_STATUS_DECLINED
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTClient* client)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- record id
   virtual uint64_t    RecordID(void) const=0;
   virtual MTAPIRES  RecordID(const uint64_t record_id)=0;
   //--- client type
   virtual uint32_t  ClientType(void) const=0;
   virtual MTAPIRES  ClientType(const uint32_t type)=0;
   //--- client status
   virtual uint32_t  ClientStatus(void) const=0;
   virtual MTAPIRES  ClientStatus(const uint32_t status)=0;
   //--- assigned manager
   virtual uint64_t    AssignedManager(void) const=0;
   virtual MTAPIRES  AssignedManager(const uint64_t manager)=0;
   //--- compliance approved by
   virtual uint64_t    ComplianceApprovedBy(void) const=0;
   virtual MTAPIRES  ComplianceApprovedBy(const uint64_t manager)=0;
   //--- compliance client category
   virtual LPCWSTR   ComplianceClientCategory(void) const=0;
   virtual MTAPIRES  ComplianceClientCategory(LPCWSTR category)=0;
   //--- compliance date approval
   virtual int64_t     ComplianceDateApproval(void) const=0;
   virtual MTAPIRES  ComplianceDateApproval(const int64_t date)=0;
   //--- compliance date termination
   virtual int64_t     ComplianceDateTermination(void) const=0;
   virtual MTAPIRES  ComplianceDateTermination(const int64_t date)=0;
   //--- lead campaign
   virtual LPCWSTR   LeadCampaign(void) const=0;
   virtual MTAPIRES  LeadCampaign(LPCWSTR campaign)=0;
   //--- lead source
   virtual LPCWSTR   LeadSource(void) const=0;
   virtual MTAPIRES  LeadSource(LPCWSTR source)=0;
   //--- introducer
   virtual LPCWSTR   Introducer(void) const=0;
   virtual MTAPIRES  Introducer(LPCWSTR introducer)=0;
   //--- person title
   virtual LPCWSTR   PersonTitle(void) const=0;
   virtual MTAPIRES  PersonTitle(LPCWSTR title)=0;
   //--- person name
   virtual LPCWSTR   PersonName(void) const=0;
   virtual MTAPIRES  PersonName(LPCWSTR name)=0;
   //--- person middle name
   virtual LPCWSTR   PersonMiddleName(void) const=0;
   virtual MTAPIRES  PersonMiddleName(LPCWSTR middle_name)=0;
   //--- person last name
   virtual LPCWSTR   PersonLastName(void) const=0;
   virtual MTAPIRES  PersonLastName(LPCWSTR last_name)=0;
   //--- person birth date
   virtual int64_t     PersonBirthDate(void) const=0;
   virtual MTAPIRES  PersonBirthDate(const int64_t date)=0;
   //--- person citizenship
   virtual LPCWSTR   PersonCitizenship(void) const=0;
   virtual MTAPIRES  PersonCitizenship(LPCWSTR citizenship)=0;
   //--- person gender
   virtual uint32_t  PersonGender(void) const=0;
   virtual MTAPIRES  PersonGender(const uint32_t gender)=0;
   //--- person taxid
   virtual LPCWSTR   PersonTaxID(void) const=0;
   virtual MTAPIRES  PersonTaxID(LPCWSTR taxid)=0;
   //--- person document type
   virtual LPCWSTR   PersonDocumentType(void) const=0;
   virtual MTAPIRES  PersonDocumentType(LPCWSTR type)=0;
   //--- person document number
   virtual LPCWSTR   PersonDocumentNumber(void) const=0;
   virtual MTAPIRES  PersonDocumentNumber(LPCWSTR number)=0;
   //--- person document date
   virtual int64_t     PersonDocumentDate(void) const=0;
   virtual MTAPIRES  PersonDocumentDate(const int64_t date)=0;
   //--- person document extra
   virtual LPCWSTR   PersonDocumentExtra(void) const=0;
   virtual MTAPIRES  PersonDocumentExtra(LPCWSTR extra)=0;
   //--- person employment
   virtual uint32_t  PersonEmployment(void) const=0;
   virtual MTAPIRES  PersonEmployment(const uint32_t employment)=0;
   //--- person industry
   virtual uint32_t  PersonIndustry(void) const=0;
   virtual MTAPIRES  PersonIndustry(const uint32_t industry)=0;
   //--- person education
   virtual uint32_t  PersonEducation(void) const=0;
   virtual MTAPIRES  PersonEducation(const uint32_t education)=0;
   //--- person wealth source
   virtual uint32_t  PersonWealthSource(void) const=0;
   virtual MTAPIRES  PersonWealthSource(const uint32_t source)=0;
   //--- person annual income
   virtual double    PersonAnnualIncome(void) const=0;
   virtual MTAPIRES  PersonAnnualIncome(const double income)=0;
   //--- person net worth
   virtual double    PersonNetWorth(void) const=0;
   virtual MTAPIRES  PersonNetWorth(const double worth)=0;
   //--- person annual deposit
   virtual double    PersonAnnualDeposit(void) const=0;
   virtual MTAPIRES  PersonAnnualDeposit(const double deposit)=0;
   //--- company name
   virtual LPCWSTR   CompanyName(void) const=0;
   virtual MTAPIRES  CompanyName(LPCWSTR name)=0;
   //--- company registration number
   virtual LPCWSTR   CompanyRegNumber(void) const=0;
   virtual MTAPIRES  CompanyRegNumber(LPCWSTR number)=0;
   //--- company registration date
   virtual int64_t     CompanyRegDate(void) const=0;
   virtual MTAPIRES  CompanyRegDate(const int64_t date)=0;
   //--- company registration authority
   virtual LPCWSTR   CompanyRegAuthority(void) const=0;
   virtual MTAPIRES  CompanyRegAuthority(LPCWSTR authority)=0;
   //--- company VAT
   virtual LPCWSTR   CompanyVat(void) const=0;
   virtual MTAPIRES  CompanyVat(LPCWSTR vat)=0;
   //--- company LEI
   virtual LPCWSTR   CompanyLei(void) const=0;
   virtual MTAPIRES  CompanyLei(LPCWSTR lei)=0;
   //--- company license number
   virtual LPCWSTR   CompanyLicenseNumber(void) const=0;
   virtual MTAPIRES  CompanyLicenseNumber(LPCWSTR number)=0;
   //--- company license authority
   virtual LPCWSTR   CompanyLicenseAuthority(void) const=0;
   virtual MTAPIRES  CompanyLicenseAuthority(LPCWSTR authority)=0;
   //--- company country
   virtual LPCWSTR   CompanyCountry(void) const=0;
   virtual MTAPIRES  CompanyCountry(LPCWSTR country)=0;
   //--- company address
   virtual LPCWSTR   CompanyAddress(void) const=0;
   virtual MTAPIRES  CompanyAddress(LPCWSTR address)=0;
   //--- company website
   virtual LPCWSTR   CompanyWebsite(void) const=0;
   virtual MTAPIRES  CompanyWebsite(LPCWSTR website)=0;
   //--- contact preferred
   virtual uint32_t  ContactPreferred(void) const=0;
   virtual MTAPIRES  ContactPreferred(const uint32_t preferred)=0;
   //--- contact language
   virtual LPCWSTR   ContactLanguage(void) const=0;
   virtual MTAPIRES  ContactLanguage(LPCWSTR language)=0;
   //--- contact email
   virtual LPCWSTR   ContactEmail(void) const=0;
   virtual MTAPIRES  ContactEmail(LPCWSTR email)=0;
   //--- contact phone
   virtual LPCWSTR   ContactPhone(void) const=0;
   virtual MTAPIRES  ContactPhone(LPCWSTR phone)=0;
   //--- contact messengers
   virtual LPCWSTR   ContactMessengers(void) const=0;
   virtual MTAPIRES  ContactMessengers(LPCWSTR messengers)=0;
   //--- contact social networks
   virtual LPCWSTR   ContactSocialNetworks(void) const=0;
   virtual MTAPIRES  ContactSocialNetworks(LPCWSTR social_networks)=0;
   //--- contact last date
   virtual int64_t     ContactLastDate(void) const=0;
   virtual MTAPIRES  ContactLastDate(const int64_t date)=0;
   //--- address country
   virtual LPCWSTR   AddressCountry(void) const=0;
   virtual MTAPIRES  AddressCountry(LPCWSTR country)=0;
   //--- address postcode
   virtual LPCWSTR   AddressPostcode(void) const=0;
   virtual MTAPIRES  AddressPostcode(LPCWSTR postcode)=0;
   //--- address street
   virtual LPCWSTR   AddressStreet(void) const=0;
   virtual MTAPIRES  AddressStreet(LPCWSTR street)=0;
   //--- address state
   virtual LPCWSTR   AddressState(void) const=0;
   virtual MTAPIRES  AddressState(LPCWSTR state)=0;
   //--- address city
   virtual LPCWSTR   AddressCity(void) const=0;
   virtual MTAPIRES  AddressCity(LPCWSTR city)=0;
   //--- experience of FX
   virtual uint32_t  ExperienceFX(void) const=0;
   virtual MTAPIRES  ExperienceFX(const uint32_t experience)=0;
   //--- experience of CFD
   virtual uint32_t  ExperienceCFD(void) const=0;
   virtual MTAPIRES  ExperienceCFD(const uint32_t experience)=0;
   //--- experience of Futures
   virtual uint32_t  ExperienceFutures(void) const=0;
   virtual MTAPIRES  ExperienceFutures(const uint32_t experience)=0;
   //--- experience of Stocks
   virtual uint32_t  ExperienceStocks(void) const=0;
   virtual MTAPIRES  ExperienceStocks(const uint32_t experience)=0;
   //--- trading group
   virtual LPCWSTR   TradingGroup(void) const=0;
   virtual MTAPIRES  TradingGroup(LPCWSTR group)=0;
   //--- client origin
   virtual uint32_t  ClientOrigin(void) const=0;
   virtual MTAPIRES  ClientOrigin(const uint32_t origin)=0;
   //--- client origin login
   virtual uint64_t    ClientOriginLogin(void) const=0;
   virtual MTAPIRES  ClientOriginLogin(const uint64_t login)=0;
   //--- external ID
   virtual LPCWSTR   ClientExternalID(void) const=0;
   virtual MTAPIRES  ClientExternalID(LPCWSTR external_id)=0;
   //--- person document expiration date
   virtual int64_t     PersonDocumentExpiration(void) const=0;
   virtual MTAPIRES  PersonDocumentExpiration(const int64_t date)=0;
   //--- KYC status
   virtual uint32_t  KYCStatus(void) const=0;
   virtual MTAPIRES  KYCStatus(const uint32_t kyc_status)=0;
   //--- record creation date
   virtual int64_t     DateCreated(void) const=0;
   virtual MTAPIRES  DateCreated(const int64_t date)=0;
   //--- record modification date
   virtual int64_t     DateModified(void) const=0;
   virtual MTAPIRES  DateModified(const int64_t date)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTClient(void) {}
  };
//+------------------------------------------------------------------+
//| Client array interface                                           |
//+------------------------------------------------------------------+
class IMTClientArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTClientArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTClient* client)=0;
   virtual MTAPIRES  AddCopy(const IMTClient* client)=0;
   virtual MTAPIRES  Add(IMTClientArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTClientArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTClient *Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTClient* client)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTClient* client)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTClient*Next(const uint32_t index) const=0;
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
                    ~IMTClientArray(void) {}
  };
//+------------------------------------------------------------------+
//| Client records notification interface                            |
//+------------------------------------------------------------------+
class IMTClientSink
  {
public:
   //--- events
   virtual void      OnClientAdd(const IMTClient*    /*client*/)           {  }
   virtual void      OnClientUpdate(const IMTClient* /*client*/)           {  }
   virtual void      OnClientDelete(const IMTClient* /*client*/)           {  }
  };
//+------------------------------------------------------------------+
