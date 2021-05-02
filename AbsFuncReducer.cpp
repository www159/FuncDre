#include "AbsFuncReducer.h"
namespace FuncDre {

	AbsFuncReducer::AbsFuncReducer(){
		simpMap = new std::map<int, ReMode>;
		simpMap->insert(Form(ADDBLOCK, &AbsFuncReducer::addSimplify));		//加法化简器
		simpMap->insert(Form(MULTBLOCK, &AbsFuncReducer::multSimplify));	//乘法化简器	
		simpMap->insert(Form(CONPWRBLOCK, &AbsFuncReducer::pwrSimplify));	
		simpMap->insert(Form(BASPWRBLOCK, &AbsFuncReducer::pwrSimplify));
		simpMap->insert(Form(GNLPWRBLOCK, &AbsFuncReducer::pwrSimplify));
		simpMap->insert(Form(SIN, &AbsFuncReducer::comSimplify));
		simpMap->insert(Form(COS, &AbsFuncReducer::comSimplify));
		simpMap->insert(Form(TAN, &AbsFuncReducer::comSimplify));
		simpMap->insert(Form(LOGBLOCK, &AbsFuncReducer::comSimplify));
		//simpMap->insert(Form(, &AbsFuncReducer:: ));
		//反三角函数待定
	}

	AbsFuncReducer::~AbsFuncReducer() {
		simpMap->clear();
		delete simpMap;
	}

	void AbsFuncReducer::reduc(AbsFuncBlock*& absFuncBlock) {
		(this->*simpMap->at(absFuncBlock->getTag()))(absFuncBlock);
	}

