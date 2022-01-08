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
#include "../include/tokenize_csv.h"

/*
1 First Name
2 Last Name
3 Email Address
4 Address 1
5 Address 2
6 City
7 State
8 ZIP Code
9 Phone
10 Cell Phone
11 "Every Adult Registrant MUST add at least one student player by using the ""Player Information"" section below.  Please add all associated players by clicking the ""ADD"" button.  You will not be able to modify the entries once registration is complete, so please don't click ""Register"" at the bottom until every student is listed.  Contact us with problems."
12 School
13 Grade
14 Playing Section?
15 "NWSRS Identification - Please look up this information as noted above; if your student is not listed, please enter ""NONE""; otherwise please enter their seven-digit ID number below."
16 "NWSRS Rating - Please find your student's rating by checking the web site above; if your student is unrated, please enter ""N/A"""
17 "USCF Membership ID (required for Open / 1300+ section - otherwise, leave blank).  Look up this information as noted above, and enter the 8-digit ID below."
18 "USCF Rating (required for Open / 1300+ section; otherwise, leave blank)",
19 Are you willing to Volunteer?  We could use your help!,
20 COMMENTS / QUESTIONS?
21 Player of First Name
22 Player of Last Name
23 Registration Date
24 Registration Status
25 Payment Status
26 Payment Type
27 Fee Type
28 Promo Code
29 Code Type
30 Discount Percent
31 Discount Amount
32 Adult Registrant Quantity
33 Adult Registrant Price
34 Scholastic Player Quantity
35 Scholastic Player Price
36 Scholastic Player (late registration) Quantity
37 Scholastic Player (late registration) Price
38 ...
*/

class ConstantContactEntry {
protected:
    std::vector<std::string> fields;
    void valid(void) const {
        if (fields.size() < 18) {
            std::cerr << "fields is not at least 18 long, it is " << fields.size() << " instead." << std::endl;
            exit(-1);
        }
    }
public:
    ConstantContactEntry(const std::vector<std::string> &f) : fields(f) { valid(); }

    std::string getFirstName(void)   const { valid(); return fields[0]; }
    std::string getLastName(void)    const { valid(); return fields[1]; }
    std::string getEmail(void)       const { valid(); return fields[2]; }
    bool didAdultCheck(void)         const { valid(); return !fields[10].empty(); }
    std::string getSchool(void)      const { valid(); return fields[11]; }
    std::string getGrade(void)       const { valid(); return fields[12]; }
    std::string getSection(void)     const { valid(); return fields[13]; }
    std::string getNwsrsId(void)     const { valid(); return fields[14]; }
    std::string getNwsrsRating(void) const { valid(); return fields[15]; }
    std::string getUscfId(void)      const { valid(); return fields[16]; }
    std::string getUscfRating(void)  const { valid(); return fields[17]; }

    bool isParent(void) const { 
        return didAdultCheck() && getSchool().empty() && getGrade().empty() && getSection().empty() && getNwsrsId().empty();
    }

    bool isPlayer(void) const {
        return !didAdultCheck() && !getSchool().empty() && !getGrade().empty() && !getSection().empty() && !getNwsrsId().empty();
    }
};

#if 0
std::vector< ConstantContactEntry > load_csv_file(const std::string &filename, bool skip_header=false) {
    std::ifstream infile(filename);
    std::vector< ConstantContactEntry > ret;

    unsigned cur_line = 1;

    while(!infile.eof()) {
        std::string whole_line;
        getline(infile, whole_line);
        if (infile.eof()) break;

        ++cur_line;

//std::cout << "=========================================================================" << std::endl;
//std::cout << whole_line << std::endl;
        if (skip_header) {
            std::cout << "Skipping header" << std::endl;
            skip_header = false;
            continue;
        }

        auto tokens = tokenize_csv(whole_line); 
        if (tokens.size() < 18) {
           std::cerr << "Problem tokenizing line " << cur_line << " " << whole_line << std::endl;
           std::cerr << "Tokens size = " << tokens.size() << std::endl;
           exit(-1);
        }
        ret.push_back(ConstantContactEntry(tokens)); 
    }

    return ret;
}
#endif

std::vector< ConstantContactEntry > load_constant_contact_file(const std::string &filename) {
    auto ccret = load_csv_file(filename, true);

    std::vector< ConstantContactEntry > ret; 
    int i;
    for (i = 0; i < ccret.size(); ++i) {
        ret.push_back(ConstantContactEntry(ccret[i]));
    }
    return ret;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Wrong number of command line arguments." << std::endl;
        std::cerr << "Correct is: " << argv[0] << " <constant_contact_file.csv>" << std::endl;
        exit(-1);
    }

    auto entries = load_constant_contact_file(argv[1]);

    unsigned num_parents = 0, num_players = 0;

    int i;
    for (i = 0; i < entries.size(); ++i) {
        if (entries[i].isParent()) {
            ++num_parents;
        }
        if (entries[i].isPlayer()) {
            std::cout << entries[i].getFirstName() << " " << entries[i].getLastName() << " " << entries[i].didAdultCheck() << " school=" << entries[i].getSchool() << " grade=" << entries[i].getGrade() << " section=" << entries[i].getSection() << " nwsrsid=" << entries[i].getNwsrsId() << std::endl;
            ++num_players;
        }
    }

    std::cout << "There are " << entries.size() << " entries in the file." << std::endl;
    std::cout << "There are " << num_parents << " parents." << std::endl;
    std::cout << "There are " << num_players << " players." << std::endl;
    if (num_parents + num_players != entries.size()) {
        std::cerr << "Some problem where number of parents plus number of players not equal to number of entries." << std::endl;
    }

    return 0;
}
