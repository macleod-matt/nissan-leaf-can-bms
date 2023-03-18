/* Nissan Leaf BMS 30KWH data request and read via CANBUS
   Authors: Matt MacLeod, Dan Pothier 
    according to: https://drive.google.com/file/d/1jH9cgm5v23qnqVnmZN3p4TvdaokWKPjM/view
    Requests for sensing paramters sent from user over Serial can be mapped to the following query groups


    Notes: 

    group1: Query: BMS_QUERY_ID 02 21 01 00 00 00 00 00
    - HX 
    - SOC
    -  
    - hv_bat_curr1 
    - hv_bat_curr2 

    group2: Query: BMS_QUERY_ID 02 21 02 00 00 00 00 00
    - cell voltages [0-95]

    group3: Query: unknown 
    - unknown -- David Blackhursts code uses group 3 to read min and max voltages?? 
    
      see: 
      https://github.com/macleod-matt/NissanLeafBMS/blob/master/Code.ino#L139-L142
      https://github.com/macleod-matt/NissanLeafBMS/blob/master/Code.ino#L204-L215

    group4: Query: BMS_QUERY_ID 02 21 04 00 00 00 00 00
    - pack temperature 

    group6: Query: BMS_QUERY_ID 02 21 06 00 00 00 00 00
    - shunts 

    group61: Query: BMS_QUERY_ID 02 21 61 00 00 00 00 00
    - SOH

*/ 
#include <Arduino.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include "SensorTypes.h"
#include "CanBusNissanLeaf.h"

#if !defined(SERIAL_COMMAND_MODE) 
  #include "CanBusTests.h" //only need to import if we want to test without the serial interface
#endif //!Defined(SERIAL_COMMAND_MODE)

#if defined(TEST_GROUP_ALL) || defined(SERIAL_COMMAND_MODE)
  #define GROUP_SIZE (5) 
  #define GET_SENSOR_GROUP_IDX(idx) (idx + 1 < GROUP_SIZE ? idx + 1: 0)  	
  group_info_t * sensor_groups[GROUP_SIZE] = {&sense_group1,&sense_group2, &sense_group4,&sense_group6,&sense_group61};
  //helper macro to iterate sensor groups
  #define GET_SENSOR_GROUP(index,psensor_group ) \
  do { \
    psensor_group = sensor_groups[index];\
    index=GET_SENSOR_GROUP_IDX(index); \ 
  } while (0)

  int SerialSensorRequest = SYS_NO_REQUEST ; 
  

#endif //defined(TEST_GROUP_ALL) || defined(SERIAL_COMMAND_MODE)


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
    
    for(int i = 0; i < NUM_CELLS; i++) { // Display Cell Voltage and Shunts
      serDebug->print("Cell ");
      serDebug->print(i);
      serDebug->print(" ");
      serDebug->println(data->cell_voltages[i]);
  }
}


/**
 * @brief send out shunt status  
 * 
 * @param sensor_request 
 */

void send_shunt_status(int sensor_request){ 
    group6_data_t * data = (group6_data_t *)sense_group6.data; 
    
    for(int i = 0; i < NUM_CELLS; i++) { // Display Cell Voltage and Shunts
      serDebug->print("shunt ");
      serDebug->print(i);
      serDebug->print(" ");
      serDebug->println(data->shunts[i]);
  }
}

/**
 * @brief send out pack temperatures
 * 
 * @param sensor_request 
 */
void send_pack_temperatures(int sensor_request){
  
  group4_data_t * data = (group4_data_t *)sense_group4.data; 

  switch(sensor_request){ 
    case REQUEST_ALL:
      sendPacket(EV_BAT_TEMP_1, (float)data->pack_temps[0]);
      sendPacket(EV_BAT_TEMP_2, (float)data->pack_temps[1]);
      sendPacket(EV_BAT_TEMP_3, (float)data->pack_temps[2]);
      sendPacket(EV_BAT_TEMP_4, (float)data->pack_temps[3]);
      return; //we will break in the event of a request all
    case EV_BAT_TEMP_1: 
      sendPacket(EV_BAT_TEMP_1, (float)data->pack_temps[0]);
      return;
    case EV_BAT_TEMP_2: 
      sendPacket(EV_BAT_TEMP_2, (float)data->pack_temps[1]);
      return; 
    case EV_BAT_TEMP_3: 
      sendPacket(EV_BAT_TEMP_3, (float)data->pack_temps[2]);
      return; 
    case EV_BAT_TEMP_4: 
      sendPacket(EV_BAT_TEMP_4, (float)data->pack_temps[3]);
      return; 
    default:
      return; //invalid request, we will return from function
  }

}



/**
 * @brief function to read cell voltages from pack
 * 
 */
