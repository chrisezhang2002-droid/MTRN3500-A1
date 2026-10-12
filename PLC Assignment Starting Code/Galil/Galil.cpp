#include "Galil.h"

Galil::Galil() {
	Functions = new EmbeddedFunctions(); // Initialize the Embedded Functions Object
	Functions->GOpen("192.168.0.120 -d", &g);
	Address = "192.168.0.120 -d";
	//Set Control Parameters to default values
	ControlParameters[0] = 0.0; // Kp
	ControlParameters[1] = 0.0; // Ki
	ControlParameters[2] = 0.0; // Kd
	setPoint = 0;	
	ownsFunctions = true; // Galil object owns the EmbeddedFunctions object
}
Galil::Galil(EmbeddedFunctions* Funcs, GCStringIn address) : Functions(Funcs), Address(address) {
	Functions->GOpen(address, &g);
	//Set Control Parameters to default values
	ControlParameters[0] = 0.0; 
	ControlParameters[1] = 0.0; 
	ControlParameters[2] = 0.0; 
	setPoint = 0;
	ownsFunctions = false; // Galil object does not own the EmbeddedFunctions object

}
Galil::Galil(const Galil& other) {
	Functions = new EmbeddedFunctions(); // Initialize the Embedded Functions Object
	Address = other.Address;
	Functions->GOpen(Address.c_str(), &g);
	// Copy Control Parameters
	ControlParameters[0] = other.ControlParameters[0]; 
	ControlParameters[1] = other.ControlParameters[1]; 
	ControlParameters[2] = other.ControlParameters[2]; 
	setPoint = other.setPoint;
	ownsFunctions = true; // Galil object owns the EmbeddedFunctions object
}
Galil::~Galil() {
	Functions->GClose(g);
	if (ownsFunctions) { // Only delete Functions if this object owns it
		delete Functions;
	}
}

//Digital Outputs
void Galil::DigitalOutput(uint16_t value) {
	int lowBank = value & 0xFF; // 0-7
	int highBank = (value >> 8) & 0xFF; // 8-15
	std::string command = "OP" + std::to_string(lowBank) + "," + std::to_string(highBank) + ";";
	lastreturn = Functions->GCommand(g, command.c_str(), response, sizeof(response), nullptr);
}
void Galil::DigitalByteOutput(bool bank, uint8_t value) {
	std::string command;
	if (bank == 0) {
		command = "OP" + std::to_string(value) + ";"; // if bank 0, write to low bank
	}
	else {
		command = "OP ," + std::to_string(value) + ";"; // if bank 1, write to high bank
	}
	lastreturn = Functions->GCommand(g, command.c_str(), response, sizeof(response), nullptr);
}
void Galil::DigitalBitOutput(bool val, uint8_t bit) {
	std::string command;
	// SB command to set bit on, CB command to clear the bit
	if (val == 1) {
		command = "SB" + std::to_string(bit) + ";"; // bit on
	}
	else {
		command = "CB" + std::to_string(bit) + ";"; // bit off command
	}
	lastreturn = Functions->GCommand(g, command.c_str(), response, sizeof(response), nullptr);
}
uint16_t Galil::DigitalInput() {
	uint16_t bit_value = 0;

	for (int i = 0; i < 16; ++i) { //loop through each bit to get the value of the digital input
		std::string command = "MG @IN[" + std::to_string(i) + "];"; 
		Functions->GCommand(g, command.c_str(), response, sizeof(response), nullptr);
		
		if (atof(response) == 1) { // if the response is 1, set the corresponding bit in value
			bit_value = bit_value | (1 << i);	
		}
	}
	return bit_value;
}
uint8_t Galil::DigitalByteInput(bool bank) {
	uint8_t bit_value = 0;
	
	if (bank == 0) { // low bank
		for (int i = 0; i < 8; ++i) {
			std::string command = "MG @IN[" + std::to_string(i) + "];";
			Functions->GCommand(g, command.c_str(), response, sizeof(response), nullptr);
			if (atof(response) == 1) {
				bit_value = bit_value | (1 << i);
			}
		}
	}
	else { // high bank
		for (int i = 8; i < 16; ++i) {
			std::string command = "MG @IN[" + std::to_string(i) + "];";
			Functions->GCommand(g, command.c_str(), response, sizeof(response), nullptr);
			if (atof(response) == 1) {
				bit_value = bit_value | (1 << (i - 8)); // shift by (i - 8) to set the correct bit in the byte
			}
		}
	}
	return bit_value;
}
bool Galil::DigitalBitInput(uint8_t bit) {
	std::string command = "MG @IN[" + std::to_string(bit) + "];"; 
	Functions->GCommand(g, command.c_str(), response, sizeof(response), nullptr);
	return atof(response) == 1;
}
bool Galil::CheckSuccessfulWrite() {
	// last command is stored in lastreturn
	if (lastreturn == G_NO_ERROR) { // return true if last command was successful
		return true;
	}
	else {
		return false; // return false if last command was not successful
	}
}
float Galil::AnalogInput(uint8_t channel) {
	std::string command = "MG @AN[" + std::to_string(channel) + "];";
	Functions->GCommand(g, command.c_str(), response, sizeof(response), nullptr);
	return atof(response); // return a float
}
void Galil::AnalogOutput(uint8_t channel, double voltage) {
	std::string volts = std::to_string(voltage);
	volts = volts.substr(0, volts.find('.') + 3);
	std::string command = "AO " + std::to_string(channel) + "," + volts + ";"; // set the string
	lastreturn = Functions->GCommand(g, command.c_str(), response, sizeof(response), nullptr);
}
void Galil::AnalogInputRange(uint8_t channel, uint8_t range) {
	std::string command = "AQ " + std::to_string(channel) + "," + std::to_string(range) + ";"; 
	// Range 1 - +5V, Range 2 - +10V, Range 3 0-5, Range 4 0-10V
	lastreturn = Functions->GCommand(g, command.c_str(), response, sizeof(response), nullptr);
}

