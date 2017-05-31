#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <algorithm>
#include <map>
#include <iomanip>
#include <sstream>
#include <set>
#include <sstream>
#include "DatFile.cpp"
#include <string.h>
#include <assert.h>

class indirect_sort {
public:
    const std::vector<Player> &players;

    indirect_sort(const std::vector<Player> &p) : players(p) {}
    bool operator() (unsigned i, unsigned j) { return (players[j].calc_highest_rating < players[i].calc_highest_rating); }
};

#define ESTABLISHED_RATING 15

class MyDate {
public:
    int month, day, year;
    bool good;

    MyDate(const char *in) {
	good = true;
        int len = strlen(in);
	if (len != 8) {
//            if (debug) {
//		    std::cout << "Bad date string " << in << std::endl;
//	    }
            good = false;
	}
	month = atoi(in);
	day = atoi(in+3);
	year = atoi(in+6);
	if (year < 100) {
	    year += 2000;
        }
    }

    bool operator<(const MyDate &rhs) const {
        if (year < rhs.year) return true;
        if (year > rhs.year) return false;
        if (month < rhs.month) return true;
        if (month > rhs.month) return false;
        if (day < rhs.day) return true;
        if (day > rhs.day) return false;
	return false;
    }
    friend std::ostream & operator<<(std::ostream &os, const MyDate &md);
};

std::ostream & operator<<(std::ostream &os, const MyDate &md) {
    os << md.month << "/" << md.day << "/" << md.year;
    return os;
}

