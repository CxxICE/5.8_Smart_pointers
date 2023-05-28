#include <iostream>
#include <utility>
#include <type_traits>//std::enable_if


template<typename T>
class MyUniquePtr
{
public:
	MyUniquePtr()
	{
		//std::cout << "ctor1 MyUniquePtr" << std::endl;
		_ptr = nullptr;
	};
	explicit MyUniquePtr(T *ptr)
	{
		//std::cout << "ctor2 MyUniquePtr" << std::endl;
		_ptr = ptr;
	};

	MyUniquePtr(MyUniquePtr &&other) noexcept
	{
		//std::cout << "ctor3 MyUniquePtr" << std::endl;
		_ptr = other._ptr;
		other._ptr = nullptr;
	};

	T &operator*()
	{
		return *_ptr;
	};
	
	T *operator->()
	{
		return _ptr;
	};

	T *get()
	{
		return _ptr;
	};

	T *release()
	{
		T *tmp = _ptr;
		_ptr = nullptr;
		return tmp;
	};

	void swap(MyUniquePtr &other) noexcept
	{
		T *tmp = _ptr;
		_ptr = other._ptr;
		other._ptr = tmp;
	};
		
	void reset(T *ptr)
	{
		if (_ptr != nullptr)
		{
			delete _ptr;
		}
		_ptr = ptr;
	};	

	MyUniquePtr &operator=(MyUniquePtr &&other) noexcept
	{
		if (this != &other)
		{
			if (_ptr != nullptr)
			{
				delete _ptr;
			}
			_ptr = other._ptr;
			other._ptr = nullptr;
		}
		return *this;
	};

	MyUniquePtr(const MyUniquePtr &) = delete;
	MyUniquePtr &operator=(const MyUniquePtr &) = delete;

	~MyUniquePtr()
	{
		//std::cout << "dtor MyUniquePtr" << std::endl;
		delete _ptr;
	}
private:
	T *_ptr;
};

template<typename T>
class MyUniquePtr<T[]>
{
public:
	MyUniquePtr()
	{
		//std::cout << "ctor1[] MyUniquePtr" << std::endl;
		_ptr = nullptr;
	};
	explicit MyUniquePtr(T *ptr)
	{
		//std::cout << "ctor2[] MyUniquePtr" << std::endl;
		_ptr = ptr;
	};

	MyUniquePtr(MyUniquePtr &&other) noexcept
	{
		//std::cout << "ctor3[] MyUniquePtr" << std::endl;
		_ptr = other._ptr;
		other._ptr = nullptr;
	};

	T &operator*()
	{
		return *_ptr;
	};

	T *operator->()
	{
		return _ptr;
	};

	T &operator[](size_t index)
	{
		return _ptr[index];
	};

	T *get()
	{
		return _ptr;
	};

	T *release()
	{
		T *tmp = _ptr;
		_ptr = nullptr;
		return tmp;
	};

	void swap(MyUniquePtr &other) noexcept
	{
		T *tmp = _ptr;
		_ptr = other._ptr;
		other._ptr = tmp;
	};

	void reset(T *ptr)
	{
		if (_ptr != nullptr)
		{
			delete [] _ptr;
		}
		_ptr = ptr;
	};

	MyUniquePtr &operator=(MyUniquePtr &&other) noexcept
	{
		if (this != &other)
		{
			if (_ptr != nullptr)
			{
				delete [] _ptr;
			}
			_ptr = other._ptr;
			other._ptr = nullptr;
		}
		return *this;
	};

	MyUniquePtr(const MyUniquePtr &) = delete;
	MyUniquePtr &operator=(const MyUniquePtr &) = delete;

	~MyUniquePtr()
	{
		//std::cout << "dtor[] MyUniquePtr" << std::endl;
		delete [] _ptr;
	}
private:
	T *_ptr;
};

