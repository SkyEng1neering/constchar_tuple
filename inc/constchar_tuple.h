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

#ifndef CONSTCHAR_TUPLE_CONSTCHARTUPLE_H_
#define CONSTCHAR_TUPLE_CONSTCHARTUPLE_H_

#include "uvector.h"

#define CONSTCHARTUPLE_VERSION          "1.2.0"

#define CONSTCHARTUPLE_DEBUG            printf

class constchar_tuple {
private:
    uvector<const char*> pointers_array;
    bool allocate_space_for_string(const char** ptr, uint32_t size_without_nullterm);

public:
    constchar_tuple();
#ifdef USE_SINGLE_HEAP_MEMORY
    constchar_tuple(uint32_t size);
#else
    constchar_tuple(heap_t *heap_ptr);
    constchar_tuple(heap_t *heap_ptr, uint32_t size);
#endif
    virtual ~constchar_tuple();

    constchar_tuple(const constchar_tuple& other);

    bool push_back(const char* string);
    bool pop_back();
    bool pop(uint32_t i);
    const char* at(uint32_t i);
    const char* back();
    const char* front();
    const char* operator[](uint32_t i);
    constchar_tuple& operator=(const constchar_tuple& other);
    uint32_t size() const;
    void assign_memory_pointer(heap_t *heap_ptr);
    bool reserve_ptrs_memory(uint32_t elements_num);
};

#endif /* CONSTCHAR_TUPLE_CONSTCHARTUPLE_H_ */
