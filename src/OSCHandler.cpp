/*
 *  oscHandler.cpp
 *  choir
 *
 *  Created by Paul Skinner on 08/12/2011.
 *  Copyright 2011 Wieden + Kennedy. All rights reserved.
 *
 */

#include "OscHandler.h"

void OscHandler::setup() {
	
	inboxPD.setup(PORT_IN_PD);
	inboxKinect.setup(PORT_IN_KINECT);
	outboxPD.setup(HOST, PORT_OUT_PD);
	numberOfFaces = 0;
	
}

void OscHandler::update() {
	
	while (inboxPD.hasWaitingMessages()) {
		
		ofxOscMessage m;
		inboxPD.getNextMessage( &m );
		
		if ( m.getAddress() == "/interactionLevel") {
			
			oscInteractionLevel = m.getArgAsInt32(0);
			
		} else if ( m.getAddress() == "/inputLevel") {
			
			inputLevel = m.getArgAsInt32(0);
		
		} else if ( m.getAddress() == "/lyricLine") {

			lyricLine = m.getArgAsString(0);
			
		} else if ( m.getAddress() == "/wordTime") {

			wordTime = m.getArgAsInt32(0);
		
		} else {

			printf("NO OSC MESSAGE DEFINED FOR %s", (string)(m.getArgAsString(0)));
		
		}
	}
	
	while (inboxKinect.hasWaitingMessages()) {
		
		ofxOscMessage m1;
		inboxKinect.getNextMessage( &m1 );
		
		if ( m1.getAddress() == "/presence" ) {
			
			switch (m1.getArgAsInt32(0)) {
				case 1:
					presence = true;
					break;
				default:
					presence = false;
					break;
			}
			
		} else if ( m1.getAddress() == "/closestObject" ) {
			
			int p = m1.getArgAsInt32(0);
			
			if (p < 10000) {
				
				proximity = p;
				
			} else {
				
				proximity = NULL;
			
			}

		} else {
			
			printf("NO OSC MESSAGE DEFINED FOR %s", (string)(m1.getArgAsString(0)));
			
		}
	}	
}

void OscHandler::setFaces(int number) {

	if(number != numberOfFaces) { 
		
		numberOfFaces = number;
		
		ofxOscMessage f;
		f.setAddress("/faces");
		f.addIntArg(number);
		outboxPD.sendMessage(f);
		f.clear();
		
		
	}
}

string OscHandler::getLyric() {
	
	return lyricLine;
	
}

int OscHandler::getTime() {
	
	return wordTime;
	
}

int OscHandler::getProximity() {
	
	return proximity;
}
