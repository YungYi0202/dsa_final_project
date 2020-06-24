#pragma once
#include<string>
#include<cstring>

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

std::string sentence_to_expression(const std::string& sentence){
	std::string expression;
	int start = 0;
	int end = 0;
	while(true){
		start = find_first_alnum(sentence, end);
        if(start == std::string::npos)break;
		end = find_first_not_alnum(sentence, start);
		
        if(end == std::string::npos){
			std::string tmp(sentence.substr(start));
			expression.append(string_tolower(tmp));
			expression.append("&");
            break;
        }
		std::string tmp(sentence.substr(start, end - start));
		expression.append(string_tolower(tmp));
		expression.append("&");
    }
	expression.pop_back();
	return expression;
}

std::string sentence_convert_check(const std::string& sentence){
	std::string expression;
	for(auto chr : sentence){
		if(isalnum(chr)){
			if(isalpha(chr)){
				expression.push_back(tolower(chr));
			}
			else expression.push_back(chr);
		}
	}
	return expression;
}