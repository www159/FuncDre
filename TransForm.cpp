#include "TransForm.h"



double TransForm::str2d(std::string s) {
	int p = 0, len = 5;
	double f = 1;
	double ans = 0;
	double tai = 0;
	double eps = 1e-6;
	if (s[p] == '-') {
		f = -f;
		p++;
	}
	if (s[p] == '+') {
		p++;
	}
	while (s[p] != '.' && p != s.size()) {
		ans = ans * 10 + ((double)s[p] - '0');
		p++;
	}
	if (p == s.length()) {
		return f * ans;
	}
	p++;
	while (len-- && p != s.length()) {
		tai = tai * 10 + s[p] - '0';
		p++;
	}
	while (tai > 1) {
		tai /= 10;
	}
	ans += tai;
	return ans * f;
}



std::string TransForm::ridz(std::string s) {
	int p = s.length() - 1;
	int t = s.length() - 2;
	while (s[p] != '.' && p) {
		p--;
	}
	if (p == 0) {
		return s;
	}

	while (s[t] == '0' && s[t + 1] == '0') {
		t--;
	}
	if (s[t] == '.') {
		t--;
	}
	return s.substr(0, t + 1);
}



std::string TransForm::d2str(double x) {
	return ridz(std::to_string(x));
}