#pragma once

#include "ofMain.h"

const int N_STEPS_PER_FRAME = 2;
const int N_TICKS_PER_STEP = 4;
const float TIME_PER_STEP = 0.0083f;
const float INV_TIME_PER_STEP = 1.0f / TIME_PER_STEP;
const float DT = N_STEPS_PER_FRAME * TIME_PER_STEP;

const float MASS = 1.0f;
const float NEIGHBOR_RADIUS = 0.2f;

// Uses Position Based Dynamics to simulate cloth
class ClothSim
{
	public:
		// Simulation parameters
		int nPoints;
		float pointMass;
		ofMesh *m;
		vector<ofVec3f> pos;
		vector<ofVec3f> vel;
		vector<ofVec3f> extForce;
		vector<list<int>> neighbors;

		ClothSim();
		ClothSim(ofMesh *mesh);

		void startStep();
		void tick();
		void endStep();
};