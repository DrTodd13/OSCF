#include "DatFile.h"

std::string trim(const std::string& str, const std::string& whitespace)
{
	const size_t strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	const size_t strEnd = str.find_last_not_of(whitespace);
	const size_t strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

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
	p.first_name = read_fixed(in, 25);   // 26
	p.school_code = read_fixed(in, 4);   // 51
	p.grade = read_fixed_char(in, 2);    // 55
	p.id = read_fixed(in, 6);            // 57
	p.nwsrs_rating = read_fixed_unsigned(in, 5); // 63
	p.num_games = read_fixed_unsigned(in, 5);    // 68
	p.games_ytd = read_fixed_unsigned(in, 5);    // 73
	p.highest_rating_this_year = read_fixed_unsigned(in, 6); // 78
	p.lowest_rating = read_fixed_unsigned(in, 6);            // 84
	p.start_rating = read_fixed_unsigned(in, 6);             // 90
	p.kirks_code = read_fixed_unsigned(in, 5);               // 96
	p.last_action_date = read_fixed(in, 9);                  // 101
	p.rating_basis = read_fixed_char(in, 2);                 // 110
	p.uscf_last_name = read_fixed(in, 25);                   // 112
	p.uscf_first_name = read_fixed(in, 25);                  // 137
	p.uscf_id = read_fixed(in, 9);                           // 162
	p.uscf_exp_date = read_fixed(in, 9);                     // 171
	p.uscf_rating = read_fixed_unsigned(in, 6);              // 180
	p.uscf_prov_codes = read_fixed(in, 5);                   // 186
	p.uscf_rating_date = read_fixed(in, 9);                  // 191
	p.state = read_fixed(in, 3);                             // 200
	char c;
	//std::cout << "New line code is " << (int)'\n' << std::endl;
	do {
		in.get(c);
		//std::cout << "Found char " << c << " " << (int)c << std::endl;
	} while (c != '\n' && !in.eof());

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

