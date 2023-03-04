#ifndef SensorTypes_h
#define SensorTypes_h
/**
 * @brief These are a subset of the various sensortype mappings for the Aretas API
 * 
 * The REST API describes metadata for 100s of different sensor types including
 * char codes, labels, descriptions, safe ranges, AI/ML hints, recommendations, units, etc, etc.
 * 
 * To access the raw metadata in JSON format you can query the open REST endpoint here:
 * 
 * https://iot.aretas.ca/rest/sensortype/list
 * 
 * Not all sensor types are available through the API (only the public enabled ones and not the ones that users override or provide in their own accounts)
 * And not every sensor type has full metadata populated
 * 
 * When you emit these types from firmware to the REST endpoint or to a middleware application, these types are somewhat important for routing (in the middleware)
 * and for analytics / ML / AI / Alerting / etc. in the Cloud service
 * 
 * To request additional metadata annotations for new sensor types, contact team-aretas@aretas.ca 
 * 
 */

#define O3_SENSOR_TYPE  0x30 //48 decimal Ozone gas concentration (ppb O3)
#define SPL_AVG_TYPE    0x31 //49 decimal  Sound pressure level average / polling interval (dB zero weighted SPL / RMS)
#define SPL_PEAK_TYPE   0x32 //50 decimal  Sound pressure level at polling time (dB zero weighted SPL / RMS)

#define ACCEL_VIB_X_TYPE 0x34 //52 decimal Vibration magnitude in the X-Axis (m/s^2)
#define ACCEL_VIB_Y_TYPE 0x35 //53 decimal Vibration magnitude in the Y-Axis (m/s^2)
#define ACCEL_VIB_Z_TYPE 0x36 //54 decimal Vibration magnitude in the Z-Axis (m/s^2)

#define ACCEL_X_TYPE     0x37 //55 decimal placeholder for X-axis acceleration (raw)
#define ACCEL_Y_TYPE     0x38 //56 decimal placeholder for Y-axis acceleration (raw)
#define ACCEL_Z_TYPE     0x39 //57 decimal placeholder for Z-axis acceleration (raw)

#define TEMP_SENSOR_TYPE    0xF6 //246 decimal Temperature (degrees C)
#define TEMP_SENSOR_TYPE_F  0xF2 //242 decimal Temperature (degrees F)
#define RH_SENSOR_TYPE      0xF8 //248 decimal Relative Humidity (percent)
#define CO2_SENSOR_TYPE     0xB5 //181 decimal Carbon Dioxide (ppm CO2)
#define ECO2_SENSOR_TYPE    0xB6 //182 decimal eCO2 (ppm eCO2)
#define NO2_SENSOR_TYPE     0x50 //80 decimal  Nitrogen Dioxide (ppm NO2)
#define CO_SENSOR_TYPE      0x40 //64 decimal  Carbon Monoxide (ppm CO)
#define COB_SENSOR_TYPE     0x41 //65 decimal  Channel 2 Carbon Monoxide (ppm CO)
#define VOC_SENSOR_TYPE     0x60 //96 decimal  Volatile Organic Compounds (tVOC ppb / ppm)
#define DPA_SENSOR_TYPE     0x63 //99 decimal  Differential pressure (+/-dPa pascals)
#define BARO_SENSOR_TYPE    0x64 //100 decimal Barometric pressure (hPa)
#define LUX_SENSOR_TYPE     0x73 //115 decimal Lux 

/**
 * @brief This particular type (0xD9) is used for single channel PM sensors 
 * that only output a relative mass concentration (e.g. Sharp GP2Y1010, DN7C3CA006, Shinyei PPD42NS, PPD40, etc.)
 * 
 * Notwithstanting having a calibration algo of course!
 * 
 * 
 */
#define PM_SENSOR_TYPE 0xD9   //217 decimal

#define AQI_SENSOR_TYPE 0x1C //28 decimal

#define PM_1_0_SENSOR_TYPE 0x20    //32 decimal (particulate matter µG/m^3)
#define PM_2_5_SENSOR_TYPE 0x21    //33 decimal (particulate matter µG/m^3)
#define PM_10_SENSOR_TYPE  0x22    //34 decimal (particulate matter µG/m^3)

#define PM_0p3_COUNT_SENSOR_TYPE 0x23    //35 decimal (particulate matter counts > 0.3µm/0.1L)
#define PM_0p5_COUNT_SENSOR_TYPE 0x24    //36 decimal (particulate matter counts > 0.5µm/0.1L)
#define PM_1p0_COUNT_SENSOR_TYPE 0x25   //37 decimal (particulate matter counts > 1.0µm/0.1L)
#define PM_2p5_COUNT_SENSOR_TYPE 0x26   //38 decimal (particulate matter counts > 2.5µm/0.1L)
#define PM_5p0_COUNT_SENSOR_TYPE 0x27   //39 decimal (particulate matter counts > 5.0µm/0.1L)
#define PM_10_COUNT_SENSOR_TYPE  0x28   //40 decimal (particulate matter counts > 10µm/0.1L)


/**
 * @brief These particular types are all related to the Nissan Leaf Battery packs 
 * These types are the first types to go beyond 0xFF
 * The old type field was a byte type.. after all whoever expected us to get beyond 255 sensor types! ;)
 * 
 * We have refactored the API and middleware to accept unsigned int types a long time ago, but this is the 
 * first batch in practice, so we should be mindful for testing
 * 
 */
 enum EVBatterySensorTypes{ 

    SYS_NO_REQUEST =    0x200,    //512 decimal (idle statue command)
    EV_BAT_HX =   0x201,    //513 decimal (high voltage battery's health)
    EV_BAT_SOC =  0x202,    //514 decimal (state of charge of the HV battery)
    EV_BAT_AHR =  0x203,    //515 decimal (Capacity of HV battery (how much energy the battery [sic] could hold when fully charged)
    EV_BAT_HV_BAT_CURRENT_1 = 0x204, //516 decimal (High voltage battery current. Positive when driving, negative when regen braking or charging)
    EV_BAT_HV_BAT_CURRENT_2 = 0x205, //517 decimal (Unclear why there are two.. but there are!)
    EV_BAT_HV_BAT_VOLTAGE =   0x206, //518 decimal (High voltage battery voltage)
    // we will not support this directly right now, but will in the future as an extended type
    EV_BAT_CELL_VOLTAGES =    0x207, //519 decimal (cells voltage - this is the voltages (in mV) from the N cell pairs (96 in the leaf for example))
    EV_BAT_TEMP_1 =           0x208, //520 decimal (packs temperatures (degrees C))
    EV_BAT_TEMP_2 =           0x209, //521 decimal           ''
    EV_BAT_TEMP_3 =           0x20A, //522 decimal           ''
    EV_BAT_TEMP_4 =           0x20B, //523 decimal           ''
    EV_BAT_SHUNTS =           0x20C, //524 decimal (shunt statuses for all cell shunts (in the case of EV leaf, 96 shuts))
    EV_BAT_SOH =              0x20D, //525 decimal (State of Health is another indication of the battery’s ability to hold and release energy. Expressed as %)
    REQUEST_ALL =             0x20E, //526 decimal 

};

typedef struct {
  uint16_t status;
  float temp;
  float rh;
}TRH;

typedef struct {
    int nSamples;           //number of samples we took
    uint32_t sampleRate;    //the calculated sample rate (samples per second)
    float rmsValue;         //the RMS value of the raw ADC output
}RMSReading;

typedef struct {
    float dbSPLRef;         //the reference SPL value (in dB SPL)
    float refRMSVal;        //the RMS ADC value 
}SPLCal;

#endif
