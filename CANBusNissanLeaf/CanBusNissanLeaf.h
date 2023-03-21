#ifndef CAN_BUS_NISSAN_LEAF_H
#define CAN_BUS_NISSAN_LEAF_H


#define BMS_QUERY_ID (0x79B)
#define CANint (2)
#define GROUP_REQEST_SIZE (8)
#define NISSAN_BMS_REPLY_ID (0x7BB)
#define CHECK_CURRENT(current) (current & 0x8000000 == 0x8000000)? ((current |-0x100000000) / 1024): (current/1024); 
#define LED2 8
#define LED3 7
#define NUM_CELLS 96
#define NUM_TEMP_SENSORS 4
#define GET_CELL_INDEX(index) (index < NUM_CELLS ? index + 1 : 0 )
#define GET_SHUNT_INDEX(index) (index < NUM_CELLS ? index + 4 : 0 )
#define CONVERT_BYTES(high_b, low_b) ((high_b << 8 | low_b)/1000.0)
#define DEVICE_MAC 303721658 
#define SAMPLE_COUNT (100)


/**
 * @brief Helper Macro to add voltage cell values to sensor group  data
 * 
 * @param highByte 
 * @param lowByte 
 * @param index
 * @param destination  
 */

#define BYTES_TO_CELL_VOLTGAGE(high,low,index, dest) \
do { \
  index=GET_CELL_INDEX(index); \ 
  dest[index]=(float)CONVERT_BYTES(high,low);\ 
} while (0)

/**
 * @brief Helper Macro to add  cell shunt values to sensor group  data
 * 
 * @param byte  
 * @param index
 * @param destination  
 */
#define BYTES_TO_SHUNT_VAL(byte, index, dest) \
do { \
  dest[index] = byte && 00001000;\
  dest[index] = byte && 00000100;\
  dest[index] = byte && 00000010;\
  dest[index] = byte && 00000001;\
  index=GET_SHUNT_INDEX(index); \ 
} while (0)




MCP_CAN CAN0(9); // Set CS to pin 9 for rp2040 

unsigned char len = 0;
unsigned char buf[8];
int sensor_group_idx = -1; 

// int cellCount = 0;

byte sendGroup1[8] = {0x02,0x21,0x01,0,0,0,0,0};
byte sendGroup2[8] = {0x02,0x21,0x02,0,0,0,0,0};
byte sendGroup3[8] = {0x02,0x21,0x03,0,0,0,0,0};
byte sendGroup4[8] = {0x02,0x21,0x04,0,0,0,0,0};
byte sendGroup6[8] = {0x02,0x21,0x06,0,0,0,0,0};
byte sendGroup61[8] = {0x02,0x21,0x61,0,0,0,0,0};
byte sendNextLine[8] = {0x30,0x01,0,0xFF,0xFF,0xFF,0xFF,0xFF};


//function pointer to print out what ever sensor type the input is 
typedef void (*p_send_func_t)(int sensor_request) ; 
//function pointer for 
typedef void (*p_decode_func_t)(); 

// structs for data types 
typedef struct _group1_data_{ 
  float hx;
  float soc;
  float ahr; 
  float hv_bat_curr1; 
  float hv_bat_curr2; 
}group1_data_t;

typedef struct _group2_data_{ 
  float cell_voltages[96];
}group2_data_t; 

typedef struct _group4_data_{ 
  byte pack_temps[4];
}group4_data_t; 

typedef struct _group6_data_{ 
  bool shunts[96];
}group6_data_t; 

typedef struct _group61_data_{ 
 float soh; 
}group61_data_t; 

HardwareSerial *serDebug = &Serial1;
HardwareSerial *serOutput = &Serial;


// generic structure for each state

typedef struct _group_info_{ 
  bool group_rec_sent; //check to sync some flow control 
  byte * group; // pointer to state grouping 
  p_send_func_t send_func; //function pointer to print function associated with the grouping 
  p_decode_func_t decode_func; //function pointer to group read function
  byte len; //length of data field 
  void * data; //pointer to data type 
}group_info_t; 

group1_data_t group1_data;
group2_data_t group2_data;
group4_data_t group4_data;
group6_data_t group6_data;
group61_data_t group61_data;


group_info_t sense_group1; 
group_info_t sense_group2;
group_info_t sense_group4;
group_info_t sense_group6;
group_info_t sense_group61;

group_info_t *  p_group_info_request;

#endif //#ndef CAN_BUS_NISSAN_LEAF_H