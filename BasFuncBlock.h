#pragma once
#include<string>
#include"BasFunc.h"
namespace BasFuncBlock {
	using namespace BasFunc;
	/// <summary>
	/// 基本抽象函数块。函数块是一个二元组。
	/// 这与后续的复合函数块相互区别。
	/// 二元组的连接符号为加，减，乘，除。
	/// 二元组的组合可以是：函数块，函数块。
	/// 单项函数不能被二元组包括，但可以被一元组包括。
	/// 
	/// TODO：后续加入复合函数块。
	/// </summary>
	class AbsFuncBlock {
#define UNIT 8
#define MULT 21
#define ADD  10
#define DIV  20
#define MINU 11
	public:

		virtual void dt() = 0;

		virtual std::string rtStr() = 0;
		/// <summary>
		/// 复制函数
		/// </summary>
		/// <returns>该函数块的复制对象</returns>
		virtual AbsFuncBlock* copy() = 0;

		void setTag(int t);

		void load(AbsFuncBlock* afb0, AbsFuncBlock* afb1);

		int getTag();
		/// <summary>
		/// 是否为单元
		/// </summary>
		/// <returns>true 是单元</returns>
		bool isUnit();

		bool isNorm();

		bool isZero();

		bool isMult();

		bool isOne();

		double getLparam();

		void setLparam(double s);
		/// <summary>
		/// 类间转化需要类内实现。
		/// 需要拷贝+删除。
		/// </summary>
		/// <param name="mfb">带转化的类</param>
		/// <returns>转化成的类</returns>
		virtual AbsFuncBlock* trans(AbsFuncBlock* mfb) = 0;
		/// <summary>
		/// 判断是否为空函数或0函数。
		/// 如果是0函数则转变为空函数
		/// </summary>
		/// <param name="afb">待判断函数</param>
		/// <returns>true: 该函数为空函数</returns>
		static bool isExist(AbsFuncBlock* afb);
		/// <summary>
		/// sym值传递，左边传到右边
		/// </summary>
		/// <param name="afb0">待传块，被传块</param>
		/// <param name="afb1"></param>
		void copySym(AbsFuncBlock* afb1);

		void setSym(bool isNor, bool isZer, bool isMul, bool isOn);

	protected:
		/// <summary>
		/// 通过标识符向下造型
		/// </summary>
		int tag;

		bool isNor = false;

		bool isZer = false;

		AbsFuncBlock* afb[2] = {NULL};

		double lparam;
	};
	/**************************************************************************/
	/// <summary>
	/// 加法函数块和减法函数块合并
	/// </summary>
	class AddFuncBlock :public AbsFuncBlock {
	public:

		void dt();

		std::string rtStr();
		/// <summary>
		/// 对块进行复制
		/// </summary>
		/// <returns>被复制的块</returns>
		AbsFuncBlock* copy();

		AbsFuncBlock* trans(AbsFuncBlock* afb);
		
	};
	/**************************************************************************/
	/// <summary>
	/// 乘法函数块，因为其求导最终转化为一般加法式，
	/// 因此需要借助外层二元组进行删改。
	/// </summary>
	class MultFuncBlock :public AbsFuncBlock {
	public:
		/// <summary>
		/// 乘法单元求导，需要外部求导造型成加法单元
		/// 更新两个子单元
		/// </summary>
		void dt();

		std::string rtStr();

		AbsFuncBlock* copy();
		//TODO 增加对0判断
		void setLpara();

};
	/**************************************************************************/
	/// <summary>
	/// 单元块
	/// 函数块与函数元的交流接口
	/// </summary>
	/// <param name="s"></param>
	class UnitFuncBlock : public AbsFuncBlock {


#define PWR 1
#define IND 2
#define ARI 3
#define TRI 4

#define ART 5


	public:

		void dt();

		std::string rtStr();
		/// <summary>
		/// 读写头保证输入的函数为带参单项基本初等函数。
		/// </summary>
		/// <param name="s">读入字符串</param>
		void load(std::string s);

		AbsFuncBlock* copy();

		void setLpara(double s);

		double getLpara();

	private:

		AbsBasFunc* abf = NULL;


	};
	/**************************************************************************/
	/// <summary>
	/// 没啥卵用的类，只是用来给读写头用来存符号的。
	/// </summary>
	class NoneFuncBlock :public AbsFuncBlock {
	public:
		void dt();

		std::string rtStr();

		void load(std::string);

		AbsFuncBlock* copy();

	};
}
