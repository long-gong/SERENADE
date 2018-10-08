//
// Created by saber on 10/7/18.
//
// source: https://www.boost.org/doc/libs/1_68_0/libs/tokenizer/doc/escaped_list_separator.htm
// simple_example_2.cpp
#include<iostream>
#include<boost/tokenizer.hpp>
#include<string>
#include <csv_utils.hpp>
#include <gtest/gtest.h>



TEST(CsvReaderTest, CorrectCsvFile){
  std::string csvfile("../data/iterations_def.csv");
  saber::CsvReader cr(csvfile);
  auto res = cr.next();
  while (!res.empty()) {
    for ( const auto& s : res ) std::cout << s << " ";
    std::cout << std::endl;
    res = cr.next();
  }
}

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}