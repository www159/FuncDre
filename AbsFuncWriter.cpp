#include "AbsFuncWriter.h"
namespace FuncDre {



	AbsFuncWriter::AbsFuncWriter() {//每一个伟大的算法都有一个伟大的预处理,根据tag映射的函数数组。
		writerContainer[0]	=	&AbsFuncWriter::conWriter;
		writerContainer[1]	=	&AbsFuncWriter::basWriter;
		writerContainer[2]	=	&AbsFuncWriter::addWriter;
		writerContainer[3]	=	&AbsFuncWriter::multWriter;
		writerContainer[4]	=	&AbsFuncWriter::gnlPwrWriter;
		writerContainer[5]	=	&AbsFuncWriter::gnlPwrWriter;
		writerContainer[6]	=	&AbsFuncWriter::gnlPwrWriter;
		writerContainer[7]	=	&AbsFuncWriter::logWriter;
		writerContainer[8]	=	&AbsFuncWriter::sinWriter;
		writerContainer[9]	=	&AbsFuncWriter::cosWriter;
		writerContainer[10] =	&AbsFuncWriter::tanWriter;
		//反三角待定。
	}



	std::string AbsFuncWriter::writeFunc(AbsFuncBlock* absFuncBlock) {//TODO,创建一个字符串即可
		std::string s = "";
		(this->*writerContainer[absFuncBlock->getTag()])(absFuncBlock, s);
		return s;
	}

	void AbsFuncWriter::conWriter(AbsFuncBlock* absFuncBlock, std::string& str) {
		ConFuncBlock* conFuncBlock = static_cast<ConFuncBlock*>(absFuncBlock);
		str = str + TransForm::d2str(conFuncBlock->getNum());
	}

	void AbsFuncWriter::basWriter(AbsFuncBlock* absFuncBlock, std::string& str) {
		str = str + "x";
	}

	void AbsFuncWriter::addWriter(AbsFuncBlock* absFuncBlock, std::string& str) {
		AddFuncBlock* addFuncBlock = static_cast<AddFuncBlock*>(absFuncBlock);
		std::list<AbsFuncBlock*>* funcContainer = addFuncBlock->getContainer();

		for (std::list<AbsFuncBlock*>::iterator it = funcContainer->begin(); it != funcContainer->end(); it++) {
			switch ((*it)->getTag()) {
			case MULTBLOCK: {
				MultFuncBlock* subMultBlock = static_cast<MultFuncBlock*>(*it);
				std::list<AbsFuncBlock*>* subMultContainer = subMultBlock->getContainer();

				if (subMultContainer->front()->getTag() == CONBLOCK) {//如果带了系数
					ConFuncBlock* cons = static_cast<ConFuncBlock*>(subMultContainer->front());
					if (cons->getNum() < 0) {//如果系数为负。
						cons->setNum(-cons->getNum());
						str = str + "-";
					}
				}
				else {
					if (it == funcContainer->begin()) {
						break;
					}
					str = str + "+";
				}
				break;
			}
			default: {
				if (it != funcContainer->begin()) {//开头加什么正号
					str = str + "+";
				}
				
				break;
			}
			}
			//输出函数体。
			(this->*writerContainer[(*it)->getTag()])(*it, str);
		}
	}

	void AbsFuncWriter::multWriter(AbsFuncBlock* absFuncBlock, std::string& str) {//试试auto
		//加法顺带解决了乘法的系数问题，只管输出。
		auto multFuncBlock = static_cast<MultFuncBlock*>(absFuncBlock);
		auto funcContainer = multFuncBlock->getContainer();
		int tag;
		for (auto it = funcContainer->begin(); it != funcContainer->end(); it++) {
			tag = (*it)->getTag();
			switch (tag) {
			case ADDBLOCK: {
				str = str + "(";
				(this->*writerContainer[tag])(*it, str);
				str = str + ")";
				break;
			}
			case CONBLOCK: {
				auto conFuncBlock = static_cast<ConFuncBlock*>(*it);
				if (it != funcContainer->begin()) {//常数两头加乘号
					str = str + "*";
					if (conFuncBlock->getNum() < 0) {
						str = str + "(";
						(this->*writerContainer[tag])(*it, str);
						str = str + ")";
					}
					else {
						(this->*writerContainer[tag])(*it, str);
					}
				}
				else {
					if ((*it)->getTag() == CONBLOCK) {
						if (static_cast<ConFuncBlock*>(*it)->getNum() == -1) {
							str = str + '-';
							break;
						}
					}
					(this->*writerContainer[tag])(*it, str);
				}
				break;
			}
			default: {
				(this->*writerContainer[tag])(*it, str);
				break;
			}
			}
		}
		if (str[str.length() - 1] == '*') {
			str.erase(str.length() - 1, 1);
		}
	}

