#pragma once
/*
*	����������Գ������Ļ���
*	
*	ʵ�ֵĹ��ܣ�
*	1.	���Ż����˷����档
*	2.	�ϲ���ϵ��(�ӵļӣ��˵ĳˣ����ݵĳ��ݣ��������ݹ涨Ϊ�ݺ���)��
*	3.	�ϲ�����x^a�� a*x�
*	
*	Ҫ��ʹ��hash�ж���ȫ��ȡ�����ӷ��ͳ˷�ʱ�������С��������ʹ��4��
*/
#include "AbsFuncBlock.h"
#include<map>
#include<math.h>
namespace FuncDre{
	//�˷���ϵ���ϲ�������tag����
	//֮����мӷ�����
	#define Form(a, b) (std::map<int, ReMode>::value_type(a, b))
	class AbsFuncReducer
{
public:
	
	AbsFuncReducer();

	~AbsFuncReducer();
	//�ⲿ�����ӿ�
	void reduc(AbsFuncBlock*&);

private:

	typedef void(AbsFuncReducer::* ReMode)(AbsFuncBlock*&);

	std::map<int, ReMode>* simpMap;

	//�ӷ�������
	void addSimplify(AbsFuncBlock*&);
	//�˷�������
	void multSimplify(AbsFuncBlock*&);
	//��ָ������
	void pwrSimplify(AbsFuncBlock*&);
	//���ϻ�����
	void comSimplify(AbsFuncBlock*&);
	//�˷���ϵ����
	bool hasCon(AbsFuncBlock*&);
	//�ǲ��ǻ���Ԫ�أ�
	bool isBas(AbsFuncBlock*&);
	//�жϴ�hash�����Ƿ�ȼۡ�
	bool isSame(AbsFuncBlock*, AbsFuncBlock*);
	//�õ�����ϣ
	int getPureHash(AbsFuncBlock*);
	//һ���˻����
	void backward(AbsFuncBlock*&);
};
}


