#include"FuncReader.h"
namespace FuncReader {
	using namespace BasFuncBlock;
	void GnlFuncReader::load(std::string s) {
		ecde.clear();
		this->end = s.length();
		//连续左括号最左边的下标
		int seqLincs = 0;
		int p, q, r, t, isSpecFunc;
		p = q = 0;
		r = -1;
		t = -1;
		//当前读到的符号下标
		int Signtag = 0;
		int INCNT = 0;
		//这里可以加入sin和log判断
		//连续右括号不能跳
		while (q < end) {
			while ((isSign(s[q]) == NORM || isSign(s[q]) == LINC) && q != end) {
				Signtag = isSign(s[q]);
				//括号累加器操作
				if (Signtag == LINC) {
					INCNT--;
				}
				else if (Signtag == RINC) {
					INCNT++;
				}

				//非左括号标识
				if (Signtag != LINC) {
					seqLincs = q;
				}

				//如果是读到连续左括号，停下来判断是否为复合函数。
				if (r >= 0 && Signtag == NORM) {
					if (isSign(s[r]) == LINC) {
						break;
					}
				}

	//		while (isSign(s[p]) == RINC) {
	//			ecde.push_back(RINC);
	//			p++;
	//		}
	//		//防止连续括号出现运算符。
	//		if (isSign(s[p]) != NORM) {
	//			ecde.push_back(isSign(s[p]));
	//			p++;
	//		}
	//		
	//		if (p >= q) {
	//			q++;
	//			r++;
	//			continue;
	//		}
	//		UnitFuncBlock* ufb = new UnitFuncBlock;
	//		ufb->load(s.substr(p, q - p));
	//		que.push(ufb);
	//		ecde.push_back(NORM);
	//		if (q != end) {
	//			ecde.push_back(isSign(s[q]));
	//		}
	//		q++;
	//		r++;
	//		p = q;
	//	}
	//	trans1();
	//	trans2();
	//}

	void GnlFuncReader::load(std::string s) {
		int p = 0, q = 0;
		end = s.length();
		while (q <= end) {
			while ((isSign(s[q]) == NORM || isSign(s[q]) == LINC) && q < end) {
				q++;
				r++;
			}

			//TODO 连续右括号的复合函数先不考虑。
			//不允许空括号的存在
			while (isSign(s[p]) == LINC) {
				INCNT++;
				ecde.push_back(LINC);
				p++;
			}

			//TODO 复合函数读取
			//如果q跳过了连续左括号，那么根据非左括号标识判断该连续括号，并借助括号累加器得到完整的复合函数。
			//如果该函数是复合函数，中缀存储器的括号元素将不会更新。
			//附带幂函数的负指数判断。
			isSpecFunc = isSpeFunc(s, seqLincs);
			if ((isSign(s[r]) == LINC && isSpecFunc != -1) || (isSpecFunc != -1 && isSign(s[q]) == MINU)) {
				if (isSpecFunc == PWR) {
					while (INCNT < 0) {
						Signtag = isSign(s[q]);
						if (Signtag == RINC) {
							INCNT++;
						}
						q++;
					}
					UnitFuncBlock* ufb = new UnitFuncBlock;
					ufb->load(s.substr(p, q - p));
					que.push(ufb);
					ecde.push_back(NORM);
					if (q != end) {
						ecde.push_back(isSign(s[q]));
					}
					p = q;
				}
				else {

				}
				continue;
			}

			while (isSign(s[p]) == RINC) {
				ecde.push_back(RINC);
				p++;
			}
			//防止连续括号出现运算符。
			if (isSign(s[p]) != NORM) {
				ecde.push_back(isSign(s[p]));
				p++;
			}
			UnitFuncBlock* ufb = new UnitFuncBlock;
			ufb->load(s.substr(p, q - p));
			que.push(ufb);
			ecde.push_back(NORM);
			if (q != end) {
				ecde.push_back(isSign(s[q]));
			}
			if (q == end) {
				break;
			}
			ecde.push_back(isSign(s[q]));
			q++;
			p = q;
		}
		trans1();
		trans2();
	}

	AbsFuncBlock* GnlFuncReader::getAfb() {
		return this->Afb;
	}

	int GnlFuncReader::isSign(char c) {
		switch (c) {
		case '(': {
			return LINC;
		}
		case ')': {
			return RINC;
		}
		case '+': {
			return ADD;
		}
		case '-': {
			return MINU;
		}
		case '*': {
			return MULT;
		}
		case '/': {
			return DIV;
		}
		default: {
			return -1;
			break;
		}
		}
		return -1;
	}


	bool GnlFuncReader::isSmaller(int x, int y) {
		return (x / 10 <= y / 10 ? true : false);
	}


	int GnlFuncReader::isSpeFunc(std::string s, int p) {
		if (p > 3) {
			if (s[p - 3] == 'c' && s[p - 4] == 'r') {
				return ART;
			}
		}
		if (p > 1) {
			if (s[p - 2] == 's' || s[p - 2] == 'c' || s[p - 2] == 't') {
				return TRI;
			}
		}
		if (p > 0) {
			if (s[p] == 'n') {
				return ARI;
			}
			if (s[p] == '^') {
				return PWR;
			}
		}
		return -1;
	}
	

	void GnlFuncReader::trans1() {
		std::stack<int> temsta;
		for (int i = 0; i < ecde.size(); i++) {
			switch (ecde[i]) {
			case NORM: {
				bacQue.push(que.front());
				que.pop();
				break;
			}
			default: {
				if (temsta.empty()) {
					temsta.push(ecde[i]);
				}
				else if (isSmaller(temsta.top(), ecde[i])) {
					temsta.push(ecde[i]);
				}
				else {
					while (!temsta.empty()) {
						if (isSmaller(ecde[i], temsta.top())) {
							AbsFuncBlock* afb = new NoneFuncBlock;
							afb->setTag(temsta.top());
							bacQue.push(afb);
							temsta.pop();
							continue;
						}
						else {
							break;
						}
					}
					temsta.push(ecde[i]);
				}
				break;
			}
			}
		}
		while (!temsta.empty()) {
			AbsFuncBlock* afb = new NoneFuncBlock;
			afb->setTag(temsta.top());
			bacQue.push(afb);
			temsta.pop();
		}
	}

	
	void GnlFuncReader::trans2() {
		std::stack<AbsFuncBlock*> afbStack;
		AbsFuncBlock* afb1, * afb2, * afb3;
		while (!bacQue.empty()) {
			afb1 = bacQue.front();
			bacQue.pop();

			int tag = afb1->getTag();
			switch (tag) {
			case ADD: case MINU:case DIV: case MULT: {
				afb2 = afbStack.top();
				afbStack.pop();
				afb3 = afbStack.top();
				afbStack.pop();
				AbsFuncBlock* afb0 = BlockGenr(tag, afb3, afb2);
				afbStack.push(afb0);
				break;
			}
			default: {
				afbStack.push(afb1);
			}
			}
		}
		afb2 = afbStack.top();
		afbStack.pop();
		this->Afb = afb2;
	}
	


	AbsFuncBlock* GnlFuncReader::BlockGenr(int tag, AbsFuncBlock* afb1, AbsFuncBlock* afb2) {
		switch (tag) {
		case ADD: {
			AddFuncBlock* adfb = new AddFuncBlock;
			adfb->setTag(ADD);
			adfb->load(afb1, afb2);
			return adfb;
		}
		case MINU: {
			AddFuncBlock* mifb = new AddFuncBlock;
			mifb->setTag(MINU);
			mifb->load(afb1, afb2);
			return mifb;
		}
		case MULT: {

		}
		case DIV: {

		}
		default:return NULL;
		}
		return NULL;
	}
}