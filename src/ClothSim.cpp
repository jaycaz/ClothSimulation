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

	// Initialize point params
	pos = m->getVertices();

	ppos = vector<ofVec3f>(nPoints);
	ppos.assign(nPoints, ofPoint(0.0f));

	vel = vector<ofVec3f>(nPoints);
	vel.assign(nPoints, ofPoint(0.0f));

	extForce = vector<ofVec3f>(nPoints);
	extForce.assign(nPoints, ofPoint(0.0f, -10.0f, 0.0f));

	// Initialize triangle params
	nTris = m->getUniqueFaces().size();
	cout << "nTriangles: " << nTris << endl;

	triMass = vector<float>(nTris);

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

	// Perform nearest neighbor calculation
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

	// Perform triangle mass calculation
	for (int i = 0; i < nTris; i++)
	{
		ofMeshFace tri = m->getFace(i);
		ofVec3f U = (tri.getVertex(1) - tri.getVertex(0));
		ofVec3f V = (tri.getVertex(2) - tri.getVertex(0));
		ofVec3f faceNormal = U.getCrossed(V);
		float triArea = faceNormal.length() / 2.0f;

		m->setColor(i, Utils::Debug1D(triArea * 162.0f));

		triMass[i] = DENSITY * triArea;
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