#include "RF24.h"
#include "RF24Mesh.h"
#include "RF24Network.h"
#include <SPI.h>

#include "config.h"

RF24 radio(RADIO_CE_PIN, RADIO_CSN_PIN);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

uint32_t displayTimer = 0;

bool begin_mesh()
{
	if (mesh.begin(MESH_DEFAULT_CHANNEL, RF24_250KBPS, MESH_RENEWAL_TIMEOUT))
	{
		radio.setPALevel(RADIO_POWER);
		return true;
	}
	return false;
}

// the setup routine runs once when you press reset:
void setup()
{
	// initialize serial communication at 9600 bits per second:
	Serial.begin(115200);
	Serial.println(F("TYRE IMPACT MONITOR"));

	// Set the nodeID to 0 for the root node
	mesh.setNodeID(0);

	Serial.print(F("     ROOT "));
	Serial.println(mesh.getNodeID());

	// Connect to the mesh
	while (!begin_mesh())
	{
		Serial.println(
		  F("Radio hardware not responding or could not connect to network."));
	}
	Serial.println(F("Connected"));
}

// the loop routine runs over and over again forever:
void loop()
{
	// Call mesh.update to keep the network updated
	mesh.update();

	// In addition, keep the 'DHCP service' running on the root node so
	// addresses will be assigned to the sensor nodes
	mesh.DHCP();

	// Check for incoming data from the sensors
	if (network.available())
	{
		RF24NetworkHeader header;
		network.peek(header);

		uint32_t dat[2] = {0, 0};
		switch (header.type)
		{
			// Display the incoming millis() values from the sensor nodes
			case 'M': // MISS
				network.read(header, dat, sizeof(uint32_t) * 2);
				Serial.print(F("MISS NODE "));
				Serial.print(dat[1]);
				Serial.print(F(" @ "));
				Serial.println(dat[0]);
				break;

			case 'H': // HIT
				network.read(header, dat, sizeof(uint32_t) * 2);
				Serial.print(F("HIT  NODE "));
				Serial.print(dat[1]);
				Serial.print(F(" @ "));
				Serial.println(dat[0]);
				break;

			default:
				network.read(header, 0, 0);
				Serial.println(header.type);
				break;
		}
	}

	if (millis() - displayTimer > 5000)
	{
		displayTimer = millis();
		Serial.println(F(" "));
		Serial.println(F("******* Assigned Addresses *******"));
		Serial.print(F("Number: "));
		Serial.println(mesh.addrListTop);
		for (int i = 0; i < mesh.addrListTop; i++)
		{
			Serial.print(F("NodeID: "));
			Serial.print(mesh.addrList[i].nodeID);
			Serial.print(F(" RF24Network Address: 0"));
			Serial.println(mesh.addrList[i].address, OCT);
		}
		Serial.println(F("**********************************"));
	}
}
