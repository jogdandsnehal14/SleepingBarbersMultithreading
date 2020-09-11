//---------------------------------------------------------------------------
// File: ThreadParam.h
// Author: Snehal Jogdand
// Date: 05/12/2020
// Program 2: Sleeping Barbers
//---------------------------------------------------------------------------

#ifndef _THREADPARAM_H_
#define _THREADPARAM_H_

#include "Shop.h"

// a set of parameters to be passed to each thread 
class ThreadParam 
{ 
public: 
  ThreadParam( Shop *shop, int id, int serviceTime ) : 
  shop( shop ), id( id ), serviceTime( serviceTime ) { }; 
  Shop *shop;       // a pointer to the Shop object 
  int id;           // a thread identifier 
  int serviceTime;  // service time (usec) for barber; 0 for customer 
};

#endif