#pragma once
#include <vector>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>

using namespace std;


//bezier旋转曲面类
class BezierFace
{
	//顶点数
	int numVertices;
	//索引数(每个四边形片6个顶点)
	int numIndices;

	float texrange_l,texrange_r;

	//uv 分段数
	
	//attributes
	vector<glm::vec3> vertices;
	vector<glm::vec2> texCoords;
	vector<glm::vec3> normals;
	vector<float> us;
	vector<float> rs;
	vector<float> ys;
	
	vector<int> indices;
	//生成
	void generate(int prec);
	//控制顶点
	float* controlPoints;
	vector<glm::vec2> controlPointsVector;

	float toRadians(float degrees);
	float Bernstein(float u, int index);
public:
	
	BezierFace();
	//BezierFace(int i); 
	BezierFace(vector<glm::vec2> controlPointsVector,float l=0,float r=1); 
	
	float getLength();
	int getNumVertices();
	int getNumIndices();
	vector<glm::vec3> getVertices();
	vector<glm::vec3> getNormals();
	vector<glm::vec2> getTexCoords();
	vector<int> getIndices();

	bool getRadiance(float pos,float &y,float &r);


};

#define step 50

float defControlPoints[] = {

	3.0, 3.0,
	3.0, 2.0,
	3.0, 1.0,
	3.0, 0.0,
	0.0, 3.0,
	1.0, 3.0,
	2.0, 3.0,
	3.0, 3.0,
	0.0, 0.0,
	1.0, 0.0,
	2.0, 0.0,
	3.0, 0.0,

};

bool BezierFace::getRadiance(float pos, float& y, float& r) {
	if (pos < texrange_l || pos >= texrange_r)return false;
	for (int i = 0; i < us.size(); i++) {
		if (pos - us[i] < 0.01) {
			y = ys[i];
			r = rs[i];
			return true;
		}
	}
	return false;

}


void BezierFace::generate(int prec)
{
	numVertices = (prec + 1) * (prec + 1);
	numIndices = prec * prec * 6;
	us.clear();
	//初始化空白数组
	for (int i = 0; i < numVertices; i++)
	{
		vertices.push_back(glm::vec3());
		normals.push_back(glm::vec3());
		texCoords.push_back(glm::vec2());
	}
	for (int i = 0; i < numIndices; i++)
	{
		indices.push_back(0);
	}

	for (int i = 0; i <= prec; i++)//i:从上到下
	{

		for (int j = 0; j <= prec; j++) { //旋转一周
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
			float r = 0.0f;
			float r1 = 0.0f;
			float y1 = 0.0f;

			float u = (float)i / prec;
			float u1 = (float)(i + 1) / prec;
			float v = (float)j / prec;
			float theta = toRadians(v);

			for (int k = 0; k <= 3; k++)
			{
				int index = k;
				r += controlPointsVector[index].x * Bernstein(u, k);
				r1 += controlPointsVector[index].x * Bernstein(u1, k);
				y += controlPointsVector[index].y * Bernstein(u, k);
				y1 += controlPointsVector[index].y * Bernstein(u1, k);

			}
			x = r * cos(theta);
			z = r * sin(theta);

			u = texrange_l + (texrange_r - texrange_l) * u;
			us.push_back(u);
			ys.push_back(y);
			rs.push_back(r);

			vertices[i * (prec + 1) + j] = glm::vec3(x, y, z);

			float nx = cos(theta);
			float nz = sin(theta);
			//dy/dr= nr/-ny
			float ny;

			float mu = 0.0001;
			if (y1 - y < mu && y - y1 < mu) {
				nx = 0;
				nz = 0;
				ny = -1;
			}
			else ny = -(1.0) * (r1 - r) / (y1 - y);

			if (y1 - y < 0 && r1 - r < 0) {
				nx = -nx;
				nz = -nz;
				ny = -ny;

			}
			normals[i * (prec + 1) + j] = glm::vec3(nx, ny, nz);
			texCoords[i * (prec + 1) + j] = glm::vec2(v, u);
		}
	}
	//计算索引
	for (int i = 0; i < prec; i++) {
		for (int j = 0; j < prec; j++) {
			int k = 6 * (i * prec + j);
			indices[k + 0] = i * (prec + 1) + j;
			indices[k + 1] = i * (prec + 1) + j + 1;
			indices[k + 2] = (i + 1) * (prec + 1) + j;

			indices[k + 3] = i * (prec + 1) + j + 1;
			indices[k + 4] = (i + 1) * (prec + 1) + j + 1;
			indices[k + 5] = (i + 1) * (prec + 1) + j;

		}
	}
}
float BezierFace::toRadians(float degrees) { return (degrees * 2.0f * 3.14159f); }

float BezierFace::Bernstein(float t, int index)
{
	switch (index)
	{
	default:
	case 0:
		return pow(1.0 - t, 3);
		break;
	case 1:
		return 3 * t * pow(1.0 - t, 2);
		break;
	case 2:
		return 3 * pow(t, 2) * (1 - t);
		break;
	case 3:
		return pow(t, 3);
		break;
	}

}
BezierFace::BezierFace() {}


BezierFace::BezierFace(vector<glm::vec2> vec, float l, float r) {
	controlPointsVector = vec;
	texrange_r = r;
	texrange_l = l;
	generate(step);
}

int BezierFace::getNumVertices()
{
	return numVertices;
}

int BezierFace::getNumIndices()
{
	return numIndices;
}

vector<glm::vec3> BezierFace::getVertices()
{
	return vertices;
}

vector<glm::vec3> BezierFace::getNormals()
{
	return normals;
}

vector<glm::vec2> BezierFace::getTexCoords()
{
	return texCoords;
}

vector<int> BezierFace::getIndices()
{
	return indices;
}