//аналог make_unique
template<typename T1, typename ... T2, std::enable_if_t<!std::is_array<T1>::value, size_t> = 0>
MyUniquePtr<T1> makeMyUniquePtr(T2&&... args)
{
	return MyUniquePtr<T1>(new T1(std::forward<T2>(args)...)); //std::forward перемещение принятых rvalue T2&&
};

template<typename T1, std::enable_if_t<std::is_array<T1>::value, size_t> = 0>
MyUniquePtr<T1> makeMyUniquePtr(size_t size)
{
	return MyUniquePtr<T1>(new std::remove_extent_t<T1>[size]);
};


//вспомогательные структура и функция для тестов
struct TestStruct
{
	int x;
	int y;
	int z;
};

template<typename T>
void fillArr(MyUniquePtr<T> &mptr, size_t size)//заполнение массива простых типов
{
	for (size_t i = 0; i < size; ++i)
	{
		mptr[i] = i;
	}
};

template<>
void fillArr(MyUniquePtr<TestStruct[]> &mptr, size_t size)//заполнение массива TestStruct
{
	for (size_t i = 0; i < size; ++i)
	{
		mptr[i].z = mptr[i].y = mptr[i].x = i;
	}
};

template<typename T>
void printArr(MyUniquePtr<T> &mptr, size_t size)//вывод массива простых типов
{
	for (size_t i = 0; i < size; ++i)
	{
		std::cout << mptr[i] << " ";
	}
	std::cout << std::endl;
};

template<>
void printArr(MyUniquePtr<TestStruct[]> &mptr, size_t size)//вывод массива TestStruct
{
	for (size_t i = 0; i < size; ++i)
	{
		std::cout << "[" << i << "]: " << "x = " << mptr[i].x << "; y = " << mptr[i].y << "; z = " << mptr[i].z << std::endl;
	}
	std::cout << std::endl;
};


