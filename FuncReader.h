#pragma once
#include "BasFuncBlock.h"
#include<vector>
#include<queue>
#include<stack>
namespace FuncReader {
	using namespace BasFuncBlock;
	/// <summary>
	/// 通用函数读写器，支持将一个字符串翻译成函数。
	/// 有幂函数，指数函数，对数函数，三角函数，反三角函数。
	/// 支持复合函数阅读。
	/// 
	/// 复合函数的tag支持掩码。
	/// </summary>
	class GnlFuncReader {
#define LINC 30
#define RINC 31
#define COMB 13
#define NORM -1
	public:
			/// <summary>
			/// 粗略翻译，将函数包装成块变成单元块。
			/// 依旧是中缀表达式。
			/// 但是内部调用了两个翻译头。
			/// 第一个将中缀表达式转成后缀表达式。
			/// 第二个将后缀表达式翻译成总函数块。
			/// </summary>
			/// <param name="s">传入表达式</param>
		void load(std::string s);

		AbsFuncBlock* getAfb();
	private:
		int end;
		/// <summary>
		/// 用于存中缀表达式的简化式，相当于把函数块挖成空格。
		/// </summary>
		std::vector<int> ecde;
		/// <summary>
		/// que:    用于存中缀表达式顺序的函数单元块。
		/// bacQue: 用于存放后缀表达式
		/// </summary>
		std::queue<AbsFuncBlock*> que, bacQue;
		//总函数块。
		AbsFuncBlock* Afb = NULL;
		/// <summary>
		/// 针对字符判断是否是运算符
		/// </summary>
		/// <param name="c">:传入字符</param>
		/// <returns>true:是运算符</returns>
		/// 
		int isSign(char c);
		/// <summary>
		/// 判断符号优先级大小
		/// </summary>
		/// <param name="x">运算符</param>
		/// <param name="y">运算符</param>
		/// <returns>true: x的优先级小于y</returns>
		bool isSmaller(int x, int y);
		/// <summary>
		/// 判断该可疑函数块是否是复合函数
		/// </summary>
		/// <param name="s">函数字符串</param>
		/// <param name="p">可疑位置</param>
		/// <returns>true 是复合函数或带负幂的pwr</returns>
		int isSpeFunc(std::string s, int p);
		/// <summary>
		/// 将中缀表达式转变为后缀表达式
		/// </summary>
		void trans1();
		/// <summary>
		/// 根据后缀表达式生成函数单元
		/// </summary>
		void trans2();
		/// <summary>
		 /// 函数块生成器
		 /// </summary>
		 /// <param name="tag">标识符</param>
		 /// <param name="afb1">函数子块1</param>
		 /// <param name="afb2">函数子块2</param>
		 /// <returns>组合函数块</returns>
		AbsFuncBlock* BlockGenr(int tag, AbsFuncBlock* afb1, AbsFuncBlock* afb2);
	};
}