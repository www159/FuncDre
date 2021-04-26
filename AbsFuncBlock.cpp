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

		*hash %= 99999989;
		return *hash;
	}

	double ConFuncBlock::getNum() {
		return num;
	}

	void ConFuncBlock::setNum(double num) {
		this->num = num;
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
	OperFuncBlock::OperFuncBlock() {
		FuncContainer = new std::list<AbsFuncBlock*>;
	}

	OperFuncBlock::~OperFuncBlock() {
		for (std::list<AbsFuncBlock*>::iterator it = FuncContainer->begin(); it != FuncContainer->end(); it++) {
			delete* it;
		}
		delete FuncContainer;
	}

	int OperFuncBlock::hashCode() {
		if (hash != NULL) {
			return *hash;
		}

		hash = new int;
		std::list<AbsFuncBlock*>::iterator it = FuncContainer->begin();
		char c;
		if (tag == ADDBLOCK) {
			c = '+';
		}
		if (tag == MULTBLOCK) {
			c = '*';
			while((*it)->getTag() == CONBLOCK) {//TODO 测试while和if的区别
				it++;//乘法hashcode需要忽略系数
			}
			
		}
		*hash = c;
		while (it != FuncContainer->end()) {
			*hash = (*hash << 4) ^ (*hash >> 28) ^ (*it)->hashCode();
			it++;
		}
		*hash %= 99999989;
		return *hash;
	}

	std::list<AbsFuncBlock*>* OperFuncBlock::getContainer() {
		return this->FuncContainer;
	}

	void OperFuncBlock::addFunc(AbsFuncBlock* absFuncBlock) {
		FuncContainer->push_back(absFuncBlock);
	}

	OperFuncBlock* OperFuncBlock::copy() {
		OperFuncBlock* operFuncBlock = new OperFuncBlock;
		for (auto& it : *FuncContainer) {
			operFuncBlock->addFunc(it->copy());
		}
		operFuncBlock->setTag(tag);
		return operFuncBlock;
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
		switch (tag) {
		case CONPWRBLOCK: {
			*hash = (*hash << 4) ^ (*hash >> 28) ^ bottomBlock->hashCode();
			break;
		}
		case BASPWRBLOCK: {
			*hash = (*hash << 4) ^ (*hash >> 28) ^ topBlock->hashCode();
			break;
		}
		case GNLPWRBLOCK: {
			*hash = (*hash << 4) ^ (*hash >> 28) ^ bottomBlock->hashCode();
			*hash = (*hash << 4) ^ (*hash >> 28) ^ topBlock->hashCode();
			break;
		}
		}
		*hash %= 99999989;
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
}