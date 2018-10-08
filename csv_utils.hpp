//
// Created by saber on 10/7/18.
//

#ifndef SERENADE_CSV_UTILS_HPP
#define SERENADE_CSV_UTILS_HPP
#include<iostream>
#include<boost/tokenizer.hpp>
#include<string>
#include <fstream>


namespace saber{
class CsvReader{
  std::string _csvfile;
  char _delimiter;
  std::ifstream _in;
  bool _failed{false};
public:
  CsvReader(std::string csvfile, char delimiter=',') : _csvfile(csvfile), _delimiter(delimiter),_in(_csvfile, std::ios::in) {
    if (!_in.is_open())
    { std::cerr << "failed to open " << _csvfile << '\n'; _failed = true;}
  }
  std::vector<std::string> next() {
    using namespace std;
    using namespace boost;

    std::vector<std::string> res;
    if (_failed || _in.eof()) return res;
    string s;
    std::getline(_in, s);
    tokenizer<escaped_list_separator<char> > tok(s);
    for(tokenizer<escaped_list_separator<char> >::iterator beg=tok.begin(); beg!=tok.end();++beg){
      res.emplace_back(*beg);
    }
    return res;
  }
};
}
#endif //SERENADE_CSV_UTILS_HPP
