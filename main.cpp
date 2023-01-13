#define FILE_EXTENSION ".txt"
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#define ASIZE 26
#define DEBUG 0
using namespace std;
namespace fs = std::filesystem;

enum Mode { Exact, Prefix, Suffix };

string what_mode[3] = {"Exact", "Prefix", "Suffix"};

struct pattern{
    string str;
    int mode;
    pattern(){
        mode = -1;
    }
    pattern(string s, int m){
        str = s;
        mode = m;
    }

    bool operator<(const pattern rhs) const{
        return mode == rhs.mode ? str < rhs.str : mode < rhs.mode;
    }
};

// important map!
map<pattern, set<int>> patterns_2_title_id;
vector<string> txt_id_2_name;

struct AC {
    struct node{
        int next[ASIZE];
        set<string> end_str;
        node(){
            fill(next, next + ASIZE, -1);
        }
    };

    vector<node> trie;
    int root, current;

    AC(){
        trie.push_back(node());
    }

    //AC(const vector<string>& words){
        //build_trie(words);
        //dfs_print_trie();
        //build_fail_ptr();
    //}

    void add_trie(const string& word){
        build_trie(word);
    }

    void finish_trie(){
        //if(DEBUG){
            //dfs_print_trie();
            //cout << '\n';
        //}
        build_fail_ptr();
    }

    void build_trie(const string& word){
        root = 0;

        int now = root;
        for(char c : word){
            int offset = c - 'a';
            if(trie[now].next[offset] == -1){
                int next_idx = trie.size();
                trie[now].next[offset] = next_idx;
                trie.push_back(node());
            }
            now = trie[now].next[offset];
        }
        trie[now].end_str.insert(word);
    }

    void dfs_print_trie(int idx = 0, int level = 0){
        for(int i=0; i<ASIZE; i++){
            int next_idx = trie[idx].next[i];
            if(next_idx != -1){
                char c = 'a' + i;
                cout << string(level, ' ') << c;
                if(trie[next_idx].end_str.size()){
                    cout << " -> ";
                    for(string end_s : trie[next_idx].end_str){
                        cout << end_s << ' ';
                    }
                }
                cout << '\n';
                dfs_print_trie(next_idx, level + 1);
            }
        }
    }

    void build_fail_ptr(){
        queue<pair<int, int>> que;

        for(int i=0; i<ASIZE; i++){
            if(trie[root].next[i] == -1){
                trie[root].next[i] = root;
            }
            else{
                que.push({root, trie[root].next[i]});
            }
        }

        while(!que.empty()){
            auto [now, nxt] = que.front();
            que.pop();

            if(trie[now].end_str.size()){
                for(auto it = trie[now].end_str.begin(); it != trie[now].end_str.end(); it++){
                    trie[nxt].end_str.insert(*it);
                }
            }

            for(int i=0; i<ASIZE; i++){
                int a = trie[now].next[i];
                int b = trie[nxt].next[i];
                if(b == -1){
                    trie[nxt].next[i] = a;
                }
                else{
                    que.push({a, b});
                }
            }
        }
        current = 0;
    }

    void look_for(const string& word){
        cout << "look for: " << word << '\n';
        int now = 0;
        for(char c : word){
            int offset = c - 'a';
            now = trie[now].next[offset];
            cout << now << '\n';
        }
        cout << "----------------------------\n";
    }

    void query(const string& word, int txt_id){
        int now = 0;
        int len = word.size();
        for(int i=0; i<len; i++){
            char c = word[i];
            int offset = c - 'a';
            now = trie[now].next[offset];
            // check terminal
            if(trie[now].end_str.size()){ // check mode
                for(string end_s : trie[now].end_str){
                    int end_s_len = end_s.size();
                    // Exact
                    if(len == end_s_len){
                        patterns_2_title_id[{end_s, Exact}].insert(txt_id);
                    }
                    // Prefix
                    if(i == end_s_len-1){
                        patterns_2_title_id[{end_s, Prefix}].insert(txt_id);
                    }
                    // Suffix
                    if(i == len-1){
                        patterns_2_title_id[{end_s, Suffix}].insert(txt_id);
                    }
                }

            }
        }
    }
};

// Utility Func

// string parser : output vector of strings (words) after parsing
vector<string> word_parse(const vector<string>& tmp_string){
	vector<string> parse_string;
	for(auto& word : tmp_string){
		string new_str;
    	for(auto &ch : word){
			if(isalpha(ch))
				new_str.push_back(tolower(ch)); // lowercase ch
		}
		parse_string.emplace_back(new_str);
	}
	return parse_string;
}

vector<pattern> process_query(const vector<string>& tmp_string){
    vector<pattern> parse_string;
    for(auto& word : tmp_string){
		string new_str;
		int mode;

    	for(auto &ch : word){
			if(isalpha(ch))
				new_str.push_back(tolower(ch)); // lowercase ch
		}

		if(word[0] == '"'){
            mode = Exact;
		}
		else if(word[0] == '*'){
            mode = Suffix;
		}
		else{
            mode = Prefix;
		}

		parse_string.emplace_back(pattern(new_str, mode));
	}
	return parse_string;
}

