#include<chrono>
#include<iostream>
#include<string>

class Timer{

public:
	Timer(std::string name, std::string micro_milli_sec = "default"): name(name), precise(micro_milli_sec), printmicro(true), printmilli(true), printsec(true){
		startTimeStamp = std::chrono::high_resolution_clock::now();
		setPrint();
	}

	void interupt(){
		long long duration  = stop();
		double milli_sec = duration * 1e-3;
		double sec = duration * 1e-6;
		
		std::cout << "========================\n";
		std::cout << name << " is interupt, current used time:\n";
		if(printmicro) std::cout << "in micro seconds: " << duration << std::endl;
		if(printmilli) {std::cout << "in milli seconds: "; printf("%0.3f\n",milli_sec);}
		if(printsec){std::cout << "in seconds:       "; printf("%0.3f\n",sec);}
		std::cout << "========================\n";
	}

	~Timer(){
		long long duration  = stop();
		double milli_sec = duration * 1e-3;
		double sec = duration * 1e-6;
		
		std::cout << "========================\n";
		std::cout << name << " used time:\n";
		if(printmicro) std::cout << "in micro seconds: " << duration << std::endl;
		if(printmilli) {std::cout << "in milli seconds: "; printf("%0.3f\n",milli_sec);}
		if(printsec){std::cout << "in seconds:       "; printf("%0.3f\n",sec);}
		std::cout << "========================\n";
	}
private:
	long long stop(){
		auto endTimeStamp = std::chrono::high_resolution_clock::now();
		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimeStamp).time_since_epoch().count();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimeStamp).time_since_epoch().count();
		auto duration = end - start;
		return duration;
	}
	void setPrint(){
		if(precise == "micro") {printsec = false; printmilli = false;}
		else if(precise == "milli") {printmicro = false; printsec = false;}
		else if(precise == "sec") {printmicro = false; printmilli = false;}
		else if(precise == "micro_milli") {printsec = false;}
		else if(precise == "milli_sec") {printmicro = false;}
		else {}
	}
	std::string name;
	std::string precise;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTimeStamp;
	bool printmicro;
	bool printmilli;
	bool printsec;

};
