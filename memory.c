#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <curses.h>
#include <time.h>
#include <string.h>

// MACROS

#define MEMORY_LENGTH (int)1e5

#define lock_BULLET_MEMORY pthread_mutex_lock(&memoryMutex)
#define unlock_BULLET_MEMORY pthread_mutex_unlock(&memoryMutex)

#define lock_ALIEN_MEMORY pthread_mutex_lock(&memoryMutex)
#define unlock_ALIEN_MEMORY pthread_mutex_unlock(&memoryMutex)

#define BLOCK_FREE 0
#define BLOCK_X 1
#define BLOCK_Y 2
#define BLOCK_NEXT 3
#define BLOCK_SIZE 4

// GLOBAL VARIABLES

int BULLET_MEMORY[MEMORY_LENGTH];
int ALIEN_MEMORY[MEMORY_LENGTH];
int LAST_ALLOCATED = -1;

pthread_mutex_t memoryMutex = PTHREAD_MUTEX_INITIALIZER;

//  BULLET_MEMORY y ALIEN_MEMORY
// 1- disponible
// 2- x
// 3- y
// 4- indice del proximo espacio disponible

void initMemory()
{
}

int getBlock(int array[], int ptr, int offset)
{
    return array[ptr + offset];
}

void setBlock(int array[], int ptr, int offset, int value)
{

    array[ptr + offset] = value;
}

int NEXT_FIT(int size, int array[])
{

    if (LAST_ALLOCATED == -1)
    {
        LAST_ALLOCATED = 0;
    }

    int current = LAST_ALLOCATED;

    do
    {
        if (array[current])
        {
            LAST_ALLOCATED = current;
            return current;
        }

        current = (current + BLOCK_SIZE) % MEMORY_LENGTH;

    } while (current != LAST_ALLOCATED);

    return -1;
}

int allocateMemory(int size, int bit)
{

    int answ;
    if (bit)
    {
        lock_BULLET_MEMORY;
        answ = NEXT_FIT(size, BULLET_MEMORY);
        unlock_BULLET_MEMORY;
    }
    else
    {
        lock_ALIEN_MEMORY;
        answ = NEXT_FIT(size, ALIEN_MEMORY);
        unlock_ALIEN_MEMORY;
    }
}

// liberar el espacio que apunta ptr
void freeMemoryBULLET(int ptr)
{

    lock_BULLET_MEMORY;

    if (BULLET_MEMORY[ptr + BLOCK_FREE])
    {
        printf("MEMORIA YA ESTA LIBRE");

        exit(1);
    }

    setBlock(BULLET_MEMORY, ptr, BLOCK_FREE, 1);

    unlock_BULLET_MEMORY;
}

void freeMemoryALIEN(int ptr)
{

    lock_ALIEN_MEMORY;

    if (BULLET_MEMORY[ptr + BLOCK_FREE])
    {
        printf("MEMORIA YA ESTA LIBRE");

        exit(1);
    }

    setBlock(ALIEN_MEMORY, ptr, BLOCK_FREE, 1);

    unlock_ALIEN_MEMORY;
}
