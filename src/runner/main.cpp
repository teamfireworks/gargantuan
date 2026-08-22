#include <flecs.h>

#include <iostream>
#include <string>

struct InstanceClass {
	std::string ClassName;
};

int main(int argc, const char *argv[]) {
	flecs::world ecs;

	ecs.entity("Test").set<InstanceClass>({"Test"});

	ecs.each([](flecs::entity e, InstanceClass &i) {
		std::cout << "Entity " << e.name() << " of id #" << e << " is of class " << i.ClassName.c_str() << std::endl;
	});

	std::cout << "Hello, Gargantuan Rewrite!" << std::endl;
	return 0;
}
