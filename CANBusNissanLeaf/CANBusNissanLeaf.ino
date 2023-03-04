/* Nissan Leaf BMS 30KWH data request and read via CANBUS
   Authors: Matt MacLeod, Dan Pothier 
    according to: https://drive.google.com/file/d/1jH9cgm5v23qnqVnmZN3p4TvdaokWKPjM/view
    Requests for sensing paramters sent from user over Serial can be mapped to the following query groups


    Notes: 

    group1: Query: 0x79B 02 21 01 00 00 00 00 00
    - HX 
    - SOC
    -  
    - hv_bat_curr1 
    - hv_bat_curr2 

    group2: Query: 0x79B 02 21 02 00 00 00 00 00
    - cell voltages [0-95]

    group3: Query: unknown 
    - unknown -- David Blackhursts code uses group 3 to read min and max voltages?? 
    
      see: 
      https://github.com/macleod-matt/NissanLeafBMS/blob/master/Code.ino#L139-L142
      https://github.com/macleod-matt/NissanLeafBMS/blob/master/Code.ino#L204-L215

    group4: Query: 0x79B 02 21 04 00 00 00 00 00
    - pack temperature 

    group6: Query: 0x79B 02 21 06 00 00 00 00 00
    - shunts 

    group61: Query: 0x79B 02 21 61 00 00 00 00 00
    - SOH

*/ 
#include <Arduino.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include "SensorTypes.h"

//#define SERIAL_COMMAND_MODE  //recieve sense group from issuing serial command 
#if !defined(SERIAL_COMMAND_MODE)
  #include "CanBusTests.h" //only need to import if we want to test without the serial interface
#endif //!Defined(SERIAL_COMMAND_MODE)



#define CANint (2)
#define GROUP_REQEST_SIZE (8)
#define NISSAN_BMS_REPLY_ID (0x7BB)
#define CHECK_CURRENT(current) (current & 0x8000000 == 0x8000000)? ((current | -0x100000000 ) / 1024): (current/1024); 
#define DEBUG_MODE //comment out when ready 
#define LED2 8
#define LED3 7

MCP_CAN CAN0(9); // Set CS to pin 9 for rp2040 

unsigned char len = 0;
unsigned char buf[8];
unsigned long line = 0;
int sensor_group_idx = -1; 

int cellCount = 0;
int shuntCount = 0;
int sensor_request = SYS_NO_REQUEST ; 

//Data to send [group] [request line]
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

HardwareSerial *serDebug = &Serial;
HardwareSerial *serOutput = &Serial1;

uint32_t mac = 1953659309;

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
group6_data_t group61_data;


group_info_t sense_group1; 
group_info_t sense_group2;
group_info_t sense_group4;
group_info_t sense_group6;
group_info_t sense_group61;


group_info_t *  p_group_info_request; 

#if defined(TEST_GROUP_ALL) || defined(SERIAL_COMMAND_MODE)
  group_info_t * sensor_groups[5] = {&sense_group1,&sense_group2, &sense_group4,&sense_group6,&sense_group61};
#endif 




/**
 * @brief send out group1 info 
 * 
 * @param sensor_request 
 * @note At present. These functions will just send the sensor values over Serial 
 * Eventaully these can be replaced with WIFI/BLE ect 
 */
void send_group1_info(int sensor_request){ 
  
  //print out sensor depending on type sent from user over Serial
  group1_data_t * data = (group1_data_t * )sense_group1.data;

  switch(sensor_request){ 
    case EV_BAT_HX:
      serDebug->println(data->hx);
      sendPacket(EV_BAT_HX, (float)data->hx);
      break; 

    case EV_BAT_SOC:
      serDebug->println(data->soc);
      sendPacket(EV_BAT_SOC, (float)data->soc);
      break;    

    case EV_BAT_AHR:
      serDebug->println(data->ahr);
      sendPacket(EV_BAT_AHR, (float)data->ahr);
      break;    
    case EV_BAT_HV_BAT_CURRENT_1:
      serDebug->println(data->hv_bat_curr1);
      sendPacket(EV_BAT_HV_BAT_CURRENT_1, (float)data->hv_bat_curr1);
      break; 
    case EV_BAT_HV_BAT_CURRENT_2:
      serDebug->println(data->hv_bat_curr2);
      sendPacket(EV_BAT_HV_BAT_CURRENT_2, (float)data->hv_bat_curr2);
      break; 
    case REQUEST_ALL:
      serDebug->println(data->hx);
      sendPacket(EV_BAT_HX, (float)data->hx);
      serDebug->println(data->soc);
      sendPacket(EV_BAT_SOC, (float)data->soc);
      serDebug->println(data->ahr);
      sendPacket(EV_BAT_AHR, (float)data->ahr);
      serDebug->println(data->hv_bat_curr1);
      sendPacket(EV_BAT_HV_BAT_CURRENT_1, (float)data->hv_bat_curr1);
      serDebug->println(data->hv_bat_curr2);
      sendPacket(EV_BAT_HV_BAT_CURRENT_2, (float)data->hv_bat_curr2);
      break; 
    default:
      break;
  }

}
/**
 * @brief send out cell voltages 
 * 
 * @param sensor_request 
 */

