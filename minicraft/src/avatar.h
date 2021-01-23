#ifndef __AVATAR__
#define __AVATAR__

#include "engine/utils/types_3d.h"
#include "engine/render/camera.h"
#include "engine/utils/timer.h"
#include "world.h"
#include "my_physics.h"

class MAvatar
{
public:
	YVec3f Position;
	YVec3f Speed;

	float pickUpDistance;
	float jumpForce;
	float walkAcceleration;
	float runAcceleration;
	float crouchAcceleration;
	float maxSpeed;
	float maxFallSpeed;
	float masse;
	float groundDamping;
	float airDamping;
	bool Move;
	bool Jump;
	bool Grounded;
	float Height;
	float CurrentHeight;
	float Width;
	bool avance;
	bool recule;
	bool gauche;
	bool droite;
	bool Standing;
	bool InWater;
	bool Crouch;
	bool Run;
	bool Hit;

	YCamera* Cam;
	MWorld* World;

	YTimer _TimerStanding;

	MAvatar(YCamera* cam, MWorld* world)
	{
		Position = YVec3f((MWorld::MAT_SIZE_METERS) / 2, (MWorld::MAT_SIZE_METERS) / 2, (MWorld::MAT_HEIGHT_METERS));
		Height = 1.8f;
		CurrentHeight = Height;
		Width = 0.3f;
		Cam = cam;
		avance = false;
		recule = false;
		gauche = false;
		droite = false;
		Standing = false;
		Jump = false;
		Grounded = false;
		World = world;
		InWater = false;
		Crouch = false;
		Run = false;
		Hit = false;
		jumpForce = 1.f;
		walkAcceleration = 10.f;
		runAcceleration = 15.f;;
		crouchAcceleration = 5.f;
		maxSpeed = 20.f;
		maxFallSpeed = 50.f;
		masse = 1;
		groundDamping = 0.01f;
		airDamping = 0.5f;
		pickUpDistance = MCube::CUBE_SIZE*2;
	}

