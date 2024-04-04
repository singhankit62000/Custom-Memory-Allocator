#include "custom.malloc.coalescing.hpp"
#include <stdlib.h>

MemoryManager :: MemoryManager () 
{

        tMetaData * temp = (tMetaData *) malloc (INITIAL_BLOCK_SIZE);

    vHead = nullptr;

    if (temp == nullptr) {

        return;

    }

	//temp -> uAddress = vHead;
	temp -> uAvailable = true;
	temp -> uNext = nullptr;
	temp -> uPrev = nullptr;
    temp -> uChunkSize = INITIAL_BLOCK_SIZE - METADATA_SIZE;

    vHead = temp;

}

MemoryManager :: ~MemoryManager () 
{
        tMetaData * curr = vHead;
        tMetaData * temp = nullptr;

	while (curr != nullptr) {

        temp = curr -> uNext;
		free (curr);
		//curr = nullptr;

        curr = temp;

	}

}

void * MemoryManager :: AllocateMemory (size_t pSize) 
{

        tMetaData   *   curr        = (tMetaData *) vHead;
        tMetaData   *   prev        = nullptr;
        tMetaData   *   new_chunk   = nullptr;
        tMetaData   *   new_block   = nullptr;
        size_t          rem         = 0;
        size_t          size        = 0;

    while (curr != nullptr) {

        prev = curr;
        if (curr -> uAvailable && curr -> uChunkSize >= pSize) {
            
            curr -> uAvailable = false;

            rem = curr -> uChunkSize - pSize;

            if (rem > METADATA_SIZE) {

                new_chunk = (tMetaData *) ((char *) (curr) + METADATA_SIZE + pSize);
                new_chunk -> uChunkSize = rem - METADATA_SIZE;
                new_chunk -> uPrev = curr;
                new_chunk -> uNext = curr -> uNext;
                new_chunk -> uAvailable = true;

                if (curr -> uNext != nullptr) {

                    curr -> uNext -> uPrev = new_chunk;

                }

                curr -> uNext = new_chunk;
                curr -> uChunkSize = pSize;

            }

            return (void *) ((char *) curr + METADATA_SIZE);

        }

        curr = curr -> uNext;

    }

    // If the blocks don't have enough memory to allocate the required size,
    // create a new block of either INITIAL_BLOCK_SIZE or pSize, whichever is greater

    size = (INITIAL_BLOCK_SIZE > (pSize + METADATA_SIZE)) ? INITIAL_BLOCK_SIZE : pSize + METADATA_SIZE;

    new_block = (tMetaData *) malloc (size);

    if (new_block == nullptr) {

        return nullptr;

    }

    new_block -> uAvailable = true;
    new_block -> uChunkSize = size - METADATA_SIZE;
    new_block -> uPrev      = prev;
    new_block -> uNext      = nullptr;

    if (prev != nullptr) {

        prev -> uNext = new_block;

    }

    // Retry allocation 
    return AllocateMemory (pSize);

}


void MemoryManager :: DeallocateMemory (void * pPtr) 
{

        tMetaData   *   address = nullptr;

    if (pPtr == nullptr) {

            return;

    }

    address = (tMetaData*) ((char*) pPtr - METADATA_SIZE);
    address -> uAvailable = true;

    CoalesceMemory (address);

}

void MemoryManager :: CoalesceMemory (void * pPtr) 
{

        tMetaData   *   ptr     = (tMetaData *) pPtr;
        tMetaData   *   curr    = ptr -> uPrev;

    
    // Merging with the previous adjacent chunk of free memory
    if ((ptr -> uPrev != nullptr && ptr -> uPrev -> uAvailable) &&
        ((char *) curr + METADATA_SIZE + curr -> uChunkSize == (char *) ptr)) {

        curr -> uNext = ptr -> uNext;
        if (ptr -> uNext != nullptr) {

            ptr -> uNext -> uPrev = curr;

        }

        curr -> uChunkSize += ptr -> uChunkSize + METADATA_SIZE;

        // If the memory gets merged with the previous adjacent chunk, 
        // then the ptr address doesn't exist independently since it's merged.
        // Hence changing ptr to curr, i.e. the new merged memory chunk
        ptr = curr;

    }


    curr = ptr -> uNext;
    // Merging with the next adjacent chunk of free memory
    if ((ptr -> uNext != nullptr && ptr -> uNext -> uAvailable) &&
        ((char *) ptr + METADATA_SIZE + ptr -> uChunkSize == (char *) curr)) {

        ptr -> uNext = curr -> uNext;
        if (curr -> uNext != nullptr) {

            curr -> uNext -> uPrev = ptr;

        }

        ptr -> uChunkSize += curr -> uChunkSize + METADATA_SIZE;
    }

}


//void * MemoryManager :: AllocateMemory (size_t pSize) 
//{
//			
//        // Traverse the linked list of blocks to find available memory
//        tMetaData* curr = vHead;
//
//    while (curr != nullptr) {
//        if (curr -> uAvailable && curr -> uChunckSize >= pSize) {
//            // Allocate memory from the current block
//            curr -> uAvailable = false;
//
//            // Split the block if the remaining space is sufficient for metadata
//            size_t rem = curr -> uChunckSize - pSize;
//            if (rem > METADATA_SIZE) {
//                tMetaData* newBlock = (tMetaData*)((char*) curr -> uAddress + pSize);
//                newBlock -> uChunckSize = rem - METADATA_SIZE;
//                newBlock -> uAvailable = true;
//                newBlock -> uAddress = (void*)((char*)newBlock + METADATA_SIZE);
//                newBlock -> uNext = curr -> uNext;
//                newBlock -> uPrev = curr;
//
//                if (curr -> uNext != nullptr) {
//                    curr -> uNext -> uPrev = newBlock;
//                }
//
//                curr -> uNext = newBlock;
//                curr -> uChunckSize = pSize;
//            }
//
//            return curr -> uAddress;
//        }
//
//        curr = curr -> uNext;
//    }
//
//    // If no suitable block is found, allocate a new block
//    tMetaData* newBlock = (tMetaData*)std::malloc (INITIAL_BLOCK_SIZE);
//    newBlock -> uChunckSize = INITIAL_BLOCK_SIZE - METADATA_SIZE;
//    newBlock -> uAvailable = true;
//    newBlock -> uAddress = (void*)((char*) newBlock + METADATA_SIZE);
//    newBlock -> uNext = nullptr;
//    newBlock -> uPrev = curr;
//             
//     if (curr != nullptr) {
//        curr -> uNext = newBlock;
//    }
//
//    return AllocateMemory (pSize); // Retry allocation in the new block
//	
//
//}