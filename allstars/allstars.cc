#include "DatFile.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <unistd.h>
#include <algorithm>
#include <map>
#include <iomanip>
#include <sstream>
#include <set>

int daysdiff(const std::string &in, const std::chrono::system_clock::time_point &now) {
    std::tm tm = {0,0,0,0,0,0,0,0,0,0,0};
    tm.tm_mon = atoi(in.substr(0,2).c_str()) - 1;
    tm.tm_mday = atoi(in.substr(3,2).c_str());
    tm.tm_year = 100 + atoi(in.substr(6,2).c_str());
//    std::cout << tm.tm_mon << " " << tm.tm_mday << " " << tm.tm_year << std::endl;

    std::time_t tt=mktime(&tm);
    std::chrono::system_clock::time_point then = std::chrono::system_clock::from_time_t(tt);
    int ret = std::chrono::duration_cast<std::chrono::hours>(now - then).count();
    ret /= 24; // convert to days
//    std::cout << "date = " << in << " day diff = " << ret << std::endl;
    return ret;
}

class indirect_sort {
public:
    const std::vector<Player> &players;

    indirect_sort(const std::vector<Player> &p) : players(p) {}
    bool operator() (unsigned i, unsigned j) { return (players[j].calc_highest_rating < players[i].calc_highest_rating); }
};

#define ESTABLISHED_RATING 15

void process_tourney_results(const std::string &file, std::vector<Player> &or_players, std::map<std::string, unsigned> &nwsrs_map) {
    std::ifstream tfile(file);
    while(!tfile.eof()) {
        std::string id;
        unsigned start_rating, end_rating, total_games, games_this_event;
        tfile >> id >> start_rating >> end_rating >> total_games >> games_this_event; 
        if (tfile.eof()) break;

        unsigned games_at_start = total_games - games_this_event;
        auto player_index = nwsrs_map.find(id);
        if (player_index == nwsrs_map.end()) {
//            std::cerr << "Player id " << id << " not found in nwsrs_map." << std::endl;
            continue;
        }
        
        Player &this_player = or_players[player_index->second];

        if (games_at_start >= ESTABLISHED_RATING) {
            if (start_rating > this_player.calc_highest_rating) {
                this_player.calc_highest_rating = start_rating;
            }
        } 
        if (total_games >= ESTABLISHED_RATING) {
            if (end_rating > this_player.calc_highest_rating) {
                this_player.calc_highest_rating = end_rating;
            }
        } 
    }
}

void process_uscf_supp(const std::string &file, std::vector<Player> &or_players, std::map<std::string, unsigned> &uscf_map, std::set<std::string> &uscf_but_not_nwsrs) {
    std::ifstream tfile(file);
    while(!tfile.eof()) {
        std::string id;
        unsigned start_rating;
        tfile >> id >> start_rating;
        if (tfile.eof()) break;

        auto player_index = uscf_map.find(id);
        if (player_index == uscf_map.end()) {
//            std::cerr << "Player id " << id << " not found in uscf_map." << std::endl;
            continue;
        }
        
        Player &this_player = or_players[player_index->second];

        if (this_player.isAdult()) continue;

        if (start_rating > this_player.calc_highest_rating) {
            // For now just remember if a player has USCF but no NWSRS games.
            if (this_player.calc_highest_rating == 0) {
                uscf_but_not_nwsrs.insert(this_player.getFullId());                
            }
            this_player.calc_highest_rating = start_rating;
        } 
    }
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

std::map<std::string, std::string> load_schoolcodes(void) {
    std::ifstream infile("allcodes.csv");
    std::map<std::string, std::string> ret;

//    ret.insert(std::pair<std::string, std::string>("HSO","Home-Schooled")); 

    while(!infile.eof()) {
        std::string whole_line;
        getline(infile, whole_line);
        if (infile.eof()) break;

//        std::cout << whole_line << std::endl;
        auto parts = split(whole_line, ',');
//        std::cout << parts[0] << " " << parts[3] << std::endl;
        ret.insert(std::pair<std::string, std::string>(parts[0],parts[3])); 
    }

    return ret;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Wrong number of command line arguments." << std::endl;
        std::cerr << "Correct is: " << argv[0] << " <rateXX-XXX.dat> <cumulative_tourney_results_file> <uscf_supplement>" << std::endl;
        exit(-1);
    }

    //std::ifstream infile("rate16-387.dat");
    std::ifstream infile(argv[1]);
    Player p; 
    int i;
    std::vector<unsigned> or_by_grade[26];
    std::vector<Player> or_players;
    std::map<std::string, unsigned> nwsrs_map;
    std::map<std::string, unsigned> uscf_map;

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    unsigned player_index = 0;

    while(!infile.eof()) {
        infile >> p;
        if (!infile.eof()) {
            if (p.state == "OR") {
                or_players.push_back(p);
                nwsrs_map.insert(std::pair<std::string,unsigned>(p.getFullId(), player_index));
                if (!p.uscf_id.empty()) {
                    uscf_map.insert(std::pair<std::string,unsigned>(p.uscf_id, player_index));
                }
                ++player_index;
            }
        } 
    }

    process_tourney_results(argv[2], or_players, nwsrs_map);
    std::set<std::string> uscf_but_no_nwsrs;
    process_uscf_supp(argv[3], or_players, uscf_map, uscf_but_no_nwsrs);

    for (i = 0; i < or_players.size(); ++i) {
        int grade_index = or_players[i].grade - 'A';
        if (grade_index < 0 || grade_index > 13) {
           std::cerr << "Bad grade index." << std::endl;
           exit(-1);
        }

        if (or_players[i].calc_highest_rating > 0) {
            or_by_grade[grade_index].push_back(i);
        }
    }

    indirect_sort isop(or_players);

    auto codes = load_schoolcodes();

    std::string grade_names[13] = { "Kindergarten",
                                    "1st Grade",
                                    "2nd Grade",
                                    "3rd Grade",
                                    "4th Grade",
                                    "5th Grade",
                                    "6th Grade",
                                    "7th Grade",
                                    "8th Grade",
                                    "Freshmen",
                                    "Sophmores",
                                    "Juniors",
                                    "Seniors" };
    //for(i = 0; i < or_players.size(); ++i) {
    //    std::cout << or_players[i] << std::endl;
    //}
    for(i = 12; i >= 0; --i) {
        std::sort(or_by_grade[i].begin(), or_by_grade[i].end(), isop);
        std::cout << grade_names[i] << std::endl;
        int j;
        for (j = 0; j < or_by_grade[i].size() && j < 10; ++j) {
            Player &star = or_players[or_by_grade[i][j]];
            std::string full_name = star.last_name + ", " + star.first_name;
            auto citer = codes.find(star.school_code);
            if(citer == codes.end()) {
               std::cerr << "Couldn't find school code entry for code " << star.school_code << std::endl;
               exit(-1);
            }
            std::string city = citer->second;
 
            std::cout << std::setw(2) << j+1 << " " << std::left << std::setw(30) << full_name << " " << std::setw(20) << city << std::right << std::setw(4) << star.calc_highest_rating;
            if (uscf_but_no_nwsrs.find(star.getFullId()) != uscf_but_no_nwsrs.end()) {
                std::cout << " Player has USCF supplement but no NWSRS games.";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;
    }
    return 0;
}
