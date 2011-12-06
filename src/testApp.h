#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "FaceTracker.h"
#include "ofxMask.h"

using namespace ofxCv;

#define _LIVE

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
private:
	void readData();
	void lerp(ofRectangle& rect, const ofRectangle& dest, float factor);
	float lerpFactor;
	
	FaceTracker faceTracker;
	
	map<unsigned, ofRectangle> faces;
	map<unsigned, ofRectangle> newFaces;
	
	float scaleFactor;
	
	ofBaseVideoDraws* videoPtr;
	
#ifdef _LIVE
	ofVideoGrabber cam;
#else
	ofVideoPlayer player;
#endif	
	
	ofFbo facesFbo;
	
	int width;
	int height;
	
	ofImage maskImage;
	Apex::ofxMask masker;
};
