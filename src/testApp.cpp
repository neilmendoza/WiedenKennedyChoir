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
	faceTracker.setup(scaleFactor, width, height);
	facesFbo.allocate(width, height, GL_RGBA, ofFbo::maxSamples());
	masker.setup(width, height);
	
	lerpFactor = 0.3;
}

void testApp::update()
{
	videoPtr->update();
	if(videoPtr->isFrameNew() && !faceTracker.isThreadRunning())	
	{
		RectTracker& tracker = faceTracker.getTracker();
		
		for (map<unsigned, ofRectangle>::iterator it = faces.begin(); it != faces.end(); ++it)
		{
			if (!tracker.existsCurrent(it->first)) faces.erase(it);
		}
		
		vector<unsigned> labels = tracker.getCurrentLabels();
		for (int i = 0; i < labels.size(); ++i)
		{
			ofRectangle rect = toOf(tracker.getCurrent(labels[i]));
			map<unsigned, ofRectangle>::iterator it = faces.find(labels[i]);
			if (it == faces.end()) faces.insert(make_pair(labels[i], rect));
			else
			{
				faces[labels[i]].x = rect.x + lerpFactor * (faces[labels[i]].x - rect.x);
				faces[labels[i]].y = rect.y + lerpFactor * (faces[labels[i]].y - rect.y);
				faces[labels[i]].width = rect.width + lerpFactor * (faces[labels[i]].width - rect.width);
				faces[labels[i]].height = rect.height + lerpFactor * (faces[labels[i]].height - rect.height);
			}
		}
		faceTracker.update(*videoPtr);
	}
	
}

void testApp::draw()
{
	ofSetColor(255, 255, 255, 127);

	//videoPtr->draw(0, 0);
	
	ofSetColor(255, 255, 255);
	
	ofNoFill();
	facesFbo.begin();
	ofClear(0, 0, 0, 0);

	videoPtr->getTextureReference().bind();

	// create faces FBO
	glBegin(GL_QUADS);
	//for(int i = 0; i < objects.size(); ++i)
	for (map<unsigned, ofRectangle>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		ofRectangle rect = it->second;
		glTexCoord2f(rect.x / scaleFactor, rect.y / scaleFactor);
		glVertex3f(rect.x / scaleFactor, rect.y / scaleFactor, 0);
		
		glTexCoord2f((rect.x + rect.width) / scaleFactor, rect.y / scaleFactor);
		glVertex3f((rect.x + rect.width) / scaleFactor, rect.y / scaleFactor, 0);
		
		glTexCoord2f((rect.x + rect.width) / scaleFactor, (rect.y + rect.height) / scaleFactor);
		glVertex3f((rect.x + rect.width) / scaleFactor, (rect.y + rect.height) / scaleFactor, 0);
		
		glTexCoord2f(rect.x / scaleFactor, (rect.y + rect.height) / scaleFactor);
		glVertex3f(rect.x / scaleFactor, (rect.y + rect.height) / scaleFactor, 0);
	}
	glEnd();
	
	videoPtr->getTextureReference().unbind();

	facesFbo.end();
	
	// create mask FBO
	masker.beginMask();
	ofClear(0, 0, 0, 0);
	ofEnableAlphaBlending();
	for (map<unsigned, ofRectangle>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		ofRectangle rect = it->second;
		maskImage.draw(rect.x / scaleFactor, rect.y / scaleFactor, rect.width / scaleFactor, rect.height / scaleFactor);
	}
	masker.endMask();
	
	masker.drawEffect(facesFbo.getTextureReference());
	
	// draw labels
	for (map<unsigned, ofRectangle>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		ofRectangle rect = it->second;
		ofDrawBitmapString(ofToString(it->first), rect.x / scaleFactor, rect.y / scaleFactor);
	}
	
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