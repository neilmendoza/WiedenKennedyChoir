#include "testApp.h"
#include "ofxSimpleGuiToo.h"

using namespace ofxCv;
using namespace cv;

/*
	A classifier helper should handle the initial resizing for you. It should
	have two "preset" modes for very accurate face finding and very fast.
	It should also do automatic drawing, and give you OF objects instead of CV
	objects.
*/

void testApp::setup()
{
	ofBackground(0, 0, 0);
	ofSetFrameRate(60);
	fadeFrom = NULL;
	fadeTo = NULL;
	scaleFactor = 0.2;
	width = 640;
	height = 480;
	interactionLevel = 0; // can be changed via keystroke
	currentMovieLevel = 0;
	choirVideoFileNames[0] = "choir-1080p-1.mov";
	choirVideoFileNames[1] = "choir-1080p-2.mov";
	choirVideoFileNames[2] = "choir-1080p-3.mov";
	drawTriangles = false; // can be changed via keystroke
	maskImage.loadImage("mask.png"); // to mask the detected faces
	
	
	messageHandler.setup();
	
#ifdef _LIVE
	//cam.setDeviceID(2);
	cam.initGrabber(width, height);
	videoPtr = &cam;
#else
	player.loadMovie("test.mov");
	player.play();
	videoPtr = &player;
#endif
	// set this up to be the width and height of the camera
	faceTracker.setup(scaleFactor, width, height);
	
	// set these up to be the width and height to be rendered
	facesFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, ofFbo::maxSamples());
	masker.setup(ofGetWidth(), ofGetHeight());
	
	// this is the smoothing factor for the live face rectangles
	facePosLerp = 0.8;
	faceSizeLerp = 0.1;
	gui.addSlider("face pos lerp", facePosLerp, 0.f, 1.f);
	gui.addSlider("face size lerp", faceSizeLerp, 0.f, 1.f);
	
	// Load and play the choir movie
	for (int i = 0; i < 3; i++) {
		choirVideos[i].loadMovie(choirVideoFileNames[i]);
	}
	
	choirVideos[interactionLevel].play();
	
	// XML for choir faces
	choirFaceDir.allowExt("xml");
	choirFaceDir.listDir("choirfacedata"); // find what's in the directory
	//vchoirFaceDir.sort(); // sort into alphabetical order
	
	//allocate the vector to have as many ofImages as there are XML files
	if( choirFaceDir.size() ){
		choirFaces.assign(choirFaceDir.size(), ChoirFace());
	}
	
	// iterate through the files and load them
	for(int i = 0; i < (int)choirFaceDir.size(); i++){
		choirFaces[i].load(choirFaceDir.getPath(i));
	}
	gui.show();
	gui.loadFromXML();
}

void testApp::update()

{
	messageHandler.update();

	if (interactionLevel != currentMovieLevel && !fadeFrom && !fadeTo)
	{
		fadeFrom = &choirVideos[currentMovieLevel];
		fadeTo = &choirVideos[interactionLevel];
		fadeTo->setFrame(fadeFrom->getCurrentFrame());
		fadeTo->play();
		fadeToAlpha = 0;
	}
	
	// update with new frames
	if (currentMovieLevel != interactionLevel)
	{
		fadeToAlpha += 40;
		if (fadeToAlpha > 255) fadeToAlpha = 255;
		if (fadeToAlpha == 255)
		{
			fadeFrom->stop();
			fadeFrom = NULL;
			fadeTo = NULL;
			currentMovieLevel = interactionLevel;
		}
		else
		{
			fadeFrom->update();
			fadeTo->update();
		}
	}
		
	if (currentMovieLevel == interactionLevel) choirVideos[interactionLevel].update();

	videoPtr->update();
	
	// create new thread
	if(videoPtr->isFrameNew() && !faceTracker.isThreadRunning())	
	{
		RectTracker& tracker = faceTracker.getTracker();
		
		for (map<unsigned, LiveFace>::iterator it = faces.begin(); it != faces.end(); ++it)
		{
			if (!tracker.existsCurrent(it->first)) faces.erase(it);
		}
		vector<unsigned> labels = tracker.getCurrentLabels();
		
		for (int i = 0; i < labels.size(); ++i)
		{
			if (tracker.existsPrevious(labels[i]))
			{
				ofRectangle rect = toOf(tracker.getCurrent(labels[i]));
				map<unsigned, LiveFace>::iterator it = faces.find(labels[i]);
				if (it == faces.end()) faces.insert(make_pair(labels[i], LiveFace(rect)));
				else it->second.setCurrent(rect);
			}
		}
		faceTracker.update(*videoPtr);
		
		messageHandler.setFaces(labels.size());
	}
	
	for (map<unsigned, LiveFace>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		it->second.lerpToCurrent(facePosLerp, faceSizeLerp);
	}
}


