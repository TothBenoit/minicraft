#ifndef __YOCTO__ENGINE_TEST__
#define __YOCTO__ENGINE_TEST__

#include "engine/engine.h"

#include "avatar.h"
#include "world.h"

class MEngineMinicraft : public YEngine {
	YFbo* Fbo;

	GLuint ShaderWorld = 0;
	GLuint ShaderCube = 0;
	GLuint ShaderSun = 0;
	GLuint ShaderPostProcess = 0;

	MWorld* World;
	YVbo* VboCube;

	MAvatar* avatar;

	YTexFile* atlas;

	float  timeOffset = 0;
	YColor SunColor = YColor(1, 1, 0.5, 1);
	float exposure;
	bool night;
	YColor skyColor;

	int lastMouseX = 0;
	int lastMouseY = 0;
	int currentMouseX = 0;
	int currentMouseY = 0;

	bool rightClickPressed = false;

	float speedRotation = 0.001f;

public:
	//Gestion singleton
	static YEngine* getInstance()
	{
		if (Instance == NULL)
			Instance = new MEngineMinicraft();
		return Instance;
	}

	/*HANDLERS GENERAUX*/
	void loadShaders()
	{
		ShaderSun = Renderer->createProgram("shaders/sun");
		ShaderCube = Renderer->createProgram("shaders/cube");
		ShaderWorld = Renderer->createProgram("shaders/world");
		ShaderPostProcess = Renderer->createProgram("shaders/postprocess");

	}

