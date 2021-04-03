#pragma once

#include "engine/render/renderer.h"
#include "engine/render/vbo.h"
#include "cube.h"

/**
  * On utilise des chunks pour que si on modifie juste un cube, on ait pas
  * besoin de recharger toute la carte dans le buffer, mais juste le chunk en question
  */
class MChunk
{
	public :

		static const int CHUNK_SIZE = 64; ///< Taille d'un chunk en nombre de cubes (n*n*n)
		static const int CHUNK_SIZE_METERS = CHUNK_SIZE * MCube::CUBE_SIZE;
		MCube _Cubes[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE]; ///< Cubes contenus dans le chunk

		YVbo * VboOpaque = NULL;
		YVbo * VboTransparent = NULL;

		MChunk * Voisins[6];

		int _XPos, _YPos, _ZPos; ///< Position du chunk dans le monde

		MChunk(int x, int y, int z)
		{
			memset(Voisins, 0x00, sizeof(void*)* 6);
			_XPos = x;
			_YPos = y;
			_ZPos = z;
		}

		/*
		Creation des VBO
		*/

		//On met le chunk dans son VBO
		void toVbos(void)
		{
			SAFEDELETE(VboOpaque);
			SAFEDELETE(VboTransparent);
			//Compter les sommets
			int opaqueVerticesCount = 0;
			int transparentVerticesCount = 0;

			foreachVisibleTriangle(true, &opaqueVerticesCount, &transparentVerticesCount, VboOpaque, VboTransparent);

			//Créer les VBO
			VboOpaque = new YVbo(4, opaqueVerticesCount, YVbo::PACK_BY_ELEMENT_TYPE);
			VboTransparent = new YVbo(4, transparentVerticesCount, YVbo::PACK_BY_ELEMENT_TYPE);
			VboOpaque->setElementDescription(0, YVbo::Element(3)); //Sommet
			VboOpaque->setElementDescription(1, YVbo::Element(3)); //Normale
			VboOpaque->setElementDescription(2, YVbo::Element(2)); //UV
			VboOpaque->setElementDescription(3, YVbo::Element(4)); //Couleur

			VboTransparent->setElementDescription(0, YVbo::Element(3)); //Sommet
			VboTransparent->setElementDescription(1, YVbo::Element(3)); //Normale
			VboTransparent->setElementDescription(2, YVbo::Element(2)); //UV
			VboTransparent->setElementDescription(3, YVbo::Element(4)); //Couleur

			VboOpaque->createVboCpu();
			VboTransparent->createVboCpu();

			int transparentVertices = 0;
			int opaqueVertices = 0;
			
			foreachVisibleTriangle(false, &opaqueVertices, &transparentVertices, VboOpaque, VboTransparent);
			//On envoie le contenu au GPU
			VboOpaque->createVboGpu();
			VboTransparent->createVboGpu();

			//On relache la mémoire CPU
			VboOpaque->deleteVboCpu();
			VboTransparent->deleteVboCpu();
			
		}

