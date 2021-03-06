#pragma once

#include<list>
#include"Hashable.h"
#include"TransForm.h"
#include"Clonable.h"
namespace FuncDre {
#define DEFAULTTAG     -1			//默认标签，用于初始化
#define CONBLOCK        0			//常数，无复合运算
#define BASBLOCK        1			//代表x的函数块，无复合运算
#define ADDBLOCK        2			//加法块，需要引入一个函数容器，无减法块。
#define MULTBLOCK       3			//乘法块
#define CONPWRBLOCK     4			//幂函数块
#define BASPWRBLOCK     5			//指数函数块
#define GNLPWRBLOCK     6			//幂指函数块
#define LOGBLOCK        7			//对数函数块
//#define TRIBLOCK        8			//三角函数块
#define SIN				8			//三角函数求导法则
#define COS				9			//余弦
#define TAN				10			//正切
#define ARCBLOCK        11			//反三角函数块，暂不加入，后续细分

	/*
	*抽象函数块。
	*没多大作用，只是存容器方便。
	*可以囊括常数块和基本块
	*/
	class AbsFuncBlock : public Hashable, public Clonable<AbsFuncBlock*>
	{
	public:

		AbsFuncBlock();

		virtual ~AbsFuncBlock();

		int hashCode() override;

		int getTag();

		void setTag(int tag);

		AbsFuncBlock* copy();

	protected:
		
		int tag = DEFAULTTAG;//该块的类型，是基块还是运算块

		int* hash = NULL;

	};



	/*
	*	常数函数块，拥有自己的常数值
	*/
	class ConFuncBlock :public AbsFuncBlock, public Clonable<ConFuncBlock*> {

	public:

		~ConFuncBlock();

		int hashCode() override;

		double getNum();

		void setNum(double num);

		ConFuncBlock* copy();

	private:

		double num;

	};



	/*
	*	通用运算函数块，只有一组作用域。
	*	数据结构是链表，支持快速删除操作
	*/
	class OperFuncBlock:public AbsFuncBlock, public Clonable<OperFuncBlock*> {

	public:

		OperFuncBlock();

		~OperFuncBlock();

		int hashCode() override;

		int pureHashCode();

		//外界获得作用域。
		std::list<AbsFuncBlock*>* getContainer();

		//向作用域中增加函数。
		void addFunc(AbsFuncBlock* absFuncBlock);

		OperFuncBlock* copy();

	private:

		std::list<AbsFuncBlock*>* FuncContainer;

		int* pureHash;

	};



	/*
	*	复合块，包装用。
	*/
	class ComFuncBlock :public AbsFuncBlock,public Clonable<ComFuncBlock*> {
	public:

		ComFuncBlock();

		~ComFuncBlock();

		int hashCode() override;

		AbsFuncBlock* getFunc();

		void setFunc(AbsFuncBlock*);

		ComFuncBlock* copy();

	private:

		AbsFuncBlock* innerFunc;
	};



	/*
	* 一般函数块。
	* 需要保存两组作用域。囊括带参函数块。
	*/
	class GnlFuncBlock :public AbsFuncBlock, public Clonable<GnlFuncBlock*> {

	public:

		GnlFuncBlock();

		~GnlFuncBlock();

		int hashCode() override;

		AbsFuncBlock* getBottomFunc();

		void setBottomFunc(AbsFuncBlock* absFuncBlcok);
		
		//外界获得上作用域。
		AbsFuncBlock* getTopFunc();
		
		//向上作用域中增加函数。
		void setTopFunc(AbsFuncBlock* absFuncBlock);

		GnlFuncBlock* copy();

	private:
		
		AbsFuncBlock* bottomBlock;//下运算函数块

		AbsFuncBlock* topBlock;//上运算函数块

	};
}


