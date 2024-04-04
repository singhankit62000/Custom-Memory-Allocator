#include <stdio.h>
#include "memorymanager.hpp"

int main()
{

		int * a = (int *) MemoryManager::Malloc (800);
		int * b = (int *) MemoryManager::Malloc (1000);
		int * c = (int *) MemoryManager::Malloc (20);

	a[98] = 98;
	b[2] = 2;
	c[1] = 198;

	MemoryManager::Free (a);
	MemoryManager::Free (c);
	MemoryManager::Free (b);

    a = (int *) MemoryManager::Calloc (20 * sizeof (int));
	b = (int *) MemoryManager::Calloc (10 * sizeof (int));
	c = (int *) MemoryManager::Calloc (30 * sizeof (int));

	MemoryManager::Free (a);
	MemoryManager::Free (c);
	MemoryManager::Free (b);

	a = new int [40];
	b = new int;
	delete [] a;
	delete b;

    return 0;

}
