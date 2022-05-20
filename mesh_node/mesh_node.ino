#include "RF24.h"
#include "RF24Mesh.h"
#include "RF24Network.h"
#include <SPI.h>

#include "config.h"
#include "filter.hpp"
#include "vec.hpp"

const uint8_t ACCEL_PIN[3] = {ACCEL_X_PIN, ACCEL_Y_PIN, ACCEL_Z_PIN};

// interval at which to blink (milliseconds)
const uint16_t interval = 1000U / sample_frequency;

RF24 radio(RADIO_CE_PIN, RADIO_CSN_PIN);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

vec<bandpass_filter<int16_t>> filters;

uint32_t previousMillis = 0; // will store last time LED was updated

uint32_t displayTimer = 0;

bool hit = false;

// By default, radio:
// * PA MAX
// * LNA enabled
// * 1 MBS

bool begin_mesh()
{
	if (mesh.begin(MESH_DEFAULT_CHANNEL, RF24_250KBPS, MESH_RENEWAL_TIMEOUT))
	{
		radio.setPALevel(RADIO_POWER);
		return true;
	}
	return false;
}

vec<int16_t> read_accel_filtered()
{
	vec<int16_t> accel;
	for (unsigned i = 0; i < 3; ++i)
		accel[i] = filters[i].bandpass(analogRead(ACCEL_PIN[i]));
	return accel;
}

// the setup routine runs once when you press reset:
void setup()
{
	// initialize serial communication
	Serial.begin(115200);
	Serial.println(F("TYRE IMPACT MONITOR"));

	pinMode(LED_PIN, OUTPUT);

	const float high_pass_cut_off = 1.0f; // cut off frequency in Hz
	const float low_pass_cut_off  = 5.0f; // cut off frequency in Hz

	for (auto &filter : filters)
		filter.init(0, low_pass_cut_off, high_pass_cut_off, sample_frequency);

	// Set the nodeID manually
	mesh.setNodeID(nodeID);

	Serial.print(F("     NODE "));
	Serial.println(mesh.getNodeID());

	// Connect to the mesh
	Serial.println(F("Connecting to the mesh..."));
	while (!begin_mesh())
	{
		Serial.println(
		  F("Radio hardware not responding or could not connect to network."));
	}
	Serial.println(F("Connected"));

	// Start sampllind and Give HP filter time to remove DC offsetts
	for (unsigned i = 0; i < 50; i++)
	{
		const vec<int16_t> accel = read_accel_filtered();
		delay(interval);
		Serial.print("X:");
		Serial.print(accel.x());
		Serial.print(", ");
		Serial.print("Y:");
		Serial.print(accel.y());
		Serial.print(", ");
		Serial.print("Z:");
		Serial.print(accel.z());
		Serial.println("");
	}
	Serial.println(F("Init Success"));
}

// the loop routine runs over and over again forever:
void loop()
{
	// Call mesh.update to keep the network updated
	mesh.update();

	const uint32_t currentMillis = millis();
	if (currentMillis - previousMillis >= interval)
	{
		// save the last time
		previousMillis = currentMillis;

		// Get ADC values
		const vec<int16_t> accel = read_accel_filtered();

		// Filter Values
		if (abs(accel.x()) > IMPACT_THRESHOLD)
		{
			Serial.print(F("Impact detected in X "));
			hit = true;
		}

		if (abs(accel.y()) > IMPACT_THRESHOLD)
		{
			Serial.print(F("Impact detected in Y "));
			hit = true;
		}

		if (abs(accel.z()) > IMPACT_THRESHOLD)
		{
			Serial.print(F("Impact detected in Z "));
			hit = true;
		}

		Serial.print("X:");
		Serial.print(accel.x());
		Serial.print(", ");
		Serial.print("Y:");
		Serial.print(accel.y());
		Serial.print(", ");
		Serial.print("Z:");
		Serial.print(accel.z());
		Serial.println("");

		digitalWrite(LED_PIN, hit ? HIGH : LOW);
	}

	// Send to the root node every second
	if (millis() - displayTimer >= 1000)
	{
		displayTimer = millis();

		uint32_t data[2] = {displayTimer, nodeID};

		if (!mesh.write(data, hit ? 'H' : 'M', sizeof(uint32_t) * 2))
		{
			// If a write fails, check connectivity to the mesh network
			if (!mesh.checkConnection())
			{
				// refresh the network address
				Serial.println(F("Renewing Address"));

				if (!mesh.renewAddress())
				{
					// If address renewal fails, reconfigure the radio and restart the
					// mesh This allows recovery from most if not all radio errors
					if (!begin_mesh()) Serial.println(F("Lost connection"));
				}
			}
			else
			{
				Serial.println(F("Send fail, Test OK"));
			}
		}
		else
		{
			Serial.print(F("Send OK: "));
			Serial.println(displayTimer);
			hit = false;
		}
	}
}