void testApp::draw()
{
	if ( currentMovieLevel != interactionLevel )
	{
		ofEnableAlphaBlending();
		ofSetColor(255, 255, 255, 255 - fadeToAlpha);
		fadeFrom->draw(0, 0);
		
		ofSetColor(255, 255, 255, fadeToAlpha);
		fadeTo->draw(0, 0);
		ofDisableAlphaBlending();
	}
	else choirVideos[interactionLevel].draw(0, 0);
	
	ofSetColor(255, 255, 255);
	
	// create faces FBO
	facesFbo.begin();
	
	ofClear(0, 0, 0, 0);
	
	// bind the video input
	videoPtr->getTextureReference().bind();
	
	vector<int> selection;
	vector<int> unallocatedSlots;
	
	for (int i = 0; i < choirFaces.size(); i++) {
		if (choirFaces[i].getInteractionLevel() <= interactionLevel) {
			selection.push_back(i);
		}
	}
	
	// create another vector that we can manipulate
	unallocatedSlots = selection;
		
	for (int i = 0; i < selection.size(); i++) {
			
		// for each face box detected
		for (map<unsigned, LiveFace>::iterator it = faces.begin(); it != faces.end(); ++it) {
			
			// while there are still unallocated slots
			if ( !unallocatedSlots.empty() ) {	
					
				// Find the frame position for that slot on this particular frame
				const Frame& frame = choirFaces[unallocatedSlots.back()].getFrame(choirVideos[0].getCurrentFrame());
				
				glPushMatrix();

				glTranslatef(frame.centre.x, frame.centre.y, 0);
				glRotatef(frame.angle, 0, 0, 1);
				
				glBegin(GL_QUADS);
				
				// grab the rect of this liveFace
				ofRectangle rect = it->second;
				
				float shrink = 0.8;
				float halfW = shrink * frame.halfW; 
				float halfH = shrink * halfW * rect.height / rect.width;
				
				// create the first vertex
				glTexCoord2f(rect.x / scaleFactor, rect.y / scaleFactor);
				glVertex3f(-halfW, -halfH, 0);
				
				// create the second vertex
				glTexCoord2f((rect.x + rect.width) / scaleFactor, rect.y / scaleFactor);
				glVertex3f(halfW, -halfH, 0);
				
				// create the third vertex
				glTexCoord2f((rect.x + rect.width) / scaleFactor, (rect.y + rect.height) / scaleFactor);
				glVertex3f(halfW, halfH, 0);
				
				// create the final vertex
				glTexCoord2f(rect.x / scaleFactor, (rect.y + rect.height) / scaleFactor);
				glVertex3f(-halfW, halfH, 0);
				
				glEnd();
				
				glPopMatrix();
				
				// this slot is used now so remove it from the vector
				unallocatedSlots.pop_back();
			}
		}
		
	}
	
	// unbind the video input feed
	videoPtr->getTextureReference().unbind();
	
	// finished creating the frame buffer object
	facesFbo.end();
	
	
	// create mask FBO
	masker.beginMask();
	ofEnableAlphaBlending();

	for (int i = 0; i < selection.size(); i++) {
		
		if (choirFaces[selection[i]].getInteractionLevel() <= interactionLevel) {

			const Frame& frame = choirFaces[selection[i]].getFrame(choirVideos[0].getCurrentFrame());
			for (map<unsigned, LiveFace>::iterator it = faces.begin(); it != faces.end(); ++it)
			{
				ofRectangle rect = it->second;
				glPushMatrix();
				glTranslatef(frame.centre.x, frame.centre.y, 0);
				glRotatef(frame.angle, 0, 0, 1);
				maskImage.draw(-frame.halfW, -frame.halfH, frame.w, frame.h);
				
				glPopMatrix();
			}
		}
	}

	masker.endMask();

	
	// the masker draws the faces FBO texture
	masker.drawEffect(facesFbo.getTextureReference());
	
	selection.clear();
	
	// Draw the unmasked faces FBO
	//facesFbo.draw(0, 0);
	
	// draw labels
	/*
	for (map<unsigned, LiveFace>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		ofRectangle rect = it->second;
		ofDrawBitmapString(ofToString(it->first), rect.x / scaleFactor, rect.y / scaleFactor);
	}*/
	
	// draw choir face triangles
	if (drawTriangles) {
		for (int i = 0; i < choirFaces.size(); i++) choirFaces[i].drawTriangle(choirVideos[0].getCurrentFrame());
	}
	
	// draw debug images
	if (gui.isOn())
	{
		faceTracker.drawThresholded(ofGetWidth() - 210, ofGetHeight() - 160, 200, 150);
		videoPtr->draw(ofGetWidth() - 210, ofGetHeight() - 320, 200, 150);
	}
	
	ostringstream oss;
	oss << "ids: ";
	for (map<unsigned, LiveFace>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		oss << it->first << ", ";
	}
	ofDrawBitmapString(oss.str(), 10, ofGetHeight() - 80);
	oss.str("");
	oss << faces.size() << " faces found";
	ofDrawBitmapString(oss.str(), 10, ofGetHeight() - 60);
	oss.str("");
	oss << ofGetFrameRate() << " FPS";
	ofDrawBitmapString(oss.str(), 10, ofGetHeight() - 40);
	
	gui.draw();
}

void testApp::keyPressed(int key)
{
#ifdef _LIVE
	if (key == 'v') cam.videoSettings();
#endif
	if (key == '1') interactionLevel = 0;
	if (key == '2') interactionLevel = 1;
	if (key == '3') interactionLevel = 2;
	if (key == 't') drawTriangles = !drawTriangles;
	if (key == 'b') faceTracker.resetBackground();
	if (key == 'g')
	{
		gui.toggleDraw();
	}
}
