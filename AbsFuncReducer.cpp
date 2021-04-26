#include "AbsFuncReducer.h"
namespace FuncDre {

	AbsFuncReducer::AbsFuncReducer(){
		simpMap = new std::map<int, ReMode>;
		simpMap->insert(Form(ADDBLOCK, &AbsFuncReducer::addSimplize));		//�ӷ�������
		simpMap->insert(Form(MULTBLOCK, &AbsFuncReducer::multSimplize));	//�˷�������	
		simpMap->insert(Form(CONPWRBLOCK, &AbsFuncReducer::pwrSimplize));	
		simpMap->insert(Form(BASPWRBLOCK, &AbsFuncReducer::pwrSimplize));
		simpMap->insert(Form(GNLPWRBLOCK, &AbsFuncReducer::pwrSimplize));
		simpMap->insert(Form(SIN, &AbsFuncReducer::comSimplize));
		simpMap->insert(Form(COS, &AbsFuncReducer::comSimplize));
		simpMap->insert(Form(TAN, &AbsFuncReducer::comSimplize));
		simpMap->insert(Form(LOGBLOCK, &AbsFuncReducer::comSimplize));
		//simpMap->insert(Form(, &AbsFuncReducer:: ));
		//�����Ǻ�������
	}

	AbsFuncReducer::~AbsFuncReducer() {
		simpMap->clear();
		delete simpMap;
	}

	/*
	*	TODO δ���ln����
	*	�ӷ��������˻���ֻ��ͽ���ݹ���ȡ�
	*	�ӷ��˻���������һ���汾��ʵ�֡�
	*/
	void AbsFuncReducer::addSimplize(AbsFuncBlock* absFuncBlock) {
		auto addFuncBlock = static_cast<OperFuncBlock*>(absFuncBlock);
		auto funcContainer = addFuncBlock->getContainer();
	
		for (auto& it : *funcContainer) {//��auto
			if (isBas(it)) {
				continue;
			}
			(this->*(simpMap->at(it->getTag())))(it);
		}

		//lambda���ʽ
		funcContainer->sort([](AbsFuncBlock* const absFuncBlock1, AbsFuncBlock* const absFuncBlock2)
			{
				if (absFuncBlock1->getTag() == absFuncBlock2->getTag()) {//��֤ͬ�׳˷����ţ���������
					return absFuncBlock1->hashCode() < absFuncBlock2->hashCode();
				}
				else {
					return absFuncBlock1->getTag() < absFuncBlock2->getTag();
				}
			}
		);
		
		auto it = funcContainer->begin();
		auto lastIt = it;
		int lastTag = (*it)->getTag();
		int tag;
		it++;
		while (it != funcContainer->end()) {
			tag = (*it)->getTag();
			if (lastTag == CONBLOCK && tag == CONBLOCK) {//���ǳ��������Һϲ�

				auto lastConFunc = static_cast<ConFuncBlock*>(*lastIt);//�������Ϊ0��ֱ��continue
				auto conFunc = static_cast<ConFuncBlock*>(*it);
				conFunc->setNum(lastConFunc->getNum() + conFunc->getNum());
				delete lastConFunc;//�ͷ��ڴ�
				funcContainer->erase(lastIt);
			}

			else if ((*lastIt)->hashCode() == (*it)->hashCode()) {//�����ϵ������ȫһ��
				//��ô��Ҫ�����Ƿ��ϵ����
				bool lastHasCon = hasCon(*lastIt),
					tHasCon = hasCon(*it);
				if (lastHasCon && tHasCon) {
					auto lastCon = static_cast<ConFuncBlock*>(static_cast<OperFuncBlock*>(*lastIt)->getContainer()->front());
					auto tCon = static_cast<ConFuncBlock*>(static_cast<OperFuncBlock*>(*it)->getContainer()->front());
					tCon->setNum(lastCon->getNum() + tCon->getNum());
					delete* lastIt;
					funcContainer->erase(lastIt);
						
				}
				else if (lastHasCon) {
					auto lastCon = static_cast<ConFuncBlock*>(static_cast<OperFuncBlock*>(*lastIt)->getContainer()->front());
					lastCon->setNum(lastCon->getNum() + 1.0);
					delete* it;
					funcContainer->erase(it);
					it = lastIt;
				}
				else if (tHasCon) {
					auto tCon = static_cast<ConFuncBlock*>(static_cast<OperFuncBlock*>(*it)->getContainer()->front());
					tCon->setNum(tCon->getNum() + 1.0);
					delete* lastIt;
					funcContainer->erase(lastIt);
				}
				else{//���������û�г�ϵ������ôֱ������
					OperFuncBlock* multFunc = new OperFuncBlock;
					multFunc->setTag(MULTBLOCK);
					ConFuncBlock* conFunc = new ConFuncBlock;
					conFunc->setTag(CONBLOCK);
					conFunc->setNum(2.0);
					multFunc->addFunc(conFunc);
					multFunc->addFunc(*it);
					delete* lastIt;//ɾȥǰһ��
					funcContainer->erase(lastIt);
					*it = multFunc;//��it�滻��multFunc
						
				}
			}

			lastIt = it;
			lastTag = tag;//������ʷ��Ϣ
			it++;
		}

		it = funcContainer->begin();
		while (it != funcContainer->end()) {//ȥ����������˷��˻����
			tag = (*it)->getTag();
			if (tag == CONBLOCK && static_cast<ConFuncBlock*>(*it)->getNum() == 0) {
				delete* it;
				funcContainer->erase(it++);
				continue;
			}
			else if (tag == MULTBLOCK) {
				auto frontFunc = static_cast<OperFuncBlock*>(*it)->getContainer()->front();
				if (frontFunc->getTag() == CONBLOCK ) {
					if (static_cast<ConFuncBlock*>(frontFunc)->getNum() == 0) {//ϵ��Ϊ0��ֱ��ɾȥ
						delete* it;
						funcContainer->erase(it++);
						continue;
					}
					else if (static_cast<ConFuncBlock*>(frontFunc)->getNum() == 1) {//���ϵ��Ϊ1������ϵ��
						delete frontFunc;
						auto subContainer = static_cast<OperFuncBlock*>(*it)->getContainer();
						subContainer->pop_front();
					}
				}
			}
			it++;
		}
	}

