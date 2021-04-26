#include "FuncLoader.h"
namespace FuncDre {



	FuncLoader::FuncLoader() {
		finalAbsFuncBlock = NULL;
		conQue		=	new std::queue<AbsFuncBlock*>;
		comQue		=	new std::queue<AbsFuncBlock*>;
		bacQue		=	new std::queue<int>;
		AddStack	=	new std::stack<OperFuncBlock*>;
		MulStack	=	new std::stack<OperFuncBlock*>;
	}



	FuncLoader::~FuncLoader() {
		delete conQue;
		delete comQue;
		delete bacQue;
		delete AddStack;
		delete MulStack;
	}



	AbsFuncBlock* FuncLoader::getFinalFunc() {
		return finalAbsFuncBlock;
	}



	void FuncLoader::load(std::string str) {
		workSubComb(str);	//�޸�str
		trans1(str);		//�޸�str
		trans2(str);		//���޸�str
		trans3();
	}


	void FuncLoader::trans1(std::string& str) {
		std::regex Con("[-]?[0-9]*\\.?[0-9]+"),
				   Time("([0-9\\)Ux])([x\\(U])"),
				   Minu("([)xU\\d])(\\-)([(xU\\d])"),
				   Minu2("^(-)([xU])"),
				   Pos("(\\()(\\+)([\\dxU])"),
				   Pos2("^(\\+)([Ux\\d])");

		std::string posRep("$1$3"),
					posRep2("$2"),
					timeRep = "$1*$2",
					minuRep = "$1+(-1)*$3",
					minuRep2 = "(-1)*$2";

		str = regex_replace(str, Minu, minuRep);	//������(���Ǹ���)ת��Ϊ(-1)*
		str = regex_replace(str, Minu2, minuRep2);	//ת����ʼ����
		str = regex_replace(str, Time, timeRep);	//�����ȫ��
		str = regex_replace(str, Pos, posRep);		//ȥ����Ч����
		str = regex_replace(str, Pos2, posRep2);	//ȥ����ͷ��Ч����

		//��ȡ����ʵ������ʵ�����У����滻ΪC;
		std::sregex_iterator pos(str.begin(), str.end(), Con), end;
		while (pos != end) {

			ConFuncBlock* conFuncBlock = new ConFuncBlock;
			conFuncBlock->setNum(TransForm::str2d(pos->str()));
			conFuncBlock->setTag(CON);
			conQue->push(conFuncBlock);
			pos++;
		}
		str = regex_replace(str, Con, "C");
	}



	void FuncLoader::trans2(std::string str) {
		int len = str.length(), sign;
		std::stack<int>* temStack = new std::stack<int>;

		for (int i = 0; i < len; i++) {
			sign = isSign(str[i]);

			if (sign == CON || sign == UNI || sign == VAR) {
				bacQue->push(sign);
			}
			else {
				switch (sign) {

				case LINC: {
					temStack->push(sign);
					break;
				}
		
				case RINC: {
					//��֤���ųɶԳ��֡�
					while (temStack->top() != LINC) {
						bacQue->push(temStack->top());
						temStack->pop();
					}
					temStack->pop();
					break;
				}
		
				default: {
					if (temStack->empty()) {
						temStack->push(sign);
					}
					else {//�ҵ����ȼ������Լ���Ϊֹ��
						while (!temStack->empty() && isSmaller(sign, temStack->top()) && temStack->top() != LINC) {
							bacQue->push(temStack->top());
							temStack->pop();
						}
						temStack->push(sign);
					}
					break;
				}
				}
			}
		}

		while (!temStack->empty()) {//����ʣ���������
			bacQue->push(temStack->top());
			temStack->pop();
		}
		delete temStack;
	}



