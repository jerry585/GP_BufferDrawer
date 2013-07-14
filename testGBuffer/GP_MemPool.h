
#ifndef GP_MEMPOOL_H__
#define GP_MEMPOOL_H__

//
// Simple fast fixed-size object pool .
//
class GP_MemPool
{
	struct Link
	{ 
		Link* next; 
	};
	
	struct Chunk
	{
		enum { size = 8*1024 - 16 };	//make sure it can be hold by a memory page.
		
		char mem[size];					//the first member mem make sure properply aligned.
		Chunk* next;
	};
	Chunk* chunks;

	const unsigned int esize;
	Link* head;
	
	GP_MemPool(GP_MemPool&);		//disable copy constructor
	void operator= (GP_MemPool&); //disable assignment
	void grow();		//make pool larger
public:
	GP_MemPool(unsigned int n);	//n is the size of elements
	~GP_MemPool();
	void* alloc();			//allocate one element;
	void free(void* b);		//put an element back into the pool;
};

inline void* GP_MemPool::alloc()
{
	if (head == 0) grow();
	Link* p = head; //return first element;
	head = p->next;
	return p;
}

inline void GP_MemPool::free(void*b)
{
	Link* p = static_cast<Link*>(b);
	p->next = head;  //put b back as first element;
	head = p;
}

inline GP_MemPool::GP_MemPool(unsigned int sz)
: esize(sz < sizeof(Link*)?sizeof(Link*):sz)
{
	head = 0;
	chunks = 0;
}

inline GP_MemPool::~GP_MemPool() //free all chunks;
{
	Chunk* n = chunks;
	while(n)
	{
		Chunk* p = n;
		n = n->next;
		delete p;
	}
}

inline void GP_MemPool::grow()
{
	Chunk* n = new Chunk;
	n->next = chunks;
	chunks = n;

	const int nelem = Chunk::size/esize;
	char* start = n->mem;
	char* last = &start[(nelem-1)*esize];

	for (char* p = start; p < last; p += esize) //assume sizeof(Link) <=esize
		reinterpret_cast<Link*>(p)->next = reinterpret_cast<Link*>(p+esize);
	reinterpret_cast<Link*>(last)->next = 0;
	head = reinterpret_cast<Link*>(start);
}

//
//PollObject template. Derive your own class from this template with paramater<your class name>
//to make your own class pool-allacatable.
//
template <typename T>
class GP_MemPoolObject
{
public:
	inline GP_MemPoolObject(){}

	inline ~GP_MemPoolObject(){}	//not virtual.

	inline static void* operator new(size_t size)
	{
		assert(size == sizeof(T));
		return m_Pool.alloc();
/*
		if (size == sizeof(T))
			return m_Pool.alloc();
		else
		{
			//
			//This should never happened.
			//
			std::cout << sizeof(T) << std::endl;
			assert(false);
			return ::operator new(size);
		}
*/
	}
	inline static void operator delete(void* p/*,size_t size*/)
	{
	//	assert(size == sizeof(T));
		m_Pool.free(p);
/*
		if (size == sizeof(T))
			m_Pool.free(p);
		else
		{
			//
			//This should never happened.
			//
			assert(false);
			::operator delete(p);
		}
*/
	}
	
private:
	static GP_MemPool	m_Pool;
};

#endif 