	void update(float elapsed)
	{
		if (elapsed > 1.0f / 60.0f)
			elapsed = 1.0f / 60.0f;

		YVec3f acceleration = YVec3f(0.f, 0.f, 0.f);
		YVec3f movementForce = YVec3f(0.f, 0.f, 0.f);
		YVec3f gravityForce = YVec3f(0.f, 0.f, -9.81f);

		YVec3f upDir = Cam->UpRef;
		YVec3f rightDir = Cam->RightVec;
		YVec3f forwardDir = Cam->Direction;
		forwardDir.Z = 0;
		forwardDir.normalize();

		if (avance)
		{
			movementForce += forwardDir;
		}
		if (recule)
		{
			movementForce -= forwardDir;
		}
		if (gauche)
		{
			movementForce -= rightDir;
		}
		if (droite)
		{
			movementForce += rightDir;
		}
		if (Jump)
		{
			Jump = false;
			Grounded = false;
			movementForce += upDir * jumpForce / elapsed;
		}

		float wantedSpeed = walkAcceleration;
		if (Crouch) wantedSpeed = crouchAcceleration;
		else if (Run) wantedSpeed = runAcceleration;

		movementForce *= wantedSpeed;
		//Sommes des forces
		acceleration += movementForce / masse;
		acceleration += gravityForce / masse;
		//Calcul de vitesse
		Speed += acceleration * elapsed;
		YVec3f horizontalSpeed(Speed.X, Speed.Y, 0);
		if (horizontalSpeed.getSqrSize() > maxSpeed * maxSpeed)
		{
			float z = Speed.Z;
			Speed = horizontalSpeed.normalize() * maxSpeed;
			Speed.Z = z;
		}
		if (Speed.Z > maxFallSpeed)
		{
			Speed.Z = maxFallSpeed;
		}
		//Calcul de position
		Position += Speed * elapsed;

		//Damping
		float damping;
		if (Grounded)
		{
			damping = groundDamping;
		}
		else
		{
			damping = airDamping;
		}

		float dampFactor = pow(damping, elapsed);
		Speed.X *= dampFactor;
		Speed.Y *= dampFactor;

		// Ne marche pas bien avec un frame rate différent, essayer de trouver une équation propre.
		if (movementForce.getSqrSize() < 0.01f)
		{

			Speed.X /= 1 + ((1 / 60.f) / elapsed) * 0.01f;
			Speed.Y /= 1 + ((1 / 60.f) / elapsed) * 0.01f;
		}

		YVec3f correctDir = YVec3f(0.f, 0.f, 0.f);
		float valueColMin = 0;

		Grounded = false; //Si on touche le sol grounded sera remis à true, sinon, on tombe :)

		for (int i = 0; i < 5; i++)
		{
			MWorld::MAxis axis = World->getMinCol(Position, Speed, Width, Height, valueColMin, true);
			if (axis == 0x00)
			{
				break;
			}
			else
			{
				correctDir.X = 0;
				correctDir.Y = 0;
				correctDir.Z = 0;
				switch (axis)
				{
				case MWorld::AXIS_X:
					correctDir.X = 1 * (valueColMin + (0.001f * sign(valueColMin)));
					Speed.X = 0.1 * -sign(Speed.X);
					break;
				case MWorld::AXIS_Y:
					correctDir.Y = 1 * (valueColMin + (0.001f * sign(valueColMin)));
					Speed.Y = 0.1 * -sign(Speed.Y);
					break;
				case MWorld::AXIS_Z:
					correctDir.Z = 1 * (valueColMin + (0.001f * sign(valueColMin)));
					Speed.Z = 0.1 * -sign(Speed.Z);
					Grounded = true;
					break;
				default:
					printf("ERROR WITH AVATAR COLISION \n");
					break;
				}
				Position += correctDir;
			}
		}

		for (int i = 0; i < 5; i++)
		{
			MWorld::MAxis axis = World->getMinCol(Position, Speed, Width, Height, valueColMin, false);
			if (axis == 0x00)
			{
				break;
			}
			else
			{
				correctDir.X = 0;
				correctDir.Y = 0;
				correctDir.Z = 0;
				switch (axis)
				{
				case MWorld::AXIS_X:
					correctDir.X = 1 * (valueColMin + (0.001f * sign(valueColMin)));
					Speed.X = 0.1 * -sign(Speed.X);
					break;
				case MWorld::AXIS_Y:
					correctDir.Y = 1 * (valueColMin + (0.001f * sign(valueColMin)));
					Speed.Y = 0.1 * -sign(Speed.Y);
					break;
				case MWorld::AXIS_Z:
					correctDir.Z = 1 * (valueColMin + (0.001f * sign(valueColMin)));
					Speed.Z = 0.1 * -sign(Speed.Z);
					Grounded = true;
					break;
				default:
					printf("ERROR WITH AVATAR COLISION \n");
					break;
				}
				Position += correctDir;
			}
		}
		Cam->moveTo(Position);

		if (!Hit) return;
		else Hit = false;
		YVec3f vecX = Cam->Direction.dot(YVec3f(1, 0, 0)) > 0 ? YVec3f(1, 0, 0) : YVec3f(-1, 0, 0);
		YVec3f vecY = Cam->Direction.dot(YVec3f(0, 1, 0)) > 0 ? YVec3f(0, 1, 0) : YVec3f(0, -1, 0);
		YVec3f vecZ = Cam->Direction.dot(YVec3f(0, 0, 1)) > 0 ? YVec3f(0, 0, 1) : YVec3f(0, 0, -1);

		float offsetX = (vecX.X > 0) ? (float)MCube::CUBE_SIZE : 0;
		float offsetY = (vecY.Y > 0) ? (float)MCube::CUBE_SIZE : 0;
		float offsetZ = (vecZ.Z > 0) ? (float)MCube::CUBE_SIZE : 0;

		YVec3f intersection;

		int cubeDistancePickUp = max((int)(pickUpDistance / MCube::CUBE_SIZE), 1);

		int startX = max((int)(Position.X / MCube::CUBE_SIZE) - cubeDistancePickUp,0);
		int startY = max((int)(Position.Y / MCube::CUBE_SIZE) - cubeDistancePickUp,0);
		int startZ = max((int)(Position.Z / MCube::CUBE_SIZE) - cubeDistancePickUp,0);
		int endX = min((int)(Position.X / MCube::CUBE_SIZE) + cubeDistancePickUp, MWorld::MAT_SIZE_CUBES);
		int endY = min((int)(Position.Y / MCube::CUBE_SIZE) + cubeDistancePickUp, MWorld::MAT_SIZE_CUBES);
		int endZ = min((int)(Position.Z / MCube::CUBE_SIZE) + cubeDistancePickUp, MWorld::MAT_HEIGHT_CUBES);

		for (int x = startX; x < endX; x++)
		{
			for (int y = startY; y < endY; y++)
			{
				for (int z = startZ; z < endZ; z++)
				{
					if (!(World->getCube(x, y, z)->isSolid())) continue;

					float xMeters = x * MCube::CUBE_SIZE;
					float yMeters = y * MCube::CUBE_SIZE;
					float zMeters = z * MCube::CUBE_SIZE;

					//X 
					YVec3f pPoint = YVec3f(xMeters +offsetX, yMeters, zMeters);
					if (CalculateIntersectionPlaneSegment(vecZ, vecY, pPoint, Cam->Direction, Cam->Position, pickUpDistance, intersection))
					{
						if (IsPointWithinFace(pPoint, YVec3f(pPoint.X, yMeters + MCube::CUBE_SIZE, zMeters), 
												YVec3f(pPoint.X, yMeters + MCube::CUBE_SIZE, zMeters + MCube::CUBE_SIZE), 
												YVec3f(pPoint.X, yMeters, zMeters + MCube::CUBE_SIZE), intersection))
						{
							World->deleteCube(x, y, z);
							return;
						}
					}

					//Y
					pPoint = YVec3f(xMeters, yMeters + offsetY, zMeters);
					if (CalculateIntersectionPlaneSegment(vecZ, vecX, pPoint, Cam->Direction, Cam->Position, pickUpDistance, intersection))
					{
						if (IsPointWithinFace(pPoint, YVec3f(xMeters + MCube::CUBE_SIZE, pPoint.Y, zMeters), 
												YVec3f(xMeters + MCube::CUBE_SIZE, pPoint.Y, zMeters + MCube::CUBE_SIZE), 
												YVec3f(xMeters, pPoint.Y, zMeters + MCube::CUBE_SIZE), intersection))
						{
							World->deleteCube(x, y, z);
							return;
						}
					}

					//Z
					pPoint = YVec3f(xMeters, yMeters, zMeters + offsetZ);

					if (CalculateIntersectionPlaneSegment(vecY, vecX, pPoint, Cam->Direction, Cam->Position, pickUpDistance, intersection))
					{
						if (IsPointWithinFace(pPoint, YVec3f(xMeters + MCube::CUBE_SIZE, yMeters, pPoint.Z), 
												YVec3f(xMeters + MCube::CUBE_SIZE, yMeters + MCube::CUBE_SIZE, pPoint.Z), 
												YVec3f(xMeters, yMeters + MCube::CUBE_SIZE, pPoint.Z), intersection))
						{
							World->deleteCube(x, y, z);
							return;
						}
					}
				}
			}
		}
	}
};

#endif