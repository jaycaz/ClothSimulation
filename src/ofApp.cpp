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
	mesh.setColorForIndices(0, mi.size(), ofColor::gray);
	
	model = ofMatrix4x4::newIdentityMatrix();
	model.postMultRotate(40.0f, 1.0f, 0.0f, 0.0f);
	model.postMultRotate(-75.0f, 0.0f, 1.0f, 0.0f);

	for (int i = 0; i < mesh.getVertices().size(); i++)
	{
		mesh.setVertex(i, model * mesh.getVertex(i));
	}

	sim = ClothSim(&mesh);

}

//--------------------------------------------------------------
void ofApp::setup(){

	resetCloth();

	light = ofLight();
	light.setDiffuseColor(ofColor(ofColor::white));
	light.lookAt(ofVec3f(1.0f, 0.0f, 0.0f));
	light.setPosition(ofVec3f(0.0f, 0.0f, 0.0f));
	light.setDirectional();

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
	for (int i = 0; i < N_STEPS_PER_FRAME; i++)
	{
		sim.startStep();
		{
			for (int j = 0; j < N_TICKS_PER_STEP; j++)
			{
				sim.tick();
			}
		}
		sim.endStep();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(50.0f);

	// Start camera
	light.enable();

	cam.begin();

	// Add lights
	//ofVec3f c = cam.getGlobalPosition();
	//ofSetLineWidth(2.0f);
	//ofDrawLine(c, c + (mouseWorld - c) * 2.0f + ofVec3f(0.0f, 0.5f, 0.0f));

	if (drawFrames)
	{
		mesh.drawFaces();
	}
	else
	{
		mesh.drawWireframe();
	}

	cam.end();
	light.disable();

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
		for (int i = 0; i < mesh.getVertices().size(); i++)
		{
			ofPoint vertex = cam.worldToScreen(mesh.getVertex(i));
			float dist = vertex.distanceSquared(mouse);
			if (dist < minDist)
			{
				selectIndex = i;
				minDist = dist;
			}
			mesh.setColor(i, ofColor::gray);
		}

		mesh.setColor(selectIndex, ofColor::green);
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if (!camMode)
	{
		if (button == OF_MOUSE_BUTTON_LEFT)
		{
			ofPoint dm = cam.screenToWorld(ofPoint(x, y)) - cam.screenToWorld(mouse);
			cout << "(dx,dy,dz) = " << dm << endl;

			selectPin.target = selectStart + dm * MOUSE_DRAG_MULT;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (!camMode)
	{
		if (selectIndex >= 0 && selectIndex < mesh.getNumVertices())
		{
			selectPin = PointPin();
			selectPin.v = selectIndex;
			selectPin.target = mesh.getVertex(selectIndex);
			selectStart = ofVec3f(mesh.getVertex(selectIndex));
			sim.addPointPin(&selectPin);
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	if (!camMode)
	{
		sim.removePointPin(&selectPin);
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