	void AbsFuncWriter::gnlPwrWriter(AbsFuncBlock* absFuncBlock, std::string& str) {
		auto gnlPwrBlock = static_cast<GnlFuncBlock*>(absFuncBlock);
		auto subFunc = gnlPwrBlock->getBottomFunc();
		int tag = subFunc->getTag();
		switch (tag) {
		case ADDBLOCK:case MULTBLOCK:case CONPWRBLOCK:case BASPWRBLOCK:
		case GNLPWRBLOCK: {//需要包括号吗？
			str = str + "(";
			(this->*writerContainer[tag])(subFunc, str);
			str = str + ")";
			break;
		}
		case CONBLOCK: {
			auto conFuncBlock = static_cast<ConFuncBlock*>(subFunc);
			if (conFuncBlock->getNum() < 0) {
				str = str + "(";
				(this->*writerContainer[tag])(subFunc, str);
				str = str + ")";
			}
		}
		default: {
			(this->*writerContainer[tag])(subFunc, str);
			break;
		}
		}
		
		subFunc = gnlPwrBlock->getTopFunc();
		str = str + '^';
		tag = subFunc->getTag();
		switch (tag) {
		case ADDBLOCK:case MULTBLOCK:case CONPWRBLOCK:case BASPWRBLOCK:
		case GNLPWRBLOCK: {//不加分数线，底部和顶部地位平等
			str = str + "(";
			(this->*writerContainer[tag])(subFunc, str);
			str = str + ")";
			break;
		}
		case CONBLOCK: {
			auto conFuncBlock = static_cast<ConFuncBlock*>(subFunc);
			if (conFuncBlock->getNum() < 0) {
				str = str + "(";
				(this->*writerContainer[tag])(subFunc, str);
				str = str + ")";
			}
			else {
				(this->*writerContainer[tag])(subFunc, str);
			}
			break;
		}
		default: {
			(this->*writerContainer[tag])(subFunc, str);
			break;
		}
		}
	}

	void AbsFuncWriter::logWriter(AbsFuncBlock* absFuncBlock, std::string& str) {
		ComFuncBlock* comFuncBlock = static_cast<ComFuncBlock*>(absFuncBlock);
		str = str + "ln";
		str = str + "(";
		AbsFuncBlock* innerFunc = comFuncBlock->getFunc();
		(this->*writerContainer[innerFunc->getTag()])(innerFunc, str);
		str = str + ")";
	}
	
	void AbsFuncWriter::sinWriter(AbsFuncBlock* absFuncBlock, std::string& str) {
		ComFuncBlock* comFuncBlock = static_cast<ComFuncBlock*>(absFuncBlock);
		str = str + "sin";
		str = str + "(";
		AbsFuncBlock* innerFunc = comFuncBlock->getFunc();
		(this->*writerContainer[innerFunc->getTag()])(innerFunc, str);
		str = str + ")";
	}

	void AbsFuncWriter::cosWriter(AbsFuncBlock* absFuncBlock, std::string& str) {
		ComFuncBlock* comFuncBlock = static_cast<ComFuncBlock*>(absFuncBlock);
		str = str + "cos";
		str = str + "(";
		AbsFuncBlock* innerFunc = comFuncBlock->getFunc();
		(this->*writerContainer[innerFunc->getTag()])(innerFunc, str);
		str = str + ")";
	}

	void AbsFuncWriter::tanWriter(AbsFuncBlock* absFuncBlock, std::string& str) {
		ComFuncBlock* comFuncBlock = static_cast<ComFuncBlock*>(absFuncBlock);
		str = str + "tan";
		str = str + "(";
		AbsFuncBlock* innerFunc = comFuncBlock->getFunc();
		(this->*writerContainer[innerFunc->getTag()])(innerFunc, str);
		str = str + ")";
	}
}