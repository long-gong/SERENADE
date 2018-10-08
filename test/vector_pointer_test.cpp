//
// Created by saber on 10/7/18.
//

#include <iostream>
#include <vector>

int main()
{
  std::vector<int> a(2,-1);
  a.reserve(10);
  int* ap1 = &a[1];
  std::cout << (*ap1) << std::endl;
  a.push_back(1);
  a.push_back(1);
  a.push_back(1);
  a.push_back(1);
  std::cout << (*ap1) << std::endl;
}