		//Ajoute un quad du cube. Attention CCW
		int addQuadToVbo(YVbo * vbo, int iVertice, YVec3f & a, YVec3f & b, YVec3f & c, YVec3f & d, MCube::MCubeType type)
		{
			YVec3f normal = (b - a).cross(d - a);
			normal.normalize();
			YColor * color = nullptr;
			float x;
			float y;

			chooseColor(type, color);
			chooseUV(type, x, y);

			if (normal.Z == 0 || normal.Z == -1)
			{
				y += 0.5f;
			}

			vbo->setElementValue(0, iVertice, a.X, a.Y, a.Z);
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
			vbo->setElementValue(2, iVertice, x + 1, y);
			vbo->setElementValue(3, iVertice, color->R,color->V,color->B, color->A);

			iVertice++;

			vbo->setElementValue(0, iVertice, b.X, b.Y, b.Z);
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
			vbo->setElementValue(2, iVertice, x, y);
			vbo->setElementValue(3, iVertice, color->R, color->V, color->B, color->A);

			iVertice++;

			vbo->setElementValue(0, iVertice, c.X, c.Y, c.Z);
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
			vbo->setElementValue(2, iVertice, x, y +0.5f);
			vbo->setElementValue(3, iVertice, color->R, color->V, color->B, color->A);

			iVertice++;

			vbo->setElementValue(0, iVertice, a.X, a.Y, a.Z);
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
			vbo->setElementValue(2, iVertice, x + 1, y);
			vbo->setElementValue(3, iVertice, color->R, color->V, color->B, color->A);

			iVertice++;

			vbo->setElementValue(0, iVertice, c.X, c.Y, c.Z);
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
			vbo->setElementValue(2, iVertice, x, y + 0.5f);
			vbo->setElementValue(3, iVertice, color->R, color->V, color->B, color->A);

			iVertice++;

			vbo->setElementValue(0, iVertice, d.X, d.Y, d.Z);
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
			vbo->setElementValue(2, iVertice, x + 1, y + 0.5f);
			vbo->setElementValue(3, iVertice, color->R, color->V, color->B, color->A);

			iVertice++;

			return 6;
		}

		void chooseUV(MCube::MCubeType type, float & x, float & y)
		{
			switch (type)
			{
			case MCube::CUBE_HERBE:
				x = 0;
				y = 0;
				break;
			case MCube::CUBE_TERRE:
				x = 1;
				y = 0;
				//foferduvert
				break;
			case MCube::CUBE_PIERRE:
				x = 3;
				y = 0;
				//foferduvert
				break;
			case MCube::CUBE_EAU:
				x = 32;
				y = 1;
				//foferduvert
				break;
			case MCube::CUBE_LAINE_01:
				x = 32;
				y = 1;
				break;
			default:
				x = 32;
				y = 1;
				break;
			}
		}

		void chooseColor(MCube::MCubeType type, YColor * &color)
		{
			switch (type)
			{
			case MCube::CUBE_HERBE:
				color = new YColor( 51.0f / 255.0f, 145.0f / 255.0f, 19.0f/255.0f, 1.0f);
				break;
			case MCube::CUBE_TERRE:
				color = new YColor(40.0f / 255.0f, 25.0f / 255.0f, 0.0f, 1.0f);
				//foferduvert
				break;
			case MCube::CUBE_PIERRE:
				color = new YColor(60.f / 255.0f, 60.f / 255.0f, 60.f / 255.0f, 1.0f);
				//foferduvert
				break;
			case MCube::CUBE_EAU:
				color = new YColor(78.f / 255.0f, 216.f / 255.0f, 237.f / 255.0f, 0.6f);
				//foferduvert
				break;
			case MCube::CUBE_LAINE_01:
				color = new YColor(1.0f, 1.0f, 1.0f, 1.0f);
				break;
			default:
				color = new YColor(1.0f, 0.0f, 1.0f, 1.0f);
				break;
			}
		}

