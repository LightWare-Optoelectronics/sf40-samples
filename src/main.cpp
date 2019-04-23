//----------------------------------------------------------------------------------------------------------------------------------
// LightWare SF40 Linux Example.
//----------------------------------------------------------------------------------------------------------------------------------
#include "common.h"
#include "lwNx.h"

//----------------------------------------------------------------------------------------------------------------------------------
// Helper utilities.
//----------------------------------------------------------------------------------------------------------------------------------
void printHexDebug(uint8_t* Data, uint32_t Size) {
	printf("Buffer: ");

	for (int i = 0; i < Size; ++i) {
		printf("0x%02X ", Data[i]);
	}

	printf("\n");
}

void exitWithMessage(const char* Msg) {
	printf("%s\nPress any key to Exit...\n", Msg);
	std::cin.ignore();
	exit(1);
}

void exitCommandFailure() {
	exitWithMessage("No response to command, terminating sample.\n");
}

//----------------------------------------------------------------------------------------------------------------------------------
// Application Entry.
//----------------------------------------------------------------------------------------------------------------------------------
int main(int args, char **argv)
{
	platformInit();
	
	printf("SF40 sample\n");

#ifdef __linux__
	const char* portName = "/dev/ttyUSB0";
#else
	const char* portName = "\\\\.\\COM39";
#endif

	int32_t baudRate = 921600;

	lwSerialPort* serial = platformCreateSerialPort();
	if (!serial->connect(portName, baudRate)) {
		exitWithMessage("Could not establish serial connection\n");
	};

	// Read the product name. (Command 0: Product name)
	char modelName[16];
	if (!lwnxCmdReadString(serial, 0, modelName)) { exitCommandFailure(); }

	// Read the hardware version. (Command 1: Hardware version)
	uint32_t hardwareVersion;
	if (!lwnxCmdReadUInt32(serial, 1, &hardwareVersion)) { exitCommandFailure(); }

	// Read the firmware version. (Command 2: Firmware version)
	uint32_t firmwareVersion;	
	if (!lwnxCmdReadUInt32(serial, 2, &firmwareVersion)) { exitCommandFailure(); }
	char firmwareVersionStr[16];
	lwnxConvertFirmwareVersionToStr(firmwareVersion, firmwareVersionStr);

	// Read the serial number. (Command 3: Serial number)
	char serialNumber[16];
	if (!lwnxCmdReadString(serial, 3, serialNumber)) { exitCommandFailure(); }

	printf("Model: %.16s\n", modelName);
	printf("Hardware: %d\n", hardwareVersion);
	printf("Firmware: %.16s (%d)\n", firmwareVersionStr, firmwareVersion);
	printf("Serial: %.16s\n", serialNumber);

	// Set the output rate to full (20010 points per second). (Command 108: Output rate)
	if (!lwnxCmdWriteUInt8(serial, 108, 0)) { exitCommandFailure(); }

	// Enable streaming of point data. (Command 30: Stream)
	if (!lwnxCmdWriteUInt32(serial, 30, 3)) { exitCommandFailure(); }

	// Continuously wait for and process the streamed point data packets.
	// The incoming point data packet is Command 48: Distance output.
	while (1) {
		lwResponsePacket response;
		
		if (lwnxRecvPacket(serial, 48, &response, 1000)) {
			uint8_t 	alarmState = response.data[4];
			uint16_t 	pointsPerSecond = (response.data[6] << 8) | response.data[5];
			int16_t 	forwardOffset = (response.data[8] << 8) | response.data[7];
			int16_t 	motorVoltage = (response.data[10] << 8) | response.data[9];
			uint8_t 	revolutionIndex = response.data[11];
			uint16_t 	pointTotal = (response.data[13] << 8) | response.data[12];
			uint16_t 	pointCount = (response.data[15] << 8) | response.data[14];
			uint16_t 	pointStartIndex = (response.data[17] << 8) | response.data[16];
			uint16_t 	pointDistances[210];
			memcpy(pointDistances, response.data + 18, pointCount * 2);

			printf("Alarm %d\n", alarmState);
			printf("pointsPerSecond %d\n", pointsPerSecond);
			printf("forwardOffset %d\n", forwardOffset);
			printf("motorVoltage %d\n", motorVoltage);
			printf("revolutionIndex %d\n", revolutionIndex);
			printf("pointTotal %d\n", pointTotal);
			printf("pointCount %d\n", pointCount);
			printf("pointStartIndex %d\n", pointStartIndex);

			for (int i = 0; i < pointCount; ++i) {
				float angle = ((float)(pointStartIndex + i) / (float)pointTotal) * 360.0f;

				printf("%3d %5d %6.2f\n", i, pointDistances[i], angle);
			}
		}
	}

	return 0;
}