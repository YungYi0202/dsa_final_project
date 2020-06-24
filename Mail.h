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
#include"string_ulti.h"
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
	static int output_row_length;
	std::string raw_mail;
	std::string check_mail;

public:
	Mail();
	void print();
	void print_full_mail();
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

int Mail::output_row_length = 60;

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
	std::getline(inFileHandler, raw_mail, '\0');
	std::stringstream in_str(raw_mail);
	check_mail.clear();

	//set [from]
	in_str.ignore(6);
	in_str >> from;
	from = string_tolower(from);//?

	//set [date]
	int year, month, day, hour, minute;
	in_str.ignore(6);
	//in_str >> date.day >> Mail_construct::str_month;
	in_str >> day >> Mail_construct::str_month;
	month = monthConverter(Mail_construct::str_month);
	in_str >> year;
	in_str.ignore(3);
	in_str >> Mail_construct::str_hr_min;
	char* colomn = strchr(Mail_construct::str_hr_min, ':');
	*colomn = '\0';
	colomn++;
	hour = atoi(Mail_construct::str_hr_min);
	minute = atoi(colomn);
	date = (unsigned long long int)year*100000000 + month*1000000 + day*10000 + hour *100 + minute;

	//set id
	in_str.ignore(12);
	in_str >> id;

	//set subject
	in_str.ignore(1);
	std::getline(in_str, Mail_construct::str_subject, '\n');
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
			check_mail.append(string_tolower(tmp));
			//globeVar::add_words(tmp);
            break;
        }
		std::string tmp(Mail_construct::str_subject.substr(start, end - start));
		content_words.insert(string_tolower(tmp));
		check_mail.append(string_tolower(tmp));
		//globeVar::add_words(tmp);
    }

	//set to
	in_str.ignore(4);
	in_str >> to;
	to = string_tolower(to);//?

	//set content
	in_str.ignore(9);
	std::getline(in_str, Mail_construct::str_content, '\0');
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
			check_mail.append(string_tolower(tmp));
			std::tie(iter, s) = content_words.insert(std::move(string_tolower(tmp)));
			alphNum_cnt += iter->length();
			//globeVar::add_words(*iter);
            break;
        }
		std::string tmp(Mail_construct::str_content.substr(start, end - start));
		std::unordered_set<std::string>::iterator iter;
		bool s;
		check_mail.append(string_tolower(tmp));
		std::tie(iter, s) = content_words.insert(std::move(string_tolower(tmp)));
		alphNum_cnt += iter->length();
		//globeVar::add_words(*iter);
    }

}

void Mail::print_full_mail(){
	// std::cout << raw_mail;
	std::stringstream in_str(raw_mail);
	std::string row;
	for(int i = 0; i < output_row_length + 2; i++) std::cout << "="; std::cout << std::endl;
	while(std::getline(in_str, row, '\n'))
	{
		while(row.size() > output_row_length){
			std::cout << "*" << row.substr(0, output_row_length) << "*\n";
			row = row.substr(output_row_length);
		}
		std::cout << "*" << row;
		for(int i = 0; i < output_row_length - row.size(); i++) std::cout << " "; std::cout << "*\n";
	}
	for(int i = 0; i < output_row_length+2; i++) std::cout << "="; std::cout << std::endl;
	
}
