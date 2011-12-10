/*
 *  oscHandler.h
 *  choir
 *
 *  Created by Paul Skinner on 08/12/2011.
 *  Copyright 2011 Wieden + Kennedy. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

#define PORT_OUT_PD 9001
#define PORT_IN_PD 8000
#define PORT_IN_KINECT 9002

#define HOST "localhost"

class OscHandler {

public:

	void setup();
	void update();
	void setFaces(int number);
	string getLyric();
	int getTime();
	int getProximity();
	ofxOscReceiver inboxPD;
	ofxOscReceiver inboxKinect;
	ofxOscSender outboxPD;

private:
	
	int numberOfFaces;
	int oscInteractionLevel;
	int inputLevel;
	string lyricLine;
	int wordTime;

	bool presence;
	int numberOfBlobs;
	int proximity;
	

};