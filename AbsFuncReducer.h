#pragma once
/*
*	化简器，针对抽象层面的化简。
*	
*	实现的功能：
*	1.	除号化作乘法的逆。
*	2.	合并常系数(加的加，乘的乘，乘幂的乘幂，常数乘幂规定为幂函数)。
*	3.	合并形如x^a， a*x项。
*	
*	要求：使用hash判断完全相等。处理加法和乘法时开个队列。队列清空使用4个
*/
#include "AbsFuncBlock.h"
#include<map>
#include<math.h>
namespace FuncDre{
	//乘法常系数合并，根据tag排序
	//之后进行加法排序
	#define Form(a, b) (std::map<int, ReMode>::value_type(a, b))
	class AbsFuncReducer
{
public:
	
	AbsFuncReducer();

	~AbsFuncReducer();
	//外部交流接口
	void reduc(AbsFuncBlock*&);

private:

	typedef void(AbsFuncReducer::* ReMode)(AbsFuncBlock*&);

	std::map<int, ReMode>* simpMap;

	//加法化简器
	void addSimplify(AbsFuncBlock*&);
	//乘法化简器
	void multSimplify(AbsFuncBlock*&);
	//幂指化简器
	void pwrSimplify(AbsFuncBlock*&);
	//复合化简器
	void comSimplify(AbsFuncBlock*&);
	//乘法有系数吗？
	bool hasCon(AbsFuncBlock*&);
	//是不是基本元素？
	bool isBas(AbsFuncBlock*&);
	//判断纯hash函数是否等价。
	bool isSame(AbsFuncBlock*, AbsFuncBlock*);
	//得到纯哈希
	int getPureHash(AbsFuncBlock*);
	//一般退化检查
	void backward(AbsFuncBlock*&);
};
}


