#include <stdio.h>
#include <math.h> // To do calculations like power and square root

// Singly linked-list from what it looks like
struct Block {
  int block_size; // # of bytes in the data section
  struct Block *next_block; // pointer to the next block
};

const int overheadSize = sizeof(struct Block); // size of block value, in bytes
const int voidPointerSize = sizeof(void*); // size of void pointer, in bytes

struct Block* free_head; // points to the head of the free_list of free blocks

void my_initialized_heap(int size) {
    free_head = (struct Block*)malloc(size); // Only time to use actual malloc to allocate memory
    free_head -> block_size = size; // Assign size value to block
    free_head -> next_block = NULL; // Next free block is null as we are just initializing
}

//---------------------------------------------------------------------------------------------

void* my_malloc(int size) {
  int fragmentation = size % voidPointerSize; // fragmentation calculation using modulo
  if (fragmentation != 0) { // Check if size is multiple of voidPointerSize
    size += voidPointerSize - fragmentation; // add fragmentation to size
  }

  struct Block* travelPointer = free_head; // Pointer that increments through the free list, current pointer
  struct Block* prevBlock = NULL; // Pointer that follows behind travelPointer

  while (travelPointer != NULL) { // walk through the free list
    if ((travelPointer -> block_size >= size)) { // Free block size bigger than or equal to given size, then we the found block needed for mem alloc, first fit heuristic
      if (travelPointer -> block_size >= size + overheadSize + voidPointerSize) { // split blocks condition
        struct Block* new_FH = (struct Block*)((char*)travelPointer + size + overheadSize); // Create new block pointer of free memory at given byte location to start
        new_FH -> block_size = travelPointer -> block_size - size - overheadSize; // new calculated block size
        new_FH -> next_block = free_head; // Next block will be whatever free_block currently is
        travelPointer -> block_size = size; // current point given assigned size (reduce size)
        free_head = new_FH; // New free_head location
        return travelPointer + 1; // same as (struct Block*)((char*) travelPointer + overheadSize), returns data region of the block;
      }
      else if (travelPointer -> block_size >= size) { // only change around the pointers if given size is less than or equal to current block size
        if (prevBlock) { // if current block is not free_head
          prevBlock -> next_block = travelPointer -> next_block; //prevBlock pointer goes to current next block
        }
        else { // if current block is free_head
          free_head = travelPointer -> next_block;
        }
        return travelPointer + 1; // same as (struct Block*)((char*) travelPointer + overheadSize), returns data region of the block;
      }
    }
    else { // Increment
      prevBlock = travelPointer; // points to curr pointer
      travelPointer = travelPointer -> next_block; // curr pointer goes to next block
    }
  }
  return NULL;
}

//---------------------------------------------------------------------------------------------

void my_free(void* data) { // De-allocates value that was allocated in data heap
  struct Block *q = (struct Block *)data - 1; // have pointer point to head portion of block, looks like data region minus overhead (it's pointer arithmetic), same as (struct Block *)((char *)data - overheadSize);
  q -> next_block = free_head; // q's next block is where free_head is pointing
  free_head = q; // assign free_head pointer to where q is
}

void test_case_1() { // Tests if my_malloc and my_free work correctly
  printf("Test Case 1\n");
  my_initialized_heap(400);
  void* test1_1 = my_malloc(sizeof(int)); // allocate first int
  printf("Address Test 1 %p\n", test1_1);
  my_free(test1_1); // free first int
  void* test1_2 = my_malloc(sizeof(int)); // allocate second int
  printf("Address Test 2 %p\n", test1_2); // print to see if second int is in same address
  my_free(test1_2); // free second int ust in case
  printf("-------------------------\n");
}

void test_case_2() { // Tests if distance between blocks are overhead size plus void* size
  my_initialized_heap(400);
  printf("Test Case 2\n");
  void* test2_1 = my_malloc(sizeof(int)); // allocate first int
  printf("Address Test 1 %p\n", test2_1);
  void* test2_2 = my_malloc(sizeof(int)); // allocate second int
  printf("Address Test 2 %p\n", test2_2);
  int m = test2_2 - test2_1; // Calculate difference in bytes
  printf("Size of difference: %d\n", m); 
  my_free(test2_1); 
  my_free(test2_2); 
  printf("-------------------------\n");
}

