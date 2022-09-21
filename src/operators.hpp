#pragma once
#include <string>
#include <vector>
bool operator==(std::string, std::vector<std::string>);
bool operator==(char target, std::string set);
std::string operator*(int size, std::string src);