#include "testApp.h"

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
	scaleFactor = 0.25;
	width = 640;
	height = 480;
	maskImage.loadImage("mask.png");
#ifdef _LIVE
	cam.initGrabber(width, height);
	videoPtr = &cam;
#else
	player.loadMovie("street.mov");
	player.play();
	videoPtr = &player;
#endif
	// set this up to be the width and height of the camera
	faceTracker.setup(scaleFactor, width, height);
	
	// set these up to be the width and height to be rendered
	facesFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, ofFbo::maxSamples());
	masker.setup(ofGetWidth(), ofGetHeight());
	
	// this is the smoothing factor for the live face rectangles
	lerpFactor = 0.5;
	
	choirVideo.loadMovie("choir-1080p - all.mov");
	choirVideo.play();
	choirFace.load("facedata.xml");
}

void testApp::update()
{
	choirVideo.update();
	
	videoPtr->update();
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
			ofRectangle rect = toOf(tracker.getCurrent(labels[i]));
			map<unsigned, LiveFace>::iterator it = faces.find(labels[i]);
			if (it == faces.end()) faces.insert(make_pair(labels[i], LiveFace(rect)));
			else it->second.setCurrent(rect);
		}
		faceTracker.update(*videoPtr);
	}
	
	for (map<unsigned, LiveFace>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		it->second.lerpToCurrent(lerpFactor);
	}
}


void testApp::draw()
{
	choirVideo.draw(0, 0);
	ofSetColor(255, 255, 255, 127);

	//videoPtr->draw(0, 0);
	
	ofSetColor(255, 255, 255);
	
	
	const Frame& frame = choirFace.getFrame(choirVideo.getCurrentFrame());
	
	// create faces FBO
	facesFbo.begin();
	ofClear(0, 0, 0, 0);
	videoPtr->getTextureReference().bind();
	
	for (map<unsigned, LiveFace>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		glPushMatrix();
		glTranslatef(frame.centre.x, frame.centre.y, 0);
		glRotatef(frame.angle, 0, 0, 1);
		
		glBegin(GL_QUADS);
		
		ofRectangle rect = it->second;
		glTexCoord2f(rect.x / scaleFactor, rect.y / scaleFactor);
		glVertex3f(-frame.halfW, -frame.halfH, 0);
		
		glTexCoord2f((rect.x + rect.width) / scaleFactor, rect.y / scaleFactor);
		glVertex3f(frame.halfW, -frame.halfH, 0);
		
		glTexCoord2f((rect.x + rect.width) / scaleFactor, (rect.y + rect.height) / scaleFactor);
		glVertex3f(frame.halfW, frame.halfH, 0);
		
		glTexCoord2f(rect.x / scaleFactor, (rect.y + rect.height) / scaleFactor);
		glVertex3f(-frame.halfW, frame.halfH, 0);
		
		glEnd();
		
		glPopMatrix();
	}
	videoPtr->getTextureReference().unbind();
	facesFbo.end();
	
	// create mask FBO
	masker.beginMask();
	ofEnableAlphaBlending();
	for (map<unsigned, LiveFace>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		ofRectangle rect = it->second;
		glPushMatrix();
		glTranslatef(frame.centre.x, frame.centre.y, 0);
		glRotatef(frame.angle, 0, 0, 1);
		maskImage.draw(-frame.halfW, -frame.halfH, frame.w, frame.h);
		
		glPopMatrix();
	}
	masker.endMask();
	
	masker.drawEffect(facesFbo.getTextureReference());
	
	
	// draw labels
	for (map<unsigned, LiveFace>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		ofRectangle rect = it->second;
		ofDrawBitmapString(ofToString(it->first), rect.x / scaleFactor, rect.y / scaleFactor);
	}
	
	// draw choir face triangles
	//choirFace.drawTriangle(choirVideo.getCurrentFrame());
	
	ostringstream oss;
	oss << faces.size() << " faces found";
	ofDrawBitmapString(oss.str(), 10, 20);
	oss.str("");
	oss << ofGetFrameRate() << " FPS";
	ofDrawBitmapString(oss.str(), 10, 40);
}

void testApp::keyPressed(int key)
{
#ifdef _LIVE
	if (key == 'v') cam.videoSettings();
#endif
}