	void LoadCube(YVbo* vbo)
	{
		//Face avant
		int iVertice = 0;
		vbo->setElementValue(0, iVertice, 1, -1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 1, 0, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, 1, 1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 1, 0, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, 1, -1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 1, 0, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)

		iVertice++;
		vbo->setElementValue(0, iVertice, 1, -1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 1, 0, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, 1, 1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 1, 0, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, 1, 1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 1, 0, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)

		//Face droite
		iVertice++;
		vbo->setElementValue(0, iVertice, 1, 1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 1, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, -1, 1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 1, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, 1, 1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 1, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)

		iVertice++;
		vbo->setElementValue(0, iVertice, 1, 1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 1, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, -1, 1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 1, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, -1, 1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 1, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)

		//Face haute
		iVertice++;
		vbo->setElementValue(0, iVertice, 1, -1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 0, 1);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, -1, 1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 0, 1);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, -1, -1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 0, 1);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)

		iVertice++;
		vbo->setElementValue(0, iVertice, 1, -1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 0, 1);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, 1, 1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 0, 1);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, -1, 1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 0, 1);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)


		//Face basse
		iVertice++;
		vbo->setElementValue(0, iVertice, -1, 1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 0, -1);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, 1, -1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 0, -1);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, -1, -1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 0, -1);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)

		iVertice++;
		vbo->setElementValue(0, iVertice, -1, 1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 0, -1);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, 1, 1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 0, -1);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, 1, -1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, 0, -1);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)

		//Face arrière gauche
		iVertice++;
		vbo->setElementValue(0, iVertice, -1, -1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, -1, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, 1, -1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, -1, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, 1, -1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, -1, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)

		iVertice++;
		vbo->setElementValue(0, iVertice, -1, -1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, -1, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, 1, -1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, -1, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, -1, -1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, 0, -1, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)

		//Face arrière droite
		iVertice++;
		vbo->setElementValue(0, iVertice, -1, 1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, -1, 0, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, -1, -1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, -1, 0, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, -1, 1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, -1, 0, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)

		iVertice++;
		vbo->setElementValue(0, iVertice, -1, 1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, -1, 0, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, -1, -1, -1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, -1, 0, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		iVertice++;
		vbo->setElementValue(0, iVertice, -1, -1, 1); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, iVertice, -1, 0, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
	}

	void init()
	{
		YLog::log(YLog::ENGINE_INFO, "Minicraft Started : initialisation");

		Renderer->setBackgroundColor(YColor(0.0f, 0.0f, 0.0f, 1.0f));
		Renderer->Camera->setPosition(YVec3f(200, 200, 200));
		Renderer->Camera->setLookAt(YVec3f());

		Fbo = new YFbo(1);
		Fbo->init(Renderer->ScreenWidth, Renderer->ScreenHeight);

		//Creation du VBO
		VboCube = new YVbo(3, 36, YVbo::PACK_BY_ELEMENT_TYPE);

		//Définition du contenu du VBO
		VboCube->setElementDescription(0, YVbo::Element(3)); //Sommet
		VboCube->setElementDescription(1, YVbo::Element(3)); //Normale
		VboCube->setElementDescription(2, YVbo::Element(2)); //UV

		//On demande d'allouer la mémoire coté CPU
		VboCube->createVboCpu();

		LoadCube(VboCube);

		//Pour créer le monde
		World = new MWorld();
		World->init_world(rand());

		//On envoie le contenu au GPU
		VboCube->createVboGpu();

		//On relache la mémoire CPU
		VboCube->deleteVboCpu();

		avatar = new MAvatar(Renderer->Camera, World);

		atlas = YTexManager::getInstance()->loadTexture("textures\\atlas.png");
	}

	void update(float elapsed)
	{
		avatar->update(elapsed);

		int deltaMouseX = currentMouseX - lastMouseX;
		int deltaMouseY = currentMouseY - lastMouseY;

		lastMouseX = currentMouseX;
		lastMouseY = currentMouseY;

		if (rightClickPressed)
		{
			Renderer->Camera->rotate(deltaMouseX * speedRotation );
			Renderer->Camera->rotateUp(deltaMouseY * speedRotation );
		}

	}

	void CalculateSkyColor(float minuteTime)
	{
		YColor fullDayColor(0.0f, 181.f / 255.f, 221.f / 255.f, 1.0f);
		YColor dawnColor(0.9f, 0.4f, 0.f, 1.f);
		YColor nightColor(0.2f, 0.2f, 0.45f, 1.f);
		float mnLever = 6 * 50;
		float mnCoucher = 19 * 60;

		float dTime = minuteTime;
		dTime -= mnLever;
		dTime /= (mnCoucher - mnLever);
		dTime *= (float)M_PI;


		if ((night = minuteTime < mnLever || minuteTime > mnCoucher))
		{
			skyColor = dawnColor.interpolate(nightColor, abs(2 * sin(dTime)));
			Renderer->setBackgroundColor(skyColor);
			exposure = 0;
		}
		else
		{
			skyColor = fullDayColor.interpolate(dawnColor, abs(cos(dTime)));
			Renderer->setBackgroundColor(skyColor);
			exposure = 1- 0.6*(abs(cos(dTime)));
		}
	}

	void renderObjects()
	{
		Fbo->setAsOutFBO(true);
		glUseProgram(0);
		//Rendu des axes
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
		glColor3d(1, 0, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(10000, 0, 0);
		glColor3d(0, 1, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 10000, 0);
		glColor3d(0, 0, 1);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, 10000);
		glEnd();

		SYSTEMTIME t;
		GetLocalTime(&t);

		//Temps écoulé depuis le début de la journée
		float minuteTime = (float)(t.wHour * 60 + t.wMinute);
		minuteTime += timeOffset;
		while (minuteTime > 24 * 60)
			minuteTime -= 24 * 60;

		CalculateSkyColor(minuteTime);
	
		YVec3f SunPos = YVec3f(MWorld::MAT_SIZE_METERS / 2, MWorld::MAT_SIZE_METERS / 2, -MWorld::MAT_HEIGHT_METERS * 2);
		SunPos.rotate(YVec3f(1,0,0), (minuteTime / (60 * 24)) * 2 * M_PI);

		glPushMatrix();
		glUseProgram(ShaderSun); //Demande au GPU de charger ces shaders
		GLuint var = glGetUniformLocation(ShaderSun, "sun_color");
		glUniform3f(var, SunColor.R, SunColor.V, SunColor.B);

		YVec3f RenderSunPos = SunPos + avatar->Position;
		glTranslatef(RenderSunPos.X, RenderSunPos.Y, RenderSunPos.Z);
		glScalef(10, 10, 10);
		Renderer->updateMatricesFromOgl(); //Calcule toute les matrices à partir des deux matrices OGL
		Renderer->sendMatricesToShader(ShaderSun); //Envoie les matrices au shader
		VboCube->render(); //Demande le rendu du VBO
		glPopMatrix();

		glUseProgram(ShaderWorld);
		Renderer->sendTimeToShader(DeltaTimeCumul, ShaderWorld);

		var = glGetUniformLocation(ShaderWorld, "skyColor");
		glUniform3f(var, skyColor.R, skyColor.V, skyColor.B);

		var = glGetUniformLocation(ShaderWorld, "sunPos");
		glUniform3f(var, SunPos.X, SunPos.Y, SunPos.Z);

		var = glGetUniformLocation(ShaderWorld, "night");
		glUniform1f(var, night);

		var = glGetUniformLocation(ShaderWorld, "camPos");
		glUniform3f(var, Renderer->Camera->Position.X, Renderer->Camera->Position.Y, Renderer->Camera->Position.Z);

		var = glGetUniformLocation(ShaderWorld, "specularColor");
		glUniform3f(var, SunColor.R, SunColor.V, SunColor.B);

		atlas->setAsShaderInput(ShaderWorld, GL_TEXTURE0, "atlas");

#if _DEBUG
		World->render_world_vbo(true, true, ShaderWorld);
		//World->render_world_basic(ShaderCube, VboCube);
#else
		World->render_world_vbo(false, true, ShaderWorld);
		//World->render_world_basic(ShaderCube, VboCube);
#endif

		Fbo->setAsOutFBO(false);

		glUseProgram(ShaderPostProcess);

		var = glGetUniformLocation(ShaderPostProcess, "sunPos");
		glUniform3f(var, RenderSunPos.X, RenderSunPos.Y, RenderSunPos.Z);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		Fbo->setColorAsShaderInput(0, GL_TEXTURE0, "TexColor");
		Fbo->setDepthAsShaderInput(GL_TEXTURE1, "TexDepth");

		var = glGetUniformLocation(ShaderPostProcess, "exposure");

		glUniform1f(var,exposure);

		Renderer->sendMatricesToShader(ShaderPostProcess); //Envoie les matrices au shader
		Renderer->sendNearFarToShader(ShaderPostProcess);
		Renderer->drawFullScreenQuad();
	}

	void resize(int width, int height) {
		Fbo->resize(width, height);
	}

	/*INPUTS*/

	void keyPressed(int key, bool special, bool down, int p1, int p2)
	{
		switch (key)
		{
		case 'E':
		case 'e':
			avatar->Hit = down;
			break;
		case 'G':
		case 'g':
			timeOffset += 20;
			if (timeOffset > 24 * 60)
				timeOffset = 0;
			break;
		case 'Z':
		case 'z':
			avatar->avance = down;
			break;
		case 'Q':
		case 'q':
			avatar->gauche = down;
			break;
		case 'S':
		case 's':
			avatar->recule = down;
			break;
		case 'D':
		case 'd':
			avatar->droite = down;
			break;
		case ' ':
			avatar->Jump = down;
			break;
		}

	}

	void mouseWheel(int wheel, int dir, int x, int y, bool inUi)
	{
	}

	void mouseClick(int button, int state, int x, int y, bool inUi)
	{
		if (button == 2)
		{
			rightClickPressed = state == 0;
		}
	}

	void mouseMove(int x, int y, bool pressed, bool inUi)
	{
		currentMouseX = x;
		currentMouseY = y;
	}
};


#endif