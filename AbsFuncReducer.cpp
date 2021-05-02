#include "AbsFuncReducer.h"
namespace FuncDre {

	AbsFuncReducer::AbsFuncReducer(){
		simpMap = new std::map<int, ReMode>;
		simpMap->insert(Form(ADDBLOCK, &AbsFuncReducer::addSimplify));		//�ӷ�������
		simpMap->insert(Form(MULTBLOCK, &AbsFuncReducer::multSimplify));	//�˷�������	
		simpMap->insert(Form(CONPWRBLOCK, &AbsFuncReducer::pwrSimplify));	
		simpMap->insert(Form(BASPWRBLOCK, &AbsFuncReducer::pwrSimplify));
		simpMap->insert(Form(GNLPWRBLOCK, &AbsFuncReducer::pwrSimplify));
		simpMap->insert(Form(SIN, &AbsFuncReducer::comSimplify));
		simpMap->insert(Form(COS, &AbsFuncReducer::comSimplify));
		simpMap->insert(Form(TAN, &AbsFuncReducer::comSimplify));
		simpMap->insert(Form(LOGBLOCK, &AbsFuncReducer::comSimplify));
		//simpMap->insert(Form(, &AbsFuncReducer:: ));
		//�����Ǻ�������
	}

	AbsFuncReducer::~AbsFuncReducer() {
		simpMap->clear();
		delete simpMap;
	}

	void AbsFuncReducer::reduc(AbsFuncBlock*& absFuncBlock) {
		(this->*simpMap->at(absFuncBlock->getTag()))(absFuncBlock);
	}