void test_case_3() { // This looks like external fragmentation test, tests if memory is allocated again between two used addresses (if my_malloc and my_free work properly)
  my_initialized_heap(400);
  printf("Test Case 3\n");
  void* test3_1 = my_malloc(sizeof(int)); // allocate first int
  void* test3_2 = my_malloc(sizeof(int)); // allocate second int, this will be freed
  void* test3_3 = my_malloc(sizeof(int)); // allocate third int
  printf("Address Test Int 1 %p\n", test3_1);
  printf("Address Test Int 2 %p\n", test3_2);
  printf("Address Test Int 3 %p\n", test3_3);
  my_free(test3_2); // free second int
  void* test3_4 = my_malloc(sizeof(double)); // allocate double, should be in the address where second int was
  printf("Address Test Double %p\n", test3_4);
  my_free(test3_1); // Just in case
  my_free(test3_4); // Just in case
  my_free(test3_3); // Just in case
  printf("-------------------------\n");
}

//------------------------------------------------------------------------------------------

void test_case_4() { // Tests if distance between blocks are overhead size plus void* size (should be same as test case 2), but this time uses char
  my_initialized_heap(400);
  printf("Test Case 4\n");
  void* test4_1 = my_malloc(sizeof(char)); // allocate char
  printf("Address Test 1 %p\n", test4_1);
  void* test4_2 = my_malloc(sizeof(int)); // allocate int
  printf("Address Test 2 %p\n", test4_2);
  int m = test4_2 - test4_1; // Calculate difference in bytes, difference should be the same as test case 2
  printf("Size of difference: %d\n", m);
  my_free(test4_1); // Just in case
  my_free(test4_2); // Just in case
  printf("-------------------------\n");
}

void test_case_5() { // Seems to tests if allocated memory stays in place
  my_initialized_heap(5000); // Don't know the size of 100-element int array I need
  printf("Test Case 5\n");
  void* this_arr = my_malloc(100 * sizeof(int)); // allocate array?
  void* mallocedI = my_malloc(sizeof(int)); // the int
  printf("Address Test 1 %p\n", this_arr);
  printf("Address Test 2 %p\n", mallocedI);
  my_free(this_arr); // free array
  printf("Address Test 3 %p\n", mallocedI); // mallocedI should still be in same address
  my_free(mallocedI); // Just in case
  printf("-------------------------\n");
}

//-------------------------------------------------------------------------------------

void standard_deviation_program() { // function for user input and standard deviation
  my_initialized_heap(5000); // Don't know the size I need
  printf("Enter a positive integer: ");
  int sizeInput; // Variable for size we're going to input, type a low number
  scanf("%d", &sizeInput); // Input for sizeInput, format specifier
  printf("\n");

  int *my_arr = (int*)my_malloc(sizeof(int) * sizeInput); // allocate n memory for bunch of number inputs, we have to type cast it to make it like an int array

  int x; // Number we'll input throughout the loop
  double mean = 0; // Initialize mean of array
  for (int i = 0; i < sizeInput; i++) {
    printf("Input number %d of %d: ", i+1, sizeInput);
    scanf("%d", &x); // Input number
    my_arr[i] = x; // Assign inputted number into array in index i
    mean += x; // Add total numbers in array
    printf("\n");
  }

  mean /= sizeInput; // Find the mean

  // Find standard deviation
  double variance = 0; // Find variance first
  for (int i = 0; i < sizeInput; i++) {
      variance += pow(my_arr[i] - mean, 2); // Same as doing (x1 - mean)^2 (f(1)) + ... + (xn - mean)^2 (f(n))
  }

  my_free(my_arr);

  variance /= sizeInput; // Get value for variance
  double sd = sqrt(variance); // standard deviation is square root of variance

  printf("Standard deviation: %f\n", sd);
}

int main(void) {
  printf("Overhead size: %d\n", overheadSize);
  printf("void* size: %d\n", voidPointerSize);
  printf("int size %d\n", sizeof(int));
  printf("double size %d\n", sizeof(double));
  printf("char size: %d\n", sizeof(char));
  printf("-------------------------\n");
  
  // Test case 1
  test_case_1();

  // Test case 2
  test_case_2();

  // Test case 3
  test_case_3();

  // Test case 4
  test_case_4();

  // Test case 5
  test_case_5();

  // Some standard deviation with inputs
  standard_deviation_program();
  return 0;
}
