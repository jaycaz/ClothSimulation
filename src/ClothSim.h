#pragma once

#include "ofMain.h"

const int N_STEPS_PER_FRAME = 2;
const int N_TICKS_PER_STEP = 4;
const float RUN_SPEED = 1.0f;
const float TIME_PER_STEP = 0.0083f * RUN_SPEED;
const float INV_TIME_PER_STEP = 1.0f / TIME_PER_STEP;
const float DT = N_STEPS_PER_FRAME * TIME_PER_STEP;
const float DENSITY = 0.5f;

const float BEND_STRENGTH = 0.01f;
const float STRETCH_STRENGTH = 0.1f;

const float MOUSE_DRAG_MULT = 10.0f;

const float BOUNDARY_SIZE = 3.0f;

const float MASS = 1.0f;

struct CollisionPlane
{
	CollisionPlane(ofVec3f o, ofVec3f n) { origin = o; normal = n.normalize(); }
	ofVec3f		origin;
	ofVec3f		normal;
};

struct Tetrahedron
{
	// Shared points
	ofIndexType p1;
	ofIndexType p2;

	// End points
	ofIndexType p3;
	ofIndexType p4;
};

// Constrains a particular mesh vertex to be pinned to a point in space
struct PointPin
{
	ofIndexType v;
	ofPoint target;
};

// Uses Position Based Dynamics to simulate cloth
class ClothSim
{
	public:
		// Simulation parameters
		int nPoints;
		int nTris;
		vector<Tetrahedron> tets;

		ofMesh *m;
		vector<ofVec3f> pos; // positions
		vector<ofVec3f> ppos; // predicted positions
		vector<ofVec3f> vel;
		vector<float> invPointMass; // Mass of each point at the start of frame
		vector<ofVec3f> extForce;
		vector<CollisionPlane> planes;
		vector<float> restDist;
		vector<float> restBend;
		
		// List of point pin constraints
		set<PointPin*> pins;

		ClothSim();
		ClothSim(ofMesh *mesh);

		void addPointPin(PointPin *p);
		void removePointPin(PointPin *p);

		void startStep();
		void tick();
		void endStep();
};