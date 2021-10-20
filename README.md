# About the project
This is the container for char strings. Based on [dalloc](https://github.com/SkyEng1neering/dalloc) allocator, that solves memory fragmentation problem. So you can use it in your embedded project and not to be afraid of program crash by reason of memory fragmentation. This is analog of "__uvector__ of __strings__" that works faster and use memory more efficiently. Use it if you need array of strings.

# Dependencies
__constchar_tuple__ based on Based on [uvector](https://github.com/SkyEng1neering/uvector) and [dalloc](https://github.com/SkyEng1neering/dalloc) allocator, so you should include it to your project.

# Usage
## Using constchar_tuple with single heap area
[dalloc](https://github.com/SkyEng1neering/dalloc) allocator is configurable, it can work with only one memory area that you define, or you can select which memory area should be used for each your allocation.

If you want to use in your project only one heap area, you should define "USE_SINGLE_HEAP_MEMORY" in file __dalloc_conf.h__. This is the simpliest way to use __constchar_tuple__ because it allows you to abstract away from working with memory.

```c++
/* File dalloc_conf.h */
#define USE_SINGLE_HEAP_MEMORY
#define SINGLE_HEAP_SIZE				4096UL //define heap size that you want to have
```

Then you should define uint8_t array in your code, that will be used for storing data. This array should be named "single_heap" and it should be have size SINGLE_HEAP_SIZE (defined in file __dalloc_conf.h__).

```c++
#include "constchar_tuple.h"

uint8_t single_heap[SINGLE_HEAP_SIZE] = {0};
```

Why you should implement this array in your code by yourself? Because you may want to store this array for example in specific memory region, or you may want to apply to this array some attributes, like this:

```c++
__attribute__((section(".ITCM_RAM"))) uint8_t single_heap[SINGLE_HEAP_SIZE] = {0};
```
or like this:

```c++
__ALIGN_BEGIN uint8_t single_heap[SINGLE_HEAP_SIZE] __ALIGN_END;
```

So for example that's how looks like example of using __constchar_tuple__ with single memory region on STM32 MCU:

```c++
#include "constchar_tuple.h"

__ALIGN_BEGIN uint8_t single_heap[SINGLE_HEAP_SIZE] __ALIGN_END;

void main(){
  constchar_tuple tuple;
  tuple.push_back("string1");
  tuple.push_back("string2");
  tuple.push_back("string3");
  tuple.push_back("string4");
  tuple.push_back("string5");

  for(uint32_t i = 0; i < tuple.size(); i++){
    printf("String %lu is: %s\n", i, tuple.at(i));
  }
  
  tuple.pop_back();//remove the last element from tuple
 
  for(uint32_t i = 0; i < tuple.size(); i++){
    printf("String %lu is: %s\n", i, tuple.at(i));
  }                                     
      
  tuple.pop(2);//remove 2-nd element from tuple
 
  for(uint32_t i = 0; i < tuple.size(); i++){
    printf("String %lu is: %s\n", i, tuple.at(i));
  }
  
  while(1){}
}  
```

By the way you can use __constchar_tuple__ also with [uvector](https://github.com/SkyEng1neering/uvector):

```c++
#include "uvector.h"
#include "constchar_tuple.h"

__ALIGN_BEGIN uint8_t single_heap[SINGLE_HEAP_SIZE] __ALIGN_END;

void main(){
  constchar_tuple tuple1;
  tuple.push_back("tuple1 string1");
  tuple.push_back("tuple1 string2");

  constchar_tuple tuple2;
  tuple.push_back("tuple2 string1");
  tuple.push_back("tuple2 string2");
                                       
  uvector<constchar_tuple> vector_of_tuples;
  
  vector_of_tuples.push_back(tuple1);
  vector_of_tuples.push_back(tuple2);
 
  for(uint32_t i = 0; i < vector_of_tuples.size(); i++){
    printf("Vector element %lu:\n", i,);
    for(uint32_t k = 0; k < vector_of_tuples.at(i).size(); k++){
      printf("  %s\n", vector_of_tuples.at(i).at(k));
    }                                              
  }
 
  while(1){}
}  
```
Full info about supported methods of __constchar_tuple__ you can see in ___constchar_tuple.h___ file

## Using constchar_tuple with different heap areas

If you want to use several different heap areas, you can define it explicitly:

```c++
/* File dalloc_conf.h */
//#define USE_SINGLE_HEAP_MEMORY //comment this define
```

```c++
#include "constchar_tuple.h"

#define HEAP_SIZE			1024

/* Declare an arrays that will be used for dynamic memory allocations */
__ALIGN_BEGIN uint8_t heap_array1[HEAP_SIZE] __ALIGN_END;
__ALIGN_BEGIN uint8_t heap_array2[HEAP_SIZE] __ALIGN_END;

/* Declare an dalloc heap structures, it will contains all allocations info */
heap_t heap1;
heap_t heap2;

void main(){
  /* Init heap memory */
  heap_init(&heap1, (void*)heap_array1, HEAP_SIZE);
  heap_init(&heap2, (void*)heap_array2, HEAP_SIZE);

  constchar_tuple tuple1(&heap1);
  tuple1.push_back("string1");
  tuple1.push_back("string2");
  tuple1.push_back("string3");

  constchar_tuple tuple2(&heap2);
  tuple2.push_back("string1");
  tuple2.push_back("string2");
  tuple2.push_back("string3");
  
  //Print tuple from heap1
  for(uint32_t i = 0; i < tuple1.size(); i++){
    printf("String %lu is: %s\n", i, tuple1.at(i));
  }
 
  //Print tuple from heap2
  for(uint32_t i = 0; i < tuple2.size(); i++){
    printf("String %lu is: %s\n", i, tuple2.at(i));
  }
                                        
  while(1){}
}  
  
```
## P.S.
In any time you can check what exactly is going on in your heap memory using functions:
```c++
/* If you use different heap areas in your project */
void print_dalloc_info(heap_t *heap_struct_ptr);
void dump_dalloc_ptr_info(heap_t* heap_struct_ptr);
void dump_heap(heap_t* heap_struct_ptr);
```
```c++
/* If you use single heap area in your project */
void print_def_dalloc_info();
void dump_def_dalloc_ptr_info();
void dump_def_heap();
```