		//Permet de compter les triangles ou des les ajouter aux VBO
		void foreachVisibleTriangle(bool countOnly, int * nbVertOpaque, int * nbVertTransp, YVbo * VboOpaque, YVbo * VboTransparent) {

			float size = MCube::CUBE_SIZE;

			YVec3f initialA(size / 2.0f, -size / 2.0f, -size / 2.0f);
			YVec3f initialB(size / 2.0f, size / 2.0f, -size / 2.0f);
			YVec3f initialC(size / 2.0f, size / 2.0f, size / 2.0f);
			YVec3f initialD(size / 2.0f, -size / 2.0f, size / 2.0f);
			YVec3f initialE(-size / 2.0f, -size / 2.0f, -size / 2.0f);
			YVec3f initialF(-size / 2.0f, size / 2.0f, -size / 2.0f);
			YVec3f initialG(-size / 2.0f, size / 2.0f, size / 2.0f);
			YVec3f initialH(-size / 2.0f, -size / 2.0f, size / 2.0f);

			YVec3f a(initialA);
			YVec3f b(initialB);
			YVec3f c(initialC);
			YVec3f d(initialD);
			YVec3f e(initialE);
			YVec3f f(initialF);
			YVec3f g(initialG);
			YVec3f h(initialH);

			MCube* cubeXPrev = NULL;
			MCube* cubeXNext = NULL;
			MCube* cubeYPrev = NULL;
			MCube* cubeYNext = NULL;
			MCube* cubeZPrev = NULL;
			MCube* cubeZNext = NULL;

			for (int x = 0; x < CHUNK_SIZE; x++)
				for (int y = 0; y < CHUNK_SIZE; y++)
					for (int z = 0; z < CHUNK_SIZE; z++)
					{
						MCube* cube = &_Cubes[x][y][z];
						if (!cube->getDraw() || cube->getType() == MCube::CUBE_AIR) continue;

						MCube::MCubeType type = cube->getType();

						get_surrounding_cubes(x, y, z, &cubeXPrev, &cubeXNext, &cubeYPrev, &cubeYNext, &cubeZPrev, &cubeZNext);

						YVec3f coord(x* size, y * size, z * size);
						a = initialA + coord;
						b = initialB + coord;
						c = initialC + coord;
						d = initialD + coord;
						e = initialE + coord;
						f = initialF + coord;
						g = initialG + coord;
						h = initialH + coord;

						if (cube->isOpaque())
						{
							if (cubeXPrev == NULL || !cubeXPrev->isOpaque())
							{
								if (countOnly)
								{
									*nbVertOpaque += 6;
								}
								else
								{
									*nbVertOpaque += addQuadToVbo(VboOpaque, *nbVertOpaque, f, e, h, g, type);
								}
							}

							if (cubeXNext == NULL || !cubeXNext->isOpaque())
							{
								if (countOnly)
								{
									*nbVertOpaque += 6;
								}
								else
								{
									*nbVertOpaque += addQuadToVbo(VboOpaque, *nbVertOpaque, a, b, c, d, type);
								}
							}

							if (cubeYPrev == NULL || !cubeYPrev->isOpaque())
							{
								if (countOnly)
								{
									*nbVertOpaque += 6;
								}
								else
								{
									*nbVertOpaque += addQuadToVbo(VboOpaque, *nbVertOpaque, e, a, d, h, type);
								}
							}

							if (cubeYNext == NULL || !cubeYNext->isOpaque())
							{
								if (countOnly)
								{
									*nbVertOpaque += 6;
								}
								else
								{
									*nbVertOpaque += addQuadToVbo(VboOpaque, *nbVertOpaque, b, f, g, c, type);
								}
							}

							if (cubeZPrev == NULL || !cubeZPrev->isOpaque())
							{
								if (countOnly)
								{
									*nbVertOpaque += 6;
								}
								else
								{
									*nbVertOpaque += addQuadToVbo(VboOpaque, *nbVertOpaque, e, f, b, a, type);
								}
							}

							if (cubeZNext == NULL || !cubeZNext->isOpaque())
							{
								if (countOnly)
								{
									*nbVertOpaque += 6;
								}
								else
								{
									*nbVertOpaque += addQuadToVbo(VboOpaque, *nbVertOpaque, c, g, h, d, type);
								}
							}
						}
						else
						{
							if (cubeXPrev == NULL || (cubeXPrev->getType() != type && !cubeXPrev->isOpaque()))
							{
								if (countOnly)
								{
									*nbVertTransp += 6;
								}
								else
								{
									*nbVertTransp += addQuadToVbo(VboTransparent, *nbVertTransp, f, e, h, g, type);
								}
							}

							if (cubeXNext == NULL || (cubeXNext->getType() != type && !cubeXNext->isOpaque()))
							{
								if (countOnly)
								{
									*nbVertTransp += 6;
								}
								else
								{
									*nbVertTransp += addQuadToVbo(VboTransparent, *nbVertTransp, a, b, c, d, type);
								}
							}

							if (cubeYPrev == NULL || (cubeYPrev->getType() != type && !cubeYPrev->isOpaque()))
							{
								if (countOnly)
								{
									*nbVertTransp += 6;
								}
								else
								{
									*nbVertTransp += addQuadToVbo(VboTransparent, *nbVertTransp, e, a, d, h, type);
								}
							}

							if (cubeYNext == NULL || (cubeYNext->getType() != type && !cubeYNext->isOpaque()))
							{
								if (countOnly)
								{
									*nbVertTransp += 6;
								}
								else
								{
									*nbVertTransp += addQuadToVbo(VboTransparent, *nbVertTransp, b, f, g, c, type);
								}
							}

							if (cubeZPrev == NULL || (cubeZPrev->getType() != type && !cubeZPrev->isOpaque()))
							{
								if (countOnly)
								{
									*nbVertTransp += 6;
								}
								else
								{
									*nbVertTransp += addQuadToVbo(VboTransparent, *nbVertTransp, e, f, b, a, type);
								}
							}

							if (cubeZNext == NULL || (cubeZNext->getType() != type && !cubeZNext->isOpaque()))
							{
								if (countOnly)
								{
									*nbVertTransp += 6;
								}
								else
								{
									*nbVertTransp += addQuadToVbo(VboTransparent, *nbVertTransp, c, g, h, d, type);
								}
							}
						}
					}
		}

