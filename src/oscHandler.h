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

#define PORT_OUT 9001
#define PORT_IN 8000
#define HOST "localhost"

class OscHandler {

public:

	void setup();
	void update();
	void setFaces(int number);
	ofxOscReceiver inbox;
	ofxOscSender outbox;

private:
	
	int numberOfFaces;
	int oscInteractionLevel;
	int inputLevel;
	string lyricLine;
	int currentWord;

	

};