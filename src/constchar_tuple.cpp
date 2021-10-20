/*
 * Copyright 2021 Alexey Vasilenko
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "constchar_tuple.h"

constchar_tuple::constchar_tuple(){

}

#ifdef USE_SINGLE_HEAP_MEMORY
constchar_tuple::constchar_tuple(uint32_t size){
    pointers_array.reserve(size);
}
#else
constchar_tuple::constchar_tuple(heap_t *heap_ptr){
    pointers_array.assign_mem_pointer(heap_ptr);
}

constchar_tuple::constchar_tuple(heap_t *heap_ptr, uint32_t size){
    pointers_array.assign_mem_pointer(heap_ptr);
    pointers_array.reserve(size);
}
#endif

constchar_tuple::~constchar_tuple(){
    for(uint32_t i = 0; i < size(); i++){
        dfree(this->pointers_array.get_mem_pointer(), (void**)&pointers_array.at(i), USING_PTR_ADDRESS);
    }
}

constchar_tuple::constchar_tuple(const constchar_tuple& other){
    this->pointers_array.reserve(other.size());
    for(uint32_t i = 0; i < other.size(); i++){
        this->push_back(other.pointers_array.data()[i]);
    }
}

bool constchar_tuple::allocate_space_for_string(const char** ptr, uint32_t size_without_nullterm){
    dalloc(this->pointers_array.get_mem_pointer(), size_without_nullterm + 1, (void**)ptr);
    if(*ptr == NULL){
        return false;
    }
    for(uint32_t i = 0; i < size_without_nullterm + 1; i++){
        const_cast<char*>(*ptr)[i] = 0;
    }
    return true;
}

bool constchar_tuple::push_back(const char *string){
    /* Check if there is enough capacity in pointers array for new pointer */
    if(pointers_array.capacity() > pointers_array.size()){
        /* Here is enough capacity to push back new pointer to pointers array without reallocation procedure */
        if(this->pointers_array.push_back((const char*)NULL) != true){
            return false;
        }
        /* Allocate space for new string */
        if(allocate_space_for_string(&(pointers_array.back()), strlen(string) + 1) != true){
            CONSTCHARTUPLE_DEBUG("ERROR: %s(): unable to allocate space for string\n", __func__);
            return false;
        }
        /* Copy string content from new string */
        for(uint32_t k = 0; k < strlen(string) + 1; k++){
            const_cast<char*>(this->pointers_array.back())[k] = string[k];
        }

        return true;
    } else {
        if(size() == 0){
            /* Push back new element to pointers array, reallocation procedure will work automatically */
            if(this->pointers_array.push_back((const char*)NULL) != true){
                return false;
            }

            /* Allocate space for new string */
            if(allocate_space_for_string(&(pointers_array.back()), strlen(string) + 1) != true){
                CONSTCHARTUPLE_DEBUG("ERROR: %s(): unable to allocate space for string\n", __func__);
                return false;
            }

            /* Copy string content from new string */
            for(uint32_t k = 0; k < strlen(string) + 1; k++){
                const_cast<char*>(this->pointers_array.back())[k] = string[k];
            }

            return true;
        }

        /* Add new pointer to pointers array will call reallocation procedure */

        const char* const_char_ptrs_arr[this->pointers_array.size()];

        /* Replace pointers addresses in heap info structure to save pointers addresses when pointers array will be reallocated */
        for(uint32_t i = 0; i < this->pointers_array.size(); i++){
            replace_pointers(this->pointers_array.get_mem_pointer(), (void**)&(pointers_array.at(i)), (void**)&const_char_ptrs_arr[i]);
        }

        /* Push back new element to pointers array, reallocation procedure will work automatically */
        if(this->pointers_array.push_back((const char*)NULL) != true){
            return false;
        }

        /* Replace pointers addresses in heap info structure back */
        for(uint32_t i = 0; i < pointers_array.size() - 1; i++){
            replace_pointers(this->pointers_array.get_mem_pointer(), (void**)&const_char_ptrs_arr[i], (void**)&(pointers_array.at(i)));
        }

        /* Allocate space for new string */
        if(allocate_space_for_string(&(pointers_array.back()), strlen(string) + 1) != true){
            CONSTCHARTUPLE_DEBUG("ERROR: %s(): unable to allocate space for string\n", __func__);
            return false;
        }

        /* Copy string content from new string */
        for(uint32_t k = 0; k < strlen(string) + 1; k++){
            const_cast<char*>(this->pointers_array.back())[k] = string[k];
        }
    }
    return true;
}

bool constchar_tuple::pop_back(){
    /* Free memory allocated for the last string */
    dfree(this->pointers_array.get_mem_pointer(), (void**)&this->pointers_array.back(), USING_PTR_ADDRESS);

    return this->pointers_array.pop_back();
}

bool constchar_tuple::pop(uint32_t i){
    /* Free memory allocated for i element */
    dfree(this->pointers_array.get_mem_pointer(), (void**)&this->pointers_array.at(i), USING_PTR_ADDRESS);

    /* Replace pointers info for elements with indexes >= i */
    for(uint32_t k = i; k < this->pointers_array.size() - 1; k++){
        replace_pointers(this->pointers_array.get_mem_pointer(), (void**)&this->pointers_array.at(k + 1), (void**)&this->pointers_array.at(k));
    }

    return this->pointers_array.pop_back();
}

const char* constchar_tuple::at(uint32_t i){
    return pointers_array.at(i);
}

const char* constchar_tuple::back(){
    return pointers_array.at(pointers_array.size() - 1);
}

const char* constchar_tuple::front(){
    return pointers_array.at(0);
}

const char* constchar_tuple::operator[](uint32_t i){
    return at(i);
}

uint32_t constchar_tuple::size() const {
    return pointers_array.size();
}

void constchar_tuple::assign_memory_pointer(heap_t* heap_ptr){
    pointers_array.assign_mem_pointer(heap_ptr);
}

bool constchar_tuple::reserve_ptrs_memory(uint32_t elements_num){
    return pointers_array.reserve(elements_num);
}

constchar_tuple& constchar_tuple::operator=(const constchar_tuple& other){
    if(this != &other){
        this->pointers_array.reserve(other.size());
        for(uint32_t i = 0; i < other.size(); i++){
            this->push_back(other.pointers_array.data()[i]);
        }
    }
    return *this;
}








