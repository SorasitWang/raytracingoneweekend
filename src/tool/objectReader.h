#ifndef OBJECT_READER_H
#define OBJECT_READER_H

#include <memory>
#include <fstream>
#include <string>
#include <vector>

#include "../common.h"
#include "../hittable/polygon.h"
#include "../math/triangle.h"

using namespace std;

namespace Reader {

	Polygon read_obj_file(std::string fileName, shared_ptr<material> mat) {
		/*
			TODO :
				- Handle > 3 vertices/face
				- Accept texture coord
				- Read material
		*/
		std::fstream file(fileName);

		if (!file)
			throw std::runtime_error( fileName + " Not found" );

		std::vector<Triangle*> faceList;
		std::string line;
		std::string xx, yy, zz;
		
		std::vector<point3> vertices;
		std::vector<vec3> normals;
		std::vector<point3> uvs;

		while (std::getline(file, line)) {
			size_t flagEndIndex = line.find(" ");
			std::string op = line.substr(0, flagEndIndex);

			if (!(op == "v" || op == "vn" || op == "f"))
				continue;
			
			std::istringstream stream(line.substr(flagEndIndex, line.size()));
			//	Split by space
			stream >> xx >> yy >> zz;

			if (op == "v")
				vertices.push_back(point3(stod(xx), stod(yy), stod(zz)));
			else if (op == "vn")
				normals.push_back(vec3(stod(xx), stod(yy), stod(zz)));
			else if (op == "f") {
				/*
					f v1 v2 v3
					f v1/vt1 v2/vt2 v3/vt3
					f v1//vn1 v2//vn2 v3//vn3
					f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
				*/
				std::vector<point3> verticesOneTriangle;
				int normalIdx = -1;
				size_t vertexIdx = -1, uvIdx = -1;

				for (std::string s : { xx, yy, zz }) {
					
					size_t prev = 0, current = 0;
					current = s.find("/", prev);
					
					if (current == std::string::npos)
						current = s.size();

					std::string faceStr = s.substr(prev, current);
					vertexIdx = stoi(faceStr);
					verticesOneTriangle.push_back(vertices.at(vertexIdx - 1));
					
					if (current == s.size())
						continue;
				
					prev = current;
					current = s.find("/", prev+1);					
					if (current == std::string::npos) {
						current = s.size();
						std::string uvStr = s.substr(prev+1, current);
						uvIdx = stoi(uvStr);
						continue;
					}

					prev = current+1;
					std::string normalStr = s.substr(prev+1, s.size() - 1);

					normalIdx = stoi(normalStr);
				}
				//	Assume that every vertex's normal are equal
				if (normalIdx != -1) {
					faceList.push_back(	new Triangle(verticesOneTriangle.data(),
										vertices.at(vertexIdx - 1)) );
				}
				else {
					faceList.push_back(new Triangle(verticesOneTriangle.data()));
				}
			}
			
		}

		file.close();

		std::cout << "Total faces from " + fileName + " : " + std::to_string(faceList.size()) << std::endl;
		
		return Polygon(faceList, mat);
	}
}

#endif