int main()
{
	setlocale(LC_ALL, "RU");
	{//тест MyUniquePtr<T> c простыми типами
		std::cout << "[SECTION_1]\n";
		MyUniquePtr<int> m1(new int(1));
		MyUniquePtr<int> m2 = makeMyUniquePtr<int>(2);
		MyUniquePtr<int> m3(new int(3));
		std::cout << "m1.get() = " << m1.get() << "; *m1 = " << *m1 << std::endl;
		std::cout << "m2.get() = " << m2.get() << "; *m2 = " << *m2 << std::endl;
		std::cout << "m3.get() = " << m3.get() << "; *m3 = " << *m3 << std::endl << std::endl;
		
		std::cout << "m1.swap(m2)" << std::endl;
		m1.swap(m2);
		std::cout << "m1.get() = " << m1.get() << "; *m1 = " << *m1 << std::endl;
		std::cout << "m2.get() = " << m2.get() << "; *m2 = " << *m2 << std::endl;
		std::cout << "m3.get() = " << m3.get() << "; *m3 = " << *m3 << std::endl << std::endl;

		std::cout << "m1.reset(new int(5))" << std::endl;
		m1.reset(new int(4));
		std::cout << "m1.get() = " << m1.get() << "; *m1 = " << *m1 << std::endl;
		std::cout << "m2.get() = " << m2.get() << "; *m2 = " << *m2 << std::endl;
		std::cout << "m3.get() = " << m3.get() << "; *m3 = " << *m3 << std::endl << std::endl;

		std::cout << "MyUniquePtr<int> m4(m1.release())" << std::endl;
		MyUniquePtr<int> m4(m1.release());
		std::cout << "m1.get() = " << m1.get() << std::endl;
		std::cout << "m2.get() = " << m2.get() << "; *m2 = " << *m2 << std::endl;
		std::cout << "m3.get() = " << m3.get() << "; *m3 = " << *m3 << std::endl;
		std::cout << "m4.get() = " << m4.get() << "; *m4 = " << *m4 << std::endl << std::endl;
		
	}

	{//тест MyUniquePtr<T> cо структурой
		std::cout << "\n\n[SECTION_2]\n";
		MyUniquePtr<TestStruct> m1(new TestStruct{ 11,12,13 });
		MyUniquePtr<TestStruct> m2 = makeMyUniquePtr<TestStruct>(TestStruct{ 21,22,23 });
		MyUniquePtr<TestStruct> m3(new TestStruct{ 31,32,33 });

		std::cout << "m1.get() = " << m1.get() << "; m1->x = " << m1->x << "; m1->y = " << m1->y << "; m1->z = " << m1->z << std::endl;
		std::cout << "m2.get() = " << m2.get() << "; m2->x = " << m2->x << "; m2->y = " << m2->y << "; m2->z = " << m2->z << std::endl;
		std::cout << "m3.get() = " << m3.get() << "; m3->x = " << m3->x << "; m3->y = " << m3->y << "; m3->z = " << m3->z << std::endl << std::endl;

		std::cout << "m1.swap(m2)" << std::endl;
		m1.swap(m2);
		std::cout << "m1.get() = " << m1.get() << "; m1->x = " << m1->x << "; m1->y = " << m1->y << "; m1->z = " << m1->z << std::endl;
		std::cout << "m2.get() = " << m2.get() << "; m2->x = " << m2->x << "; m2->y = " << m2->y << "; m2->z = " << m2->z << std::endl;
		std::cout << "m3.get() = " << m3.get() << "; m3->x = " << m3->x << "; m3->y = " << m3->y << "; m3->z = " << m3->z << std::endl << std::endl;
		
		std::cout << "m1.reset(new TestStruct{ 41,42,43 })" << std::endl;
		m1.reset(new TestStruct{ 41,42,43 });
		std::cout << "m1.get() = " << m1.get() << "; m1->x = " << m1->x << "; m1->y = " << m1->y << "; m1->z = " << m1->z << std::endl;
		std::cout << "m2.get() = " << m2.get() << "; m2->x = " << m2->x << "; m2->y = " << m2->y << "; m2->z = " << m2->z << std::endl;
		std::cout << "m3.get() = " << m3.get() << "; m3->x = " << m3->x << "; m3->y = " << m3->y << "; m3->z = " << m3->z << std::endl << std::endl;

		std::cout << "MyUniquePtr<TestStruct> m4(m1.release())" << std::endl;
		MyUniquePtr<TestStruct> m4(m1.release());
		std::cout << "m1.get() = " << m1.get() << std::endl;
		std::cout << "m2.get() = " << m2.get() << "; m2->x = " << m2->x << "; m2->y = " << m2->y << "; m2->z = " << m2->z << std::endl;
		std::cout << "m3.get() = " << m3.get() << "; m3->x = " << m3->x << "; m3->y = " << m3->y << "; m3->z = " << m3->z << std::endl;
		std::cout << "m4.get() = " << m4.get() << "; m4->x = " << m4->x << "; m4->y = " << m4->y << "; m4->z = " << m4->z << std::endl << std::endl;
	}



	{//тест MyUniquePtr<T[]> с простыми типами
		std::cout << "\n\n[SECTION_3]\n";
		size_t size1 = 5, size2 = 10, size3 = 15, size4 = 20;
		MyUniquePtr<int[]> m1(new int[size1]);
		MyUniquePtr<int[]> m2 = makeMyUniquePtr<int[]>(size2);
		MyUniquePtr<int[]> m3(new int[size3]);
		
		fillArr(m1, size1);
		fillArr(m2, size2);
		fillArr(m3, size3);
		
		std::cout << "m1.get() = " << m1.get() << "; m1: ";
		printArr(m1, size1);
		std::cout << "m2.get() = " << m2.get() << "; m2: ";
		printArr(m2, size2);
		std::cout << "m3.get() = " << m3.get() << "; m3: ";
		printArr(m3, size3);
		std::cout << std::endl;

		std::cout << "m1.swap(m2)" << std::endl;
		m1.swap(m2);
		std::swap(size1, size2);
		std::cout << "m1.get() = " << m1.get() << "; m1: ";
		printArr(m1, size1);
		std::cout << "m2.get() = " << m2.get() << "; m2: ";
		printArr(m2, size2);
		std::cout << "m3.get() = " << m3.get() << "; m3: ";
		printArr(m3, size3);
		std::cout << std::endl;

		std::cout << "m1.reset(new int[size4])" << std::endl;
		m1.reset(new int[size4]);
		fillArr(m1, size4);
		std::cout << "m1.get() = " << m1.get() << "; m1: ";
		printArr(m1, size4);
		std::cout << "m2.get() = " << m2.get() << "; m2: ";
		printArr(m2, size2);
		std::cout << "m3.get() = " << m3.get() << "; m3: ";
		printArr(m3, size3);
		std::cout << std::endl;


		std::cout << "MyUniquePtr<int[]> m4(m1.release())" << std::endl;
		MyUniquePtr<int[]> m4(m1.release());
		std::cout << "m1.get() = " << m1.get() << std::endl;
		std::cout << "m2.get() = " << m2.get() << "; m2: ";
		printArr(m2, size2);
		std::cout << "m3.get() = " << m3.get() << "; m3: ";
		printArr(m3, size3);
		std::cout << "m4.get() = " << m4.get() << "; m4: ";
		printArr(m4, size4);
		std::cout << std::endl << std::endl;
	}
	
	{//тест MyUniquePtr<T[]> со структурами
		std::cout << "\n\n[SECTION_4]\n";
		size_t size1 = 3, size2 = 5, size3 = 7, size4 = 10;
		MyUniquePtr<TestStruct[]> m1(new TestStruct[size1]);
		MyUniquePtr<TestStruct[]> m2 = makeMyUniquePtr<TestStruct[]>(size2);
		MyUniquePtr<TestStruct[]> m3(new TestStruct[size3]);
	
		fillArr(m1, size1);
		fillArr(m2, size2);
		fillArr(m3, size3);

		std::cout << "m1.get() = " << m1.get() << "\n";
		printArr(m1, size1);
		std::cout << "m2.get() = " << m2.get() << "\n";
		printArr(m2, size2);
		std::cout << "m3.get() = " << m3.get() << "\n";
		printArr(m3, size3);
		std::cout << std::endl;

		std::cout << "m1.swap(m2)" << std::endl;
		m1.swap(m2);
		std::swap(size1, size2);
		std::cout << "m1.get() = " << m1.get() << "\n";
		printArr(m1, size1);
		std::cout << "m2.get() = " << m2.get() << "\n";
		printArr(m2, size2);
		std::cout << "m3.get() = " << m3.get() << "\n";
		printArr(m3, size3);
		std::cout << std::endl;

		std::cout << "m1.reset(new TestStruct[size4])" << std::endl;
		m1.reset(new TestStruct[size4]);
		fillArr(m1, size4);
		std::cout << "m1.get() = " << m1.get() << "\n";
		printArr(m1, size4);
		std::cout << "m2.get() = " << m2.get() << "\n";
		printArr(m2, size2);
		std::cout << "m3.get() = " << m3.get() << "\n";
		printArr(m3, size3);
		std::cout << std::endl;

		std::cout << "MyUniquePtr<TestStruct[]> m4(m1.release())" << std::endl;
		MyUniquePtr<TestStruct[]> m4(m1.release());
		std::cout << "m1.get() = " << m1.get() << "\n";
		std::cout << "m2.get() = " << m2.get() << "\n";
		printArr(m2, size2);
		std::cout << "m3.get() = " << m3.get() << "\n";
		printArr(m3, size3);
		std::cout << "m4.get() = " << m4.get() << "\n";
		printArr(m4, size4);
		std::cout << std::endl;
	}	
}