	/*
	*	TODO 未添加ln化简
	*	加法并不会退化，只是徒增递归深度。
	*	加法退化将放在下一个版本中实现。
	*/
	void AbsFuncReducer::addSimplify(AbsFuncBlock*& absFuncBlock) {
		auto addFuncBlock = static_cast<AddFuncBlock*>(absFuncBlock);
		auto funcContainer = addFuncBlock->getContainer();
	
		for (auto& it : *funcContainer) {//大爱auto
			if (isBas(it)) {
				continue;
			}
			(this->*(simpMap->at(it->getTag())))(it);
		}

		//lambda表达式
		funcContainer->sort([=](AbsFuncBlock* absFuncBlock1, AbsFuncBlock* absFuncBlock2)->int
			{
				if (absFuncBlock1->getTag() == absFuncBlock2->getTag()) {//保证同底乘法并排，常数并排
					
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
			if (lastTag == CONBLOCK && tag == CONBLOCK) {//都是常数，向右合并

				auto lastConFunc = static_cast<ConFuncBlock*>(*lastIt);//如果常数为0，直接continue
				auto conFunc = static_cast<ConFuncBlock*>(*it);
				conFunc->setNum(lastConFunc->getNum() + conFunc->getNum());
				delete lastConFunc;//释放内存
				funcContainer->erase(lastIt);
			}

			else if ((*lastIt)->getTag() == (*it)->getTag() && getPureHash(*it) == getPureHash(*lastIt)) {//如果除系数外完全一致
				//那么就要区分是否带系数。
				bool lastHasCon = hasCon(*lastIt),
					tHasCon = hasCon(*it),
					pwrNotSame = false;
				if (lastHasCon && tHasCon) {//如果都有系数，那都是乘法
					auto lastCon = static_cast<ConFuncBlock*>(static_cast<MultFuncBlock*>(*lastIt)->getContainer()->front());
					auto tCon = static_cast<ConFuncBlock*>(static_cast<MultFuncBlock*>(*it)->getContainer()->front());
					tCon->setNum(lastCon->getNum() + tCon->getNum());//未经深拷贝直接修改
					(*it)->clear();//修改后清空两个hash
					delete* lastIt;
					funcContainer->erase(lastIt);

				}
				else if (lastHasCon) {
					auto lastCon = static_cast<ConFuncBlock*>(static_cast<MultFuncBlock*>(*lastIt)->getContainer()->front());
					lastCon->setNum(lastCon->getNum() + 1.0);
					(*lastIt)->clear();//清除hash
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
				else {//如果两个都没有常系数，那么直接生成
					MultFuncBlock* multFunc = new MultFuncBlock;
					multFunc->setTag(MULTBLOCK);
					ConFuncBlock* conFunc = new ConFuncBlock;
					conFunc->setTag(CONBLOCK);
					conFunc->setNum(2.0);
					multFunc->addFunc(conFunc);
					multFunc->addFunc(*it);
					delete* lastIt;//删去前一个
					funcContainer->erase(lastIt);
					*it = multFunc;//将it替换成multFunc

				}
			}

			lastIt = it;
			lastTag = tag;//保留历史信息
			it++;
		}

		it = funcContainer->begin();
		while (it != funcContainer->end()) {//去除零项，包括乘法退化检查
			tag = (*it)->getTag();
			if (tag == CONBLOCK && static_cast<ConFuncBlock*>(*it)->getNum() == 0) {
				delete* it;
				funcContainer->erase(it++);
				continue;
			}
			else if (tag == MULTBLOCK) {
				auto frontFunc = static_cast<MultFuncBlock*>(*it)->getContainer()->front();
				if (frontFunc->getTag() == CONBLOCK ) {
					if (static_cast<ConFuncBlock*>(frontFunc)->getNum() == 0) {//系数为0，直接删去
						delete* it;
						funcContainer->erase(it++);
						continue;
					}
					else if (static_cast<ConFuncBlock*>(frontFunc)->getNum() == 1) {//如果系数为1，忽略系数
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

	void AbsFuncReducer::multSimplify(AbsFuncBlock*& absFuncBlock) {//需要幂指类完成指数上移
		auto funcContainer = static_cast<MultFuncBlock*>(absFuncBlock)->getContainer();
		for (auto& it : *funcContainer) {//逐一化简
			if (isBas(it)) {
				continue;
			}
			(this->*(simpMap->at(it->getTag())))(it);
		}
		funcContainer->sort([=](AbsFuncBlock* const subFunc1,AbsFuncBlock* const subFunc2)->int {//lambda排序
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

		while (it != funcContainer->end()) {//合并同类项
			tag = (*it)->getTag();
			if (lastTag == CONBLOCK && tag == CONBLOCK) {//合并常数
				auto conFunc = static_cast<ConFuncBlock*>(*it);
				auto lastConFunc = static_cast<ConFuncBlock*>(*lastIt);
				conFunc->setNum(conFunc->getNum() * lastConFunc->getNum());
				delete* lastIt;
				funcContainer->erase(lastIt);
			}
			else if ((*it)->getTag() == (*lastIt)->getTag() && getPureHash(*it) == getPureHash(*lastIt)) {//向右归并

				if (lastTag == BASPWRBLOCK && tag == BASPWRBLOCK) {//都是指数函数，底数相乘
					auto conFunc = static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(*it)->getBottomFunc());
					auto lastConFunc = static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(*lastIt)->getBottomFunc());
					conFunc->setNum(conFunc->getNum() * lastConFunc->getNum());
					delete* lastIt;
					funcContainer->erase(lastIt);
				}
				else if (lastTag == CONPWRBLOCK && tag == CONPWRBLOCK) {//都是同底幂函数
					auto conFunc = static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(*it)->getTopFunc());
					auto lastConFunc = static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(*lastIt)->getTopFunc());
					conFunc->setNum(conFunc->getNum() + lastConFunc->getNum());//幂数相加
					delete* lastIt;
					funcContainer->erase(lastIt);
				}
				else if (lastTag == CONPWRBLOCK && tag != BASPWRBLOCK) {//有一个指数为1的幂函数
					auto lastConFunc = static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(*lastIt)->getTopFunc());
					lastConFunc->setNum(lastConFunc->getNum() + 1.0);
					delete* it;
					funcContainer->erase(it);
					it = lastIt;
				}
				else if (lastTag != BASPWRBLOCK && tag == CONPWRBLOCK) {//同上
					auto conFunc = static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(*it)->getTopFunc());
					conFunc->setNum(conFunc->getNum() + 1.0);
					delete* lastIt;
					funcContainer->erase(lastIt);
				}
				else {//形如x*x类型，需要new一个幂函数
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

		//乘法退化检查，0项只会出现在合并同类相之后
		if (funcContainer->front()->getTag() == CONBLOCK) {
			auto conFunc = static_cast<ConFuncBlock*>(funcContainer->front()->copy());
			if (conFunc->getNum() == 0) {//直接退化为常数0
				conFunc = conFunc->copy();//需要深拷贝一份常函数
				delete absFuncBlock;
				absFuncBlock = conFunc;
				return;//直接返回
			}
		}
		
		it = funcContainer->begin();
		while (it != funcContainer->end()) {//删除1项，包括幂数退化检查
			tag = (*it)->getTag();
			if (tag == CONBLOCK && static_cast<ConFuncBlock*>(*it)->getNum() == 1) {
				delete* it;
				funcContainer->erase(it++);
				continue;
			}
			else if (tag == BASPWRBLOCK && static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(*it)->getBottomFunc())->getNum() == 1) {
				//指数函数1退化
				delete* it;
				funcContainer->erase(it++);
				continue;
			}
			else if (tag == CONPWRBLOCK) {
				auto conFunc = static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(*it)->getTopFunc());
				if (conFunc->getNum() == 1) {//幂数1退化
					delete conFunc;//必须深拷贝，因为hash也是实例
					auto tem = *it;
					*it = static_cast<GnlFuncBlock*>(*it)->getBottomFunc()->copy();//退化为常函数
					delete* it;
					
				}
				else if (conFunc->getNum() == 0) {//幂数0退化
					conFunc = conFunc->copy();//深拷贝一份
					delete* it;
					*it = conFunc;//退化为常数
				}
			}
			it++;
		}

		backward(absFuncBlock);
	}

	void AbsFuncReducer::pwrSimplify(AbsFuncBlock*& absFuncBlock) {//内外层合并
		auto theFunc = static_cast<GnlFuncBlock*>(absFuncBlock);
		switch (absFuncBlock->getTag()) {
		case CONPWRBLOCK: {//内外层合并，系数外移
			auto innerFunc = theFunc->getBottomFunc();
			if (isBas(innerFunc)) {
				break;
			}
			(this->*simpMap->at(innerFunc->getTag()))(innerFunc);//内部化简
			switch (innerFunc->getTag()) {
			case CONPWRBLOCK: {
				if (isBas(innerFunc)) {
					break;
				}
				auto innerGnlFunc = static_cast<UniPwrBlock*>(innerFunc)->copy();//注意是幂函数
				auto innerCon = static_cast<ConFuncBlock*>(innerGnlFunc->getTopFunc());//内部需要经过一次拷贝
				auto theCon = static_cast<ConFuncBlock*>(theFunc->getTopFunc());
				innerCon->setNum(theCon->getNum() * innerCon->getNum());
				innerGnlFunc->setTopFunc(innerCon);
				delete theFunc;
				absFuncBlock = innerGnlFunc;
				break;
			}
			case BASPWRBLOCK: case GNLPWRBLOCK: {//需要防止乘法套乘法。
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
					(this->*simpMap->at(MULTBLOCK))(innerTopFunc);//再次化简
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
		case GNLPWRBLOCK: {//TODO幂指函数未完成

			break;
		}
		}
		backward(absFuncBlock);//退化检查
	}

	void AbsFuncReducer::comSimplify(AbsFuncBlock*& absFuncBlock) {
		auto theFunc = static_cast<ComFuncBlock*>(absFuncBlock);
		auto innerFunc = theFunc->getFunc();
		if (isBas(innerFunc)) {
			return;
		}
		(this->*simpMap->at(innerFunc->getTag()))(innerFunc);
		if (theFunc->getFunc() != innerFunc) {//根据地址判断内部函数是否被修改
			theFunc->setFunc(innerFunc);
		}
		switch (absFuncBlock->getTag()) {
		case LOGBLOCK: {//TODO log化简

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

	void AbsFuncReducer::backward(AbsFuncBlock*& absFuncBlock) {//对自身的检查，子项合并退化之后再检查。
		switch (absFuncBlock->getTag()) {
		case ADDBLOCK:case MULTBLOCK: {
			auto funcContainer = static_cast<AddFuncBlock*>(absFuncBlock)->getContainer();
			if (funcContainer->size() == 0) {//只剩0项
				auto conFunc = new ConFuncBlock;
				conFunc->setNum(0);
				conFunc->setTag(CONBLOCK);
				delete absFuncBlock;
				absFuncBlock = conFunc;
			}
			else if (funcContainer->size() == 1) {//只剩1项
				auto tem = absFuncBlock;
				absFuncBlock = funcContainer->front()->copy();//为了删除容器，必须深拷贝
				delete tem;
			}
			break;
		}
		case CONPWRBLOCK: {//防止恶意输入1，0
			auto conPwrFunc = static_cast<GnlFuncBlock*>(absFuncBlock);
			auto conFunc = static_cast<ConFuncBlock*>(conPwrFunc->getTopFunc());
			if (conPwrFunc->getBottomFunc()->getTag() == CONBLOCK) {
				conFunc->setNum(pow(static_cast<ConFuncBlock*>(conPwrFunc->getBottomFunc())->getNum(), conFunc->getNum()));
				conFunc = conFunc->copy();
				delete absFuncBlock;
				absFuncBlock = conFunc;
			}
			else if (conFunc->getNum() == 0) {//如果指数为0
				conFunc = conFunc->copy();
				delete absFuncBlock;
				absFuncBlock = conFunc;
			}
			else if (conFunc->getNum() == 1) {
				auto bottomFunc = static_cast<GnlFuncBlock*>(absFuncBlock)->getBottomFunc()->copy();//深拷贝一份
				delete absFuncBlock;//删除整块
				absFuncBlock = bottomFunc;//重定向
			}
			break;
		}
		case BASPWRBLOCK : {//同样防止恶意输入
			auto conFunc = static_cast<ConFuncBlock*>(static_cast<GnlFuncBlock*>(absFuncBlock)->getBottomFunc());
			if (conFunc->getNum() == 0 || conFunc->getNum() == 1) {//如果底数为0或1，效果相同
				conFunc = conFunc->copy();
				delete absFuncBlock;
				absFuncBlock = conFunc;
			}
			break;
		}
		default: break;//其他函数同理
		}
	}
}
