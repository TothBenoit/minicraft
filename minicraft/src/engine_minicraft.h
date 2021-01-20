#ifndef __YOCTO__ENGINE_TEST__
#define __YOCTO__ENGINE_TEST__

#include "engine/engine.h"

#include "avatar.h"
#include "world.h"

class MEngineMinicraft : public YEngine {
	GLuint ShaderWorld = 0;
	GLuint ShaderCube = 0;
	GLuint ShaderSun = 0;
	YVbo* VboCube;
	MAvatar* avatar;

	float  timeOffset = 0;

	MWorld* World;

	int lastMouseX = 0;
	int lastMouseY = 0;
	int currentMouseX = 0;
	int currentMouseY = 0;

	bool forwardPressed = false;
	bool leftPressed = false;
	bool rightPressed = false;
	bool backwardPressed = false;
	bool ctrlPressed = false;
	bool rightClickPressed = false;
	bool middleMousePressed = false;

	YVec3f wantedMoveCameraSpace;
	YVec3f wantedMoveWorldSpace;

	float speed = 0.5f;
	float speedRotation = 0.001f;
	float scrollSpeed = 35.f;

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
	}

	void update(float elapsed)
	{
		avatar->update(elapsed);

		int deltaMouseX = currentMouseX - lastMouseX;
		int deltaMouseY = currentMouseY - lastMouseY;

		lastMouseX = currentMouseX;
		lastMouseY = currentMouseY;

		//wantedMoveWorldSpace.X = 0;
		//wantedMoveWorldSpace.Y = 0;
		//wantedMoveWorldSpace.Z = 0;

		//YVec3f rightDir = Renderer->Camera->RightVec;
		//YVec3f forwardDir = Renderer->Camera->Direction;

		//if (forwardPressed)
		//{
		//	wantedMoveWorldSpace += forwardDir * speed * elapsed;
		//}
		//if (backwardPressed)
		//{
		//	wantedMoveWorldSpace -= forwardDir * speed * elapsed;
		//}
		//if (leftPressed)
		//{
		//	wantedMoveWorldSpace -= rightDir * speed * elapsed;
		//}
		//if (rightPressed)
		//{
		//	wantedMoveWorldSpace += rightDir * speed * elapsed;
		//}
		if (rightClickPressed)
		{
			if (ctrlPressed)
			{
				Renderer->Camera->rotateAround(deltaMouseX * speedRotation );
				Renderer->Camera->rotateUpAround(deltaMouseY * speedRotation );
			}
			else
			{
				Renderer->Camera->rotate(deltaMouseX * speedRotation );
				Renderer->Camera->rotateUp(deltaMouseY * speedRotation );
			}
		}
		/*if (middleMousePressed)
		{
			YVec3f scrollDir = -rightDir * deltaMouseX + forwardDir * deltaMouseY;
			scrollDir.Z = 0;
			Renderer->Camera->move(scrollDir * scrollSpeed * elapsed);
		}*/
		//Renderer->Camera->move(wantedMoveWorldSpace);

	}

	void renderObjects()
	{
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

		YColor fullDayColor(0.0f, 181.f / 255.f, 221.f / 255.f, 1.0f);
		YColor dawnColor(0.9f, 0.4f, 0.f, 1.f);
		YColor nightColor(0.2f, 0.2f, 0.45f, 1.f);
		float mnLever = 6 * 50;
		float mnCoucher = 19 * 60;

		float dTime = minuteTime;
		dTime -= mnLever;
		dTime /= (mnCoucher - mnLever);
		dTime *= (float)M_PI;

		float ambientValue = 1;
		YColor skyColor;
		if (minuteTime < mnLever || minuteTime > mnCoucher)
		{
			skyColor = dawnColor.interpolate(nightColor, abs(2 * sin(dTime)));
			Renderer->setBackgroundColor(skyColor);
			ambientValue = 0.2f;
		}
		else
		{
			skyColor = fullDayColor.interpolate(dawnColor, abs(cos(dTime)));
			Renderer->setBackgroundColor(skyColor);
			ambientValue = 1 + (0.2f - 1) * abs(cos(dTime)); //lerp
		}
		YVec3f SunPos = YVec3f(MWorld::MAT_SIZE_METERS / 2, MWorld::MAT_SIZE_METERS / 2, -MWorld::MAT_HEIGHT_METERS * 2);
		SunPos.rotate(YVec3f(1,0,0), (minuteTime / (60 * 24)) * 2 * M_PI);

		glPushMatrix();
		glUseProgram(ShaderSun); //Demande au GPU de charger ces shaders
		GLuint var = glGetUniformLocation(ShaderSun, "sun_color");
		YColor SunColor(1, 1, 0.5, 1);
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

		var = glGetUniformLocation(ShaderWorld, "ambientColor");
		YColor ambientColor;
		skyColor.toHSV(&ambientColor.R, &ambientColor.V, &ambientColor.B,&ambientColor.A);
		ambientColor.V /= 2;
		ambientColor.B *= 2;
		ambientColor.fromHSV(ambientColor.R, ambientColor.V, ambientColor.B, ambientColor.A);
		glUniform3f(var, ambientColor.R, ambientColor.V, ambientColor.B);

		var = glGetUniformLocation(ShaderWorld, "sunPos");
		glUniform3f(var, SunPos.X, SunPos.Y, SunPos.Z);

		var = glGetUniformLocation(ShaderWorld, "camPos");
		glUniform3f(var, Renderer->Camera->Position.X, Renderer->Camera->Position.Y, Renderer->Camera->Position.Z);

		var = glGetUniformLocation(ShaderWorld, "specularColor");
		glUniform3f(var, SunColor.R, SunColor.V, SunColor.B);

#if _DEBUG
		World->render_world_vbo(true, true, ShaderWorld);
		//World->render_world_basic(ShaderCube, VboCube);
#else
		World->render_world_vbo(false, true, ShaderWorld);
		//World->render_world_basic(ShaderCube, VboCube);
#endif



	}

	void resize(int width, int height) {

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
			//forwardPressed = down;
			avatar->avance = down;
			break;
		case 'Q':
		case 'q':
			//leftPressed = down;
			avatar->gauche = down;
			break;
		case 'S':
		case 's':
			//backwardPressed = down;
			avatar->recule = down;
			break;
		case 'D':
		case 'd':
			//rightPressed = down;
			avatar->droite = down;
			break;
		case ' ':
			avatar->Jump = down;
			break;
		case GLUT_KEY_CTRL_L:
			ctrlPressed = down;
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
		if (button == 1)
		{
			middleMousePressed = state == 0;
		}
	}

	void mouseMove(int x, int y, bool pressed, bool inUi)
	{
		currentMouseX = x;
		currentMouseY = y;
	}

};


#endif