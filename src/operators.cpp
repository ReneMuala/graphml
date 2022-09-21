#include "operators.hpp"

bool operator==(std::string target, std::vector<std::string> set){
    for(std::string value : set){
        if(value == target){
            return true;
        }
    } return false;
}


bool operator==(char target, std::string set){
    for(auto value : set){
        if(value == target){
            return true;
        }
    } return false;
}

std::string operator*(int size, std::string src){
    std::string dest;
    for (size_t i = 0; i < size; i++)
    {
        dest+=src;
    } return dest;
}