#include <string>
#include <iostream>
#include <cstdlib>

std::string trim(const std::string& str,
                 const std::string& whitespace = " \t")
{
    const size_t strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const size_t strEnd = str.find_last_not_of(whitespace);
    const size_t strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

class Player {
public:
    std::string last_name;
    std::string first_name;
    std::string school_code;
    char grade;
    std::string id;
    unsigned nwsrs_rating;
    unsigned num_games;
    unsigned games_ytd;
    unsigned highest_rating_this_year;
    unsigned lowest_rating;
    unsigned start_rating;
    unsigned kirks_code;
    std::string last_action_date;
    char rating_basis;
    std::string uscf_last_name; 
    std::string uscf_first_name; 
    std::string uscf_id;
    std::string uscf_exp_date;
    unsigned uscf_rating;
    std::string uscf_prov_codes;
    std::string uscf_rating_date;
    std::string state;

    unsigned calc_highest_rating;

    friend std::ostream& operator<<(std::ostream &os, Player &p);
    friend std::istream& operator>>(std::istream &in, Player &p);

    unsigned get_higher_rating(void) const {
        return nwsrs_rating > uscf_rating ? nwsrs_rating : uscf_rating;
    }

    std::string getFullId(void) const {
        return school_code + grade + id;
    }

    bool isAdult(void) const {
        return grade == 'N';
    }

    Player(void) : grade('Z'), nwsrs_rating(0), num_games(0), games_ytd(0), highest_rating_this_year(0), lowest_rating(0), start_rating(0), kirks_code(0), rating_basis(' '), uscf_rating(0), calc_highest_rating(0) {}
};

std::string read_fixed(std::istream &in, unsigned length) {
    char buf[128] = "";
    in.read(buf, length); 
    return trim(std::string(buf));
}

char read_fixed_char(std::istream &in, unsigned length) {
    std::string temp = read_fixed(in, length);
    return temp[0];
}

unsigned read_fixed_unsigned(std::istream &in, unsigned length) {
    std::string temp = read_fixed(in, length);
    return std::atoi(temp.c_str());
}

std::istream& operator>>(std::istream &in, Player &p) {
    p.last_name = read_fixed(in, 25); 
    p.first_name = read_fixed(in, 25); 
    p.school_code = read_fixed(in, 4); 
    p.grade = read_fixed_char(in, 2);
    p.id = read_fixed(in, 6); 
    p.nwsrs_rating = read_fixed_unsigned(in, 5); 
    p.num_games = read_fixed_unsigned(in, 5); 
    p.games_ytd = read_fixed_unsigned(in, 5); 
    p.highest_rating_this_year = read_fixed_unsigned(in, 6); 
    p.lowest_rating = read_fixed_unsigned(in, 6); 
    p.start_rating = read_fixed_unsigned(in, 6); 
    p.kirks_code = read_fixed_unsigned(in, 5); 
    p.last_action_date = read_fixed(in, 9); 
    p.rating_basis = read_fixed_char(in, 2);
    p.uscf_last_name = read_fixed(in, 25); 
    p.uscf_first_name = read_fixed(in, 25); 
    p.uscf_id = read_fixed(in, 9); 
    p.uscf_exp_date = read_fixed(in, 9); 
    p.uscf_rating = read_fixed_unsigned(in, 6); 
    p.uscf_prov_codes = read_fixed(in, 5); 
    p.uscf_rating_date = read_fixed(in, 9); 
    p.state = read_fixed(in, 3);
    char c;
    //std::cout << "New line code is " << (int)'\n' << std::endl;
    do {
       in.get(c);
       //std::cout << "Found char " << c << " " << (int)c << std::endl;
    } while(c != '\n');
    
    return in;
}

std::ostream& operator<<(std::ostream &os, Player &p) {
    os << p.last_name << " ";
    os << p.first_name << " ";
    os << p.school_code << " ";
    os << p.grade << " ";
    os << p.id << " ";
    os << p.nwsrs_rating << " ";
    os << p.num_games << " ";
    os << p.games_ytd << " ";
    os << p.highest_rating_this_year << " ";
    os << p.lowest_rating << " ";
    os << p.start_rating << " ";
    os << p.kirks_code << " ";
    os << p.last_action_date << " ";
    os << p.rating_basis << " ";
    os << p.uscf_last_name << " ";
    os << p.uscf_first_name << " ";
    os << p.uscf_id << " ";
    os << p.uscf_exp_date << " ";
    os << p.uscf_rating << " ";
    os << p.uscf_prov_codes << " ";
    os << p.uscf_rating_date << " ";
    os << p.state;
    
    return os;
}

