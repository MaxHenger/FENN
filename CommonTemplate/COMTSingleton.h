#ifndef COMT_COMTSINGLETON_H
#define COMT_COMTSINGLETON_H

#include <cassert>

#include "COMTConfig.h"

namespace comt
{
	template <typename T>
	class SingletonLazy
	{
	public:
		static T& Get() 
		{
			static T instance;
			return instance;
		}

	private:
		Singleton() {};
		Singleton(const Singleton<T> &val) {};
		void operator = (const Singleton<T> &val) {};
	};

	template <typename T>
	class SingletonDynamic
	{
	public:
		SingletonDynamic()
			: m_pT(NULL)
		{
		}

		SingletonDynamic()
		{
			if (m_pT) {
				delete m_pT;
				m_pT = NULL;
			}
		}

		static void Instantiate()
		{
			assert(!m_pT);
			m_pT = new T;
		}

		static void Instantiate(const T &t)
		{
			assert(!m_pT);
			m_pT = new T(t);
		}

		static T* Get()
		{
			return m_pT;
		}

	private:
		static T *m_pT;

		SingletonDynamic(const SingletonDynamic<T> &val) {};
		void operator SingletonDynamic(const SingletonDynamic<T> &val) {};
	};
}

#endif