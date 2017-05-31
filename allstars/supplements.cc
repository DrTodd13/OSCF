#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h>

typedef unsigned USCF_ID;
typedef unsigned USCF_RATING;

std::map<USCF_ID, USCF_RATING> process_supplements(const std::vector<std::string> &filenames) {
    std::map<USCF_ID, USCF_RATING> ret;
    int i;
    for (i = 0; i < filenames.size(); ++i) {
        std::ifstream infile(filenames[i]);
        do {
            USCF_ID id;
            USCF_RATING rating;
            infile >> id >> rating;
            if(!infile.eof()) {
                if (rating > 3000) {
                   std::cerr << "Out of bounds rating " << rating << " in file " << filenames[i] << std::endl;
                   exit(-1);
                }
                auto it = ret.find(id);
                if (it == ret.end()) {
//std::cout << "Inserting new " << id << " " << rating << std::endl;
                    ret.insert(std::pair<USCF_ID,USCF_RATING>(id,rating));
                } else {
                    if (it->second < rating) {
//std::cout << "Updating rating " << id << " " << it->second << "=>" << rating << std::endl;
                        it->second = rating;
                    } else {
//std::cout << "Not Updating rating " << id << " " << it->second << " " << rating << std::endl;
                    }
                }    
            }
        } while(!infile.eof());
    }
    return ret;
}

int main(int argc, char *argv[]) {
    std::vector<std::string> files;
    int i;
    for (i = 1; i < argc; ++i) {
        files.push_back(std::string(argv[i]));
    }
//    files.push_back(std::string("RSQ1602/2.or"));
//    files.push_back(std::string("RSQ1603/3.or"));
//    files.push_back(std::string("RSQ1604/4.or"));
//    files.push_back(std::string("RSQ1605/5.or"));
    auto supps = process_supplements(files);    
    for(auto iter = supps.begin(); iter != supps.end(); ++iter) {
        std::cout << iter->first << " " << iter->second << std::endl;
    }
    return 0;
}
