// User Configuration: nodeID - A unique identifier for each radio. Allows
// addressing to change dynamically with physical changes to the mesh.
//
// A unique value from 1-255 (0 is reserved for the root node) must be
// configured for each node. This will be stored in EEPROM on AVR devices, so
// remains persistent between further uploads, loss of power, etc.
const uint32_t nodeID = 1;

// Time in between samples in Hz
const float sample_frequency = 50.0f;

//#define RADIO_POWER RF24_PA_MIN   //(0)  -18 dBm
//#define RADIO_POWER RF24_PA_LOW   //(1)  -12 dBm
//#define RADIO_POWER RF24_PA_HIGH  //(2)  -6 dBm
#define RADIO_POWER RF24_PA_MAX //(3)   0 dBm

#define RADIO_SCK_PIN  13
#define RADIO_MISO_PIN 12
#define RADIO_MOSI_PIN 11
#define RADIO_CSN_PIN  8
#define RADIO_CE_PIN   7
#define RADIO_IRQ_PIN  2

#define IMPACT_THRESHOLD 80 // raw ADC value

#define ACCEL_X_PIN A7
#define ACCEL_Y_PIN A6
#define ACCEL_Z_PIN A5
#define LED_PIN     4
