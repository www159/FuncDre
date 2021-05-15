#include "AbsFuncDerivator.h"

namespace FuncDre {
	AbsFuncDerivator::AbsFuncDerivator(){
		dervMap = new std::map<int, ReMode>;
		dervMap->insert(Form(BASBLOCK, &AbsFuncDerivator::basDerivate));
		dervMap->insert(Form(ADDBLOCK, &AbsFuncDerivator::addDerivate));
		dervMap->insert(Form(MULTBLOCK, &AbsFuncDerivator::multDerivate));
		dervMap->insert(Form(CONPWRBLOCK, &AbsFuncDerivator::conPwrDerivate));
		dervMap->insert(Form(BASPWRBLOCK, &AbsFuncDerivator::basPwrDerivate));
		dervMap->insert(Form(GNLPWRBLOCK, &AbsFuncDerivator::gnlPwrDerivate));
		dervMap->insert(Form(SIN, &AbsFuncDerivator::sinDerivate));
		dervMap->insert(Form(COS, &AbsFuncDerivator::cosDerivate));
		dervMap->insert(Form(TAN, &AbsFuncDerivator::tanDerivate));
		dervMap->insert(Form(LOGBLOCK, &AbsFuncDerivator::lnDerivate));
		//dervMap->insert(Form( , &AbsFuncDerivator::));
	}

	AbsFuncDerivator::~AbsFuncDerivator() {
		dervMap->clear();
		delete dervMap;
	}

	void AbsFuncDerivator::dervt(AbsFuncBlock*& absFuncBlock) {
		(this->*(dervMap->at(absFuncBlock->getTag())))(absFuncBlock);
	}

	void AbsFuncDerivator::basDerivate(AbsFuncBlock*& absFuncBlock) {
		auto conFunc = new ConFuncBlock;
		conFunc->setTag(CONBLOCK);
		conFunc->setNum(1.0);
		delete absFuncBlock;
		absFuncBlock = conFunc;
	}

	void AbsFuncDerivator::addDerivate(AbsFuncBlock*& absFuncBlock) {
		auto addFunc = static_cast<AddFuncBlock*>(absFuncBlock);
		for (auto& it : *addFunc->getContainer()) {
			if (isCon(it)) continue;
			(this->*(dervMap->at(it->getTag())))(it);
		}
	}

	void AbsFuncDerivator::multDerivate(AbsFuncBlock*& absFuncBlock) {
		auto multFunc = static_cast<MultFuncBlock*>(absFuncBlock);
		auto funcContainer = multFunc->getContainer();
		AddFuncBlock* addFunc = new AddFuncBlock;
		addFunc->setTag(ADDBLOCK);
		int pos = 0;
		for (auto it = funcContainer->begin(); it != funcContainer->end();pos++, it++) {
			if (isCon(*it)) continue;
			auto temFunc = (*it)->copy();
			(this->*(dervMap->at(temFunc->getTag())))(temFunc);
			auto newMult = new MultFuncBlock;
			newMult->setTag(MULTBLOCK);
			int npos = pos;
			for (auto& anoit : *funcContainer) {
				pos--;
				if (pos == -1) {
					newMult->addFunc(temFunc);
					continue;
				}
				newMult->addFunc(anoit->copy());
			}
			pos = npos;
			addFunc->addFunc(newMult);
		}
		delete multFunc;
		absFuncBlock = addFunc;
	}

	void AbsFuncDerivator::conPwrDerivate(AbsFuncBlock*& absFuncBlock) {

	}

	void AbsFuncDerivator::basPwrDerivate(AbsFuncBlock*& absFuncBlock) {

	}

	void AbsFuncDerivator::gnlPwrDerivate(AbsFuncBlock*& absFuncBlock) {

	}

	void AbsFuncDerivator::sinDerivate(AbsFuncBlock*& absFuncBlock) {
		auto sinFunc = static_cast<ComFuncBlock*>(absFuncBlock);
		sinFunc->setTag(COS);

		auto innerFunc = sinFunc->getFunc()->copy();
		(this->*(dervMap->at(innerFunc->getTag())))(innerFunc);
		//合并
		absFuncBlock = combMerge(innerFunc, sinFunc);
	}

	void AbsFuncDerivator::cosDerivate(AbsFuncBlock*& absFuncBlock) {
		auto cosFunc = static_cast<ComFuncBlock*>(absFuncBlock);
		cosFunc->setTag(SIN);

		auto innerFunc = cosFunc->getFunc()->copy();
		(this->*(dervMap->at(innerFunc->getTag())))(innerFunc);
		//合并
		absFuncBlock = combMerge(innerFunc, cosFunc);

		auto conFunc = new ConFuncBlock;
		conFunc->setTag(CONBLOCK);
		conFunc->setNum(-1.0);
		static_cast<MultFuncBlock*>(absFuncBlock)->addFunc(conFunc);
	}

	void AbsFuncDerivator::tanDerivate(AbsFuncBlock*& absFuncBlock) {
		auto tanFunc = static_cast<ComFuncBlock*>(absFuncBlock);
		tanFunc->setTag(COS);
		
		//tan转cos^-2，
		auto basPwrFunc = new UniPwrBlock;
		basPwrFunc->setTag(BASPWRBLOCK);
		auto conFunc = new ConFuncBlock;
		conFunc->setTag(CONBLOCK);
		conFunc->setNum(-2.0);
		basPwrFunc->setTopFunc(conFunc);
		basPwrFunc->setBottomFunc(tanFunc);
		//内层求导
		auto innerFunc = tanFunc->getFunc()->copy();
		(this->*(dervMap->at(innerFunc->getTag())))(innerFunc);

		absFuncBlock = combMerge(innerFunc, basPwrFunc);
	}

	void AbsFuncDerivator::lnDerivate(AbsFuncBlock*& absFuncBlock) {
		auto lnFunc = static_cast<ComFuncBlock*>(absFuncBlock);
		
		auto basPwrFunc = new UniPwrBlock;
		basPwrFunc->setTag(BASPWRBLOCK);
		auto conFunc = new ConFuncBlock;
		conFunc->setTag(CONBLOCK);
		conFunc->setNum(-1.0);
		basPwrFunc->setTopFunc(conFunc);
		basPwrFunc->setBottomFunc(lnFunc->getFunc()->copy());

		auto innerFunc = lnFunc->getFunc()->copy();
		(this->*(dervMap->at(innerFunc->getTag())))(innerFunc);

		absFuncBlock = combMerge(innerFunc, basPwrFunc);
		delete lnFunc;
	}

	bool AbsFuncDerivator::isCon(AbsFuncBlock*& absFucBlock) {
		switch (absFucBlock->getTag()) {
		case CONBLOCK: {
			return true;
		}
		}
		return false;
	}

	 AbsFuncBlock* AbsFuncDerivator::combMerge(AbsFuncBlock* innerFunc, AbsFuncBlock* theFunc) {
		if (innerFunc->getTag() == MULTBLOCK) {
			auto tempFunc = static_cast<MultFuncBlock*>(innerFunc);
			tempFunc->addFunc(theFunc);
			return innerFunc;
		}
		else {
			auto multFunc = new MultFuncBlock;
			multFunc->setTag(MULTBLOCK);
			multFunc->addFunc(innerFunc);
			multFunc->addFunc(theFunc);
			return multFunc;
		}
	}

}
