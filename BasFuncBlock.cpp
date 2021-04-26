#include"BasFuncBlock.h"
namespace BasFuncBlock {
	using namespace BasFunc;
	/**************************************************************************/
	void AbsFuncBlock::setTag(int t) {
		this->tag = t;
	}


	void AbsFuncBlock::load(AbsFuncBlock* afb0, AbsFuncBlock* afb1) {
		afb[0] = afb0;
		afb[1] = afb1;
	}


	int AbsFuncBlock::getTag(){
		return tag;
	}


	bool AbsFuncBlock::isUnit() {
		switch (tag)
		{
		case ADD: case MULT: case MINU:case DIV: {
			return false;
			break;
		}
		default:return true;
			break;
		}
		return true;
	}


	bool AbsFuncBlock::isNorm() {
		return isNor;
	}


	bool AbsFuncBlock::isZero() {
		return isZer;
	}


	bool AbsFuncBlock::isOne() {
		return isOn;
	}


	bool AbsFuncBlock::isMult() {
		return isMul;
	}


	void AbsFuncBlock::copySym(AbsFuncBlock* afb1) {
		afb1->setSym(isNor, isZer, isMul, isOn);
	}


	double AbsFuncBlock::getLparam() {
		return this->lparam;
	}
	
	void AbsFuncBlock::setLparam(double s) {
		this->lparam = s;
	}

	AbsFuncBlock* AbsFuncBlock::trans(AbsFuncBlock* mfb) {
		int tag = mfb->getTag();
		if (mfb->isNorm() || mfb->isOne()) {
			return static_cast<UnitFuncBlock*>(static_cast<AbsFuncBlock*>(mfb));
		}
		else if (mfb->isMult() && tag == ADD) {
			return static_cast<AddFuncBlock*>(static_cast<AbsFuncBlock*>(mfb));
		}
		else {
			return mfb;
		}
	}

	bool AbsFuncBlock::isExist(AbsFuncBlock* afb) {
		if (afb == NULL) {
			return false;
		}
		if (afb->isZero()) {
			delete afb;
			afb = NULL;
			return false;
		}
		return true;
	}


	void AbsFuncBlock::setSym(bool isNor, bool isZer, bool isMul, bool isOn) {
		this->isNor = isNor;
		this->isZer = isZer;
		this->isMul = isMul;
		this->isOn = isOn;
	}


	/**************************************************************************/
	void AddFuncBlock::dt() {
		afb[0]->dt();
		AddFuncBlock* transBlock;
		if (afb[0]->getTag() == MULT) {
			transBlock = static_cast<AddFuncBlock*>(static_cast<AbsFuncBlock*>(afb[0]));
			transBlock->setTag(ADD);
			afb[0] = transBlock;
		}

		afb[1]->dt();
		if (afb[1]->getTag() == MULT) {
			transBlock = static_cast<AddFuncBlock*>(static_cast<AbsFuncBlock*>(afb[1]));
			transBlock->setTag(ADD);
			afb[1] = transBlock;
	}
	}


	std::string AddFuncBlock::rtStr() {
		//判断纯幂函数串是否为0。
		if (isZer) {
			return "";
		}
		bool t1, t2;
		std::string afb0 = afb[0]->rtStr(),
					afb1 = afb[1]->rtStr();
		//为0有两种情况，一是初始为零，体现为返回空串
		//二是一直为0，体现为返回true
		if (afb0.empty()) {
			t1 = true;
		}
		if (afb1.empty()) {
			t2 = true;
		}

		if (t1 && t2) {
			isZer = true;
			return "";
		}
		else if (t1) {
			return afb1;
		}
		else if (t2) {
			return afb0;
		}
		//加减函数块不用考虑常数。
		if (tag == ADD) {
			afb0 = afb0 + "+";
		}
		else {
			afb0 = afb0 + "-";
		}
		afb0 = afb0 + afb1;
		return afb0;
	}


	AbsFuncBlock* AddFuncBlock::copy() {
		AbsFuncBlock* blockUnit1, * blockUnit2, * newAfb;
		newAfb = new AddFuncBlock;
		blockUnit1 = afb[0]->copy();
		blockUnit2 = afb[1]->copy();
		newAfb->load(blockUnit1, blockUnit2);
		newAfb->setTag(tag);
		newAfb->setLparam(lparam);
		copySym(newAfb);

		return newAfb;
	}