	void FuncLoader::trans3() {
		int sign = -1;
		int lastSign = -1,	//��¼��һ�������ķ���(���Ǻ�����)��
			lastSign2 = -1;	//��¼���ϸ������ķ���(���Ǻ�����)��
		std::stack<AbsFuncBlock*>* temStack = new std::stack<AbsFuncBlock*>;
		std::stack<int>* signStack = new std::stack<int>;
		while (!bacQue->empty()) {
			sign = bacQue->front();
			bacQue->pop();
			switch (sign) {
			case CON: {
				temStack->push(conQue->front());
				conQue->pop();
				signStack->push(CON);
				break;
			}

			case VAR: {
				AbsFuncBlock* absFuncBlock = new AbsFuncBlock;
				absFuncBlock->setTag(BASBLOCK);
				temStack->push(absFuncBlock);
				signStack->push(VAR);
				break;
			}

			case UNI: {
				temStack->push(comQue->front());
				comQue->pop();
				signStack->push(UNI);
				break;
			}

			case DIV: {
				GnlFuncBlock* pwrFuncBlock = new GnlFuncBlock;
				pollHisInSignStack(lastSign, lastSign2, signStack);
				pwrFuncBlock->setBottomFunc(pollBlockInStack(lastSign, temStack));
				ConFuncBlock* conFuncBlock = new ConFuncBlock;
				conFuncBlock->setNum(-1.0);
				conFuncBlock->setTag(CONBLOCK);
				pwrFuncBlock->setTopFunc(conFuncBlock);
				pwrFuncBlock->setTag(CONPWRBLOCK);
				switch (lastSign2) {
				case MULT: {
					MulStack->top()->addFunc(pwrFuncBlock);
					break;
				}
				default: {
					OperFuncBlock* multFuncBlock = new OperFuncBlock;
					multFuncBlock->addFunc(pollBlockInStack(lastSign2, temStack));
					multFuncBlock->addFunc(pwrFuncBlock);
					multFuncBlock->setTag(MULTBLOCK);
					MulStack->push(multFuncBlock);
					break;
				}
				}
				signStack->push(MULT);//����ת��Ԫ
				break;
			}

			case PWR: {
				GnlFuncBlock* pwrFuncBlock = new GnlFuncBlock;
				pollHisInSignStack(lastSign, lastSign2, signStack);
				pwrFuncBlock->setTopFunc(pollBlockInStack(lastSign, temStack));
				pwrFuncBlock->setBottomFunc(pollBlockInStack(lastSign2, temStack));
				if (lastSign == CON) {
					pwrFuncBlock->setTag(CONPWRBLOCK);
				}
				else if (lastSign2 == CON) {
					pwrFuncBlock->setTag(BASPWRBLOCK);
				}
				else {
					pwrFuncBlock->setTag(GNLPWRBLOCK);
				}
				temStack->push(pwrFuncBlock);
				signStack->push(PWR);
				break;
			}

			case ADD: {
				//��ͨջת�ơ�
				pollHisInSignStack(lastSign, lastSign2, signStack);
				switch (lastSign2) {
				case ADD://����ʱջ��ת�Ƶ��ӷ�ջ
					AddStack->top()->addFunc(pollBlockInStack(lastSign, temStack));
					break;
				default://����ʱջ��ѡ����ѹ���ӷ�ջ
					OperFuncBlock* addFuncBlock = new OperFuncBlock;
					//��֤��׺���ʽ׼ȷ����
					addFuncBlock->addFunc(pollBlockInStack(lastSign, temStack));
					addFuncBlock->addFunc(pollBlockInStack(lastSign2, temStack));
					addFuncBlock->setTag(ADDBLOCK);
					AddStack->push(addFuncBlock);
					break;
				}
				signStack->push(ADD);
				break;
			}

			case MULT: {
				//ԭ��ͬ�ӷ��顣
				pollHisInSignStack(lastSign, lastSign2, signStack);
				switch (lastSign2) {
				case MULT:
					MulStack->top()->addFunc(pollBlockInStack(lastSign, temStack));
					break;
				default:
					OperFuncBlock* multFuncBlock = new OperFuncBlock;
					//��֤��׺���ʽ׼ȷ����
					multFuncBlock->addFunc(pollBlockInStack(lastSign, temStack));
					multFuncBlock->addFunc(pollBlockInStack(lastSign2, temStack));
					multFuncBlock->setTag(MULTBLOCK);
					MulStack->push(multFuncBlock);
					break;
				}
				signStack->push(MULT);
				break;
			}

			default: break;
			}
		}

		pollHisInSignStack(lastSign, lastSign2, signStack);
		finalAbsFuncBlock = pollBlockInStack(lastSign, temStack);
		delete signStack;
		delete temStack;
		
	}



