#pragma once
#include "AbsFuncBlock.h"
#include<map>
/*
*	求导器，针对抽象层面的求导
*/
namespace FuncDre {

#define Form(a, b) (std::map<int, ReMode>::value_type(a, b))
	class AbsFuncDerivator
	{
	public:
		AbsFuncDerivator();

		~AbsFuncDerivator();

		void dervt(AbsFuncBlock*&);

	private:

		typedef void(AbsFuncDerivator::* ReMode)(AbsFuncBlock*&);

		std::map<int, ReMode>* dervMap;

		void basDerivate(AbsFuncBlock*&);

		void addDerivate(AbsFuncBlock*&);

		void multDerivate(AbsFuncBlock*&);

		void conPwrDerivate(AbsFuncBlock*&);

		void basPwrDerivate(AbsFuncBlock*&);

		void gnlPwrDerivate(AbsFuncBlock*&);

		void sinDerivate(AbsFuncBlock*&);

		void cosDerivate(AbsFuncBlock*&);

		void tanDerivate(AbsFuncBlock*&);

		void lnDerivate(AbsFuncBlock*&);

		bool isCon(AbsFuncBlock*&);

	
		/*
		*	AbsFuncBlock* combMerge(innerFunc, theFunc);
		*/
		AbsFuncBlock* combMerge(AbsFuncBlock*, AbsFuncBlock*);
	};
}
