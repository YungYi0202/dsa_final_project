#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_set>
#include <iostream>


namespace globeVar{
	std::map<std::string, int> all_words;
    void add_words(std::string str){
        if(all_words.count(str)>0) all_words[str]++;
        else all_words[str]=1;
    }
    void add_words(std::unordered_set<std::string> words){
        for(auto& word: words){
            if(all_words.count(word)>0) all_words[word]++;
            else all_words[word]=1;
        }
    }
    void remove_words(std::unordered_set<std::string> words){
        for(auto& word: words){
            all_words[word]--;
        }
    }
    void print(){
	    std::cout << "=========================================\n";
        printf("globeVar::all_words:\n");
        for(auto& word: all_words){
            std::cout << word.first << " " << word.second <<std::endl;    
        }
	    std::cout << "=========================================\n";

    }

};