	/*
	*	TODO δ���ln����
	*	�ӷ��������˻���ֻ��ͽ���ݹ���ȡ�
	*	�ӷ��˻���������һ���汾��ʵ�֡�
	*/
	void AbsFuncReducer::addSimplify(AbsFuncBlock*& absFuncBlock) {
		auto addFuncBlock = static_cast<AddFuncBlock*>(absFuncBlock);
		auto funcContainer = addFuncBlock->getContainer();
	
		for (auto& it : *funcContainer) {//��auto
			if (isBas(it)) {
				continue;
			}
			(this->*(simpMap->at(it->getTag())))(it);
		}

		//lambda���ʽ
		funcContainer->sort([=](AbsFuncBlock* absFuncBlock1, AbsFuncBlock* absFuncBlock2)->int
			{
				if (absFuncBlock1->getTag() == absFuncBlock2->getTag()) {//��֤ͬ�׳˷����ţ���������
					
					return getPureHash(absFuncBlock1) < getPureHash(absFuncBlock2);
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

			else if ((*lastIt)->getTag() == (*it)->getTag() && getPureHash(*it) == getPureHash(*lastIt)) {//�����ϵ������ȫһ��
				//��ô��Ҫ�����Ƿ��ϵ����
				bool lastHasCon = hasCon(*lastIt),
					tHasCon = hasCon(*it),
					pwrNotSame = false;
				if (lastHasCon && tHasCon) {//�������ϵ�����Ƕ��ǳ˷�
					auto lastCon = static_cast<ConFuncBlock*>(static_cast<MultFuncBlock*>(*lastIt)->getContainer()->front());
					auto tCon = static_cast<ConFuncBlock*>(static_cast<MultFuncBlock*>(*it)->getContainer()->front());
					tCon->setNum(lastCon->getNum() + tCon->getNum());//δ�����ֱ���޸�
					(*it)->clear();//�޸ĺ��������hash
					delete* lastIt;
					funcContainer->erase(lastIt);

				}
				else if (lastHasCon) {
					auto lastCon = static_cast<ConFuncBlock*>(static_cast<MultFuncBlock*>(*lastIt)->getContainer()->front());
					lastCon->setNum(lastCon->getNum() + 1.0);
					(*lastIt)->clear();//���hash
					delete* it;
					funcContainer->erase(it);
					it = lastIt;
				}
				else if (tHasCon) {
					auto tCon = static_cast<ConFuncBlock*>(static_cast<MultFuncBlock*>(*it)->getContainer()->front());
					tCon->setNum(tCon->getNum() + 1.0);
					(*it)->clear();
					delete* lastIt;
					funcContainer->erase(lastIt);
				}
				else {//���������û�г�ϵ������ôֱ������
					MultFuncBlock* multFunc = new MultFuncBlock;
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
				auto frontFunc = static_cast<MultFuncBlock*>(*it)->getContainer()->front();
				if (frontFunc->getTag() == CONBLOCK ) {
					if (static_cast<ConFuncBlock*>(frontFunc)->getNum() == 0) {//ϵ��Ϊ0��ֱ��ɾȥ
						delete* it;
						funcContainer->erase(it++);
						continue;
					}
					else if (static_cast<ConFuncBlock*>(frontFunc)->getNum() == 1) {//���ϵ��Ϊ1������ϵ��
						delete frontFunc;
						auto subContainer = static_cast<MultFuncBlock*>(*it)->getContainer();
						subContainer->pop_front();
					}
				}
			}
			it++;
		}
		backward(absFuncBlock);
	}

	void AbsFuncReducer::multSimplify(AbsFuncBlock*& absFuncBlock) {//��Ҫ��ָ�����ָ������
		auto funcContainer = static_cast<MultFuncBlock*>(absFuncBlock)->getContainer();
		for (auto& it : *funcContainer) {//��һ����
			if (isBas(it)) {
				continue;
			}
			(this->*(simpMap->at(it->getTag())))(it);
		}
		funcContainer->sort([=](AbsFuncBlock* const subFunc1,AbsFuncBlock* const subFunc2)->int {//lambda����
				if (subFunc1->getTag() == subFunc2->getTag()) {
					return getPureHash(subFunc1) < getPureHash(subFunc2);
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
			else if ((*it)->getTag() == (*lastIt)->getTag() && getPureHash(*it) == getPureHash(*lastIt)) {//���ҹ鲢

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
					GnlFuncBlock* conPwrFunc = new UniPwrBlock;
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
					delete conFunc;//�����������ΪhashҲ��ʵ��
					auto tem = *it;
					*it = static_cast<GnlFuncBlock*>(*it)->getBottomFunc()->copy();//�˻�Ϊ������
					delete* it;
					
				}
				else if (conFunc->getNum() == 0) {//����0�˻�
					conFunc = conFunc->copy();//���һ��
					delete* it;
					*it = conFunc;//�˻�Ϊ����
				}
			}
			it++;
		}

		backward(absFuncBlock);
	}

	void AbsFuncReducer::pwrSimplify(AbsFuncBlock*& absFuncBlock) {//�����ϲ�
		auto theFunc = static_cast<GnlFuncBlock*>(absFuncBlock);
		switch (absFuncBlock->getTag()) {
		case CONPWRBLOCK: {//�����ϲ���ϵ������
			auto innerFunc = theFunc->getBottomFunc();
			if (isBas(innerFunc)) {
				break;
			}
			(this->*simpMap->at(innerFunc->getTag()))(innerFunc);//�ڲ�����
			switch (innerFunc->getTag()) {
			case CONPWRBLOCK: {
				if (isBas(innerFunc)) {
					break;
				}
				auto innerGnlFunc = static_cast<UniPwrBlock*>(innerFunc)->copy();//ע�����ݺ���
				auto innerCon = static_cast<ConFuncBlock*>(innerGnlFunc->getTopFunc());//�ڲ���Ҫ����һ�ο���
				auto theCon = static_cast<ConFuncBlock*>(theFunc->getTopFunc());
				innerCon->setNum(theCon->getNum() * innerCon->getNum());
				innerGnlFunc->setTopFunc(innerCon);
				delete theFunc;
				absFuncBlock = innerGnlFunc;
				break;
			}
			case BASPWRBLOCK: case GNLPWRBLOCK: {//��Ҫ��ֹ�˷��׳˷���
				GnlFuncBlock* innerGnlFunc;
				if (innerFunc->getTag() == BASPWRBLOCK) {
					innerGnlFunc = static_cast<UniPwrBlock*>(innerFunc)->copy();
				}
				else {
					innerGnlFunc = static_cast<GnlFuncBlock*>(innerFunc)->copy();
				}
				if (innerGnlFunc->getTopFunc()->getTag() == MULTBLOCK) {
					static_cast<MultFuncBlock*>(innerGnlFunc->getTopFunc())->addFunc(theFunc->getTopFunc()->copy());
					auto innerTopFunc = innerGnlFunc->getTopFunc();
					(this->*simpMap->at(MULTBLOCK))(innerTopFunc);//�ٴλ���
				}
				else {
					auto multFunc = new MultFuncBlock;
					multFunc->setTag(MULTBLOCK);
					multFunc->addFunc(theFunc->getTopFunc()->copy());
					multFunc->addFunc(innerGnlFunc->getTopFunc());
					innerGnlFunc->setTopFunc(multFunc);
				}
				delete theFunc;
				absFuncBlock = innerGnlFunc;
				break;
			}
			default:break;
			}
			break;
		}
		case BASPWRBLOCK: {
			auto innerFunc = theFunc->getTopFunc();
			if (isBas(innerFunc)) {
				return;
			}
			(this->*simpMap->at(innerFunc->getTag()))(innerFunc);
			break;
		}
		case GNLPWRBLOCK: {//TODO��ָ����δ���

			break;
		}
		}
		backward(absFuncBlock);//�˻����
	}

	void AbsFuncReducer::comSimplify(AbsFuncBlock*& absFuncBlock) {
		auto theFunc = static_cast<ComFuncBlock*>(absFuncBlock);
		auto innerFunc = theFunc->getFunc();
		if (isBas(innerFunc)) {
			return;
		}
		(this->*simpMap->at(innerFunc->getTag()))(innerFunc);
		if (theFunc->getFunc() != innerFunc) {//���ݵ�ַ�ж��ڲ������Ƿ��޸�
			theFunc->setFunc(innerFunc);
		}
		switch (absFuncBlock->getTag()) {
		case LOGBLOCK: {//TODO log����

			break;
		}
		case SIN: {

			break;
		}
		case COS: {

			break;
		}
		case TAN: {

			break;
		}
		default:break;
		}
	}

	bool AbsFuncReducer::hasCon(AbsFuncBlock*& absFuncBlock) {
		if (absFuncBlock->getTag() == MULTBLOCK) {
			auto multFuncBlock = static_cast<MultFuncBlock*>(absFuncBlock);
			auto frontFunc = multFuncBlock->getContainer()->front();
			if (frontFunc->getTag() == CONBLOCK) {
				return true;
			}
		}
		return false;
	}

	bool AbsFuncReducer::isBas(AbsFuncBlock*& absFuncBlock) {
		if (absFuncBlock->getTag() == CONBLOCK || absFuncBlock->getTag() == BASBLOCK) {
			return true;
		}
		return false;
	}

	int AbsFuncReducer::getPureHash(AbsFuncBlock* absFuncBlock) {
		switch (absFuncBlock->getTag()) {
		case MULTBLOCK: {
			return static_cast<MultFuncBlock*>(absFuncBlock)->pureHashCode();
		}
		case CONPWRBLOCK: case BASPWRBLOCK:{
			return static_cast<UniPwrBlock*>(absFuncBlock)->pureHashCode();
		}
		default: {
			return absFuncBlock->hashCode();
		}
		}
	}

	void AbsFuncReducer::backward(AbsFuncBlock*& absFuncBlock) {//������ļ�飬����ϲ��˻�֮���ټ�顣
		switch (absFuncBlock->getTag()) {
		case ADDBLOCK:case MULTBLOCK: {
			auto funcContainer = static_cast<AddFuncBlock*>(absFuncBlock)->getContainer();
			if (funcContainer->size() == 0) {//ֻʣ0��
				auto conFunc = new ConFuncBlock;
				conFunc->setNum(0);
				conFunc->setTag(CONBLOCK);
				delete absFuncBlock;
				absFuncBlock = conFunc;
			}
			else if (funcContainer->size() == 1) {//ֻʣ1��
				auto tem = absFuncBlock;
				absFuncBlock = funcContainer->front()->copy();//Ϊ��ɾ���������������
				delete tem;
			}
			break;
		}
		case CONPWRBLOCK: {//��ֹ��������1��0
			auto conPwrFunc = static_cast<GnlFuncBlock*>(absFuncBlock);
			auto conFunc = static_cast<ConFuncBlock*>(conPwrFunc->getTopFunc());
			if (conPwrFunc->getBottomFunc()->getTag() == CONBLOCK) {
				conFunc->setNum(pow(static_cast<ConFuncBlock*>(conPwrFunc->getBottomFunc())->getNum(), conFunc->getNum()));
				conFunc = conFunc->copy();
				delete absFuncBlock;
				absFuncBlock = conFunc;
			}
			else if (conFunc->getNum() == 0) {//���ָ��Ϊ0
				conFunc = conFunc->copy();
				delete absFuncBlock;
				absFuncBlock = conFunc;
			}
			else if (conFunc->getNum() == 1) {
				auto bottomFunc = static_cast<GnlFuncBlock*>(absFuncBlock)->getBottomFunc()->copy();//���һ��
				delete absFuncBlock;//ɾ������
				absFuncBlock = bottomFunc;//�ض���
			}
			break;
		}
		case BASPWRBLOCK : {//ͬ����ֹ��������
			auto conFunc = static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(absFuncBlock)->getBottomFunc());
			if (conFunc->getNum() == 0 || conFunc->getNum() == 1) {//�������Ϊ0��1��Ч����ͬ
				conFunc = conFunc->copy();
				delete absFuncBlock;
				absFuncBlock = conFunc;
			}
			break;
		}
		default: break;//��������ͬ��
		}
	}
}
