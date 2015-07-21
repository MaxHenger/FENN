#include <COMTSingleton.h>
#include <iostream>

class Test
{
public:
	void Set(int i) {
		m_i = i;
	}

	int Get() const {
		return m_i;
	}

protected:
	int m_i;
};

typedef comt::SingletonLazy<Test> test;

int main(int argc, char *pargv[])
{
	std::cout << "Hello world" << std::endl;

	{
		test::Get().Set(1050);
	}

	std::cout << test::Get().Get() << std::endl;
	test::Get().Set(5);
	std::cout << test::Get().Get() << std::endl;
	test::Get().Set(100);
	std::cout << test::Get().Get() << std::endl;
	std::cin.get();
	return 0;
}