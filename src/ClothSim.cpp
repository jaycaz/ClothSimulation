#include "ClothSim.h"
#include "Utils.h"

ClothSim::ClothSim()
{
}

ClothSim::ClothSim(ofMesh *mesh)
{
	// Initialize simulation
	m = mesh;
	nPoints = m->getVertices().size();
	pointMass = MASS / nPoints;

	pos = m->getVertices();

	vel = vector<ofVec3f>(nPoints);
	vel.assign(nPoints, ofPoint(0.0f));

	extForce = vector<ofVec3f>(nPoints);
	extForce.assign(nPoints, ofPoint(0.0f, -10.0f, 0.0f));

	neighbors = vector<list<int>>(nPoints);
	for (int i = 0; i < nPoints; i++)
	{
		neighbors[i] = list<int>();
	}
}

void ClothSim::startStep()
{
	// Apply external forces, i.e. gravity
	for (int i = 0; i < nPoints; i++)
	{
		vel[i] += DT * extForce[i] * MASS;
		pos[i] += DT * vel[i];
	}

	for (int i = 0; i < nPoints; i++)
	{
		neighbors[i].clear();

		for (int j = 0; j < nPoints; j++)
		{
			if (i == j) continue;
			if (pos[i].distance(pos[j]) < NEIGHBOR_RADIUS)
			{
				neighbors[i].push_back(j);
			}
		}
	}
}

void ClothSim::tick()
{

}

void ClothSim::endStep()
{
	// Final mesh update
	for (int i = 0; i < nPoints; i++)
	{
		m->setVertex(i, pos[i]);
	}
}