void decode_cell_voltages() {
  
  static int cellCount = -1; 
  static group2_data_t *data = (group2_data_t *)sense_group2.data; //cast as group 2 data 
  static uint8_t prevCellVoltage = 0; 

  int dataType = (buf[0] %2); // Two sets of data, one with split cell at the end and one at the start

  if (buf[0] == 0x10 && buf[3] == 0x02) { // First Line
    BYTES_TO_CELL_VOLTGAGE(buf[4],buf[5], cellCount, data->cell_voltages);
    BYTES_TO_CELL_VOLTGAGE(buf[6],buf[7], cellCount, data->cell_voltages);
    
    CAN0.sendMsgBuf(BMS_QUERY_ID, 0, GROUP_REQEST_SIZE, sendNextLine);
  } else if (dataType == 1) {
    BYTES_TO_CELL_VOLTGAGE(buf[1],buf[2], cellCount, data->cell_voltages);
    BYTES_TO_CELL_VOLTGAGE(buf[3],buf[4], cellCount, data->cell_voltages);
    BYTES_TO_CELL_VOLTGAGE(buf[5],buf[6], cellCount, data->cell_voltages);  
    prevCellVoltage = buf[7]; 

    CAN0.sendMsgBuf(BMS_QUERY_ID, 0, GROUP_REQEST_SIZE, sendNextLine);
  } else if (dataType == 0) {
    BYTES_TO_CELL_VOLTGAGE(prevCellVoltage,buf[1], cellCount, data->cell_voltages);
    BYTES_TO_CELL_VOLTGAGE(buf[2],buf[3], cellCount, data->cell_voltages);
    BYTES_TO_CELL_VOLTGAGE(buf[4],buf[5], cellCount, data->cell_voltages);
    BYTES_TO_CELL_VOLTGAGE(buf[6],buf[7], cellCount, data->cell_voltages);

    CAN0.sendMsgBuf(BMS_QUERY_ID, 0, GROUP_REQEST_SIZE, sendNextLine);
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
    CAN0.sendMsgBuf(BMS_QUERY_ID, 0, GROUP_REQEST_SIZE, sendNextLine);
  } else if (buf[0] == 0x21) { // Second Line
    data->pack_temps[1] = buf[2]; 
    data->pack_temps[2] = buf[5]; 
    CAN0.sendMsgBuf(BMS_QUERY_ID, 0, GROUP_REQEST_SIZE, sendNextLine);
  } else if (buf[0] == 0x22) { // Third Line
    data->pack_temps[3] = buf[1]; 

  }

}

/**
 * 
 * @brief decodes shunt voltages from can bus 
 * 
 *
 */