void send_cell_voltages(int sensor_request){ 

    
    group2_data_t * data = (group2_data_t *)sense_group2.data; 
    
    for(int i = 0; i < 96; i++) { // Display Cell Voltage and Shunts
      serDebug->print("Cell ");
      serDebug->print(i);
      serDebug->print(" ");
      serDebug->print(data->cell_voltages[i]);
  }
}


/**
 * @brief send out shunt status  
 * 
 * @param sensor_request 
 */

void send_shunt_status(int sensor_request){ 
    group6_data_t * data = (group6_data_t *)sense_group6.data; 
    
    for(int i = 0; i < 96; i++) { // Display Cell Voltage and Shunts
      serDebug->print("shunt ");
      serDebug->print(i);
      serDebug->print(" ");
      serDebug->print(data->shunts[i]);
  }
}

/**
 * @brief send out pack temperatures
 * 
 * @param sensor_request 
 */
void send_pack_temperatures(int sensor_request){
  
  group4_data_t * data = (group4_data_t *)sense_group4.data; 

  for(int i = 0; i < 4; i++) {  // Display Temps
      
      serDebug->print("Temp ");
      serDebug->print(i);
      serDebug->print(" ");
      serDebug->println(data->pack_temps[i]);

      sendPacket(EV_BAT_TEMP_1 + i, (float)data->pack_temps[i]);
  }
}


/**
 * @brief helper function to add voltage cell values to sensor group  data
 * 
 * @param highByte 
 * @param lowByte 
 */
void add_cell(byte highByte, byte lowByte) {

  group2_data_t *data = (group2_data_t *)sense_group2.data; //cast as group 2 data 
  if(cellCount < 96) {
    //cellVoltage[cellCount] = ((float)(highByte * 256) + lowByte) / 1000;
    data->cell_voltages[cellCount] = ((float)(highByte * 256) + lowByte) / 1000;
    cellCount++;
  }

}

/**
 * @brief function to read cell voltages from pack
 * 
 */
void decode_cell_voltages() {
  
  int dataType = (buf[0] %2); // Two sets of data, one with split cell at the end and one at the start
  if (buf[0] == 0x10 && buf[3] == 0x02) { // First Line
    cellCount = 0;
    add_cell(buf[4],buf[5]);
    add_cell(buf[6],buf[7]);
    CAN0.sendMsgBuf(0x79b, 0, sizeof(sendNextLine), sendNextLine);
  } else if (dataType == 1) {
    add_cell(buf[1],buf[2]);
    add_cell(buf[3],buf[4]);
    add_cell(buf[5],buf[6]);
    CAN0.sendMsgBuf(0x79b, 0, sizeof(sendNextLine), sendNextLine);
  } else if (dataType == 0) {
    add_cell(buf[7],buf[1]);
    add_cell(buf[2],buf[3]);
    add_cell(buf[4],buf[5]);
    add_cell(buf[6],buf[7]);
    CAN0.sendMsgBuf(0x79b, 0, sizeof(sendNextLine), sendNextLine);
  }

}

/**
 * @brief Decode the temperature data
 * 
 */