void process_tourney_results(const std::string &file, std::vector<Player> &or_players, std::map<std::string, unsigned> &nwsrs_map, std::ofstream &logfile) {
    std::ifstream tfile(file);
    while(!tfile.eof()) {
        std::string fullid;
        unsigned start_rating, end_rating, total_games, games_this_event;
        tfile >> fullid >> start_rating >> end_rating >> total_games >> games_this_event; 
        if (tfile.eof()) break;

        unsigned games_at_start = total_games - games_this_event;

        logfile << "process " << fullid << " " << start_rating << " " << end_rating << " " << total_games << " " << games_this_event << " " << games_at_start << std::endl;
	assert(fullid.length() == 8);
	std::string id = fullid.substr(4,4);

        auto player_index = nwsrs_map.find(id);
        if (player_index == nwsrs_map.end()) {
            logfile << "Player id " << id << " not found in nwsrs_map." << std::endl;
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

void process_uscf_supp(const std::string &file, std::vector<Player> &or_players, std::map<std::string, unsigned> &uscf_map, std::set<std::string> &uscf_but_not_nwsrs, const MyDate &start_date, std::ofstream &logfile) {
    std::ifstream tfile(file);
    while(!tfile.eof()) {
        std::string id;
        unsigned rating;
        tfile >> id >> rating;
        if (tfile.eof()) break;

        auto player_index = uscf_map.find(id);

	logfile << "uscf supp " << id << " " << rating << std::endl;

        if (player_index == uscf_map.end()) {
            logfile << "Player id " << id << " not found in uscf_map." << std::endl;
            continue;
        }
        
        Player &this_player = or_players[player_index->second];

	logfile << "Player " << this_player.last_name << " " << this_player.first_name << " " << this_player.calc_highest_rating << " " << this_player.isAdult() << " " << this_player.isUSCFProv() << " " << this_player.uscf_exp_date << std::endl;

        if (this_player.isAdult()) continue;
        if (this_player.isUSCFProv()) continue;
        //if (this_player.isUSCFProvOrExpired()) continue;
	MyDate temp_date(this_player.uscf_exp_date.c_str());
	logfile << "exp date " << temp_date << " start date " << start_date << " " << (temp_date < start_date) << std::endl;
	if (temp_date < start_date) continue;

        if (rating > this_player.calc_highest_rating) {
	    logfile << "USCF supp rating higher than calc_highest_rating." << std::endl;
            // For now just remember if a player has USCF but no NWSRS games.
            if (this_player.calc_highest_rating == 0) {
                uscf_but_not_nwsrs.insert(this_player.id);                
            }
            this_player.calc_highest_rating = rating;
	    if (this_player.nwsrs_rating > this_player.calc_highest_rating) {
	        logfile << "nwsrs rating higher than calc." << std::endl;
                this_player.calc_highest_rating = this_player.nwsrs_rating;
	    }
        } else {
	    logfile << "USCF supp rating higher than calc_highest_rating." << std::endl;
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
    if (argc != 6 && argc != 7) {
        std::cerr << "Wrong number of command line arguments." << std::endl;
        std::cerr << "Correct is: " << argv[0] << " <rateXX-XXX.dat> <cumulative_tourney_results_file> <uscf_supplement> <start date MM/DD/YYYY> <end date MM/DD/YYYY>" << std::endl;
        exit(-1);
    }
    
    MyDate start_date(argv[4]);
    MyDate end_date(argv[5]);

    std::ifstream infile(argv[1]);
    Player p; 
    int i;
    std::vector<unsigned> or_by_grade[26];
    std::vector<Player> or_players;
    std::map<std::string, unsigned> nwsrs_map;
    std::map<std::string, unsigned> uscf_map;
    std::ofstream logfile("allstars.log");

    unsigned player_index = 0;

    logfile << "All Stars for date range " << start_date << " to " << end_date << std::endl;
    logfile << "Loading ratings data." << std::endl;

    while(!infile.eof()) {
        infile >> p;
        if (!infile.eof()) {
            if (p.state == "OR") {
                or_players.push_back(p);
                nwsrs_map.insert(std::pair<std::string,unsigned>(p.id, player_index));
                if (!p.uscf_id.empty()) {
                    uscf_map.insert(std::pair<std::string,unsigned>(p.uscf_id, player_index));
                }
		logfile << p.last_name << " " << p.first_name << " " << p.id << " " << p.uscf_id << " " << player_index << std::endl;
                ++player_index;
            }
        } 
    }

    logfile << "Done loading ratings data." << std::endl;

    process_tourney_results(argv[2], or_players, nwsrs_map, logfile);
    std::set<std::string> uscf_but_no_nwsrs;
    process_uscf_supp(argv[3], or_players, uscf_map, uscf_but_no_nwsrs, start_date, logfile);

    for (i = 0; i < or_players.size(); ++i) {
        int grade_index = or_players[i].grade - 'A';
        if (grade_index < 0 || grade_index > 13) {
           std::cerr << "Bad grade index for player " << or_players[i].last_name << " " << or_players[i].first_name << " grade code = " << or_players[i].grade << std::endl;
           exit(-1);
        }

        if (or_players[i].calc_highest_rating > 0) {
            or_by_grade[grade_index].push_back(i);
            logfile << "Adding " << or_players[i].last_name << " " << or_players[i].first_name << " " << or_players[i].id << " " << or_players[i].uscf_id << " " << i << " to grade index " << grade_index << std::endl;
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
        int num_players_on_list = 10;

        if (or_by_grade[i].size() < 10) {
            num_players_on_list = or_by_grade[i].size();
        }

        // By default there are 10 players on the list except for when there is a tie for 10th place.
        if (or_by_grade[i].size() > 10) {
            // Get the 10th players.
            Player &star10 = or_players[or_by_grade[i][9]];
            // For 11th position and onwards.
            for (j = 10; j < or_by_grade[i].size(); ++j) {
                Player &this_star = or_players[or_by_grade[i][j]];
                // If there is a tie with the 10th player then add an additional player to the list.
                if (this_star.calc_highest_rating == star10.calc_highest_rating) {
                    ++num_players_on_list;
                } else {
                    break;
                }
            }
        }

        bool in_tie = false;
        unsigned start_tie = 0;

        for (j = 0; j < num_players_on_list; ++j) {
            Player &star = or_players[or_by_grade[i][j]];
            std::string full_name = star.last_name + ", " + star.first_name;
            auto citer = codes.find(star.school_code);
            if(citer == codes.end()) {
               std::cerr << "Couldn't find school code entry for code " << star.school_code << std::endl;
               exit(-1);
            }
            std::string city = citer->second;
 
            std::stringstream position_str;

            // If there is another player on the list after us.
            if (j != num_players_on_list - 1) {
                Player &next_star = or_players[or_by_grade[i][j+1]];
                if (star.calc_highest_rating == next_star.calc_highest_rating) {
                    if(in_tie) {
                        position_str << "T" << start_tie;
                    } else {
                        in_tie = true;
                        start_tie = j+1;
                        position_str << "T" << start_tie;
                    }
                } else {
                    if(in_tie) {
                        in_tie = false;
                        position_str << "T" << start_tie;
                    } else {
                        position_str << j+1;
                    }
                }
            } else {
                if(in_tie) {
                    position_str << "T" << start_tie;
                } else {
                    position_str << j+1;
                }
            }

            std::cout << std::setw(4) << position_str.str() << " " << std::left << std::setw(30) << full_name << " " << std::setw(20) << city << std::right << std::setw(4) << star.calc_highest_rating;
            if (uscf_but_no_nwsrs.find(star.id) != uscf_but_no_nwsrs.end()) {
//                std::cout << " Player has USCF supplement but no NWSRS games.";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;
    }
    return 0;
}
