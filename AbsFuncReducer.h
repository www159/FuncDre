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
namespace FuncDre{
	//�˷���ϵ���ϲ�������tag����
	//֮����мӷ�����
	#define Form(a, b) (std::map<int, ReMode>::value_type(a, b))
	class AbsFuncReducer
{
public:
	
	AbsFuncReducer();

	~AbsFuncReducer();

private:

	typedef void(AbsFuncReducer::* ReMode)(AbsFuncBlock*);

	std::map<int, ReMode>* simpMap;
	//�ӷ�������
	void addSimplize(AbsFuncBlock*);
	//�˷�������
	void multSimplize(AbsFuncBlock*);
	//��ָ������
	void pwrSimplize(AbsFuncBlock*);
	//���ϻ�����
	void comSimplize(AbsFuncBlock*);
	//�˷���ϵ����
	bool hasCon(AbsFuncBlock*);
	//�ǲ��ǻ���Ԫ�أ�
	bool isBas(AbsFuncBlock*);
};
}


