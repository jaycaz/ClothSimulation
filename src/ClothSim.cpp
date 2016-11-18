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

	invPointMass = vector<float>(nPoints);
	invPointMass.assign(nPoints, 0.0f);

	// Initialize triangle & edge params
	nTris = m->getUniqueFaces().size();
	cout << "nTriangles: " << nTris << endl;

	restDist = vector<float>(nTris * 3);
	restDist.assign(nTris * 3, -1.0f);

	pins = set<PointPin*>();

	//// Calculate mesh tetrahedrons (pairs of triangles that share an edge)
	tets = vector<Tetrahedron>();
	auto prevTriPoint = vector<vector<ofIndexType>>(nPoints);
	for (int i = 0; i < nPoints; i++)
	{
		auto v = vector<ofIndexType>(nPoints);
		v.assign(nPoints, numeric_limits<ofIndexType>::max());
		prevTriPoint[i] = v;
		//m->setColor(i, ofColor(0.0f));
	}

	for (int i = 0; i < m->getIndices().size(); i+=3)
	{
		for (int j = 0; j < 3; j++)
		{
			ofIndexType i0 = m->getIndex(i + (j % 3));
			ofIndexType i1 = m->getIndex(i + ((j+1) % 3)); 
			ofIndexType i2 = m->getIndex(i + ((j+2) % 3)); 

			// If triangle with same edge has been added, make a tet with the 4 points
			if (prevTriPoint[i0][i1] < nPoints)
			{
				Tetrahedron t;
				t.p1 = i0;
				t.p2 = i1;
				t.p3 = prevTriPoint[i0][i1];
				t.p4 = i2;

				//m->setColor(t.p1, m->getColor(t.p1) + ofColor(255.0f));
				//m->setColor(t.p2, m->getColor(t.p2) + ofColor(255.0f));
				//m->setColor(t.p3, m->getColor(t.p3) + ofColor(255.0f));
				//m->setColor(t.p4, m->getColor(t.p4) + ofColor(255.0f));

				tets.push_back(t);
			}
			// Else, add this edge to the graph and continue
			else
			{
				prevTriPoint[i0][i1] = i2;
				prevTriPoint[i1][i0] = i2;
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

		float d = ofClamp(t1.dot(t2), -1.0f, 1.0f);
		restBend.push_back(acosf(d));
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

void ClothSim::addPointPin(PointPin *p)
{
	pins.insert(p);
}

void ClothSim::removePointPin(PointPin *p)
{
	if (pins.count(p) > 0)
	{
		pins.erase(p);
	}
}

void ClothSim::startStep()
{
	// Perform triangle mass calculation
	invPointMass.assign(nPoints, 0.0f);
	auto nPointTris = vector<int>(nPoints);
	for (int i = 0; i < nTris; i++)
	{
		ofMeshFace tri = m->getFace(i);
		ofVec3f U = (tri.getVertex(1) - tri.getVertex(0));
		ofVec3f V = (tri.getVertex(2) - tri.getVertex(0));
		ofVec3f faceNormal = U.getCrossed(V);
		float triArea = faceNormal.length() / 2.0f;

		float triMass = DENSITY * triArea;
		// Collect average mass over three triangles, then invert it
		for (int j = i * 3; j < i * 3 + 3; j++)
		{
			invPointMass[m->getIndex(j)] += triMass;
			nPointTris[m->getIndex(j)] += 1;
		}
	}

	for (int i = 0; i < nPoints; i++)
	{
		invPointMass[i] /= nPointTris[i];
		invPointMass[i] = 1.0f / invPointMass[i];
		//m->setColor(i, Utils::Debug1D(324.0f / invPointMass[i]));
	}

	// Apply point pin constraints, if any
	for(auto it = pins.begin(); it != pins.end(); it++)
	{
		PointPin *p = *it;
		pos[p->v] = p->target;
		vel[p->v] = ofVec3f();
		invPointMass[p->v] = 0.0f; // Infinite mass
	}

	// Apply external forces, i.e. gravity
	for (int i = 0; i < nPoints; i++)
	{
		vel[i] += DT * extForce[i];
		ppos[i] = pos[i] + DT * vel[i];
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
			float s = (p0.distance(p1) - restDist[i*3+j]) / (invPointMass[i0] + invPointMass[i1]);
			ofVec3f dp0 = -invPointMass[i0] * s * dnorm;
			ofVec3f dp1 = invPointMass[i1] * s * dnorm;

			ppos[i0] += STRETCH_STRENGTH * dp0;
			ppos[i1] += STRETCH_STRENGTH * dp1;
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
		d = ofClamp(d, -1.0f, 1.0f);

		ofVec3f q3 = (p2.crossed(n2) + (n1.crossed(p2) * d)) / (p2.crossed(p3)).length();
		ofVec3f q4 = (p2.crossed(n1) + (n2.crossed(p2) * d)) / (p2.crossed(p4)).length();
		ofVec3f q2 = - ((p3.crossed(n2) + (n1.crossed(p3) * d)) / p2.crossed(p3).length())
				     - ((p4.crossed(n1) + (n2.crossed(p4) * d)) / p2.crossed(p4).length());
		ofVec3f q1 = -q2 - q3 - q4;

		float denom = invPointMass[t.p1] * q1.lengthSquared()
					+ invPointMass[t.p2] * q2.lengthSquared()
					+ invPointMass[t.p3] * q3.lengthSquared()
					+ invPointMass[t.p4] * q4.lengthSquared();

		ppos[t.p1] += BEND_STRENGTH * q1 * -(invPointMass[t.p1] * sqrtf(1.0f - d*d)) * (acosf(d) - restBend[i]) / denom;
		ppos[t.p2] += BEND_STRENGTH * q2 * -(invPointMass[t.p2] * sqrtf(1.0f - d*d)) * (acosf(d) - restBend[i]) / denom;
		ppos[t.p3] += BEND_STRENGTH * q3 * -(invPointMass[t.p3] * sqrtf(1.0f - d*d)) * (acosf(d) - restBend[i]) / denom;
		ppos[t.p4] += BEND_STRENGTH * q4 * -(invPointMass[t.p4] * sqrtf(1.0f - d*d)) * (acosf(d) - restBend[i]) / denom;
	}
}

void ClothSim::endStep()
{
	// Final mesh update
	for (int i = 0; i < nPoints; i++)
	{
		vel[i] = (ppos[i] - pos[i]) / DT;
		pos[i] = ppos[i];
		m->setVertex(i, pos[i]);
	}
}