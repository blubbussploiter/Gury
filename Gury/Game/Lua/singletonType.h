#pragma once

namespace RBX
{
	template<typename Real>
	class Singleton
	{
	private:
		static Real* singleton;
	public:

		static Real* get()
		{
			return singleton;
		}
	};

#define DEFINE_SINGLETON(SingleClass) SingleClass* RBX::Singleton<SingleClass>::singleton = new SingleClass()
}