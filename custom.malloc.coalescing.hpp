#ifndef CUSTOM_MALLOC_COALESCING
#define CUSTOM_MALLOC_COALESCING

#define						INITIAL_BLOCK_SIZE			1024
#define						METADATA_SIZE				sizeof (tMetaData)

struct tMetaData {

	size_t			uChunkSize;
	bool			uAvailable;
	tMetaData	*	uNext;
	tMetaData	*	uPrev;

};

class MemoryManager {

	public:
static	void		  *		Malloc						(size_t pSize);
static	void		  *		Calloc						(size_t pSize);
static	void				Free						(void * pPtr);

	private:
							MemoryManager				();
							~MemoryManager				();
		void				CoalesceMemory				(void * pPtr);
		void		  *		AllocateMemory				(size_t pSize);
		void				DeallocateMemory			(void * pPtr);
static	MemoryManager &		GetInstance					();

		tMetaData	  *		vHead;

};

#endif // !CUSTOM_MALLOC_COALESCING