pattern make_pattern(const string& word){
    string new_str;
    int mode;

    for(auto &ch : word){
        if(isalpha(ch))
            new_str.push_back(tolower(ch)); // lowercase ch
    }

    if(word[0] == '"'){
        mode = Exact;
    }
    else if(word[0] == '*'){
        mode = Suffix;
    }
    else{
        mode = Prefix;
    }

    return pattern(new_str, mode);
}

vector<string> split(const string& str, const string& delim) {
	vector<string> res;
	if("" == str) return res;
	//先將要切割的字串從string型別轉換為char*型別
	char * strs = new char[str.length() + 1] ; //不要忘了
	strcpy(strs, str.c_str());

	char * d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while(p) {
		string s = p; //分割得到的字串轉換為string型別
		res.push_back(s); //存入結果陣列
		p = strtok(NULL, d);
	}

	return res;
}

int main(int argc, char *argv[])
{	
    string data_dir = "./data-more/";
    string query = "./query_more.txt";
    string output;

    data_dir = argv[1] + string("/");
    query = string(argv[2]);
    output = string(argv[3]);
    int data_dir_num = distance(fs::directory_iterator(data_dir), fs::directory_iterator{});

	string file, title_name, tmp;
	fstream fi, fo;
	vector<string> tmp_string;

    // process query, AC, and map<pattern -> set<title id>>
    AC ac_trie;

    fi.open(query, ios::in);

    while(getline(fi, tmp)){
        tmp_string = split(tmp, " +/");
        vector<pattern> one_query_patterns = process_query(tmp_string);
        for(pattern p : one_query_patterns){
            ac_trie.add_trie(p.str);
        }
    }

    ac_trie.finish_trie();

    fi.close();

    // start data search
    for(int txt_id=0; txt_id<data_dir_num; txt_id++){
        fi.open(data_dir + to_string(txt_id) + ".txt", ios::in);

        // GET TITLENAME
        getline(fi, title_name);

        // insert in title_id_2_name
        txt_id_2_name.push_back(title_name);

        // GET TITLENAME WORD ARRAY
        tmp_string = split(title_name, " ");

        vector<string> title = word_parse(tmp_string);

        for(auto &word : title){
            ac_trie.query(word, txt_id);
        }

        // GET CONTENT LINE BY LINE
        while(getline(fi, tmp)){

            // GET CONTENT WORD VECTOR
            tmp_string = split(tmp, " ");

            // PARSE CONTENT
            vector<string> content = word_parse(tmp_string);

            for(auto &word : content){
                ac_trie.query(word, txt_id);
            }
            //......
        }

        // CLOSE FILE
        fi.close();
    }
    // end of data search

    // eat query again and make output
    fi.open(query, ios::in);
    fo.open(output, ios::out);

    while(getline(fi, tmp)){
        tmp_string = split(tmp, " ");

        pattern pat = make_pattern(tmp_string[0]);

        set<int> ans(patterns_2_title_id[pat]);

        int len = tmp_string.size();

        for(int i=2; i<len; i++){
            if(i % 2 == 0){ // calculate when meet a query st;
                pat = make_pattern(tmp_string[i]);
                set<int> new_ans;
                if(tmp_string[i-1] == string("/")){ // 聯集
                    // ans /= patterns_2_title_id[pat];
                    set_union(ans.begin(), ans.end(),
                                     patterns_2_title_id[pat].begin(), patterns_2_title_id[pat].end(),
                                     inserter(new_ans, new_ans.begin()));
                }
                else if(tmp_string[i-1] == string("+")){ // 交集
                    // ans += patterns_2_title_id[pat];
                    set_intersection(ans.begin(), ans.end(),
                                     patterns_2_title_id[pat].begin(), patterns_2_title_id[pat].end(),
                                     inserter(new_ans, new_ans.begin()));
                }
                swap(ans, new_ans); // instead of ans = new_ans;
            }
        }

        if(ans.empty()){
            fo << "Not Found!\n";
        }
        for(auto it=ans.begin(); it!=ans.end(); it++){
            fo << txt_id_2_name[*it] << '\n';
        }
    }

    fi.close();
    fo.close();
}


// 1. UPPERCASE CHARACTER & LOWERCASE CHARACTER ARE SEEN AS SAME.
// 2. FOR SPECIAL CHARACTER OR DIGITS IN CONTENT OR TITLE -> PLEASE JUST IGNORE, YOU WONT NEED TO CONSIDER IT.
//    EG : "AB?AB" WILL BE SEEN AS "ABAB", "I AM SO SURPRISE!" WILL BE SEEN AS WORD ARRAY AS ["I", "AM", "SO", "SURPRISE"].
// 3. THE OPERATOR IN "QUERY.TXT" IS LEFT ASSOCIATIVE
//    EG : A + B / C == (A + B) / C

//

//////////////////////////////////////////////////////////
