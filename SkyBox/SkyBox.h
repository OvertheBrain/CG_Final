#pragma once
#include "../include/shader.h"
#include <vector>
using namespace std;

float skyboxVertices[] = {
    // positions          
    -30.0f,  30.0f, -30.0f,
    -30.0f, -30.0f, -30.0f,
     30.0f, -30.0f, -30.0f,
     30.0f, -30.0f, -30.0f,
     30.0f,  30.0f, -30.0f,
    -30.0f,  30.0f, -30.0f,

    -30.0f, -30.0f,  30.0f,
    -30.0f, -30.0f, -30.0f,
    -30.0f,  30.0f, -30.0f,
    -30.0f,  30.0f, -30.0f,
    -30.0f,  30.0f,  30.0f,
    -30.0f, -30.0f,  30.0f,

     30.0f, -30.0f, -30.0f,
     30.0f, -30.0f,  30.0f,
     30.0f,  30.0f,  30.0f,
     30.0f,  30.0f,  30.0f,
     30.0f,  30.0f, -30.0f,
     30.0f, -30.0f, -30.0f,

    -30.0f, -30.0f,  30.0f,
    -30.0f,  30.0f,  30.0f,
     30.0f,  30.0f,  30.0f,
     30.0f,  30.0f,  30.0f,
     30.0f, -30.0f,  30.0f,
    -30.0f, -30.0f,  30.0f,

    -30.0f,  30.0f, -30.0f,
     30.0f,  30.0f, -30.0f,
     30.0f,  30.0f,  30.0f,
     30.0f,  30.0f,  30.0f,
    -30.0f,  30.0f,  30.0f,
    -30.0f,  30.0f, -30.0f,

    -30.0f, -30.0f, -30.0f,
    -30.0f, -30.0f,  30.0f,
     30.0f, -30.0f, -30.0f,
     30.0f, -30.0f, -30.0f,
    -30.0f, -30.0f,  30.0f,
     30.0f, -30.0f,  30.0f
};

class SkyBox {
private:
	unsigned int skyboxVAO, skyboxVBO;
	unsigned int forestTexture,factoryTexture,wallTexture;
	string texture;
	int textureID; 

	unsigned int loadCubemap(vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrComponents;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                // glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            }
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			}
			stbi_image_free(data);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}

public:
	Shader forestShader,factoryShader,wallShader;

	SkyBox():forestShader(Shader("../SkyBox/Forest.vs", "../SkyBox/Forest.fs")),
			factoryShader(Shader("../SkyBox/Factory.vs", "../SkyBox/Factory.fs")) ,
			wallShader(Shader("../SkyBox/Wall.vs", "../SkyBox/Wall.fs")){
		this->texture="wall";
		this->textureID = 1; 
		// skybox VAO
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		// load textures
		// -------------
		vector<std::string> faces1
		{
			"../SkyBox/Skansen/PX.jpg",//+x
			"../SkyBox/Skansen/NX.jpg",//-x
			"../SkyBox/Skansen/PY.jpg",//+y
			"../SkyBox/Skansen/NY.jpg",//-y
			"../SkyBox/Skansen/PZ.jpg",//+z
			"../SkyBox/Skansen/NZ.jpg",//-z
		};
		forestTexture = loadCubemap(faces1);
		forestShader.use();
		forestShader.setInt("forest", 0);

		vector<std::string> faces2
		{
			"../SkyBox/Factory/PX.jpg",//+x
			"../SkyBox/Factory/NX.jpg",//-x
			"../SkyBox/Factory/PY.jpg",//+y
			"../SkyBox/Factory/NY.jpg",//-y
			"../SkyBox/Factory/PZ.jpg",//+z
			"../SkyBox/Factory/NZ.jpg",//-z
		};
		factoryTexture = loadCubemap(faces2);
		factoryShader.use();
		factoryShader.setInt("factory", 0);
		
		vector<std::string> faces3
		{
			"../SkyBox/Wall/PX.jpg",//+x
			"../SkyBox/Wall/NX.jpg",//-x
			"../SkyBox/Wall/PY.jpg",//+y
			"../SkyBox/Wall/NY.jpg",//-y
			"../SkyBox/Wall/PZ.jpg",//+z
			"../SkyBox/Wall/NZ.jpg",//-z
		};
		wallTexture = loadCubemap(faces3);
		wallShader.use();
		wallShader.setInt("wall", 0);

	}

	void drawSkybox(glm::mat4 view, glm::mat4 projection) {
		glDepthFunc(GL_LEQUAL);
		glActiveTexture(GL_TEXTURE0);
		if(texture=="forest"){
			glBindTexture(GL_TEXTURE_CUBE_MAP, forestTexture);
		
    	    forestShader.use();
			forestShader.setMat4("view", view);
			forestShader.setMat4("projection", projection);
		} else if(texture=="factory")
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, factoryTexture);
		
    	    factoryShader.use();
			factoryShader.setMat4("view", view);
			factoryShader.setMat4("projection", projection);
		} else if(texture=="wall"){
			glBindTexture(GL_TEXTURE_CUBE_MAP, wallTexture);
		
    	    wallShader.use();
			wallShader.setMat4("view", view);
			wallShader.setMat4("projection", projection);
		}
		
		glBindVertexArray(skyboxVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
	}

	void changeTexture(){
		textureID++;
		switch(textureID % 3){
			case 1: texture = "wall";break; 
			case 2: texture = "forest";break;
			case 0: texture = "factory";break;
		}
			
	}
};
