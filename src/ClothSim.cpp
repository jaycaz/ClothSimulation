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

	//// Calculate mesh tetrahedrons (pairs of triangles that share an edge)
	tets = vector<Tetrahedron>();
	auto pointTris = vector<list<ofIndexType>>(nPoints);
	pointTris.assign(nPoints, list<ofIndexType>());

	// group all tris that share a common point
	for (ofIndexType i = 0; i < m->getIndices().size(); i++)
	{
		pointTris[m->getIndex(i)].push_back(3 * (i / 3));
	}
	
	// find adjacent pairs of tris
	for (int i = 0; i < pointTris.size(); i++)
	{
		for (auto t0 = pointTris[i].begin(); t0 != pointTris[i].end(); t0++)
		{
			for (auto t1 = t0; t1 != pointTris[i].end(); t1++)
			{
				if (t0 == t1)
					continue;

				// Attempt to form a tetrahedron
				auto icount = map<ofIndexType, int>();
				for (int j = 0; j < 3; j++)
				{
					icount[m->getIndex(*t0+j)] += 1;
					icount[m->getIndex(*t1+j)] += 1;
				}

				bool v4 = false; bool v2 = false;
				if (icount.size() == 4)
				{
					Tetrahedron t;
					for (auto it = icount.begin(); it != icount.end(); it++)
					{
						if (it->second == 1 && v4)
							t.p4 = it->first;
						else if (it->second == 2 && v2)
							t.p2 = it->first;
						else if (it->second == 1 && !v4)
						{
							t.p3 = it->first;
							v4 = true;
						}
						else if (it->second == 2 && !v2)
						{
							t.p1 = it->first;
							v2 = true;
						}
						else
						{
							throw std::exception("Triangle pair found with more than two points in common");
						}
					}
					tets.push_back(t);
				}
			}
		}
	}

	// Store tet rotation angles
	for (int i = 0; i < tets.size(); i++)
	{
		Tetrahedron t = tets[i];

		ofVec3f p1 = m->getVertex(t.p1);
		ofVec3f p2 = m->getVertex(t.p2);
		ofVec3f p3 = m->getVertex(t.p3);
		ofVec3f p4 = m->getVertex(t.p4);

		ofVec3f t1 = (p2 - p1).crossed(p3 - p1).normalized();
		ofVec3f t2 = (p2 - p1).crossed(p4 - p1).normalized();

		restBend.push_back(t1.angleRad(t2));
	}

	////

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

	// Apply bend constraints
	for (int i = 0; i < tets.size(); i++)
	{
		Tetrahedron t = tets[i];

		ofVec3f p1 = m->getVertex(t.p1);
		ofVec3f p2 = m->getVertex(t.p2) - p1;
		ofVec3f p3 = m->getVertex(t.p3) - p1;
		ofVec3f p4 = m->getVertex(t.p4) - p1;
		p1 = ofVec3f::zero();

		ofVec3f n1 = p2.crossed(p3).normalized();
		ofVec3f n2 = p2.crossed(p4).normalized();
		float d = n1.dot(n2);

		ofVec3f q3 = (p2.crossed(n2) + (n1.crossed(p2) * d)) / (p2.crossed(p3)).length();
		ofVec3f q4 = (p2.crossed(n1) + (n2.crossed(p2) * d)) / (p2.crossed(p4)).length();
		ofVec3f q2 = - ((p3.crossed(n2) + (n1.crossed(p3) * d)) / p2.crossed(p3).length())
				     - ((p4.crossed(n1) + (n2.crossed(p4) * d)) / p2.crossed(p4).length());
		ofVec3f q1 = -q2 - q3 - q4;

		//ppos[t.p1] += -((1.0f / pointMass[t.p1]) * sqrtf(1.0f - d*d)) * (acosf(d) - restBend[i]);
		//ppos[t.p2] += -((1.0f / pointMass[t.p2]) * sqrtf(1.0f - d*d)) * (acosf(d) - restBend[i]);
		//ppos[t.p3] += -((1.0f / pointMass[t.p3]) * sqrtf(1.0f - d*d)) * (acosf(d) - restBend[i]);
		//ppos[t.p4] += -((1.0f / pointMass[t.p4]) * sqrtf(1.0f - d*d)) * (acosf(d) - restBend[i]);
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