	bool FuncLoader::isSmaller(int sign1, int sign2) {
		return sign1 / 10 <= sign2 / 10;
	}



	int FuncLoader::isSign(char c) {
		switch (c) {
		case 'C': return CON;
		case 'U': return UNI;
		case 'x': return VAR;
		case '+': return ADD;
		case '*': return MULT;
		case '/': return DIV;
		case '^': return PWR;
		case '(': return LINC;
		case ')': return RINC;
		default : return -1;
		}
	}



	void FuncLoader::workSubComb(std::string& str) {
		int len = str.length(), p = 0, q = 0, r = 0;
		int cnt = 0, tag = DEFAULTTAG;
		while (q < len) {

			while (p < len) {
				if (str[p] == 's') {
					tag = SIN;
					break;
				}
				if (str[p] == 'c') {
					tag = COS;
					break;
				}
				if (str[p] == 't') {
					tag = TAN;
					break;
				}
				if (str[p] == 'l') {
					tag = LOGBLOCK;
					break;
				}
				p++;
			}

			if (p >= len) {
				return;
			}

			q = p;

			while (cnt == 0 && q < len) {
				if (str[q] == '(') {
					cnt--;
				}
				q++;
			}

			r = q;//��ʱp�ڵ�һ�������ŵ��ұ�

			while (cnt != 0 && r < len) {
				if (str[r] == '(') {
					cnt--;
				}
				if (str[r] == ')') {
					cnt++;
				}
				r++;
			}

			//��֤�����걸,��ʱq����������ŵ��ұߡ�
			FuncLoader* funcLoader = new FuncLoader;
			funcLoader->load(str.substr(q, int(r - q - 1)));//������Ҫ���á�
			ComFuncBlock* comFuncBlock = new ComFuncBlock;
			comFuncBlock->setFunc(funcLoader->getFinalFunc());
			delete funcLoader;

			comFuncBlock->setTag(tag);//���ø��Ͽ��ǩ��
			comQue->push(comFuncBlock);//���Ͽ����push���Ͽ顣
			str.replace(p , int(r - p), "U");//���滻�ĳ�����Ҫ�������š�

			p++;
			len = str.length();
		}
	}



	AbsFuncBlock* FuncLoader::pollBlockInStack(int sign, std::stack<AbsFuncBlock*>* temStack) {
		switch (sign) {
		case ADD: {
			AbsFuncBlock* absFuncBlock = AddStack->top();
			AddStack->pop();
			return absFuncBlock;
		}
		
		case MULT: {
			AbsFuncBlock* absFuncBlock = MulStack->top();
			MulStack->pop();
			return absFuncBlock;
		}

		default: {
			AbsFuncBlock* absFuncBlock = temStack->top();
			temStack->pop();
			return absFuncBlock;
		}
		}
	}

	void FuncLoader::pollHisInSignStack(int& lastSign, int& lastSign2, std::stack<int>* signStack) {
		if (signStack->empty()) {
			lastSign = -1;
			lastSign2 = -1;
		}
		else if (signStack->size() == 1) {
			lastSign = signStack->top();
			signStack->pop();
		}
		else {
			lastSign = signStack->top();
			signStack->pop();
			lastSign2 = signStack->top();
			signStack->pop();
		}
	}
}