//Done by: Aaron Lim
//Student ID: 5985171
//Assn: 1

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	//defining openCL library
#define __CL_ENABLE_EXCEPTIONS				//enable openCL exceptions

//c++ standard libraries
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <cwctype>
#include <fstream>

//openCL header, depending on OS
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

//include errors handler header
#include "error.h"

//avoid having to use prefixes
//using namespace std;
//using namespace cl;

//functions created
//print initiate
void printStart();

//addition functions
//credit for insensitive case checking
//https://www.oreilly.com/library/view/c-cookbook/0596007612/ch04s14.html
//https://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c

//for insensitive case checking
inline bool caseInsCharComp(char, char);
bool caseInsCompare(const std::string &, const std::string &);

//checking if string is int
bool is_number(const std::string &);

//task 1
cl_device_type selectDevice(std::string);
std::string inputDevice();

//task 3
void choiceOfDevice(std::vector<int> &, std::vector<cl::Platform> &,
	std::vector <int> &, std::vector<cl::Device> &, cl::Context &,
	std::vector<cl::Device> &, cl::CommandQueue &);

//task 4
cl::Program createProgram(std::ifstream &, std::vector<cl::Device> &, cl::Context &);

//task 5
void createKernel(std::vector<cl::Kernel> &, cl::Program &);

int main() {
	std::vector<cl::Platform> platforms;			//available platforms
	std::vector<cl::Device> devices;				//available devices
	std::string outputString;						//output purposes
	std::string userInput;							//get user input
	std::vector <int> platformID;					//storing platform int data
	std::vector <int> deviceID;						//storing device int data
	cl::Context context;							//context of device
	std::vector<cl::Device> contextDevice;			//device with context
	cl::CommandQueue queue;							//Command queue for context and device
	long mem_size;									//for storing memory size data
	int outputInt;									//for outputting group size and compute units
	int workItem[3];								//for work item, 3D
	int arraySize = 3;								//for work item

	//device chosen by the user
	cl_device_type deviceChosen;
	cl_device_type type;

	//display start
	printStart();

	//incase have openCL errors
	try {
		//task 1
		std::cout << "Task 1: " << std::endl;
		userInput = inputDevice();
		deviceChosen = selectDevice(userInput);

		//task 2
		//display available platform and devices
		std::cout << "--------------------------- " << std::endl;
		std::cout << "Task 2: " << std::endl;
		std::cout << "Available options: " << std::endl;

		unsigned int optionCounter = 0;				//option counter

		//getting all platform
		cl::Platform::get(&platforms);
		//std::cout << "Number of OpenCL platforms: " << deviceChosen << std::endl;

		for (unsigned int i = 0; i < platforms.size(); i++) {
			//for per devices

			platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &devices);
			//store available devices on the platform

			for (unsigned int j = 0; j < devices.size(); j++) {
				//check if user's choice is type (cl_device_type)
				//and print out all the devices that is user's choice
				//store the data type in "type"
				devices[j].getInfo(CL_DEVICE_TYPE, &type);
				if (type == deviceChosen) {
					//increment if found
					optionCounter++;
					std::cout << "---------------------------" << std::endl;

					//platform index
					std::cout << "Platform: " << optionCounter << std::endl;

					//getting the platform name / vendor / version
					//with getInfo() and store the data in outputString
					platforms[i].getInfo(CL_PLATFORM_NAME, &outputString);
					std::cout << "\tCL_PLATFORM_NAME: " << outputString << std::endl;

					platforms[i].getInfo(CL_PLATFORM_VENDOR, &outputString);
					std::cout << "\tCL_PLATFORM_VENDOR: " << outputString << std::endl;

					platforms[i].getInfo(CL_PLATFORM_VERSION, &outputString);
					std::cout << "\tCL_PLATFORM_VERSION: " << outputString << std::endl;

					//check if user input is GPU or CPU
					if (caseInsCompare(userInput, "GPU")) {
						std::cout << "\tCL_DEVICE_TYPE: GPU" << std::endl;
					}
					else {
						std::cout << "\tCL_DEVICE_TYPE: CPU" << std::endl;
					}
					std::cout << "Device " << optionCounter << ": " << devices[j].getInfo<CL_DEVICE_NAME>() << std::endl;

					//GET COMPUTER UNIT(INT), MAX  WORK GROUPSIZE(INT)
					//WORK ITEM(ARRAY OF INT, size of 3(because 3d)
					//local memory(long)
					devices[j].getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &outputInt);
					std::cout << "\tNumber of compute units: " << outputInt << std::endl;

					devices[j].getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &outputInt);
					std::cout << "\tNumber of group size: " << outputInt << std::endl;

					devices[j].getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &workItem);
					std::cout << "\tNumber of work item: "
						<< workItem[0] << " * "
						<< workItem[1] << " * "
						<< workItem[2] << std::endl;

					mem_size = devices[j].getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();
					std::cout << "\tLocal memory size: " << mem_size << " bytes" << std::endl;

					//store the details of platform and device
					platformID.push_back(i);
					deviceID.push_back(j);
				}
			}
		}

		//check if there is device
		if (deviceID.size() > 0) {
			std::cout << "---------------------------" << std::endl;
			//task 3
			std::cout << "Task 3: " << std::endl;
			choiceOfDevice(deviceID, platforms, platformID, devices, context, contextDevice, queue);
		}
		//task 4
		std::ifstream programFile("source.cl");
		cl::Program program = createProgram(programFile, contextDevice, context);


		//task 5
		std::cout << "Task 5: " << std::endl;
		//creation of all kernals
		std::vector<cl::Kernel> allKernels;

		createKernel(allKernels, program);
	}
	catch (cl::Error e) {
		handle_error(e);
	}

