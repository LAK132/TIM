#include "RF24.h"
#include "RF24Mesh.h"
#include "RF24Network.h"
#include <SPI.h>

#include "config.h"

RF24 radio(RADIO_CE_PIN, RADIO_CSN_PIN);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

uint32_t displayTimer = 0;

struct node_state
{
	bool connected     = false;
	bool hit           = false;
	uint8_t time       = 0;
	uint32_t prev_time = 0;
};

constexpr uint16_t node_count = 4;
node_state nodes[node_count]  = {};

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

		uint32_t data[2] = {0, 0};
		switch (header.type)
		{
			// Display the incoming millis() values from the sensor nodes
			case 'M': // MISS
				network.read(header, data, sizeof(uint32_t) * 2);
				nodes[data[1]].connected = true;
				nodes[data[1]].time      = 0;
				// don't set hit to false, just reset the heartbeat timer
				break;

			case 'H': // HIT
				network.read(header, data, sizeof(uint32_t) * 2);
				nodes[data[1]].connected = true;
				nodes[data[1]].time      = 0;
				nodes[data[1]].hit       = true;
				break;

			default:
				network.read(header, nullptr, 0);
				Serial.println(header.type);
				break;
		}
	}

	if (millis() - displayTimer > 1000)
	{
		displayTimer = millis();

		// Clear serial terminal
		Serial.write(27);       // ESC command
		Serial.print(F("[2J")); // clear screen command
		Serial.write(27);       // ESC command
		Serial.print(F("[H"));  // cursor to home command
		delay(20);

		Serial.println(F(" TYRE IMPACT MONITOR"));
		Serial.println();
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
		Serial.println();
		Serial.println(F("Enter \"c\" to clear hits"));
		Serial.println();

		for (uint16_t i = 0; i < node_count; ++i)
		{
			Serial.print(F("Node "));
			Serial.print(i);
			Serial.print(F(" "));

			if (nodes[i].hit)
			{
				Serial.print(F("HIT"));
				tone(BUZZER_PIN, NOTE);
				if (!nodes[i].connected) Serial.print(F(" (NO CONNECTION)"));
			}
			else if (nodes[i].connected)
				Serial.print(F("MISS"));
			else
				Serial.print(F("NO CONNECTION"));

			Serial.println();

			if (nodes[i].time <= NODE_CONNECTION_TIMEOUT)
				++nodes[i].time;
			else
				nodes[i].connected = false;
		}
	}

	if (Serial.available() > 0)
	{
		int value = Serial.read();
		if (value == 'c' || value == 'C')
		{
			noTone(BUZZER_PIN);
			for (uint16_t i = 1; i <= node_count; i++) nodes[i].hit = false;
			Serial.println("Hits cleared");
		}
	}
}
