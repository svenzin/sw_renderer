#include <Model.hh>

#include <iostream>

#include <string>
#include <sstream>

#define CHECK(statement) {             \
	if ( ! (statement) ) return false; \
}

std::string ground = R"(
v -1 -1 -1
v  1 -1 -1
v  1 -1  1
v -1 -1  1

vt 0 0
vt 1 0
vt 1 1
vt 0 1

vn 0 1 0

f 3/3/1 2/2/1 1/1/1
f 4/4/1 3/3/1 1/1/1

)";

template <class U, class V> bool equal(U expected, V actual) { return expected == actual; }

bool Test_Ground() {
	std::istringstream obj(ground);
	auto m = Model::OBJ(obj);

	CHECK(equal(4, m.vertices.size()));
	CHECK(equal(2, m.faces.size()));

	return true;
}

#define RUN(test) {                                    \
	std::cout << "Running "#test"...";                 \
	bool b = test();                                   \
	std::cout << (b ? " Done" : " FAIL") << std::endl; \
	success = success && b;                            \
}

int main(int argc, char ** argv) {
	bool success = true;

	RUN(Test_Ground);

	char c;
	std::cin >> c;

	return success ? 0 : 1;
}
