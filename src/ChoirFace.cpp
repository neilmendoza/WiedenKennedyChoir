/*
 *  ChoirFace.cpp
 *
 *  Copyright (c) 2011, Neil Mendoza, http://www.neilmendoza.com
 *  All rights reserved. 
 *  
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met: 
 *  
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer. 
 *  * Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *  * Neither the name of 16b.it nor the names of its contributors may be used 
 *    to endorse or promote products derived from this software without 
 *    specific prior written permission. 
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *  POSSIBILITY OF SUCH DAMAGE. 
 *
 */
#include "ChoirFace.h"
#include "ofxXmlSettings.h"

const ofVec2f Frame::X_AXIS(1, 0);
const ofVec2f ChoirFace::CROP_SHIFT(-472, -192);

Frame::Frame(ofVec2f p1, ofVec2f p2, ofVec2f p3) : p1(p1), p2(p2), p3(p3)
{
	centre = (p1 + p2 + p3) / 3.f; 
	angle = p1.angle(p2);
	w = 5.f * (p2 - p1).length();
	halfW = 0.5f * w;
	h = 5.f * (p3 - (p1 + p2) * .5f).length();
	halfH = 0.5f * h;
}

void ChoirFace::load(const string& fileName)
{
	ofxXmlSettings xml;
	xml.loadFile(fileName);
	xml.pushTag("CHOIRFACE");
	for (int i = 0; i < xml.getNumTags("FRAME"); ++i)
	{
		xml.pushTag("FRAME", i);
		ofVec2f pt1(xml.getValue("POINT1:X", 0) + CROP_SHIFT.x, xml.getValue("POINT1:Y", 0) + CROP_SHIFT.y);
		ofVec2f pt2(xml.getValue("POINT2:X", 0) + CROP_SHIFT.x, xml.getValue("POINT2:Y", 0) + CROP_SHIFT.y);
		ofVec2f pt3(xml.getValue("POINT3:X", 0) + CROP_SHIFT.x, xml.getValue("POINT3:Y", 0) + CROP_SHIFT.y);
		frames.push_back(Frame(pt1, pt2, pt3));
		xml.popTag();
	}
}

void ChoirFace::drawTriangle(int frameNum)
{
	//cout << frames[frameNum].p1 << " " << frames[frameNum].p2 << endl;
	ofLine(frames[frameNum].p1, frames[frameNum].p2);
	ofLine(frames[frameNum].p2, frames[frameNum].p3);
	ofLine(frames[frameNum].p3, frames[frameNum].p1);
}
