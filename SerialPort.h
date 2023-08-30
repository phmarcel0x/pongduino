#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <Windows.h>  // For Windows serial communication

class SerialPort {
public:
    SerialPort(const char* portName, int baudRate) {
        hSerial = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hSerial == INVALID_HANDLE_VALUE) {
            std::cerr << "Error opening serial port." << std::endl;
            return;
        }

        DCB dcbSerialParams = { 0 };
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        if (!GetCommState(hSerial, &dcbSerialParams)) {
            std::cerr << "Error getting serial port state." << std::endl;
            CloseHandle(hSerial);
            return;
        }

        dcbSerialParams.BaudRate = baudRate;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;
        if (!SetCommState(hSerial, &dcbSerialParams)) {
            std::cerr << "Error setting serial port parameters." << std::endl;
            CloseHandle(hSerial);
            return;
        }
    }

    ~SerialPort() {
        CloseHandle(hSerial);
    }

    std::string Readline() {
        char buffer[256];
        DWORD bytesRead;
        std::string result;

        ReadFile(hSerial, buffer, sizeof(buffer), &bytesRead, NULL);
        for (DWORD i = 0; i < bytesRead; ++i) {
            if (buffer[i] == '\n') {
                break;
            }
            result += buffer[i];
        }

        return result;
    }

private:
    HANDLE hSerial;
};

int main() {
    const char* serialPortName = "COM3";  // Adjust the COM port as needed
    const int baudRate = 9600;
    SerialPort arduino(serialPortName, baudRate);

    while (true) {
        std::string arduinoData = arduino.Readline();
        std::cout << "Arduino data: " << arduinoData << std::endl;

        // Add your paddle control logic here

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