void decode_shunts() {

  static int shunt_index = -1;
  static group6_data_t * data = ( group6_data_t *)sense_group6.data; 

  if (buf[0] == 0x10 && buf[3] == 0x06) { // First Line
    shunt_index = 0;
    for(int i = 4; i < 8; i++) {
      BYTES_TO_SHUNT_VAL(buf[i], shunt_index, data->shunts);
    }
    CAN0.sendMsgBuf(BMS_QUERY_ID, 0, GROUP_REQEST_SIZE, sendNextLine);
  } else if (buf[0] == 0x21 || buf[0] == 0x22) {
    for(int i = 0; i < 8; i++) {
      BYTES_TO_SHUNT_VAL(buf[i],shunt_index,data->shunts);
    }
    CAN0.sendMsgBuf(BMS_QUERY_ID, 0, GROUP_REQEST_SIZE, sendNextLine);
  } else if (buf[0] == 0x23) {
    for(int i = 0; i < 4; i++) {
      BYTES_TO_SHUNT_VAL(buf[i],shunt_index, data->shunts);
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
    CAN0.sendMsgBuf(BMS_QUERY_ID, 0, GROUP_REQEST_SIZE, sendNextLine);
  
  } else if (buf[0] == 0x21) {
    
    hv2_current = (buf[4] << 24) | (buf[5] << 16 | ((buf[6] << 8) | buf[7])); 
    data->hv_bat_curr2 = CHECK_CURRENT(hv2_current);
   
    CAN0.sendMsgBuf(BMS_QUERY_ID, 0, GROUP_REQEST_SIZE, sendNextLine);
  } else if (buf[0] == 0x22) {
    CAN0.sendMsgBuf(BMS_QUERY_ID, 0, GROUP_REQEST_SIZE, sendNextLine);
  } else if (buf[0] == 0x23) {
    CAN0.sendMsgBuf(BMS_QUERY_ID, 0, GROUP_REQEST_SIZE, sendNextLine);
  } else if (buf[0] == 0x24) {
    data->hx = (float)((buf[4] << 8) | buf[5] ) / 102.4; //hx formula according to nissan 2018 doc 
    SOC_HB = buf[7]; // get the highbyte from the formula      
    CAN0.sendMsgBuf(BMS_QUERY_ID, 0, GROUP_REQEST_SIZE, sendNextLine);
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

  group61_data_t * data = (group61_data_t *) sense_group61.data; 

  serDebug->print(sensor_request );
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

  // Query: BMS_QUERY_ID 02 21 61 00 00 00 00 00
  // Answer: 0x7BB 11 4B 61 61 26 9A 25 CA
  // Formula: SOH = (( data[6] << 8 ) | data[7] ) / 100
  
  if (buf[0] == 0x11 && buf[2] == 0x61) { // First Line
      data->soh = (float)(((buf[6] << 8) | buf[7]) /100.0);
      CAN0.sendMsgBuf(BMS_QUERY_ID, 0, GROUP_REQEST_SIZE, sendNextLine);
  }
}




/**
 * @brief generic helper function to request the data from the group message
 * 
 * @param group_request 
 */
void request_data_from_group(byte * group_request){ 

  CAN0.sendMsgBuf(BMS_QUERY_ID, 0, GROUP_REQEST_SIZE, group_request); 

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

		serOutput->print(DEVICE_MAC, DEC);
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

  while(!serOutput); 

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

  #if defined(TEST_GROUP_ALL)
    //we need to initalize the pointer to the first group in the event of a test group all 
    p_group_info_request = &sense_group1; 
  #endif //defined(TEST_GROUP_ALL)
  
  serDebug->println("Sys Ready");



}


/**
 * @brief helper function desern the sensor group corresponding to the serial command 
 * 
 * 
 */
#if defined(SERIAL_COMMAND_MODE)

void get_sensor_group_from_serial(){ 
  
  static int groupIndex = 0; 
  String serialRecStr; 
 //get sensor_request from Serial port 
  //we will use the serial.readString function because it will make life easier to recieve commands from the serial port if we search for a terminating character 
  if(serOutput->available() > 0)
  {
    // SerialSensorRequest = serDebug->read();
    serialRecStr = serOutput->readString();  //read until timeout
    serialRecStr.trim();                        // remove any \r \n whitespace at the end of the String
    SerialSensorRequest = serialRecStr.toInt(); //convert string to int 
  }

  if(SerialSensorRequest == REQUEST_ALL ){ 
    GET_SENSOR_GROUP(groupIndex,p_group_info_request); 
  }else if( SYS_NO_REQUEST  < SerialSensorRequest && SerialSensorRequest < EV_BAT_CELL_VOLTAGES){ 
    p_group_info_request = &sense_group1;
    return;
  }else if(SerialSensorRequest == EV_BAT_CELL_VOLTAGES){ 
    p_group_info_request = &sense_group2;
    return;
  }else if(EV_BAT_CELL_VOLTAGES < SerialSensorRequest && SerialSensorRequest < EV_BAT_SHUNTS){ 
    p_group_info_request = &sense_group4;
    return; 
  } else if(SerialSensorRequest == EV_BAT_SHUNTS){ 
    p_group_info_request = &sense_group6;
  }else if(SerialSensorRequest == EV_BAT_SOH){
    p_group_info_request = &sense_group61;
    return; 
  }
  //if we get here, there is an invalid request being sent from the user. We dont care if the user imputs a sys no request 
  if(SerialSensorRequest != SYS_NO_REQUEST ){
    serDebug->print("Invalid sensor request recieved over serial "); 
    serDebug->println(SerialSensorRequest, HEX); 
  }
}
#endif //defined(SERIAL_COMMAND_MODE)



void loop() {

  static bool request_recv_flag = false; //flag to indicate group reequest recieved  
  static unsigned long CanMsgID = 0; 
  static int SensorRequest = SYS_NO_REQUEST ; 
  static int sampleCounter = 0; 

  #if defined(SERIAL_COMMAND_MODE)
    get_sensor_group_from_serial(); 
    SensorRequest = SerialSensorRequest;
  #elif defined(TEST_GROUP_ALL)
    static int groupIndex = 0; //since we are already initalized to group1, we will initalize this index to 1 
    SensorRequest = REQUEST_ALL; 
    //we want to take a few samples before transitioning 
    if(sampleCounter == 10000){
      GET_SENSOR_GROUP(groupIndex,p_group_info_request); 
      sampleCounter = 0; //reset sample counter after transitioning states 
    }
    sampleCounter++; // increment sample counter 
  #elif !defined(TEST_GROUP_ALL) && !defined(SERIAL_COMMAND_MODE)
    SensorRequest = REQUEST_ALL; 
    p_group_info_request = SENSE_GROUP;
  #endif //!defined(SERIAL_COMMAND_MODE)

  if(CAN_MSGAVAIL == CAN0.checkReceive()) { // Check to see whether data is read
    CAN0.readMsgBufID(&CanMsgID, &len, buf);    // Read data
  } else {
    CanMsgID = 0; // No data so reset ID
  }
  //pseudo flow control here with locking booleans until ceratin IDs arrive over the wire  
  if(!p_group_info_request->group_rec_sent){ 

    request_data_from_group(p_group_info_request->group); 

    p_group_info_request->group_rec_sent = true; // indicate the group request has been sent 
  }

  if(CanMsgID == 0x1DB && !request_recv_flag ){ 

    //only send the data every 100 samples 
    if((sampleCounter % 100) == 0){
      p_group_info_request->send_func(SensorRequest); //decode data from reply 
      request_recv_flag = true; 
    }
  }
  
  if(request_recv_flag){
    request_recv_flag = false;

    p_group_info_request->group_rec_sent = false; // indicate the group request has been sent 

  }
  
  if(CanMsgID == NISSAN_BMS_REPLY_ID) {

    p_group_info_request->decode_func(); //decode data from reply 
  }
}


