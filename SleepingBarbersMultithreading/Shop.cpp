//---------------------------------------------------------------------------
// File: Shop.cpp
// Author: Snehal Jogdand
// Date: 05/12/2020
// Program 2: Sleeping Barbers
//---------------------------------------------------------------------------

#include <iostream> // cout
#include <sstream>  // stringstream
#include <string>   // string
#include "Shop.h"

string Shop::int2string( int i ) {
  stringstream out;
  out << i;
  return out.str( );
}

void Shop::print(personType type, int personId, string message ) {
  cout << ( ( type != BARBER ) ? "customer[" : "barber  [" )
       << personId << "]: " << message << endl;
}

void Shop::init( ) {
   pthread_mutex_init(&mutex, NULL);

   // Create a map of barbers
   for (int i = 0; i < maxBarbers; i++) {
      Barber barber = Barber();
      barber.id = i;
      barbers[i] = barber;

      pthread_cond_init(&barbers[i].cond, NULL);
   }
}

int Shop::visitShop( int customerId ) {
   pthread_mutex_lock(&mutex); // lock

   // If all chairs are full, customer leaves the shop
   if (waiting_chairs.size() == maxChairs) {
      print(CUSTOMER, customerId, "leaves the shop because of no available waiting chairs.");
      ++nDropsOff;
      pthread_mutex_unlock(&mutex); // unlock
      return -1;
   }

   // Set a new customer 
   customers[customerId] = Customer();
   customers[customerId].id = customerId;
   
   pthread_cond_init(&customers[customerId].cond, NULL);

   int barberId;

   // If all barbers are busy, customer has to wait
   if (sleeping_barbers.empty()) {
      // Take a waiting chair (Push the customer in a waiting queue).  
      waiting_chairs.push(customerId);
      print(CUSTOMER, customerId, "takes a waiting chair. # waiting seats available = " + int2string((maxChairs - waiting_chairs.size())));
   
      // Wait for a barber to wake me up. 
      while (customers[customerId].currentBarber == -1) pthread_cond_wait(&customers[customerId].cond, &mutex);
   
      // Barber is assigned
      barberId = customers[customerId].currentBarber;
   }
   else {
      // Barber is available for hair cut
      barberId = sleeping_barbers.front();

      // Set the assigned Barber and Customer 
      customers[customerId].currentBarber = barberId;
      barbers[barberId].currentCustomer = customerId;
   
      // Remove assigned barber from sleeping queue
      sleeping_barbers.pop();
   }

   printf("customer[%d]: moves to a service chair[%i], # waiting seats available = %d\n", customerId, barberId, (int)(maxChairs - waiting_chairs.size()));   

   customers[customerId].state = WAITING_ON_CHAIR;
   pthread_cond_signal(&(barbers[barberId].cond));

   pthread_mutex_unlock(&mutex); // unlock
   
   return barberId;
}

void Shop::leaveShop( int customerId, int barberId ){
   pthread_mutex_lock(&mutex); // lock
   printf("customer[%i]: wait for barber[%i] to be done with hair-cut\n", customerId, barberId);

   // While barberId is cutting my hair, Wait
   while (customers[customerId].currentBarber != -1) pthread_cond_wait(&customers[customerId].cond, &mutex);

   printf("customer[%i]: says good-bye to barber[%i]\n", customerId, barberId);

   // Pay barber  
   customers[customerId].state = MONEY_PAID;

   // Signal 
   pthread_cond_signal(&(barbers[barberId].cond));

   pthread_mutex_unlock(&mutex); // unlock
}

void Shop::helloCustomer( int barberId ) {
   pthread_mutex_lock(&mutex); // lock

   // If I have no customer and all the waiting chairs are empty 
   if (barbers[barberId].currentCustomer == -1) {
      print(BARBER, barberId, "sleeps because of no customers.");

      sleeping_barbers.push(barberId);

      // Wait until a customer wakes me up. 
      while (barbers[barberId].currentCustomer == -1) pthread_cond_wait(&(barbers[barberId].cond), &mutex);
   }

   while (customers[barbers[barberId].currentCustomer].state != WAITING_ON_CHAIR) pthread_cond_wait(&(barbers[barberId].cond), &mutex);

   printf("barber [%i]: starts a hair-cut service for customer[%i]\n", barberId, barbers[barberId].currentCustomer);

   pthread_mutex_unlock(&mutex); // unlock
}

void Shop::byeCustomer( int barberId ) {
   pthread_mutex_lock(&mutex); // lock

   printf("barber [%i]: says he's done with a hair-cut service for customer[%i]\n", barberId, barbers[barberId].currentCustomer);
   customers[barbers[barberId].currentCustomer].currentBarber = -1;

   // Wakes up my customer
   pthread_cond_signal(&customers[barbers[barberId].currentCustomer].cond);

   // Wait for my customer to pay before I take a new one
   while (customers[barbers[barberId].currentCustomer].state != MONEY_PAID) pthread_cond_wait(&(barbers[barberId].cond), &mutex);

   barbers[barberId].currentCustomer = -1;
   
   print(BARBER, barberId, "calls in another customer");

   // If a customer is waiting
   if (!waiting_chairs.empty()) {
      int customerId = waiting_chairs.front();
      waiting_chairs.pop();
      barbers[barberId].currentCustomer = customerId;
      customers[customerId].currentBarber = barberId;

      // Wakes up another customer who is waiting on a waiting chair.  
      pthread_cond_signal(&customers[customerId].cond);
   }

   pthread_mutex_unlock(&mutex); // unlock
}