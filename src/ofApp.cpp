#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	mesh = ofMesh::plane(4.0f, 4.0f, 10, 10);
	auto mi = mesh.getIndices();
	mesh.setColorForIndices(0, mi.size(), ofColor(255.0f));

	model.makeRotationMatrix(90.0f, 1.0f, 0.0f, 0.0f);


	cam.setPosition(0.0, 1.0, -10.0);
	cam.lookAt(ofVec3f(0.0, -1.0, 0.0));
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	cam.begin();
	ofPushMatrix();
	ofMultMatrix(model);
	mesh.drawFaces();
	ofPopMatrix();
	cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
