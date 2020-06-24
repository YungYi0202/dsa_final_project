#include <fstream>
#include <string>
#include<iostream>
#include <algorithm>
#include"Mail.h"
#include"Treap.h"
//#include"Timer.h"
#include"Query_related.h"
#define DEBUG 0

//for main.cpp add()
std::ifstream inFileHandler;
std::string fileName;

//for main.cpp query()
bool flag_from , flag_to , flag_date ;
std::string sender, receiver;
unsigned long long int start_t , end_t ;
std::vector<std::string> postfix ;

//all the mail's data is in here
std::list<Mail> all_mails;

//reference
Treap mail_treap = Treap();

//necessary
void add();
void remove();
void longest();
void query();
//additional
void information();

int main(){
	#if DEBUG == 1
	Timer main_timer("main function");
	#endif

	std::string command;
	while(std::cin >> command){
        if(command == "add") add();
        else if(command == "remove") remove();
        else if(command == "longest") longest();
        else if(command == "query") query();
		else if(command == "information") information();
        //to be added...
    }


	return 0;
}

void add(){
	#if DEBUG == 1
	std::cout << "=========================================\n";
	std::cout << "handling add command\n";
	#endif

	//create Mail data type
	std::cin >> fileName;
	inFileHandler.open(fileName);
	all_mails.emplace_front(Mail());// pass inFileHandler
	inFileHandler.close();

	#if DEBUG == 1
	all_mails.front().print();
	#endif

	//insert iterator to treap
	if(mail_treap.insert(all_mails.begin())){
		//insert successful
		std::cout << mail_treap.Num() << std::endl;
		//do other things here
		
		auto this_mail_ptr = all_mails.begin();
		
		Query_related::dict_add(this_mail_ptr);
		
		//update the statistic in globeVar::all_words
		globeVar::add_words(this_mail_ptr->getContent());
	}
	else{
		//insert fail
		all_mails.erase(all_mails.begin());
		std::cout << "-\n";	
		//do other things here

	}
	#if DEBUG == 1
	mail_treap.Print();
	std::cout << "=========================================\n";
	#endif
}

void remove(){
	#if DEBUG == 1
	std::cout << "=========================================\n";
	std::cout << "handling remove command\n";
	#endif

	int target_id;
	std::cin >> target_id;
	try{
		auto remove_iter = mail_treap.remove(target_id);
		//remove from treap successful
		std::cout << mail_treap.Num() << std::endl;
		//do other things here
		Query_related::dict_remove(remove_iter);
		
		//update the statistic in globeVar::all_words
		globeVar::remove_words(remove_iter->getContent());

		//remove link list
		all_mails.erase(remove_iter);
	}
	catch(std::logic_error e){
		//remove from treap fail
		std::cout << "-\n";
	}

	#if DEBUG == 1
	mail_treap.Print();
	std::cout << "=========================================\n";
	#endif
}

void longest(){
	#if DEBUG == 1
	std::cout << "=========================================\n";
	std::cout << "handling longest command\n";
	#endif

	if(mail_treap.empty()){
		std::cout << "-\n";
	}
	else{
		std::cout << mail_treap.getMax()->getid() << " " << mail_treap.getMax()->get_alphNum_cnt()  << std::endl;
		//std::cout << mail_treap.getMax()->getid() << " " << mail_treap.getMax()->get_alphNum_cnt() << std::endl;
	}

	#if DEBUG == 1
	std::cout << "=========================================\n";
	#endif
}