// Encoder Functions
void Galil::WriteEncoder() {
	lastreturn = Functions->GCommand(g, "WE 0;", response, sizeof(response), nullptr); // write to 0
}
int Galil::ReadEncoder() {
	Functions->GCommand(g, "QE 0;", response, sizeof(response), nullptr); //pull the encoder value (n0 only)
	return atoi(response);
}

// Control Functions
void Galil::setSetPoint(int s){
	setPoint = s;
}							
double Galil::getSetPoint() {
	return setPoint;
}
void Galil::setKp(double gain) {
	ControlParameters[0] = gain;
}
double Galil::getKp() {
	return ControlParameters[0];	
}
void Galil::setKi(double gain) {
	ControlParameters[1] = gain;
}
double Galil::getKi() {
	return ControlParameters[1];
}
void Galil::setKd(double gain) {
	ControlParameters[2] = gain;
}
double Galil::getKd() {
	return ControlParameters[2];
}

// Operator Overloading
std::ostream& operator<<(std::ostream& output, Galil& galil) {
	galil.Functions->GInfo(galil.g, galil.response, sizeof(galil.response));
	output << galil.response << "\n\n";
	galil.Functions->GVersion(galil.response, sizeof(galil.response));
	output << galil.response << "\n\n";
	return output;
}
Galil& Galil::operator= (const Galil& other) {
	Functions->GClose(g);
	if (ownsFunctions) {
		delete Functions; // delete the current Functions object if this object owns it
	}

	Functions = new EmbeddedFunctions(); // same as copy constructor
	ownsFunctions = true; // Galil object owns the EmbeddedFunctions object
	Address = other.Address;
	Functions->GOpen(Address.c_str(), &g);
	ControlParameters[0] = other.ControlParameters[0]; // Kp
	ControlParameters[1] = other.ControlParameters[1]; // Ki
	ControlParameters[2] = other.ControlParameters[2]; // Kd
	setPoint = other.setPoint;
	return *this;
}