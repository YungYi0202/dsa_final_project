#pragma once
#include<unordered_set>
#include<set>
#include<string>
#include<vector>
#include<exception>
#include<fstream>
#include<cstring>
#include<utility>
#include<iostream>
//#include"Datetime.h"
#include"globeVar.h"

extern std::ifstream inFileHandler;
extern std::string fileName;

namespace Mail_construct{
	std::string str_month;
	std::string str_subject;
	std::string str_content;
	char str_hr_min[10];
}
int monthConverter(const std::string& m){
    if(m == "January") return 1;
    else if(m == "February") return 2;
    else if(m == "March") return 3;
    else if(m == "April") return 4;
    else if(m == "May") return 5;
    else if(m == "June") return 6;
    else if(m == "July") return 7;
    else if(m == "August") return 8;
    else if(m == "September") return 9;
    else if(m == "October") return 10;
    else if(m == "November") return 11;
    else if(m == "December") return 12;
    else return -1;
}
std::size_t find_first_alnum(const std::string& str, std::size_t startPos){
	try{
		while(std::isalnum(str.at(startPos)) == 0){
			startPos++;
		}
	}
	catch(std::out_of_range e){
		return std::string::npos;
	}
	return startPos;
}

std::size_t find_first_not_alnum(const std::string& str, std::size_t startPos){
	try{
		while(std::isalnum(str.at(startPos)) != 0){
			startPos++;
		}
	}
	catch(std::out_of_range e){
		return std::string::npos;
	}
	return startPos;
}

std::string& string_tolower(std::string& str){
	for(char& chr : str){
		chr = tolower(chr);
	}
	return str;
}
//heavy data type, should avoid copy
class Mail{
private:
	
	//should be const
	int id;
	std::string from;
	std::string to;
	//Datetime date;
	unsigned long long int date;
	std::unordered_set<std::string> content_words;
	int alphNum_cnt;

public:
	Mail();
	void print();
	int getid(){return id;}
	std::string& getFrom(){return from;}
	std::string& getTo(){return to;}
	unsigned long long int& getDate(){return date;}
	std::unordered_set<std::string>& getContent(){ return content_words; }
	int get_alphNum_cnt(){return alphNum_cnt; }
	bool operator>(const Mail& other){ return this->id > other.id; }
    bool operator<(const Mail& other){ return this->id < other.id; }
    bool operator==(const Mail& other){ return this->id == other.id; }
    bool operator!=(const Mail& other){return this->id != other.id; }
	static bool priority_less(const Mail& a, const Mail& b){
		if(a.alphNum_cnt < b.alphNum_cnt) return true;
		if(a.alphNum_cnt > b.alphNum_cnt) return false;
		if(a.id > b.id) return true;
		return false;
	}
};


void Mail::print(){
	std::cout << "From: " << from << std::endl;
	//std::cout << "Date: " << date.year << " " << date.month << " " << date.day << " " << date.hour << ":" << date.minute << std::endl;
	std::cout << "Date: " << date << std::endl;
	std::cout << "Message-ID: " << id << std::endl;
	std::cout << "To: " << to << std::endl;
	std::cout << "content, subject:\n";
	for(auto& word : content_words) std::cout << word << " "; std::cout << std::endl;
	std::cout << "\n";	
	
}

Mail::Mail() : alphNum_cnt(0){
	

	// std::cout << "contructing Mail\n";
	

	//set [from]
	inFileHandler.ignore(6);
	inFileHandler >> from;
	from = string_tolower(from);//?

	//set [date]
	int year, month, day, hour, minute;
	inFileHandler.ignore(6);
	//inFileHandler >> date.day >> Mail_construct::str_month;
	inFileHandler >> day >> Mail_construct::str_month;
	month = monthConverter(Mail_construct::str_month);
	inFileHandler >> year;
	inFileHandler.ignore(3);
	inFileHandler >> Mail_construct::str_hr_min;
	char* colomn = strchr(Mail_construct::str_hr_min, ':');
	*colomn = '\0';
	colomn++;
	hour = atoi(Mail_construct::str_hr_min);
	minute = atoi(colomn);
	date = (unsigned long long int)year*100000000 + month*1000000 + day*10000 + hour *100 + minute;

	//set id
	inFileHandler.ignore(12);
	inFileHandler >> id;

	//set subject
	inFileHandler.ignore(1);
	std::getline(inFileHandler, Mail_construct::str_subject, '\n');
	int start = 9;
    int end = 9;
    while(true){ //could get better performer
		start = find_first_alnum(Mail_construct::str_subject, end);
        // start = Mail_construct::str_subject.find_first_of(alph, end);
        if(start == std::string::npos)break;
		end = find_first_not_alnum(Mail_construct::str_subject, start);
        // end = Mail_construct::str_subject.find_first_not_of(alph, start);
		
        if(end == std::string::npos){
            // end = Mail_construct::str_subject.length(); 
			std::string tmp(Mail_construct::str_subject.substr(start));
			//content_words.insert(std::move(string_tolower(tmp)));
			content_words.insert(string_tolower(tmp));
			//globeVar::add_words(tmp);
            break;
        }
		std::string tmp(Mail_construct::str_subject.substr(start, end - start));
		content_words.insert(string_tolower(tmp));
		//globeVar::add_words(tmp);
    }

	//set to
	inFileHandler.ignore(4);
	inFileHandler >> to;
	to = string_tolower(to);//?

	//set content
	inFileHandler.ignore(9);
	std::getline(inFileHandler, Mail_construct::str_content, '\0');
	start = 0;
	end = 0;
	 while(true){ //could get better performer
		start = find_first_alnum(Mail_construct::str_content, end);
        // start = Mail_construct::str_content.find_first_of(alph, end);
        if(start == std::string::npos)break;
		end = find_first_not_alnum(Mail_construct::str_content, start);
        // end = Mail_construct::str_content.find_first_not_of(alph, start);
        if(end == std::string::npos){
            // end = Mail_construct::str_subject.length(); 
			std::string tmp(Mail_construct::str_content.substr(start));
			std::unordered_set<std::string>::iterator iter;
			bool s;
			std::tie(iter, s) = content_words.insert(std::move(string_tolower(tmp)));
			alphNum_cnt += iter->length();
			//globeVar::add_words(*iter);
            break;
        }
		std::string tmp(Mail_construct::str_content.substr(start, end - start));
		std::unordered_set<std::string>::iterator iter;
		bool s;
		std::tie(iter, s) = content_words.insert(std::move(string_tolower(tmp)));
		alphNum_cnt += iter->length();
		//globeVar::add_words(*iter);
    }

}
