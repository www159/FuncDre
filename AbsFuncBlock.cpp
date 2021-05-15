#include "AbsFuncBlock.h"
namespace FuncDre {

	/*
	*	抽象函数
	*/

	AbsFuncBlock::AbsFuncBlock() {
		hash = NULL;
	}

	AbsFuncBlock::~AbsFuncBlock(){
		delete hash;
	}

	int AbsFuncBlock::hashCode() {
		if (hash != NULL) {
			return *hash;
		}

		hash = new int;
		*hash = BASBLOCK;
		return *hash;
	}

	int AbsFuncBlock::getTag() {
		return this->tag;
	}

	void AbsFuncBlock::setTag(int tag) {
		this->tag = tag;
	}

	AbsFuncBlock* AbsFuncBlock::copy() {
		AbsFuncBlock* absFuncBlock = new AbsFuncBlock;
		absFuncBlock->setTag(BASBLOCK);
		return absFuncBlock;
	}

	void AbsFuncBlock::clear() {
		delete hash;
		hash = NULL;
	}



	/*
	*	常数函数块
	*/
	ConFuncBlock::~ConFuncBlock() {

	}

	int ConFuncBlock::hashCode() {
		if (hash != NULL) {
			return *hash;
		}

		hash = new int;
		std::string str = TransForm::d2str(num);
		int i;
		for (*hash = str.length(), i = 0; i < str.length(); ++i)
			*hash = (*hash << 4) ^ (*hash >> 28) ^ str[i];

		*hash %= pr;
		return *hash;
	}

	double ConFuncBlock::getNum() {
		return num;
	}

	void ConFuncBlock::setNum(double num) {//默认清空hash
		this->num = num;
		delete hash;
		hash = NULL;
	}

	ConFuncBlock* ConFuncBlock::copy() {
		ConFuncBlock* conFuncBlock = new ConFuncBlock;
		conFuncBlock->setNum(num);
		conFuncBlock->setTag(tag);
		return conFuncBlock;
	}



	/*
	*	运算函数块
	*/
	AddFuncBlock::AddFuncBlock() {
		FuncContainer = new std::list<AbsFuncBlock*>;
	}

	AddFuncBlock::~AddFuncBlock() {
		for (std::list<AbsFuncBlock*>::iterator it = FuncContainer->begin(); it != FuncContainer->end(); it++) {
			delete* it;
		}
		delete FuncContainer;
	}

	int AddFuncBlock::hashCode() {
		if (hash != NULL) {
			return *hash;
		}

		hash = new int;
		std::list<AbsFuncBlock*>::iterator it = FuncContainer->begin();
		char c = 0;
		*hash = c;
		while (it != FuncContainer->end()) {
			*hash = (*hash << 4) ^ (*hash >> 28) ^ (*it)->hashCode();
			it++;
		}
		*hash %= pr;
		return *hash;
	}

	std::list<AbsFuncBlock*>* AddFuncBlock::getContainer() {
		return this->FuncContainer;
	}

	void AddFuncBlock::addFunc(AbsFuncBlock* absFuncBlock) {
		FuncContainer->push_back(absFuncBlock);
	}

	AddFuncBlock* AddFuncBlock::copy() {
		AddFuncBlock* addFuncBlock = new AddFuncBlock;
		for (auto& it : *FuncContainer) {
			addFuncBlock->addFunc(it->copy());
		}
		addFuncBlock->setTag(tag);
		return addFuncBlock;
	}

	MultFuncBlock::MultFuncBlock() {
		pureHash = NULL;
	}

	MultFuncBlock::~MultFuncBlock() {
		delete pureHash;
	}

	int MultFuncBlock::pureHashCode() {
		if (pureHash != NULL) {
			return *pureHash;
		}
		pureHash = new int;
		int cnt = 0;
		for (auto& it : *FuncContainer) {
			if (it->getTag() == CONBLOCK) {
				cnt++;
				continue;
			}
			*pureHash = (*pureHash << 4) ^ (*pureHash >> 28) ^ it->hashCode();
		}
		if (cnt + 1 == FuncContainer->size()) {//如果是孤单项
			*pureHash = FuncContainer->back()->hashCode();
		}
		*pureHash %= pr;
		return *pureHash;
	}