		/*
		Gestion du chunk
		*/

		void reset(void)
		{
			for(int x=0;x<CHUNK_SIZE;x++)
				for(int y=0;y<CHUNK_SIZE;y++)
					for(int z=0;z<CHUNK_SIZE;z++)
					{
						_Cubes[x][y][z].setDraw(false);
						_Cubes[x][y][z].setType(MCube::CUBE_AIR);
					}
		}

		void setVoisins(MChunk * xprev, MChunk * xnext, MChunk * yprev, MChunk * ynext, MChunk * zprev, MChunk * znext)
		{
			Voisins[0] = xprev;
			Voisins[1] = xnext;
			Voisins[2] = yprev;
			Voisins[3] = ynext;
			Voisins[4] = zprev;
			Voisins[5] = znext;
		}

		void get_surrounding_cubes(int x, int y, int z, MCube ** cubeXPrev, MCube ** cubeXNext,
			MCube ** cubeYPrev, MCube ** cubeYNext,
			MCube ** cubeZPrev, MCube ** cubeZNext)
		{

			*cubeXPrev = NULL;
			*cubeXNext = NULL;
			*cubeYPrev = NULL;
			*cubeYNext = NULL;
			*cubeZPrev = NULL;
			*cubeZNext = NULL;

			if (x == 0 && Voisins[0] != NULL)
				*cubeXPrev = &(Voisins[0]->_Cubes[CHUNK_SIZE - 1][y][z]);
			else if (x > 0)
				*cubeXPrev = &(_Cubes[x - 1][y][z]);

			if (x == CHUNK_SIZE - 1 && Voisins[1] != NULL)
				*cubeXNext = &(Voisins[1]->_Cubes[0][y][z]);
			else if (x < CHUNK_SIZE - 1)
				*cubeXNext = &(_Cubes[x + 1][y][z]);

			if (y == 0 && Voisins[2] != NULL)
				*cubeYPrev = &(Voisins[2]->_Cubes[x][CHUNK_SIZE - 1][z]);
			else if (y > 0)
				*cubeYPrev = &(_Cubes[x][y - 1][z]);

			if (y == CHUNK_SIZE - 1 && Voisins[3] != NULL)
				*cubeYNext = &(Voisins[3]->_Cubes[x][0][z]);
			else if (y < CHUNK_SIZE - 1)
				*cubeYNext = &(_Cubes[x][y + 1][z]);

			if (z == 0 && Voisins[4] != NULL)
				*cubeZPrev = &(Voisins[4]->_Cubes[x][y][CHUNK_SIZE - 1]);
			else if (z > 0)
				*cubeZPrev = &(_Cubes[x][y][z - 1]);

			if (z == CHUNK_SIZE - 1 && Voisins[5] != NULL)
				*cubeZNext = &(Voisins[5]->_Cubes[x][y][0]);
			else if (z < CHUNK_SIZE - 1)
				*cubeZNext = &(_Cubes[x][y][z + 1]);
		}

