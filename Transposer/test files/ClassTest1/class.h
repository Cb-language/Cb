#pragma once
#include <string>
#include "includes/Object.h"
#include "includes/String.h"
#include "includes/Array.h"
#include "includes/Primitive.h"
#include "includes/SafePtr.h"

class Test
{
private:
	Primitive<int> do_x;
	String re_b = String(String("sdadasd"));
	void f();;

public:
	Primitive<bool> mi_m = Primitive<bool>(Primitive<bool>(true));
	Test(Primitive<int> x);
protected: };
class Test2 : public Test
{
private:
	Primitive<int> do_y;

public:
	Test2();
protected: };
