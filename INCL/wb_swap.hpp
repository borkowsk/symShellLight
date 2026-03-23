#ifndef WB_SWAP
#define WB_SWAP

template <class T>
inline void wb_swap(T& a,T& b)
{
	T c=a;
	a=b;
	b=c;
}

#endif

