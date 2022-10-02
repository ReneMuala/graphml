#pragma once
#include <stdlib.h>
#include <iostream>
/**
 * @brief A simple std::optional replacement.
 * 
 * @tparam Any 
 */
template <typename Any>
class Optional {
    bool doHaveValue;
    Any * value = NULL;

    public:

    /**
     * @brief Construct a new Optional object
     * 
     * @param value 
     */
    Optional(Any * value){
        doHaveValue = true;
        this->value = value;
    }

    Optional(){
        doHaveValue = false;
        this->value = NULL;
    }

    void free(){
        if(hasValue()){
            delete value;
        }  doHaveValue = false;
    }

    /**
     * @brief value is or not NULL.
     * 
     * @return true 
     * @return false 
     */
    bool hasValue(){
        return doHaveValue;
    }

  /**
     * @brief Get Value
     * 
     * @return Any* 
     */
    Any * getValue(){
        return value;
    }

};