#pragma once

#include<list>
#include"Hashable.h"
#include"TransForm.h"
#include"Clonable.h"
namespace FuncDre {
#define DEFAULTTAG     -1			//Ĭ�ϱ�ǩ�����ڳ�ʼ��
#define CONBLOCK        0			//�������޸�������
#define BASBLOCK        1			//����x�ĺ����飬�޸�������
#define ADDBLOCK        2			//�ӷ��飬��Ҫ����һ�������������޼����顣
#define MULTBLOCK       3			//�˷���
#define CONPWRBLOCK     4			//�ݺ�����
#define BASPWRBLOCK     5			//ָ��������
#define GNLPWRBLOCK     6			//��ָ������
#define LOGBLOCK        7			//����������
//#define TRIBLOCK        8			//���Ǻ�����
#define SIN				8			//���Ǻ����󵼷���
#define COS				9			//����
#define TAN				10			//����
#define ARCBLOCK        11			//�����Ǻ����飬�ݲ����룬����ϸ��

	/*
	*�������顣
	*û������ã�ֻ�Ǵ��������㡣
	*��������������ͻ�����
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
		
		int tag = DEFAULTTAG;//�ÿ�����ͣ��ǻ��黹�������

		int* hash = NULL;

	};



	/*
	*	���������飬ӵ���Լ��ĳ���ֵ
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
	*	ͨ�����㺯���飬ֻ��һ��������
	*	���ݽṹ������֧�ֿ���ɾ������
	*/
	class OperFuncBlock:public AbsFuncBlock, public Clonable<OperFuncBlock*> {

	public:

		OperFuncBlock();

		~OperFuncBlock();

		int hashCode() override;

		//�����������
		std::list<AbsFuncBlock*>* getContainer();

		//�������������Ӻ�����
		void addFunc(AbsFuncBlock* absFuncBlock);

		OperFuncBlock* copy();

	protected:

		std::list<AbsFuncBlock*>* FuncContainer;

	};



	/*
	*	���Ͽ飬��װ�á�
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
	* һ�㺯���顣
	* ��Ҫ���������������������κ����顣
	*/
	class GnlFuncBlock :public AbsFuncBlock, public Clonable<GnlFuncBlock*> {

	public:

		GnlFuncBlock();

		~GnlFuncBlock();

		int hashCode() override;

		AbsFuncBlock* getBottomFunc();

		void setBottomFunc(AbsFuncBlock* absFuncBlcok);
		
		//�������������
		AbsFuncBlock* getTopFunc();
		
		//���������������Ӻ�����
		void setTopFunc(AbsFuncBlock* absFuncBlock);

		GnlFuncBlock* copy();

	private:
		
		AbsFuncBlock* bottomBlock;//�����㺯����

		AbsFuncBlock* topBlock;//�����㺯����

	};
}


