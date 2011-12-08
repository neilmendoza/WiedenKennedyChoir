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
	
	inbox.setup(PORT_IN);
	outbox.setup(HOST, PORT_OUT);
	numberOfFaces = 0;
	
}

void OscHandler::update() {
	
	while (inbox.hasWaitingMessages()) {
		
		ofxOscMessage m;
		inbox.getNextMessage( &m );
		
		if ( m.getAddress() == "/interactionLevel") {
			
			oscInteractionLevel = m.getArgAsInt32(0);
			
		} else if ( m.getAddress() == "/inputLevel") {
			
			inputLevel = m.getArgAsInt32(0);
		
		} else if ( m.getAddress() == "/lyricLine") {

			lyricLine = m.getArgAsString(0);
			
		} else if ( m.getAddress() == "/currentWord") {

			currentWord = m.getArgAsInt32(0);
		
		} else {

			printf("NO OSC MESSAGE DEFINED FOR %s", (string)(m.getArgAsString(0)));
		
		}
	}
	
	
	
	
	
}

void OscHandler::setFaces(int number) {

	if(number != numberOfFaces) { 
		
		numberOfFaces = number;
		
		ofxOscMessage f;
		f.setAddress("/faces");
		f.addIntArg(number);
		outbox.sendMessage(f);
		f.clear();
		
		
	}

}