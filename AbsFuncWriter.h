#pragma once
#include"AbsFuncBlock.h"
namespace FuncDre {
	/*
*	��������github��
*	���Կ���ԭ��
*
*	����һ��������֮��������ַ�����
*
*	�����������������ת��Ϊ�ַ������ڻ���֮��
*	��Ҫ����
*	1.	���Ÿò��üӡ�
*	2.	��ϵ��ת���š�
*	3.	�˷���Ԫת������
*	4.	ûʲô�˰ɡ�
*
*	��������һ�¿��ֵĺ���ָ��ͻص�������
*	������������ֱ��д�����߿鼴�ɡ�
*/
	class AbsFuncWriter
	{
	public:
		//��Ҫ�����������顣
		AbsFuncWriter();
		//����һ���ַ��������ں�GUI������
		std::string writeFunc(AbsFuncBlock*);

	private:
		typedef void(AbsFuncWriter::* Mode)(AbsFuncBlock*, std::string&);

		Mode writerContainer[20];
		//����
		void conWriter(AbsFuncBlock*, std::string&);
		//x
		void basWriter(AbsFuncBlock*, std::string&);
		//�ӷ�
		void addWriter(AbsFuncBlock*, std::string&);
		//�˷�
		void multWriter(AbsFuncBlock*, std::string&);
		//��
		void conPwrWriter(AbsFuncBlock*, std::string&);
		//ָ��
		void basPwrWriter(AbsFuncBlock*, std::string&);
		//��ָ
		void gnlPwrWriter(AbsFuncBlock*, std::string&);
		//����
		void logWriter(AbsFuncBlock*, std::string&);
		//����
		void	sinWriter(AbsFuncBlock*, std::string&);
		//����
		void cosWriter(AbsFuncBlock*, std::string&);
		//����
		void tanWriter(AbsFuncBlock*, std::string&);
	};
}

