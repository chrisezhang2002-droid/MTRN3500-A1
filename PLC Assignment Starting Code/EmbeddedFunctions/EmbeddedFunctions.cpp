#include "EmbeddedFunctions.h"
using namespace System::Text;

EmbeddedFunctions::EmbeddedFunctions() {
	GalilClient = nullptr;
	GalilStream = nullptr; //Set placeholder for the TcpClient and NetworkStream objects. These will be initialized in GOpen() when a connection is established.
}

EmbeddedFunctions::~EmbeddedFunctions() {
	GClose();
}

void EmbeddedFunctions::GOpen(String^ address, const int port) {
	if (address == nullptr || address->Length == 0) {
		throw gcnew Exception("Address is Invalid");
	}
	if (port < 0 || port > 65535) {
		throw gcnew Exception("Port is Invalid"); // Throw Error for invalid port or address
	}
	String^ Address = address->Split(' ')[0];
	GClose(); // Close any existing connection
	GalilClient = gcnew TcpClient(Address, port); // Connects the TCP socket to Address and Port
	GalilClient->SendTimeout = 500;
	GalilClient->ReceiveTimeout = 500; //I/O timeouts in milliseconds
	GalilClient->NoDelay = true;
	GalilStream = GalilClient->GetStream(); // Gets a stream wrapper over the socket


}

void EmbeddedFunctions::GClose() {
	if (GalilStream != nullptr) { // Close the NetworkStream if it is not null
		GalilStream->Close();
		GalilStream = nullptr;
	}
	if (GalilClient != nullptr) { // Close the TcpClient if it is not null
		GalilClient->Close();
		GalilClient = nullptr; // Close the TCP socket and set the TcpClient and NetworkStream objects to null
	}
}

String^ EmbeddedFunctions::GCommand(String^ command) {
	if (GalilClient == nullptr) { //Send Error if not connected
		throw gcnew Exception("Not Connected to Galil, Call GOpen()");
	}	
	if (!command->EndsWith(";")) { //Add semicolon if the command does not end with one
		command += ";";
	}
	
	array<Byte>^ SendData = Encoding::ASCII->GetBytes(command+"\r"); //Convert the command string to a byte array
	GalilStream->Write(SendData, 0, SendData->Length); //Sends the command bytes
	Threading::Thread::Sleep(10);

	array<Byte>^ ReceiveData = gcnew array<Byte>(1024); //Create a byte array to hold the response
	int bytecount = GalilStream->Read(ReceiveData, 0, ReceiveData->Length); //Read the response bytes
	//return of Read gives the byte count
	Threading::Thread::Sleep(10);
	
	String^ Response = Encoding::ASCII->GetString(ReceiveData, 0, bytecount); //Convert the response bytes to a string
	return Response->Trim(); //Return the response string
	
}