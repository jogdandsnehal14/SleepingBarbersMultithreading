//---------------------------------------------------------------------------
// File: Shop.h
// Author: Snehal Jogdand
// Date: 05/12/2020
// Program 2: Sleeping Barbers
//---------------------------------------------------------------------------

#ifndef _SHOP_H_
#define _SHOP_H_

#include <pthread.h>
#include <queue>
#include <iostream>
#include <map>

using namespace std;

#define DEFAULT_CHAIRS 3
#define DEFAULT_BARBERS 1

enum personType { CUSTOMER, BARBER };
enum customerState { ENTER_SHOP, WAITING_ON_CHAIR, MONEY_PAID };

// Structure of a Barber to manage Barber state
struct Barber {
   int id;                            // barberId
   pthread_cond_t cond;               // barber condition
   int currentCustomer = -1;          // barber assigned customer id
};

// Structure of a Customer to manage Customer state
struct Customer {
   int id;                            // customerId
   pthread_cond_t cond;               // customer condition
   int currentBarber = -1;            // customer assigned barber id
   customerState state = ENTER_SHOP;  // customer current state (default: ENTER_SHOP)
};

class Shop {
  public:
   Shop( int nBarbers, int nChairs ) :
       maxBarbers( ( nBarbers > 0 ) ? nBarbers : DEFAULT_BARBERS ), 
       maxChairs( ( nChairs > 0 ) ? nChairs : DEFAULT_CHAIRS ), 
       nDropsOff( 0 ) { init( ); }

   Shop( ) : 
   maxBarbers ( DEFAULT_BARBERS ),
   maxChairs( DEFAULT_CHAIRS ), 
    nDropsOff( 0 ) { init( ); };

   int visitShop( int customerId ); // return barber ID or -1 (not served) 
   void leaveShop( int customerId, int barberId ); 
   void helloCustomer( int barberId ); 
   void byeCustomer( int barberId ); 
   int nDropsOff;                 // the number of customers dropped off 

  private:
   const int maxBarbers;          // the max number of barbers available
   const int maxChairs;           // the max number of customers that can wait
   queue<int> waiting_chairs;     // includes the ids of customers waiting on chairs
   queue<int> sleeping_barbers;   // includes the ids of barbers who are sleeping (available for haircut)

   map<int, Customer> customers;  // map of customers 
   map<int, Barber> barbers;      // map of barbers
  
   pthread_mutex_t mutex;

   void init( );
   string int2string( int i );
   void print( personType type, int personId, string message );
};

#endif