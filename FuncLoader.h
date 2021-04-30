#pragma once
#include "AbsFuncBlock.h"
#include "AbsFuncReducer.h"
#include "TransForm.h"
#include<queue>
#include<regex>
#include<stack>
namespace FuncDre {
	/*
	*	���ߺ�����������
	*	�����������������ַ���ת�ɺ�����Ĺ��̡�
	*
	*	������ַ���Ӧ���������¹���
	*	1.	�ϸ�����˺���ȫ�ԡ�
	*	2.	�����ȼ��г��ֵ����ȼ��ϸ�����š�
	*
	*	ʵ�ֵĹ��ܣ�
	*	1.	������֮�󷵻�һ���ܺ����顣
	*	2.	�����ܶ�����ӵ�м��غͼ���-������
	*		(�������)��
	*	3.	���ڸ������Ķ�ȡ�����򻯴���һ�С�
	*	4.	���������ӷ�����
	*
	*	��Ϊ����Ҫ����new�����пռ������ջ��
	*	��
	*
	*	���Ͽ�Ĵ���
	*	1.	����ָ�ͣ��ݹ�Loader��
	*	2.	��ָ�ͣ�ֱ�Ӹ�
	*
	*	�滻���򣺸�����ΪC, ���Ͽ�ΪU, ������Ϊx��
	* 
	*	��׺���ʽ�Ǹ��ö�����
	*	1.	������ȸ����и��򲻸ı�Ҷ�ӽڵ��˳��
	*	2.	�Զ�ȥ���������š�
	* 
	*/
#define		CON		0		//����
#define		UNI		1		//���Ͽ�
#define		VAR		3		//����
#define		ADD		10		//�Ӻ�
#define		MULT	20		//�˺�
#define		DIV		21		//����
#define		PWR		30		//����
#define		LINC	40		//������
#define		RINC	41		//������
	class FuncLoader
	{

	public:

		FuncLoader();

		//�����ڵĶ�����ɾ����ΪҪ�����ڴ档
		~FuncLoader();

		//������պ����顣
		AbsFuncBlock* getFinalFunc();

		//Ԥ�������ַ������أ���Ϊ��׺���ʽ�ʹ��ַ���ջ������ָ���Ͽ���Ҫ�ݹ顣
		void load(std::string str);

	private:

		AbsFuncBlock* finalAbsFuncBlock;		//�����ܺ����顣

		std::queue<AbsFuncBlock*>* conQue;		//��˳�򸡵�����queue,֧��upperbound��

		std::queue<AbsFuncBlock*>* comQue;		//�渴�Ͽ��queue��

		std::queue<int>* bacQue;				//���׺���ʽ��list,��bacQue�á�

		std::stack<AddFuncBlock*>* AddStack;	//��ӷ������ʱջ��

		std::stack<MultFuncBlock*>* MulStack;	//��˷������ʱջ��


		//��һ��ת��������������ʽ�õ�˳�������У��������ȫ�˺š�
		void trans1(std::string&);

		//�ڶ���ת������Ԥ����õ���ǳ�����Ϊ��׺���ʽ��
		void trans2(std::string);

		//������ת��������׺���ʽת��Ϊ�顣�ڵ��ϸ�ڣ���Ҫ�������ˣ����ӡ�
		void trans3();

		//�ж���������ȼ�������true:sign1 < sign2��
		bool isSmaller(int, int);

		//�ַ����궨���ӳ�䡣
		int isSign(char c);

		//�ҵ���һ���Ӹ��Ͽ鲢���ء�ֱ�����������Ӹ��Ͽ�����滻�����顣
		void workSubComb(std::string& );

		//ģ��java��poll������
		AbsFuncBlock* pollBlockInStack(int, std::stack<AbsFuncBlock*>*);

		void pollHisInSignStack(int&, int&, std::stack<int>*);
	};
}


