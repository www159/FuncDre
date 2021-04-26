#pragma once
#include<string>
/*
*	转接头，用于double和string之间相互转化。
*	还有去除小数多余0的功能。
*/
class TransForm
{
public:

	static double str2d(std::string s);

	static std::string d2str(double x);
	
private:
	//除去多余的末尾0，用于高精
	static std::string ridz(std::string s);
};