	MultFuncBlock* MultFuncBlock::copy() {
		MultFuncBlock* multFuncBlock = new MultFuncBlock;
		for (auto& it : *FuncContainer) {
			multFuncBlock->addFunc(it->copy());
		}
		multFuncBlock->setTag(MULTBLOCK);
		return multFuncBlock;
	}

	void MultFuncBlock::clear() {
		delete hash, pureHash;
		hash = NULL;
		pureHash = NULL;

	}

	/*
	*	复合函数块，通用块的阉割版
	*/
	ComFuncBlock::ComFuncBlock() {
		innerFunc = NULL;
	}
	
	ComFuncBlock::~ComFuncBlock() {
		delete innerFunc;
	}

	int ComFuncBlock::hashCode() {
		if (hash != NULL) {
			return *hash;
		}
		hash = new int;
		switch (tag) {
		case LOGBLOCK: {
			*hash = 'l';
			break;
		}
		case SIN: {
			*hash = 's';
			break;
		}
		case COS: {
			*hash = 'c';
			break;
		}
		case TAN: {
			*hash = 't';
			break;
		}
		}
		*hash = (*hash << 4) ^ (*hash >> 28) ^ innerFunc->hashCode();
		*hash %= pr;
		return *hash;
	}

	AbsFuncBlock* ComFuncBlock::getFunc() {
		return innerFunc;
	}

	void ComFuncBlock::setFunc(AbsFuncBlock* absFuncBlock) {
		innerFunc = absFuncBlock;
	}
	
	ComFuncBlock* ComFuncBlock::copy() {
		ComFuncBlock* comFuncBlock = new ComFuncBlock;
		comFuncBlock->setFunc(innerFunc->copy());
		comFuncBlock->setTag(tag);
		return comFuncBlock;
	}
	/*
	*	通用函数块
	*	函数真鸡儿多
	*/

	GnlFuncBlock::GnlFuncBlock() {
		bottomBlock = NULL;
		topBlock = NULL;
	}

	GnlFuncBlock::~GnlFuncBlock() {
		delete bottomBlock;
		delete topBlock;
	}

	int GnlFuncBlock::hashCode() {
		if (hash != NULL) {
			return *hash;
		}

		hash = new int;
		*hash = '^';
		*hash = (*hash << 4) ^ (*hash >> 28) ^ bottomBlock->hashCode();
		*hash = (*hash << 4) ^ (*hash >> 28) ^ topBlock->hashCode();
		*hash %= pr;
		return *hash;
	}

	AbsFuncBlock* GnlFuncBlock::getBottomFunc() {
		return bottomBlock;
	}

	void GnlFuncBlock::setBottomFunc(AbsFuncBlock* absFuncBlock) {
		bottomBlock = absFuncBlock;
	}

	AbsFuncBlock* GnlFuncBlock::getTopFunc() {
		return topBlock;
	}

	void GnlFuncBlock::setTopFunc(AbsFuncBlock* absFuncBlock) {
		topBlock = absFuncBlock;
	}

	GnlFuncBlock* GnlFuncBlock::copy() {
		GnlFuncBlock* gnlFuncBlock = new GnlFuncBlock;
		gnlFuncBlock->setBottomFunc(bottomBlock->copy());
		gnlFuncBlock->setTopFunc(topBlock->copy());
		gnlFuncBlock->setTag(tag);
		return gnlFuncBlock;
	}



	UniPwrBlock::UniPwrBlock() {
		pureHash = NULL;
	}

	UniPwrBlock::~UniPwrBlock() {
		delete pureHash;
	}

	int UniPwrBlock::pureHashCode() {
		if (pureHash != NULL) {
			return *pureHash;
		}

		pureHash = new int;
		switch (tag)
		{
		case CONPWRBLOCK: {
			*pureHash = bottomBlock->hashCode();
			break;
		}
		default: {
			*pureHash = topBlock->hashCode();
			break;
		}
		}

		*pureHash %= pr;
		return *pureHash;
	}

	UniPwrBlock* UniPwrBlock::copy() {
		UniPwrBlock* uniPwrBlock = new UniPwrBlock;
		uniPwrBlock->setTopFunc(topBlock->copy());
		uniPwrBlock->setBottomFunc(bottomBlock->copy());
		uniPwrBlock->setTag(tag);
		return uniPwrBlock;
	}

	void UniPwrBlock::clear() {
		delete hash, pureHash;
		hash = NULL;
		pureHash = NULL;
	}
}

