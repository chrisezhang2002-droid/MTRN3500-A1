#include "Galil.h"

int main(void) {
	EmbeddedFunctions funcs(true);
	Galil myGalil(&funcs, "192.168.0.120 -d");

	float encoderValue = myGalil.ReadEncoder();
	std::cout << "Encoder Value: " << encoderValue << std::endl;
	//myGalil.AnalogOutput(0, 1);
	//myGalil.AnalogInputRange(0,2);
	//float voltage = myGalil.AnalogInput(0);
	//std::cout << "Voltage: " << voltage << std::endl;
	//myGalil.DigitalOutput(0xFFFF);

	//uint16_t inputs = myGalil.DigitalBitInput(7);          // read all 16 inputs
	//std::cout << "Inputs: " << inputs << std::endl;    // show the result

	//return 0;
}