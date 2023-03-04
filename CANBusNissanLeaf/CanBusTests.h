#ifndef NISSAN_LEAF_TESTS_H
#define NISSAN_LEAF_TESTS_H
#include "SensorTypes.h"



//un-comment what ever group you wish to test OR send in
// #define TEST_GROUP1  //test retrieving sensing paramters from the wire that belong to group1
//#define TEST_GROUP2  //test retrieving sensing paramters from the wire that belong to group2
#define TEST_GROUP4  //test retrieving sensing paramters from the wire that belong to group4
//#define TEST_GROUP6  //test retrieving sensing paramters from the wire that belong to group6
// #define TEST_GROUP61  //test retrieving sensing paramters from the wire that belong to group61
// #define TEST_GROUP_ALL     //test retriieving all sensing paramters from BSM 


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
#if defined(TEST_GROUP_ALL) 
  #define GROUP_SIZE (5) 
  #define GET_SENSOR_GROUP_IDX(idx) (idx + 1 < GROUP_SIZE ? idx + 1: 0)  	
#endif 







#endif //#ifndef NISSAN_LEAF_TESTS_H