	void AbsFuncReducer::multSimplize(AbsFuncBlock* absFuncBlock) {//��Ҫ��ָ�����ָ������
		auto funcContainer = static_cast<OperFuncBlock*>(absFuncBlock)->getContainer();
		for (auto& it : *funcContainer) {//��һ����
			if (isBas(it)) {
				continue;
			}
			(this->*(simpMap->at(it->getTag())))(it);
		}
		funcContainer->sort([](AbsFuncBlock* const subFunc1,AbsFuncBlock* const subFunc2) {//lambda����
				if (subFunc1->getTag() == subFunc2->getTag()) {
					return subFunc1->hashCode() < subFunc2->hashCode();
				}
				else {
					return subFunc1->getTag() < subFunc2->getTag();
				}
			}
		);

		auto it = funcContainer->begin();
		auto lastIt = it;
		int tag, lastTag = (*lastIt)->getTag();
		it++;

		while (it != funcContainer->end()) {//�ϲ�ͬ����
			tag = (*it)->getTag();
			if (lastTag == CONBLOCK && tag == CONBLOCK) {//�ϲ�����
				auto conFunc = static_cast<ConFuncBlock*>(*it);
				auto lastConFunc = static_cast<ConFuncBlock*>(*lastIt);
				conFunc->setNum(conFunc->getNum() * lastConFunc->getNum());
				delete* lastIt;
				funcContainer->erase(lastIt);
			}
			else if ((*lastIt)->hashCode() == (*it)->hashCode()) {//���ҹ鲢

				if (lastTag == BASPWRBLOCK && tag == BASPWRBLOCK) {//����ָ���������������
					auto conFunc = static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(*it)->getBottomFunc());
					auto lastConFunc = static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(*lastIt)->getBottomFunc());
					conFunc->setNum(conFunc->getNum() * lastConFunc->getNum());
					delete* lastIt;
					funcContainer->erase(lastIt);
				}
				else if (lastTag == CONPWRBLOCK && tag == CONPWRBLOCK) {//����ͬ���ݺ���
					auto conFunc = static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(*it)->getTopFunc());
					auto lastConFunc = static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(*lastIt)->getTopFunc());
					conFunc->setNum(conFunc->getNum() + lastConFunc->getNum());//�������
					delete* lastIt;
					funcContainer->erase(lastIt);
				}
				else if (lastTag == CONPWRBLOCK && tag != BASPWRBLOCK) {//��һ��ָ��Ϊ1���ݺ���
					auto lastConFunc = static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(*lastIt)->getTopFunc());
					lastConFunc->setNum(lastConFunc->getNum() + 1.0);
					delete* it;
					funcContainer->erase(it);
					it = lastIt;
				}
				else if (lastTag != BASPWRBLOCK && tag == CONPWRBLOCK) {//ͬ��
					auto conFunc = static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(*it)->getTopFunc());
					conFunc->setNum(conFunc->getNum() + 1.0);
					delete* lastIt;
					funcContainer->erase(lastIt);
				}
				else {//����x*x���ͣ���Ҫnewһ���ݺ���
					GnlFuncBlock* conPwrFunc = new GnlFuncBlock;
					conPwrFunc->setTag(CONPWRBLOCK);
					conPwrFunc->setBottomFunc(*it);
					ConFuncBlock* conFunc = new ConFuncBlock;
					conFunc->setTag(CONBLOCK);
					conFunc->setNum(2.0);
					conPwrFunc->setTopFunc(conFunc);
					delete* lastIt;
					funcContainer->erase(lastIt);
					*it = conPwrFunc;
				}
			}
			lastIt = it;
			lastTag = (*lastIt)->getTag();
			it++;
		}

		//�˷��˻���飬0��ֻ������ںϲ�ͬ����֮��
		if (funcContainer->front()->getTag() == CONBLOCK) {
			auto conFunc = static_cast<ConFuncBlock*>(funcContainer->front()->copy());
			if (conFunc->getNum() == 0) {//ֱ���˻�Ϊ����0
				conFunc = conFunc->copy();//��Ҫ���һ�ݳ�����
				delete absFuncBlock;
				absFuncBlock = conFunc;
				return;//ֱ�ӷ���
			}
		}
		
		it = funcContainer->begin();
		while (it != funcContainer->end()) {//ɾ��1����������˻����
			tag = (*it)->getTag();
			if (tag == CONBLOCK && static_cast<ConFuncBlock*>(*it)->getNum() == 1) {
				delete* it;
				funcContainer->erase(it++);
				continue;
			}
			else if (tag == BASPWRBLOCK && static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(*it)->getBottomFunc())->getNum() == 1) {
				//ָ������1�˻�
				delete* it;
				funcContainer->erase(it++);
				continue;
			}
			else if (tag == CONPWRBLOCK) {
				auto conFunc = static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(*it)->getTopFunc());
				if (conFunc->getNum() == 1) {//����1�˻�
					delete conFunc;
					*it = static_cast<GnlFuncBlock*>(*it)->getBottomFunc();//�˻�Ϊ������
				}
				else if (conFunc->getNum() == 0) {//����0�˻�
					conFunc = conFunc->copy();//���һ��
					delete* it;
					*it = conFunc;//�˻�Ϊ����
				}
			}
			it++;
		}
	}

	bool AbsFuncReducer::hasCon(AbsFuncBlock* absFuncBlock) {
		if (absFuncBlock->getTag() == MULTBLOCK) {
			auto multFuncBlock = static_cast<OperFuncBlock*>(absFuncBlock);
			auto frontFunc = multFuncBlock->getContainer()->front();
			if (frontFunc->getTag() == CONBLOCK) {
				return true;
			}
		}
		return false;
	}

	bool AbsFuncReducer::isBas(AbsFuncBlock* absFuncBlock) {
		if (absFuncBlock->getTag() == CONBLOCK || absFuncBlock->getTag() == BASBLOCK) {
			return true;
		}
		return false;
	}
}
