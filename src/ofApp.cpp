#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	mesh = ofMesh::plane(4.0f, 4.0f, 10, 10);
	auto mi = mesh.getIndices();
	mesh.getColors().resize(mi.size());
	mesh.setColorForIndices(0, mi.size(), ofColor(255.0f));

	model.makeRotationMatrix(90.0f, 1.0f, 0.0f, 0.0f);

	for (int i = 0; i < mesh.getVertices().size(); i++)
	{
		mesh.setVertex(i, mesh.getVertex(i) * model);
	}

	sim = ClothSim(&mesh);

	cam.setPosition(0.0, 2.0, -10.0);
	cam.lookAt(ofVec3f(0.0, -2.0, 0.0));
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
	cam.begin();

	if (drawFrames)
	{
		mesh.drawFaces();
	}
	else
	{
		mesh.drawWireframe();
	}

	ofPushMatrix();
	ofMultMatrix(model);
	ofPopMatrix();
	cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
