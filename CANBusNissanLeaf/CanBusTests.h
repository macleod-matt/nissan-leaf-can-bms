#ifndef NISSAN_LEAF_TESTS_H
#define NISSAN_LEAF_TESTS_H
#include "SensorTypes.h"

#define DEBUG_MODE // un-comment to enter debug mode to access test cases 

//delete entire chunk of code if TTL converter is installed on CanBed2040 board 
// #define DEBUG_PRINT // un-comment to send data out over serial 


#if defined(DEBUG_MODE)

  //un-comment what ever group you wish to test OR send in
  // #define TEST_GROUP1  //test retrieving sensing paramters from the wire that belong to group1 (hx,soc,ahr)
  // #define TEST_GROUP2  //test retrieving sensing paramters from the wire that belong to group2 (cell voltages)
  // #define TEST_GROUP4  //test retrieving sensing paramters from the wire that belong to group4 (temperatures)
  // #define TEST_GROUP6  //test retrieving sensing paramters from the wire that belong to group6 (shunts)
  // #define TEST_GROUP61  //test retrieving sensing paramters from the wire that belong to group61 (soh)
  #define TEST_GROUP_ALL     //test retrieving all sensing parameters from BSM 

  //macro defined test cases 
  #if defined(TEST_GROUP1)
    #define SENSE_GROUP &sense_group1 ;
  #endif 
  #if defined(TEST_GROUP2)
    #define SENSE_GROUP &sense_group2
  #endif 
  #if defined(TEST_GROUP4)
    #define SENSE_GROUP &sense_group4 
  #endif 
  #if defined(TEST_GROUP6)
    #define SENSE_GROUP &sense_group6 
  #endif 
  #if defined(TEST_GROUP61)
    #define SENSE_GROUP &sense_group61
  #endif 


#else // cant have debug mode and serial command mode both enabled 

  #define SERIAL_COMMAND_MODE  //recieve sense group from issuing serial command 


#endif // defined(DEBUG_MODE)


#endif //#ifndef NISSAN_LEAF_TESTS_H