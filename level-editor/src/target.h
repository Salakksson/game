#ifndef TARGET_H_
#define TARGET_H_

template<typename T>
struct target
{
	T wish;
	T current;
	inline target(T value)
	{
		current = wish = value;
	}
	inline operator T()
	{
		return current;
	}
	inline T update(float delta_time)
	{
		return current += delta_time * (wish - current);
	}
};



#endif