void decode_temperature() {

  group4_data_t * data = ( group4_data_t *)sense_group4.data; 
  if (buf[0] == 0x10 && buf[3] == 0x04) { // First Line
    data->pack_temps[0] = buf[6]; 
    CAN0.sendMsgBuf(0x79b, 0, sizeof(sendNextLine), sendNextLine);
  } else if (buf[0] == 0x21) { // Second Line
    data->pack_temps[1] = buf[2]; 
    data->pack_temps[2] = buf[5]; 
    CAN0.sendMsgBuf(0x79b, 0, sizeof(sendNextLine), sendNextLine);
  } else if (buf[0] == 0x22) { // Third Line
    data->pack_temps[3] = buf[1]; 

  }

}

/**
 * 
 * @brief helper function to decode add shunts to the global array 
 * 
 * @param raw_shunt_data 
 */
void add_shunts(byte raw_shunt_data) {

  group6_data_t * data = ( group6_data_t *)sense_group6.data; 

  data->shunts[shuntCount] = raw_shunt_data && 00001000;
  data->shunts[shuntCount] = raw_shunt_data && 00000100;
  data->shunts[shuntCount] = raw_shunt_data && 00000010;
  data->shunts[shuntCount] = raw_shunt_data && 00000001;

  shuntCount += 4;

}
/**
 * 
 * @brief decodes shunt voltages from can bus 
 * 
 *
 */
void decode_shunts() {

  if (buf[0] == 0x10 && buf[3] == 0x06) { // First Line
    shuntCount = 0;
    for(int i = 4; i < 8; i++) {
      add_shunts(buf[i]);
    }
    CAN0.sendMsgBuf(0x79b, 0, sizeof(sendNextLine), sendNextLine);
  } else if (buf[0] == 0x21 || buf[0] == 0x22) {
    for(int i = 0; i < 8; i++) {
      add_shunts(buf[i]);
    }
    CAN0.sendMsgBuf(0x79b, 0, sizeof(sendNextLine), sendNextLine);
  } else if (buf[0] == 0x23) {
    for(int i = 0; i < 4; i++) {
      add_shunts(buf[i]);
    }
  }

}


/**
 * @brief reads the group 1 (intrinsic pack paramters into the group 1 data field)
 * 
 */
void decode_group1_info() {

  group1_data_t * data = (group1_data_t *) sense_group1.data; 
  int64_t hv1_current =0, hv2_current = 0; 
  uint16_t SOC_HB = 0 , SOC_LB = 0; // high byte and low byte for soc formula:
  // 0x7BB 24 01 70 00 26 9A 00 0C
  // 0x7BB 25 44 B5 00 11 0B B8 80
  // SOC = (data_24[7] << 16 | ((data_25[1] << 8) | data_25[2]))/10000
  
  if (buf[0] == 0x10 && buf[3] == 0x01) { // First Line
    hv1_current = (buf[4] << 24) | (buf[5] << 16 | ((buf[6] << 8) | buf[7])); 
    data->hv_bat_curr1 = CHECK_CURRENT(hv1_current);
    CAN0.sendMsgBuf(0x79b, 0, sizeof(sendNextLine), sendNextLine);
  
  } else if (buf[0] == 0x21) {
    
    hv2_current = (buf[4] << 24) | (buf[5] << 16 | ((buf[6] << 8) | buf[7])); 
    data->hv_bat_curr2 = CHECK_CURRENT(hv2_current);
   
    CAN0.sendMsgBuf(0x79b, 0, sizeof(sendNextLine), sendNextLine);
  } else if (buf[0] == 0x22) {
    CAN0.sendMsgBuf(0x79b, 0, sizeof(sendNextLine), sendNextLine);
  } else if (buf[0] == 0x23) {
    //data-> = ((float)(buf[3] * 256) + buf[4]) / 1024;
    CAN0.sendMsgBuf(0x79b, 0, sizeof(sendNextLine), sendNextLine);
  } else if (buf[0] == 0x24) {
    data->hx = (float)((buf[4] << 8) | buf[5] ) / 102.4; //hx formula according to nissan 2018 doc 
    SOC_HB = buf[7]; // get the highbyte from the formula      
    CAN0.sendMsgBuf(0x79b, 0, sizeof(sendNextLine), sendNextLine);
  } else if (buf[0] == 0x25) {
    SOC_LB = (buf[1] << 8 | buf[2]); 
    data->soc = (float)(SOC_HB << 16 | SOC_LB ) / 10000; 
    data->ahr = (float) (buf[4] << 16 | ((buf[5] << 8) | buf[6])) / 100000;  

  }
}

