#pragma once
#include"AbsFuncBlock.h"
namespace FuncDre {
	/*
*	至死不用github。
*	试试开闭原理。
*
*	输入一个抽象函数之后，能输出字符串。
*
*	抽象表达器，将抽象块转化为字符串。在化简之后。
*	需要处理：
*	1.	括号该不该加。
*	2.	负系数转负号。
*	3.	乘法逆元转除法。
*	4.	没什么了吧。
*
*	单纯想试一下快乐的函数指针和回调函数。
*	本来函数调用直接写到接线块即可。
*/
	class AbsFuncWriter
	{
	public:
		//需要填满函数数组。
		AbsFuncWriter();
		//传回一个字符串，用于和GUI交流。
		std::string writeFunc(AbsFuncBlock*);

	private:
		typedef void(AbsFuncWriter::* Mode)(AbsFuncBlock*, std::string&);

		Mode writerContainer[20];
		//常数
		void conWriter(AbsFuncBlock*, std::string&);
		//x
		void basWriter(AbsFuncBlock*, std::string&);
		//加法
		void addWriter(AbsFuncBlock*, std::string&);
		//乘法
		void multWriter(AbsFuncBlock*, std::string&);
		//幂
		void conPwrWriter(AbsFuncBlock*, std::string&);
		//指数
		void basPwrWriter(AbsFuncBlock*, std::string&);
		//幂指
		void gnlPwrWriter(AbsFuncBlock*, std::string&);
		//对数
		void logWriter(AbsFuncBlock*, std::string&);
		//正弦
		void	sinWriter(AbsFuncBlock*, std::string&);
		//余弦
		void cosWriter(AbsFuncBlock*, std::string&);
		//正切
		void tanWriter(AbsFuncBlock*, std::string&);
	};
}

