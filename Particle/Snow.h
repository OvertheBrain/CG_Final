#pragma once

#include "particle.h"

#define MILD 1
#define BLIZZARD 0

using namespace std;

class Snow {
public:
	snowShader sShader;
	snowMesh sMesh;
	ParticleGenerator snowPG;
	int snowMode;
	//float scale = 0.1f;

	Snow(const char* path, int mode, const char* smogPath = NULL) {
		sShader = snowShader("../Particle/snow.vs", "../Particle/snow.fs");
		sMesh = snowMesh(path, true, false);
		snowMode = mode;
		if(mode == MILD)
		snowPG = ParticleGenerator(sShader, sMesh, 15000, 10, glm::vec3(-10.0, 20.0, -10.0), 0, glm::vec3(20.0, 0.0, 20.0),
			glm::vec4(1.0, 1.0, 1.0, 1), glm::vec4(0.0, 0.0, 0.0, 0.0),
			glm::vec4(1.0, 1.0, 1.0, 1), 1,
			80.0, 2.0,
			0.0, 10.0, 2.0, glm::vec3(0.0, 1.0, 0.0),
			glm::vec3(0.0, -0.2, 0.0), glm::vec3(-0.00, -0.03, -0.00),
			0.3, 0.1);
		else if (mode == BLIZZARD) 
			snowPG = ParticleGenerator(sShader, sMesh, 5000, 20, glm::vec3(-20.0, 20.0, -20.0), 0, glm::vec3(0.0, -20.0, 40.0),
				glm::vec4(1.0, 1.0, 1.0, 1), glm::vec4(0.0, 0.0, 0.0, 0.0),
				glm::vec4(1.0, 1.0, 1.0, 1), 1,
				100.0, 2.0,
				0.0, 10.0, 2.0, glm::vec3(0.0, 1.0, 0.0),
				glm::vec3(0.25, -0.2, 0.0), glm::vec3(-0.00, -0.03, -0.00),
				0.3, 0.1);
		sMesh.setupTexture("../Particle/SnowFlake.png");
		sShader.use();
		sShader.setInt("texture0", 0);
	}

	void SetWeather(bool ing) {
		if (ing && snowMode == BLIZZARD) snowPG.UpdateGrow(20);
		else if (ing && snowMode == MILD) snowPG.UpdateGrow(5);
		else if (!ing) snowPG.UpdateGrow(0);
	}

	void Draw(glm::mat4 view, glm::mat4 projection, glm::vec3 lightpos, Camera camera) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sMesh.texture);
		sShader.use();
		//fire.setShader(snowShader);
		//c.setShader(snowShader, true);
		//snowShader.setBool("isDay", light.isDay);
		//cout << light.isDay << endl;
		snowPG.DrawParticle(view, projection, lightpos, camera.Position);
		if(snowMode == 1)
			snowPG.UpdateGenPos(glm::vec3(camera.Position.x - 20.0f, 20.0f, camera.Position.z - 20.0f));
		else
			snowPG.UpdateGenPos(glm::vec3(camera.Position.x - 10.0f, 20.0f, camera.Position.z - 10.0f));
		snowPG.Update(0.2);
		glDisable(GL_BLEND);
	}

};