		void render(bool transparent)
		{
			if (transparent)
				VboTransparent->render();
			else
				VboOpaque->render();
		}

		/**
		  * On verifie si le cube peut être vu
		  */
		bool test_hidden(int x, int y, int z)
		{
			MCube * cubeXPrev = NULL; 
			MCube * cubeXNext = NULL; 
			MCube * cubeYPrev = NULL; 
			MCube * cubeYNext = NULL; 
			MCube * cubeZPrev = NULL; 
			MCube * cubeZNext = NULL; 

			if(x == 0 && Voisins[0] != NULL)
				cubeXPrev = &(Voisins[0]->_Cubes[CHUNK_SIZE-1][y][z]);
			else if(x > 0)
				cubeXPrev = &(_Cubes[x-1][y][z]);

			if(x == CHUNK_SIZE-1 && Voisins[1] != NULL)
				cubeXNext = &(Voisins[1]->_Cubes[0][y][z]);
			else if(x < CHUNK_SIZE-1)
				cubeXNext = &(_Cubes[x+1][y][z]);

			if(y == 0 && Voisins[2] != NULL)
				cubeYPrev = &(Voisins[2]->_Cubes[x][CHUNK_SIZE-1][z]);
			else if(y > 0)
				cubeYPrev = &(_Cubes[x][y-1][z]);

			if(y == CHUNK_SIZE-1 && Voisins[3] != NULL)
				cubeYNext = &(Voisins[3]->_Cubes[x][0][z]);
			else if(y < CHUNK_SIZE-1)
				cubeYNext = &(_Cubes[x][y+1][z]);

			if(z == 0 && Voisins[4] != NULL)
				cubeZPrev = &(Voisins[4]->_Cubes[x][y][CHUNK_SIZE-1]);
			else if(z > 0)
				cubeZPrev = &(_Cubes[x][y][z-1]);

			if(z == CHUNK_SIZE-1 && Voisins[5] != NULL)
				cubeZNext = &(Voisins[5]->_Cubes[x][y][0]);
			else if(z < CHUNK_SIZE-1)
				cubeZNext = &(_Cubes[x][y][z+1]);

			if( cubeXPrev == NULL || cubeXNext == NULL ||
				cubeYPrev == NULL || cubeYNext == NULL ||
				cubeZPrev == NULL || cubeZNext == NULL )
				return false;

			if (cubeXPrev->isOpaque() == true && //droite
				cubeXNext->isOpaque() == true && //gauche
				cubeYPrev->isOpaque() == true && //haut
				cubeYNext->isOpaque() == true && //bas
				cubeZPrev->isOpaque() == true && //devant
				cubeZNext->isOpaque() == true)  //derriere
				return true;
			return false;
		}

		void disableHiddenCubes(void)
		{
			for(int x=0;x<CHUNK_SIZE;x++)
				for(int y=0;y<CHUNK_SIZE;y++)
					for(int z=0;z<CHUNK_SIZE;z++)
					{
						_Cubes[x][y][z].setDraw(true);
						if(test_hidden(x,y,z))
							_Cubes[x][y][z].setDraw(false);
					}
		}


};