#ifdef _WIN32
	//wait for any keypress on window OS before exiting
	std::cout << "\nPress a key to exit";
	std::cin.ignore();
#endif
}

void printStart() {
	std::cout << "Assignment 1"
		<< std::endl;
}

//for insensitive case checking
inline bool caseInsCharComp(char a, char b) {
	return (toupper(a) == toupper(b));
}

bool caseInsCompare(const std::string &s1, const std::string &s2) {
	return (s1.size() == s2.size() && equal(s1.begin(), s1.end(), s2.begin(), caseInsCharComp));
}

//checking if string is int
bool is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

//task 1
std::string inputDevice() {
	bool ok = true;
	std::string userInput;
	while (ok) {
		std::cout << "Please input the device you want to use(GPU/CPU): ";
		std::getline(std::cin, userInput);
		if (caseInsCompare(userInput, "GPU") || caseInsCompare(userInput, "CPU")) {
			ok = false;
		}
		else {
			std::cout << "Entered a invalid input, please enter GPU/CPU" << std::endl;
		}
	}
	return userInput;
}

cl_device_type selectDevice(std::string userInput) {
	cl_device_type deviceChosen;
	if (caseInsCompare(userInput, "GPU")) {
		deviceChosen = CL_DEVICE_TYPE_GPU;
	}
	else {
		deviceChosen = CL_DEVICE_TYPE_CPU;
	}
	return deviceChosen;
}

