#pragma once
#include <stdlib.h>
/**
 * @brief A simple std::optional replacement.
 * 
 * @tparam Any 
 */
template <typename Any>
class Optional {
    Any * value;

    public:

    /**
     * @brief Construct a new Optional object
     * 
     * @param value 
     */
    Optional(Any * value){
        this->value = value;
    }

    /**
     * @brief Construct a new Optional object
     * 
     * @param value 
     */
    Optional(Any value){
        this->value = &value;
    }

    Optional(){
        this->value = NULL;
    }

    /**
     * @brief value is or not NULL.
     * 
     * @return true 
     * @return false 
     */
    bool hasValue(){
        return this->value;
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