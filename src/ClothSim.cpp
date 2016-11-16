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

	ppos = vector<ofVec3f>(nPoints);
	ppos.assign(nPoints, ofPoint(0.0f));

	vel = vector<ofVec3f>(nPoints);
	vel.assign(nPoints, ofPoint(0.0f));

	extForce = vector<ofVec3f>(nPoints);
	extForce.assign(nPoints, ofPoint(0.0f, -10.0f, 0.0f));

	neighbors = vector<list<int>>(nPoints);
	for (int i = 0; i < nPoints; i++)
	{
		neighbors[i] = list<int>();
	}

	// Initialize boundary planes
	float bdry = BOUNDARY_SIZE / 2.0f;
	planes = {
		CollisionPlane(ofVec3f(bdry, 0.0f, 0.0f), ofVec3f(-1.0f, 0.0f, 0.0f)),
		CollisionPlane(ofVec3f(0.0f, bdry, 0.0f), ofVec3f(0.0f, -1.0f, 0.0f)),
		CollisionPlane(ofVec3f(0.0f, 0.0f, bdry), ofVec3f(0.0f, 0.0f, -1.0f)),
		CollisionPlane(ofVec3f(-bdry, 0.0f, 0.0f), ofVec3f(1.0f, 0.0f, 0.0f)),
		CollisionPlane(ofVec3f(0.0f, -bdry, 0.0f), ofVec3f(0.0f, 1.0f, 0.0f)),
		CollisionPlane(ofVec3f(0.0f, 0.0f, -bdry), ofVec3f(0.0f, 0.0f, 1.0f))
	};
}

void ClothSim::startStep()
{
	// Apply external forces, i.e. gravity
	for (int i = 0; i < nPoints; i++)
	{
		vel[i] += DT * extForce[i] * MASS;
		ppos[i] = pos[i] + DT * vel[i];
	}

	for (int i = 0; i < nPoints; i++)
	{
		neighbors[i].clear();

		for (int j = 0; j < nPoints; j++)
		{
			if (i == j) continue;
			if (ppos[i].distance(ppos[j]) < NEIGHBOR_RADIUS)
			{
				neighbors[i].push_back(j);
			}
		}
	}
}

void ClothSim::tick()
{
	// Check for point-plane collisions and respond
	for (int i = 0; i < nPoints; i++)
	{
		for (int j = 0; j < planes.size(); j++)
		{
			ofVec3f planeToPart = ppos[i] - planes[j].origin;
			float distToPlane = planeToPart.dot(planes[j].normal);
			if (distToPlane < -0.001f)
			{
				// Collision: Apply simple position shift
				pos[i] -= distToPlane * planes[j].normal * 0.5f;
				ppos[i] -= distToPlane * planes[j].normal;
				//particles[i].vel = particles[i].vel * 0.8f;
			}
		}
	}
}

void ClothSim::endStep()
{
	// Final mesh update
	for (int i = 0; i < nPoints; i++)
	{
		pos[i] = ppos[i];
		m->setVertex(i, pos[i]);
	}
}