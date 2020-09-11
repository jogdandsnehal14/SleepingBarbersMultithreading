//---------------------------------------------------------------------------
// File: driver.cpp
// Author: Snehal Jogdand
// Date: 05/12/2020
// Program 2: Sleeping Barbers
//---------------------------------------------------------------------------

#include <iostream>     // cout
#include <sys/time.h>   // gettimeofday
#include <unistd.h>     //usleep
#include "Shop.h"
#include "ThreadParam.h"

using namespace std;

// function prototype
void *barber( void * );    // the prototype of the barber thread function
void *customer( void * );  // the prototype of the customer thread function

int main( int argc, char *argv[] ) {
  if ( argc != 5 ) {
    cerr << "4 arguments expected: " << endl;
    cerr << "nBarbers: The number of barbers working in your barbershop." << endl; 
    cerr << "nChairs: The number of chairs available for customers to wait in." << endl; 
    cerr << "nCustomers: The number of customers who need a haircut service." << endl; 
    cerr << "serviceTime: Each barber’s service time (in µ seconds)." << endl; 
    return -1;
  }

  int nBarbers = atoi( argv[1] );
  int nChairs = atoi( argv[2] );
  int nCustomers = atoi( argv[3] );
  int serviceTime = atoi( argv[4] );

  pthread_t barber_thread[nBarbers];
  pthread_t customer_threads[nCustomers];
  Shop shop( nBarbers, nChairs );
  
  for ( int i = 0; i < nBarbers; i++ ) { // instantiate barbers
    ThreadParam *param = new ThreadParam( &shop, i, serviceTime );
    pthread_create( &barber_thread[i], NULL, barber, (void *)param );
  }

  for ( int i = 0; i < nCustomers; i++ ) {
    usleep( rand( ) % 1000 );
    ThreadParam *param = new ThreadParam( &shop, i + 1, 0 );
    pthread_create( &customer_threads[i], NULL, customer, (void *)param );
  }

  for ( int i = 0; i < nCustomers; i++ ) pthread_join( customer_threads[i], NULL );

  for ( int i = 0; i < nBarbers; i++ ) pthread_cancel( barber_thread[i] );

  cout << "# customers who didn't receive a service = " << shop.nDropsOff << endl;

  return 0;
}

// the barber thread function 
void *barber( void *arg ) {  
  // extract parameters 
  ThreadParam &param = *(ThreadParam *)arg; 
  Shop &shop = *(param.shop); 
  int id = param.id; 
  int serviceTime = param.serviceTime; 
  delete &param; 
   
  // keep working until being terminated by the main 
  while( true ) { 
    shop.helloCustomer( id ); // pick up a new customer 
    usleep( serviceTime );
    shop.byeCustomer( id ); // release the customer 
  } 

  return NULL;
}

// the customer thread function
void *customer( void *arg ) {
  // extract parameters
  ThreadParam &param = *(ThreadParam *)arg;
  Shop &shop = *(param.shop);
  int id = param.id;
  delete &param;

  int barber = shop.visitShop( id );
  if ( barber != -1 ) shop.leaveShop( id, barber );
  
  return NULL;
}