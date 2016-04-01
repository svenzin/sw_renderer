#pragma once

#include <string>
#include <fstream>
#include <sstream>

#include <vector>

#include <Vec3D.hh>

struct Model {
	typedef struct {
		int vertices[3];
	} Triangle;

	std::vector<Vec3D> vertices;
	std::vector<Triangle> faces;

	static Model OBJ(const std::string & filename);
	static Model OBJ(std::istream & input);
};

Model Model::OBJ(const std::string & filename) {
	std::ifstream file(filename);
	return Model::OBJ(file);
}

Model Model::OBJ(std::istream & input) {
	Model m;

	std::string line;
	while (!input.eof()) {
		std::getline(input, line);

		std::istringstream str(line);
		//str.exceptions(std::ios_base::badbit);

		std::string type;
		str >> type;
		if (type == "v") {
			Vec3D v;
			str >> v.x >> v.y >> v.z;
			if (!str.fail()) m.vertices.push_back(v);
		} else if (type == "f") {
			Triangle t;
			char c;
			float f;
			for (int i = 0; i < 3; ++i) {
				str >> t.vertices[i] >> c >> f >> c >> f;
				t.vertices[i] -= 1;
			}
			if (!str.fail()) m.faces.push_back(t);
		}
	}

	return m;
}
