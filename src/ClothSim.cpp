#include "ClothSim.h"

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
}

void ClothSim::startStep()
{
	// Apply external forces, i.e. gravity
	for (int i = 0; i < nPoints; i++)
	{
		vel[i] += DT * extForce[i] * MASS;
		pos[i] += DT * vel[i];
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