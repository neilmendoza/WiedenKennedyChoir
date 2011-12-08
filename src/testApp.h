#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofFileUtils.h"
#include "FaceTracker.h"
#include "ofxMask.h"
#include "LiveFace.h"
#include "ChoirFace.h"

using namespace ofxCv;

//#define _LIVE

class testApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
private:
	float faceSizeLerp;
	float facePosLerp;
	
	FaceTracker faceTracker;
	
	ofDirectory choirFaceDir;
	vector<ofFile> faceDataFiles;
	
	map<unsigned, LiveFace> faces;
	vector<ChoirFace> choirFaces;
	//ChoirFace choirFace;
	
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
	int interactionLevel;
	bool drawTriangles; 
	
	ofImage maskImage;
	//vector<Apex::ofxMask> masker;
	Apex::ofxMask masker;
	
};
