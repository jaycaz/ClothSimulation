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
	cout << "nPoints: " << nPoints << endl;

	// Initialize point params
	pos = m->getVertices();

	ppos = vector<ofVec3f>(nPoints);
	ppos.assign(nPoints, ofPoint(0.0f));

	vel = vector<ofVec3f>(nPoints);
	vel.assign(nPoints, ofPoint(0.0f));

	extForce = vector<ofVec3f>(nPoints);
	extForce.assign(nPoints, ofPoint(0.0f, -10.0f, 0.0f));

	pointMass = vector<float>(nPoints);
	pointMass.assign(nPoints, 0.0f);

	// Initialize triangle & edge params
	nTris = m->getUniqueFaces().size();
	cout << "nTriangles: " << nTris << endl;

	restDist = vector<float>(nTris*3);
	restDist.assign(nTris*3, -1.0f);


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

	// Perform triangle mass calculation
	pointMass.assign(nPoints, 0.0f);
	for (int i = 0; i < nTris; i++)
	{
		ofMeshFace tri = m->getFace(i);
		ofVec3f U = (tri.getVertex(1) - tri.getVertex(0));
		ofVec3f V = (tri.getVertex(2) - tri.getVertex(0));
		ofVec3f faceNormal = U.getCrossed(V);
		float triArea = faceNormal.length() / 2.0f;

		float triMass = DENSITY * triArea;

		// update point with one third of this triangle's mass
		for (int j = i * 3; j < i * 3 + 3; j++)
		{
			pointMass[m->getIndex(j)] += triMass;
		}
	}

	for (int i = 0; i < nPoints; i++)
	{
		pointMass[i] /= 3.0f;
		m->setColor(i, Utils::Debug1D(pointMass[i] * 54));
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

	// Apply stretch constraints
	for (int i = 0; i < nTris; i++)
	{
		// Iterate over all edges of the triangle
		for (int j = 0; j < 3; j++)
		{
			int i0 = m->getIndex(i * 3 + (j % 3));
			int i1 = m->getIndex(i * 3 + ((j + 1) % 3));
			ofVec3f p0 = ppos[i0];
			ofVec3f p1 = ppos[i1];

			// Assume that the mesh at the beginning is in a rest state,
			// i.e. the initial distances for edges are the rest distances
			if (restDist[i*3+j] < 0)
			{
				restDist[i*3+j] = p0.distance(p1);
			}

			ofVec3f d = (p0 - p1);
			ofVec3f dnorm = d.normalized();
			float s = (p0.distance(p1) - restDist[i*3+j]) / ((1.0f / pointMass[i0]) + (1.0f / pointMass[i1]));
			ofVec3f dp0 = (-1.0f / pointMass[i0]) * s * dnorm;
			ofVec3f dp1 = (1.0f / pointMass[i1]) * s * dnorm;

			ppos[i0] += dp0;
			ppos[i1] += dp1;
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