/**
 * @brief send out state of health 
 * 
 * @param sensor_request 
 */

void send_soh(int sensor_request){ 

    group61_data_t * data = (group61_data_t *)sense_group61.data; 

    serDebug->print(sensor_request);
    serDebug->println(data->soh);

}


/**
 * @brief decode the battery state of health from the ca bus message 
 * State of Health is another indication of the battery’s ability to hold and release energy and is
 * reported as a percentage. When the battery is new SOH=100%
 * 
 */

void decode_soh(){ 
  group61_data_t * data = (group61_data_t *)sense_group61.data; 

  // Query: 0x79B 02 21 61 00 00 00 00 00
  // Answer: 0x7BB 11 4B 61 61 26 9A 25 CA
  // Formula: SOH = (( data[6] << 8 ) | data[7] ) / 100
  
  if (buf[0] == 0x11 && buf[2] == 0x61) { // First Line
      data->soh = ((buf[6] << 8) | buf[7]) /100;
      //CAN0.sendMsgBuf(0x79b, 0, sizeof(sendNextLine), sendNextLine);
  } 
}




/**
 * @brief generic helper function to request the data from the group message
 * 
 * @param group_request 
 */
void request_data_from_group(byte * group_request){ 

  CAN0.sendMsgBuf(0x79B, 0, GROUP_REQEST_SIZE, group_request); 

}

/**
 * @brief We're also unlikely to use this, but when we write large packets to certain slow UARTs
 * we might fill up the UART input buffer before it gets written over the air / wire / etc.
 * 
 * @param serial 
 * @return int 
 */
int blockOnBuffer(HardwareSerial *serial){
  
  unsigned long totalMillis = 0;
	unsigned long lastMillis = millis();
	unsigned long nowMillis = 0;
	unsigned long maxDelay = 1000; //1 second
  
  while(serial->availableForWrite() < 30){
    
    nowMillis = millis();
    totalMillis += nowMillis - lastMillis;
    lastMillis = nowMillis;
    
    if(totalMillis > maxDelay){
      return -1;  
    }
	}

	return serial->availableForWrite();
}

/**
 * @brief We're unlikely to use this, but if we implement some basic flow control we might want to wait for CTS
 * 
 * @return boolean 
 */
boolean waitForCTS(){

	unsigned long totalMillis = 0;
	unsigned long lastMillis = millis();
	unsigned long nowMillis = 0;
	unsigned long maxDelay = 1000; //1 second

  //this is normally specified in a class scope
  int _ctsPin = 0;

	while(digitalRead(_ctsPin) == HIGH){

		nowMillis = millis();
		totalMillis += nowMillis - lastMillis;
		lastMillis = nowMillis;

		if(totalMillis > maxDelay){
			return false;
		}
	}

	return true;

}

/**
 * @brief Write a data packet to the UART the middleware can decode
 * 
 * @param type 
 * @param data 
 */
void sendPacket(uint16_t type, float data){

    //probably don't need this in this context
		//blockOnBuffer(serOutput);
    //waitForCTS();

		serOutput->print(mac, DEC);
		serOutput->print(",");
		serOutput->print(type, DEC);
		serOutput->print(",");
		serOutput->print(data, 6);
		serOutput->print("\n");

    delay(1);

}



