#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "FaceTracker.h"
#include "ofxMask.h"
#include "LiveFace.h"
#include "ChoirFace.h"

using namespace ofxCv;

#define _LIVE

class testApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
private:
	float lerpFactor;
	
	FaceTracker faceTracker;
	
	map<unsigned, LiveFace> faces;
	ChoirFace choirFace;
	
	float scaleFactor;
	
	ofBaseVideoDraws* videoPtr;
	
#ifdef _LIVE
	ofVideoGrabber cam;
#else
	ofVideoPlayer player;
#endif	
	
	ofVideoPlayer choirVideo;
	
	ofFbo facesFbo;
	
	int width;
	int height;
	
	ofImage maskImage;
	Apex::ofxMask masker;
};