	AbsFuncBlock* AddFuncBlock::trans(AbsFuncBlock* afb) {
		
	}
	/**************************************************************************/
	void MultFuncBlock::dt() {
		AbsFuncBlock* mulUnit1, * mulUnit2, * t1, * t2;
		mulUnit1 = new MultFuncBlock;
		mulUnit2 = new MultFuncBlock;
		t1 = afb[0]->copy();
		t2 = afb[1]->copy();
		//乘法法则
		mulUnit1->load(t1, afb[1]);
		mulUnit1->setTag(MULT);
		mulUnit2->load(afb[0], t2);
		mulUnit2->setTag(MULT);

		afb[0] = mulUnit1;
		afb[1] = mulUnit2;
	}

	//TODO
	std::string MultFuncBlock::rtStr() {
		if (isZer) {
			return "";
		}
		bool t1, t2;
		std::string afb1 = afb[0]->rtStr();
		std::string afb2 = afb[1]->rtStr();
 		int tag1 = afb[0]->getTag(),
			tag2 = afb[1]->getTag();
		
		if (afb1.empty()) {
			t1 = true;
		}
		if (afb2.empty()) {
			t2 = true;
		}
		//如果子块出现0，则返回空串。
		if (t1 || t2) {
			isZer = true;
		return "";
	}

		std::string rt;
		if (tag1 / 10 <= 1) {
			rt = "(" + afb1;
			rt = rt + ")";
		}
		else {
			rt = afb1;
		}
		
		rt = rt + "*";

		if (tag2 / 10 <= 1) {
			rt = "(" + afb2;
			rt = rt + ")";
		}
		else {
			rt =afb2;
		}

		return rt;
	}


	AbsFuncBlock* MultFuncBlock::copy() {
		AbsFuncBlock* blockUnit1, * blockUnit2, * newAfb;
		newAfb = new MultFuncBlock;
		newAfb->setTag(tag);
		blockUnit1 = afb[0]->copy();
		blockUnit2 = afb[1]->copy();
		newAfb->load(blockUnit1, blockUnit2);
		copySym(newAfb);
		return newAfb;
	}


	//TODO
	void MultFuncBlock::setLpara() {
		bool tag1 = afb[0]->getTag(),
			 tag2 = afb[1]->getTag();

	}


	/**************************************************************************/
	void UnitFuncBlock::dt() {
		abf->dt();
	}


	std::string UnitFuncBlock::rtStr() {
		if (isZer) {
			return "";
		}

		//幂函数单元才有0的结果
		if (tag == PWR) {
			if (abf->isZero()) {
				isZer = true;
				return "";
			}
		}
		return abf->rtStr();
	}


	AbsFuncBlock* PwrFuncBlock::copy() {
		AbsFuncBlock* newAfb, * subBlock0, * subBlock1;
		bool e0 = isExist(afb[0]), e1 = isExist(afb[1]);
		if (!e0) {
			delete afb[1];
			isZer = true;
			return NULL;
		}
		if (!e1) {
			delete afb[0];
			UnitFuncBlock* uafb = new UnitFuncBlock;
			uafb->load("1.0");
			return uafb;
		}
		subBlock0 = afb[0]->copy();
		subBlock1 = afb[1]->copy();
		newAfb = new PwrFuncBlock;
		newAfb->load(subBlock0, subBlock1);
		copySym(newAfb);
		return newAfb;
	}


	void UnitFuncBlock::load(std::string s) {
		//单元块load需要switch
		//TODO
		if (s.find("arc") != s.npos) {

		}
		else if (s.find("ln") != s.npos) {

	//单变量求导一定为常数
	void UnitFuncBlock::dt() {
		if (isZer) {
			return;
		}
		if (isNor) {
			isZer = true;
			return;
		}
		lparam = 1.0;
		isNor = true;
	}


		}
		else if (s.find("^x") != s.npos) {

		}
		else {
			abf = new BasPwrFunc;
			tag = PWR;
		}
		abf->load(s);
	}


	AbsFuncBlock* UnitFuncBlock::copy() {
		AbsFuncBlock* newAfb = new UnitFuncBlock;
		newAfb->setTag(UNIT);
		newAfb->setLparam(lparam);
		copySym(newAfb);
		return newAfb;
	}


	void UnitFuncBlock::setLpara(double s) {
		abf->setLpara(s);
	}


	double UnitFuncBlock::getLpara() {
		return abf->getLpara();
	}


	/**************************************************************************/
	void  NoneFuncBlock::dt() {}


	std::string NoneFuncBlock::rtStr() { return ""; }


	void NoneFuncBlock::load(std::string) {}


	AbsFuncBlock* NoneFuncBlock::copy() { return NULL; }
}