//setup function 
void setup() {

  serDebug->begin(115200);
  serOutput->begin(115200);

  while(!serDebug); 

  pinMode(23, OUTPUT);
  digitalWrite(23, HIGH);

  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(CANint, INPUT);
  digitalWrite(LED2, LOW);
  
  
  serDebug->println(F("Initalizing Can Bus"));

  if (CAN0.begin(CAN_500KBPS) == CAN_OK) {
    serDebug->println("Can Init Success");
  } else {
    serDebug->println("Can Init Failed");
    while (1) {
      serDebug->println("CAN bus failed");
      delay(1000);
    }
  }

  //map out group transition functions

  //group 1 is intrinsic pack paramters 
  sense_group1.group = sendGroup1;
  sense_group1.send_func = &send_group1_info; 
  sense_group1.data = &group1_data;
  sense_group1.decode_func = &decode_group1_info;
  sense_group1.group_rec_sent = false; 
  
  //group 2 is cell voltages 
  sense_group2.group = sendGroup2;
  sense_group2.send_func = &send_cell_voltages; 
  sense_group2.data = &group2_data;
  sense_group2.decode_func = &decode_cell_voltages;
  sense_group2.group_rec_sent = false; 

  //group3 is pack temperatures 
  sense_group4.group = sendGroup4;
  sense_group4.send_func = &send_pack_temperatures; 
  sense_group4.data = &group4_data;
  sense_group4.decode_func = &decode_temperature;
  sense_group4.group_rec_sent = false; 

  //group6 is shunt status 
  sense_group6.group = sendGroup6;
  sense_group6.send_func = &send_shunt_status; 
  sense_group6.data = &group6_data;
  sense_group6.decode_func = &decode_shunts;
  sense_group6.group_rec_sent = false; 

  //group61 is state of health 
  sense_group61.group = sendGroup61;
  sense_group61.send_func = &send_soh; 
  sense_group61.data = &group61_data;
  sense_group61.decode_func = &decode_soh;
  sense_group61.group_rec_sent = false; 

  serDebug->println("Sys Ready");
}


/**
 * @brief helper function desern the sensor group corresponding to the serial command 
 * 
 * 
 */
#if defined(SERIAL_COMMAND_MODE)

void get_sensor_group_from_serial(){ 

 //get sys state from Serial port 
  if(serDebug->available())
  {
    sensor_request = serDebug->read();
  }

  if(sensor_request == REQUEST_ALL ){ 
    sensor_group_idx = GET_SENSOR_GROUP_IDX(sensor_group_idx); //get sensor index in between iterations 
    p_group_info_request = sensor_groups[sensor_group_idx]; // get corresponding group 
  }else if( SYS_NO_REQUEST  < sensor_request && sensor_request < EV_BAT_CELL_VOLTAGES){ 
    p_group_info_request = &sense_group1;
  }else if(sensor_request == EV_BAT_CELL_VOLTAGES){ 
    p_group_info_request = &sense_group2;
  }else if(EV_BAT_CELL_VOLTAGES < sensor_request && sensor_request < EV_BAT_SHUNTS){ 
    p_group_info_request = &sense_group4;
  } else if(sensor_request == EV_BAT_SHUNTS){ 
    p_group_info_request = &sense_group6;
  }else if(sensor_request == EV_BAT_SOH){ 
    p_group_info_request = &sense_group61;
  }
}
#endif //defined(SERIAL_COMMAND_MODE)



void loop() {

  static bool request_recv_flag = false; //flag to indicate group reequest recieved  
  static unsigned long can_id = 0; 

  #if defined(SERIAL_COMMAND_MODE)
    get_sensor_group_from_serial(); 
  #elif defined(TEST_GROUP_ALL)
    sensor_group_idx = GET_SENSOR_GROUP_IDX(sensor_group_idx); //get sensor index in between iterations 
    p_group_info_request = sensor_groups[sensor_group_idx]; // get corresponding group 
    sensor_request = REQUEST_ALL; 
  #elif !defined(TEST_GROUP_ALL) && !defined(SERIAL_COMMAND_MODE)
    sensor_request = REQUEST_ALL; 
    p_group_info_request = SENSE_GROUP;
  #endif //!defined(SERIAL_COMMAND_MODE)

  if(CAN_MSGAVAIL == CAN0.checkReceive()) { // Check to see whether data is read
    CAN0.readMsgBufID(&can_id, &len, buf);    // Read data
  } else {
    can_id = 0; // No data so reset ID
  }
  //pseudo flow control here with locking booleans until ceratin IDs arrive over the wire  
  if(!p_group_info_request->group_rec_sent){ 

    request_data_from_group(p_group_info_request->group); 

    p_group_info_request->group_rec_sent = true; // indicate the group request has been sent 
  }

  if(can_id == 0x1DB && !request_recv_flag ){ 

    p_group_info_request->send_func(sensor_request); //decode data from reply 
    request_recv_flag = true; 
  }
  
  if(request_recv_flag){
    request_recv_flag = false;

    p_group_info_request->group_rec_sent = false; // indicate the group request has been sent 

  }
  
  if(can_id == NISSAN_BMS_REPLY_ID) {

    p_group_info_request->decode_func(); //decode data from reply 
  }
}