//task 3
void choiceOfDevice(std::vector<int> &deviceID,
	std::vector<cl::Platform> &platforms,
	std::vector<int> &platformID,
	std::vector<cl::Device> &devices,
	cl::Context &context,
	std::vector<cl::Device> &contextDevice,
	cl::CommandQueue &queue) {
	bool ok = true;
	std::string userChoice;
	while (ok) {
		std::cout << "Enter one device ID to create context(1~"
			<< deviceID.size()
			<< "): ";
		std::getline(std::cin, userChoice);
		if (is_number(userChoice)) {
			//converting string to int
			int num = std::atoi(userChoice.c_str());
			if (num <= deviceID.size() && num > 0) {
				//create a context for the chosen device on the platform
				std::cout << "---------------------------"
					<< "\nCreating context for platform: "
					<< platforms[platformID[num - 1]].getInfo<CL_PLATFORM_NAME>()
					<< std::endl;

				//create context for chosen device on that platform
				platforms[platformID[num - 1]].getDevices(CL_DEVICE_TYPE_ALL, &devices);
				context = cl::Context(devices[deviceID[num - 1]]);

				//getting context on device
				contextDevice = context.getInfo<CL_CONTEXT_DEVICES>();

				//printing the device name that have the context
				for (unsigned int i = 0; i < contextDevice.size(); i++) {
					std::cout << "Context created: " << contextDevice[i].getInfo<CL_DEVICE_NAME>() << std::endl;
				}

				//create command queue
				queue = cl::CommandQueue(context, devices[deviceID[num - 1]]);
				std::cout << "Command Queue created" << std::endl;
				ok = false;
			}
			else {
				std::cout << "Please input (1~"
					<< deviceID.size()
					<< ")"
					<< std::endl;
			}
		}

		else {
			std::cout << "Please input (1~"
				<< deviceID.size()
				<< ")"
				<< std::endl;
		}
	}
}

//task 4
cl::Program createProgram(std::ifstream & programFile, std::vector<cl::Device> &contextDevice, cl::Context &context) {
	std::string outputString;
	//check if program opened
	if (!programFile.is_open()) {
		quit_program("File not found");
	}

	//create program string and load contents from the file
	std::string programString(std::istreambuf_iterator<char>(programFile), (std::istreambuf_iterator<char>()));

	//output the content of source.cl
	std::cout << "---------------------------" << std::endl;
	std::cout << "Task 4: " << std::endl;
	std::cout << "Contents of program string: " << std::endl;
	std::cout << programString << std::endl;

	//create program source from one input string
	cl::Program::Sources source(1, std::make_pair(programString.c_str(), programString.length() + 1));

	//create program from source
	cl::Program program(context, source);

	//try to build the program for the device in the context
	try {
		program.build(contextDevice);

		std::cout << "---------------------------" << std::endl;
		// output build log for all devices in context
		for (unsigned int i = 0; i < contextDevice.size(); i++)
		{
			// get device name
			outputString = contextDevice[i].getInfo<CL_DEVICE_NAME>();
			std::string build_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(contextDevice[i]);

			std::cout << "Device - " << outputString << ", build log:" << std::endl;
			std::cout << build_log << "--------------------" << std::endl;
		}

		std::cout << "Program build: Successful" << std::endl;
		std::cout << "---------------------------" << std::endl;
	}
	catch (cl::Error e) {
		// if failed to build program
		if (e.err() == CL_BUILD_PROGRAM_FAILURE)
		{
			// output program build log
			std::cout << e.what() << ": Failed to build program." << std::endl;

			// output build log for all devices in context
			for (unsigned int i = 0; i < contextDevice.size(); i++)
			{
				// get device name
				outputString = contextDevice[i].getInfo<CL_DEVICE_NAME>();
				std::string build_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(contextDevice[i]);

				std::cout << "Device - " << outputString << ", build log:" << std::endl;
				std::cout << build_log << "--------------------" << std::endl;
			}
		}
		else
		{
			throw e;
		}
	}
	return program;
}

//task 5
void createKernel(std::vector<cl::Kernel> &allKernels, cl::Program &program) {
	std::string outputString;
	//adding all the kernels which is in the program to the kernels
	program.createKernels(&allKernels);

	std::cout << "Kernel names" << std::endl;

	//display all the kernels
	for (unsigned int i = 0; i < allKernels.size(); i++) {
		outputString = allKernels[i].getInfo<CL_KERNEL_FUNCTION_NAME>();
		std::cout << "Kernal " << i << ": " << outputString << std::endl;
	}

	std::cout << "---------------------------" << std::endl;
}