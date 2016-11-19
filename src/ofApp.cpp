#include "ofApp.h"

void ofApp::resetCloth()
{
	const float PLANE_WIDTH = 2.0f;
	const float PLANE_HEIGHT = 2.0f;
	const int POINTS_WIDTH = 40;
	const int POINTS_HEIGHT = 40;

	mesh = ofMesh::plane(PLANE_WIDTH, PLANE_HEIGHT, POINTS_WIDTH, POINTS_HEIGHT, OF_PRIMITIVE_TRIANGLES);
	auto mi = mesh.getIndices();
	mesh.getColors().resize(mi.size());
	mesh.setColorForIndices(0, mi.size(), defaultColor);
	
	model = ofMatrix4x4::newIdentityMatrix();
	model.postMultRotate(30.0f, 0.0f, 1.0f, 0.0f);

	for (int i = 0; i < mesh.getVertices().size(); i++)
	{
		mesh.setVertex(i, model * mesh.getVertex(i));
	}

	sim = ClothSim(&mesh);
	int nPoints = POINTS_WIDTH * POINTS_HEIGHT;
	sim.addPointPin(new PointPin(1558, mesh.getVertex(1558) + ofVec3f(0.2f, 0.0f, 0.0f)));
	sim.addPointPin(new PointPin(1521, mesh.getVertex(1521) - ofVec3f(0.2f, 0.0f, 0.0f)));

}

//--------------------------------------------------------------
void ofApp::setup(){

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

	resetCloth();

	// Setup light
	light = ofLight();
	light.setAmbientColor(ofColor(10.0f));
	light.setDiffuseColor(ofColor(ofColor::white));
	light.setSpecularColor(ofColor(ofColor::white));
	light.setPosition(ofVec3f(0.0f, 0.0f, 4.0f));
	light.setOrientation(ofVec3f(-45.0f, 0.0f, 0.0f));
	light.setScale(ofVec3f(1.0f));
	light.setDirectional();

	light2 = ofLight();
	light2.setAmbientColor(ofColor(10.0f));
	light2.setDiffuseColor(ofColor(ofColor::white));
	light2.setSpecularColor(ofColor(ofColor::white));
	light2.setPosition(ofVec3f(0.0f, 0.0f, 4.0f));
	light2.setOrientation(ofVec3f(45.0f, 0.0f, 0.0f));
	light2.setScale(ofVec3f(1.0f));
	light.setDirectional();

	// Setup camera
	cam.setAutoDistance(false);
	cam.disableMouseInput();
	cam.setNearClip(0.1f);
	cam.setFarClip(1000.0f);
	cam.setPosition(ofVec3f(0.0f, 0.0f, -4.0f));
	cam.lookAt(ofVec3f(0.0f, -1.0f, 0.0f), ofVec3f(0.0f, 1.0f, 0.0f));
	cam.setTarget(ofVec3f(0.0f, -1.0f, 0.0f));
}

//--------------------------------------------------------------
void ofApp::update(){

	if (paused) return;

	// Perform one full frame in the simulation
	for (int i = 0; i < sim.N_STEPS_PER_FRAME; i++)
	{
		sim.startStep();
		{
			for (int j = 0; j < sim.N_TICKS_PER_STEP; j++)
			{
				sim.tick();
			}
		}
		sim.endStep();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	//ofClear(ofColor::lightSteelBlue);
	ofBackground(ofColor::lightSteelBlue);

	// Start camera
	cam.begin();
	light.enable();
	light2.enable();

	if (drawFrames)
	{
		mesh.drawFaces();
	}
	else
	{
		mesh.drawWireframe();
	}

	light2.disable();
	light.disable();
	cam.end();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	if (key == 'r')
	{
		resetCloth();
	}

	if (key == 'w')
	{
		drawFrames = !drawFrames;
	}

	if (key == ' ')
	{
		paused = !paused;
	}

	if (key == OF_KEY_RIGHT)
	{
		paused = !paused;
		update();
		paused = !paused;
	}

	if (key == OF_KEY_CONTROL)
	{
		camMode = true;
		cam.enableMouseInput();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

	if (key == OF_KEY_CONTROL)
	{
		camMode = false;
		cam.disableMouseInput();
	}

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	mouse = ofPoint(x, y);

	if (!camMode)
	{
		float minDist = numeric_limits<float>::max();
		// Detect closest point to mouse and highlight it
		selectIndex = -1;
		ofIndexType minIndex = -1;
		for (int i = 0; i < mesh.getVertices().size(); i++)
		{
			ofPoint vertex = cam.worldToScreen(mesh.getVertex(i));
			float dist = vertex.distanceSquared(mouse);
			if (dist < minDist)
			{
				minIndex = i;
				minDist = dist;
			}
			mesh.setColor(i, defaultColor);
		}

		if (sqrt(minDist) < 30.0f)
		{
			selectIndex = minIndex;
			mesh.setColor(selectIndex, ofColor::green);
			cout << "Selected: " << selectIndex << " at " << mesh.getVertex(selectIndex) << endl;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if (!camMode)
	{
		if (selectIndex < mesh.getNumVertices() && selectPin)
		{
			ofPoint dm = cam.screenToWorld(ofPoint(x, y)) - cam.screenToWorld(mouse);

			selectPin->target = selectStart + dm * sim.MOUSE_DRAG_MULT;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (!camMode)
	{
		if (selectIndex < mesh.getNumVertices())
		{
			selectPin = new PointPin();
			selectPin->v = selectIndex;
			selectPin->target = mesh.getVertex(selectIndex);
			selectStart = ofVec3f(mesh.getVertex(selectIndex));
			sim.addPointPin(selectPin);
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	if (selectPin && !camMode && button == OF_MOUSE_BUTTON_LEFT)
	{
		sim.removePointPin(selectPin);
		delete selectPin;
	}
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