void query(){
	//extern variable
	flag_from = 0 ; flag_to = 0 ; flag_date = 0 ;
	start_t = 0; end_t = 300000000000;	
	
	std::string expression;

	//flags getter
	while(cin >> expression){
		int dash = expression.find_first_of('-');
		if( dash == std::string::npos) break;
		if(expression.at(dash+1) == 'f'){
			flag_from = true;
			sender = expression.substr(3,expression.length()-4);
			sender = string_tolower(sender);
		}
		else if(expression.at(dash+1) == 't'){
			flag_to = true;
			receiver = expression.substr(3,expression.length()-4);
			receiver = string_tolower(receiver);
		}
		else if(expression.at(dash+1) == 'd'){
			flag_date = true;
			if(expression.at(2) == '~'){
				end_t = stoull(expression.substr(3,12));
			}
			else if(expression.at(expression.length()-1) == '~'){
				start_t = stoull(expression.substr(2,12));
			}
			else{
				start_t = stoull(expression.substr(2,12));
				end_t = stoull(expression.substr(15,12));
			}
		}
	}

	//check whether the flag is valid
	if(flag_from && Query_related::from_dict.count(sender) == 0){
		printf("-\n");
		return;
	}
	if(flag_to && Query_related::to_dict.count(receiver) == 0){
		printf("-\n");
		return;
	}

	// make postfix and find valid_ids
    Query_related::makePostfix(expression);
    #if DEBUG == 1
	Query_related::postfix_print();
	#endif

	if(flag_from || flag_to){
		if(flag_from && flag_to){
			if(Query_related::from_dict[sender].size() < Query_related::to_dict[receiver].size())
				Query_related::query_conduct_from();
			else 
				Query_related::query_conduct_to();
		}
		else if(flag_from) Query_related::query_conduct_from();
		else Query_related::query_conduct_to();
	}
	else{// deal with no flags query
		std::vector<int> valid_id;
		for( auto& mail_curr : all_mails ){ // unordered_set
			if(flag_date && !(start_t <= mail_curr.getDate() && mail_curr.getDate()<=end_t )) continue;
			std::stack<bool> Container;
			for(auto& token: postfix){
				if( isalnum(token[0]) ){
					Container.push( mail_curr.getContent().count(token)>0 );
				}
				else{//is operator
					if(token == "!"){//may have bugs
						Container.top() = !(Container.top());
					}
					else{
						bool a,b;
						a = Container.top(); Container.pop();
						b = Container.top(); Container.pop();

						if( token == "&") Container.push(a&&b);
						else Container.push(a||b);
					}
				}
			}
			if(Container.size() != 1) printf("Error happened.\n");
			else if(Container.top() == true) valid_id.emplace_back(mail_curr.getid());
		}

		//print
		if(valid_id.size() == 0) printf("-\n");
		else{
			sort(valid_id.begin(), valid_id.end());
			printf("%d", valid_id[0]);
			for(int i = 1; i < valid_id.size(); i++){
				printf(" %d",valid_id[i]);
			}
			printf("\n");
		}	
	}	
}

void information(){
	std::string name;
	std::cin >> name;
	// (1) The user sends most mail to "name" :
	// (2) The user receives most mail from "name" :
	// (3) The latest mail ID and date in "name"'s mailbox: 
	unsigned long long int latest_time = 0;
	int latest_ID;
	// (1)
	std::map<std::string, int> times_cnt;
	std::vector<std::string> top1;
	std::cout << "The user sends most mail to " << name << " : ";
	if( Query_related::to_dict[name].size() == 0) std::cout << "-\n";
	else{
		for(auto& mail_curr : Query_related::to_dict[name] ){
			std::string& Sender = mail_curr.second->getFrom();
			if(times_cnt.count(Sender) > 0) times_cnt[Sender]++;
			else times_cnt[Sender] = 1;
			//latest mail
			if(mail_curr.second->getDate() > latest_time){
				latest_time = mail_curr.second->getDate();
				latest_ID = mail_curr.second->getid();
			}
			//time_cnt
			if(top1.size()==0) top1.push_back(Sender);
			else if(times_cnt[Sender] > times_cnt[top1[0]]){
				top1.clear();
				top1.push_back(Sender);
			}
			else if(times_cnt[Sender] == times_cnt[top1[0]]){
				top1.push_back(Sender);
			}
		}
		for(auto& user:top1){
			std::cout << user << " ";
		}
		std::cout << std::endl;
	}
	
	// (2)
	times_cnt.clear();
	top1.clear();
	std::cout << "The user receives most mail from " << name << " : ";
	if( Query_related::from_dict[name].size() == 0) std::cout << "-\n";
	else{
		for(auto& mail_curr : Query_related::from_dict[name] ){
			std::string& Receiver = mail_curr.second->getTo();
			if(times_cnt.count(Receiver) > 0) times_cnt[Receiver]++;
			else times_cnt[Receiver] = 1;
			//latest mail
			if(mail_curr.second->getDate() > latest_time){
				latest_time = mail_curr.second->getDate();
				latest_ID = mail_curr.second->getid();
			}
			//time_cnt
			if(top1.size()==0) top1.push_back(Receiver);
			else if(times_cnt[Receiver] > times_cnt[top1[0]]){
				top1.clear();
				top1.push_back(Receiver);
			}
			else if(times_cnt[Receiver] == times_cnt[top1[0]]){
				top1.push_back(Receiver);
			}
		}
		for(auto& user:top1){
			std::cout << user << " ";
		}
		std::cout << std::endl;
	}
	// (3)
	std::cout << "The latest mail date and ID in " << name <<"'s mailbox : ";
	if(latest_time == 0) std::cout << "-\n";
	else{
		std::cout << latest_time/100000000 << "/";
		latest_time %= 100000000;
		printf("%02d/", int(latest_time/1000000));
		latest_time %= 1000000;
		printf("%02d ", int(latest_time/10000));
		latest_time %= 10000;
		printf("%02d:%02d", int(latest_time/100),int(latest_time%100));
		std::cout << " " << latest_ID << "\n";
	}
}
