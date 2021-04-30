#pragma once
#include "AbsFuncBlock.h"
#include "AbsFuncReducer.h"
#include "TransForm.h"
#include<queue>
#include<regex>
#include<stack>
namespace FuncDre {
	/*
	*	抽线函数加载器。
	*	函数加载器作用在字符串转成函数块的过程。
	*
	*	输入的字符串应当满足以下规则：
	*	1.	严格满足乘号完全性。
	*	2.	高优先级中出现低优先级严格加括号。
	*
	*	实现的功能：
	*	1.	加载完之后返回一个总函数块。
	*	2.	化简功能独立。拥有加载和加载-化简功能
	*		(方便测试)。
	*	3.	对于浮点数的读取，正则化处理一切。
	*	4.	减法当作加法处理。
	*
	*	因为不需要反复new，队列空间管理交给栈处
	*	理。
	*
	*	复合块的处理：
	*	1.	非幂指型，递归Loader。
	*	2.	幂指型，直接刚
	*
	*	替换规则：浮点数为C, 复合块为U, 常变量为x。
	* 
	*	后缀表达式是个好东西：
	*	1.	后根序，先根序，中根序不改变叶子节点的顺序。
	*	2.	自动去除冗余括号。
	* 
	*/
#define		CON		0		//常数
#define		UNI		1		//复合块
#define		VAR		3		//变量
#define		ADD		10		//加号
#define		MULT	20		//乘号
#define		DIV		21		//除号
#define		PWR		30		//乘幂
#define		LINC	40		//左括号
#define		RINC	41		//右括号
	class FuncLoader
	{

	public:

		FuncLoader();

		//容器内的对象不能删，因为要共用内存。
		~FuncLoader();

		//获得最终函数块。
		AbsFuncBlock* getFinalFunc();

		//预处理。从字符串加载，化为中缀表达式和存字符的栈。非幂指复合块需要递归。
		void load(std::string str);

	private:

		AbsFuncBlock* finalAbsFuncBlock;		//最终总函数块。

		std::queue<AbsFuncBlock*>* conQue;		//存顺序浮点数的queue,支持upperbound。

		std::queue<AbsFuncBlock*>* comQue;		//存复合块的queue。

		std::queue<int>* bacQue;				//存后缀表达式的list,当bacQue用。

		std::stack<AddFuncBlock*>* AddStack;	//存加法块的临时栈。

		std::stack<MultFuncBlock*>* MulStack;	//存乘法块的临时栈。


		//第一次转化，利用正则表达式得到顺序常数队列，并添加完全乘号。
		void trans1(std::string&);

		//第二次转化，将预处理得到的浅层抽象化为后缀表达式。
		void trans2(std::string);

		//第三次转化，将后缀表达式转化为块。亿点点细节，需要考虑连乘，连加。
		void trans3();

		//判断运算符优先级，返回true:sign1 < sign2。
		bool isSmaller(int, int);

		//字符到宏定义的映射。
		int isSign(char c);

		//找到第一个子复合块并返回。直到生成所有子复合块后再替换常数块。
		void workSubComb(std::string& );

		//模仿java的poll操作。
		AbsFuncBlock* pollBlockInStack(int, std::stack<AbsFuncBlock*>*);

		void pollHisInSignStack(int&, int&, std::stack<int>*);
	};
}


