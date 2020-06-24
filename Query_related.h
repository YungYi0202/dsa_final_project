#include<vector>
#include<stack>
#include<set>
#include<string>
#include<map>
#include<list>
#include"Mail.h"
#define DEBUG 0
typedef std::list<Mail>::iterator Elemt_Iter;//to be rewrited to template

//for main.cpp query()
extern bool flag_from , flag_to , flag_date ;
extern std::string sender, receiver;
extern unsigned long long int start_t , end_t ;
extern std::vector<std::string> postfix ;

namespace Query_related{
    void postfix_print(){
        std::cout <<"postfix: ";
        for(auto& curr : postfix){
            std::cout << curr << " ";
        }
        std::cout << std::endl;
    }
    //Optimization==================================
    std::map<std::string, std::map<int, Elemt_Iter> > from_dict;    
    std::map<std::string, std::map<int, Elemt_Iter> > to_dict;    
    void dict_add(Elemt_Iter& mail){
        from_dict[mail->getFrom()][mail->getid()] = mail;
        to_dict[mail->getTo()][mail->getid()] = mail;
    }
    void dict_remove(Elemt_Iter& mail){
        from_dict[mail->getFrom()].erase(mail->getid());
        to_dict[mail->getTo()].erase(mail->getid());
        if(from_dict[mail->getFrom()].size()==0){
            from_dict.erase(mail->getFrom());
        }
        if(to_dict[mail->getTo()].size()==0){
            to_dict.erase(mail->getTo());
        }
    }
    void query_conduct_from(std::vector<Mail*>& out_mail_ptrs){
        #if DEBUG ==1
        postfix_print();
        #endif

        bool print_start = false;
        for( auto& mail_curr : from_dict[sender] ){ // unordered_set
			if(flag_to && mail_curr.second->getTo()!= receiver) continue;
            if(flag_date && !(start_t <= mail_curr.second->getDate() && mail_curr.second->getDate()<=end_t )) continue;
            std::stack<bool> Container;
            for(auto& token: postfix){
                if( isalnum(token[0]) ){
                    Container.push( mail_curr.second->getContent().count(token)>0 );
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
            else if(Container.top() == true){
				out_mail_ptrs.push_back(&(*(mail_curr.second)));

                // if(print_start) printf(" %d", mail_curr.first);
                // else{
                //     printf("%d", mail_curr.first);
                //     print_start = true;
                // }
            } 
        }
        // if(print_start) printf("\n");
        // else printf("-\n"); 
    }
    void query_conduct_to(std::vector<Mail*>& out_mail_ptrs){
        #if DEBUG ==1
        postfix_print();
        #endif


        bool print_start = false;
        for( auto& mail_curr : to_dict[receiver] ){ // unordered_set
			if(flag_from && mail_curr.second->getFrom()!= sender) continue;
            if(flag_date && !(start_t <= mail_curr.second->getDate() && mail_curr.second->getDate()<=end_t )) continue;
            std::stack<bool> Container;
            for(auto& token: postfix){
                if( isalnum(token[0]) ){
                    Container.push( mail_curr.second->getContent().count(token)>0 );
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
            else if(Container.top() == true){
				out_mail_ptrs.push_back(&(*(mail_curr.second)));
                // if(print_start) printf(" %d", mail_curr.first);
                // else{
                //     printf("%d", mail_curr.first);
                //     print_start = true;
                // }
            } 
        }
        // if(print_start) printf("\n");
        // else printf("-\n"); 
    }
    void debug_print(std::string& name){
        std::cout << "from_dict:[" << name << "]: ";
        for(auto&curr : from_dict[name]){
            std::cout << curr.first << " ";
        }
        std::cout << std::endl;
        std::cout << "to_dict:[" << name << "]: ";
        for(auto&curr : to_dict[name]){
            std::cout << curr.first << " ";
        }
        std::cout << std::endl;
    }

    //Wildcard===============================
    bool wildcard;
    //s could be empty and contains only lowercase letters a-z.
    //p could be empty and contains only lowercase letters a-z, and characters like ? or *.
    bool isMatch(std::string s, std::string& p) {
        //Dynamic Programming
        std::vector<std::vector<bool> > dp(p.size() + 1, std::vector<bool>(s.size() + 1, false));
        dp[0][0] = true;
        for(int i = 1; i <= p.size(); i++){
            if(p[i-1]=='*' && dp[i-1][0]) dp[i][0] = true;
        }
        for(int i = 1; i <= p.size(); i++){
            for(int j = 1; j <= s.size(); j++){
                switch(p[i-1]){
                    case '*':
                        if(dp[i-1][j] || dp[i][j-1]) dp[i][j] = true;
                        break;
                    case '?':
                        if(dp[i-1][j-1]) dp[i][j] = true;
                        break;
                    default:
                        if(p[i-1] == s[j-1] && dp[i-1][j-1]) dp[i][j] = true;
                        break;
                }
                
            }
        }
        return dp.back().back();
    }
    void processMatching(std::string& card){//time-consuming! to be improved!!
        bool not_first = false;
        //printf("processMatching\n");
        for(auto& word: globeVar::all_words){
            //std::cout << word.first << " " << word.second <<std::endl;
            if(word.second <= 0) continue;
            if( isMatch(word.first, card) ){
                postfix.push_back(word.first);
                if(not_first) postfix.push_back("|");
                else not_first = true;
            }
        }
    }
    int isValidChar(const char& c){
        if(std::isalnum(c)) return 1;
        if(c == '?' || c == '*'){
            wildcard = true;
            return 2;
        }
        return 0;
    }
    //Basic==================================
    void makePostfix(std::string& str);
    std::string findWord(const std::string& str, int& index);
    std::string findOpr(const std::string& c, int& index);
    int oprValue(const std::string& c);
    
    std::size_t find_first_not_alnum(const std::string& str, std::size_t startPos){
        try{
            while(isValidChar(str.at(startPos)) >0){
                startPos++;
            }
        }
        catch(std::out_of_range e){
            return std::string::npos;
        }
        return startPos;
    }
    //copy from Mail.h
    std::string& string_tolower(std::string& str){
        for(char& chr : str){
            chr = tolower(chr);
        }
        return str;
    }
    // makePostfix======================================
    void makePostfix(std::string& str){
        postfix.clear();
        std::stack<std::string> postfix_opr;
        int index=0;
        while(index < str.length()){
            if(isalnum(str[index])) postfix.push_back(findWord(str,index));
            else{
                std::string curr = findOpr(str,index);
                if(curr == ")"){
                    while(postfix_opr.top()!= "("){
                        postfix.push_back(postfix_opr.top());
                        postfix_opr.pop();
                    }
                    postfix_opr.pop();// pop "("
                }
                else if(postfix_opr.empty()) postfix_opr.push(curr);
                else {
                    if(oprValue(curr) >= 18 && oprValue(postfix_opr.top()) <= oprValue(curr)){
                        postfix_opr.push(curr);
                    }
                    else if(oprValue(postfix_opr.top()) < oprValue(curr)) postfix_opr.push(curr);
                    else{
                        while(postfix_opr.top()!= "(" && oprValue(postfix_opr.top()) >= oprValue(curr)){
                            postfix.push_back(postfix_opr.top());
                            postfix_opr.pop();
                            if(postfix_opr.empty()) break;
                        }
                        postfix_opr.push(curr);
                    }
                }
            }
        }
        while(!postfix_opr.empty()){
            postfix.push_back(postfix_opr.top());
            postfix_opr.pop();
        }
    }
    std::string findWord(const std::string& str, int& index){
        int end = find_first_not_alnum(str, index);
        if(end == std::string::npos) end = str.length();
        std::string word = str.substr(index, end - index);
        index = end;//update index
        word = string_tolower(word);
        return word;
    }
    std::string findOpr(const std::string& c, int& index){
        index++;
        return c.substr(index - 1, 1);
    }
    int oprValue(const std::string& c){
        if(c == "(") return 20;
        else if(c == "!") return 18;//unary
        else if(c == "&") return 8;
        else if(c == "|") return 4;
        else return -1;//default
    }
    // End makePostfix======================================
};