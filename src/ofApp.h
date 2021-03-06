#pragma once

#include "ofMain.h"
#include "ClothSim.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void resetCloth();

		bool drawFrames = true;
		bool paused = false;
		bool camMode = false;

		ofColor defaultColor = ofColor::darkRed;

		ofPoint mouse;
		ofIndexType selectIndex;
		PointPin *selectPin;
		ofVec3f selectStart;

		ClothSim sim;

		ofLight light;
		ofLight light2;
		ofEasyCam cam;
		ofMesh mesh;
		ofMatrix4x4 model;
};
