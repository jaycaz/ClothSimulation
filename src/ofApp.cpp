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
	mesh.setColorForIndices(0, mi.size(), ofColor(100.0f));

	model = ofMatrix4x4::newIdentityMatrix();
	model.postMultRotate(-75.0f, 0.0f, 1.0f, 0.0f);
	model.postMultRotate(30.0f, 1.0f, 0.0f, 0.0f);

	for (int i = 0; i < mesh.getVertices().size(); i++)
	{
		mesh.setVertex(i, model * mesh.getVertex(i));
	}

	sim = ClothSim(&mesh);

}

//--------------------------------------------------------------
void ofApp::setup(){

	resetCloth();

	cam.setAutoDistance(false);
	cam.setNearClip(0.1f);
	cam.setFarClip(1000.0f);
	cam.setPosition(ofVec3f(0.0f, 0.0f, -4.0f));
	cam.lookAt(ofVec3f(0.0f, -1.0f, 0.0f), ofVec3f(0.0f, 1.0f, 0.0f));
	cam.setTarget(ofVec3f(0.0f, -1.0f, 0.0f));
}

//--------------------------------------------------------------
void ofApp::update(){

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
	auto spotlight = ofLight();
	spotlight.enable();
	spotlight.lookAt(ofVec3f(0.0f, 0.0f, 1.0f));
	cam.begin();

	// Add lights

	if (drawFrames)
	{
		mesh.drawFaces();
	}
	else
	{
		mesh.drawWireframe();
	}

	cam.end();
	spotlight.disable();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	if (key == 'r')
	{
		resetCloth();
	}

	if (key == 'w')
	{
		drawFrames = !drawFrames;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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
