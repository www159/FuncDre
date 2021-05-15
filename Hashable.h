#pragma once
class Hashable
{
public:
	virtual int hashCode() = 0;
protected:
	const int pr = 1e9 + 7;
};


