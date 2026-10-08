#include "Galil.h"

int main(void) {
	EmbeddedFunctions funcs(true);
	Galil myGalil(&funcs, "192.168.0.120 -d");
	myGalil.DigitalOutput(0xFF);    // turns output 1 on
	return 0;
}                                         // ← this closing brace is missing