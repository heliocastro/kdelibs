    /*

    Copyright (C) 2000 Stefan Westerfeld stefan@space.twc.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Permission is also granted to link this program with the Qt
    library, treating Qt like a library that normally accompanies the
    operating system kernel, whether or not that is in fact the case.

    */

#include "wrapper.h"
#include "utils.h"
#include <iostream.h>
#include <stdio.h>

int x = 0;
int active_d_objects = 0;

class D_impl: virtual public D_skel {
private:
	long _value;
public:
	void value(long newVal) { _value = newVal; }
	long value() { return _value; }

	D_impl() :_value(0) { active_d_objects++; }
	~D_impl() { active_d_objects--; }
	string a() { return "a"; }
	string b() { return "b"; }
	string c() { return "c"; }
	string d() { return "d"; }
	void dummy() { x++; }

	long sum(A_base *v1, A_base *v2) { return v1->value() + v2->value(); }
	D_base *self() { return _copy(); }
};

REGISTER_IMPLEMENTATION(D_impl);

#define CALLS 100000000

void check(const char *name,bool passed)
{
	printf("%30s ... %s\n",name,passed?"PASSED":"FAIL");
}

/*
generic inheritance test, tests that using a D object, you can call the a,b,c,d
methods without problems
*/
void test0()
{
	D d;
	assert(active_d_objects == 0);
	C c = d;
	assert(active_d_objects == 0);
	string abcd = A(d).a()+d.b()+c.c()+d.d();
	check("generic inheritance test",abcd == "abcd");

	/*
	   should be exactly one here, not more than one through the various
	   assignments
	 */
	assert(active_d_objects == 1);
}

/*
- SmartWrappers should be fast when doing method calls
 
  => check that calling a "base->x()" function is not more than factor 2 or so
     faster than calling a "wrapper.x()" function                               
*/
void test1()
{
	int i;

	/* "old" calls */
	benchmark(BENCH_BEGIN);
	D_var d_oldstyle = D_base::_create();
	x = 0;
	for(i=0;i<CALLS;i++) d_oldstyle->dummy();
	assert(x == CALLS);
	float oldspeed = (float)CALLS/benchmark(BENCH_END);

	/* "new" calls */
	benchmark(BENCH_BEGIN);
	D d_newstyle;
	x = 0;
	for(i=0;i<CALLS;i++) d_newstyle.dummy();
	assert(x == CALLS);

	float newspeed = (float)CALLS/benchmark(BENCH_END);

	check("speed for calls",oldspeed < newspeed * 2.0);
	cout << "  -> old " << (long)(oldspeed) << " calls/sec" << endl;
	cout << "  -> new " << (long)(newspeed) << " calls/sec" << endl;
}

void test2()
{
}
int main()
{
	Dispatcher dispatcher;

	assert(active_d_objects == 0);
	test0();
	assert(active_d_objects == 0);
	test1();
	assert(active_d_objects == 0);

	return 0;
}
