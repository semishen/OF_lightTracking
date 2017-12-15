#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //xml
    if(!xml.loadFile("videoRes.xml")){
        ofLogError() << "Couldn't load the file!";
    }
    
    int _width = xml.getValue("videoRes:width",0) + 300;
    if (_width < 680){ _width = 680; }
    int _height = xml.getValue("videoRes:height",0) + 300;
    
    ofSetWindowShape(_width, _height);
    
    //videos
    
    string rgbName = xml.getValue("videoRes:rgb","");
    vidRGB.load(rgbName);
    vidRGB.setLoopState(OF_LOOP_NORMAL);
    vidRGB.play();
    
    if(xml.getValue("videoRes:bw","") != ""){
        string bwName = xml.getValue("videoRes:bw","");
        vidBW.load(bwName);
        vidBW.setLoopState(OF_LOOP_NORMAL);
        vidBW.play();
        bwIsFound = true;
    }
    else { bwIsFound = false; }
    
    //gui
    gui.setup();
    gui.setPosition(xml.getValue("videoRes:width",0)+40, 20);
    
    openCv.setName("OpenCV");
    openCv.add(threshold.set("Threshold", 160, 1, 255));
    openCv.add(miniArea.set("Mini Area", 20, 1, 400));
    openCv.add(maxArea.set("Max Area", 150, 1, 400));
    openCv.add(startFrame.set("Start Frame", 1, 1, vidRGB.getTotalNumFrames()-1));
    openCv.add(endFrame.set("End Frame", vidRGB.getTotalNumFrames()-1, 1, vidRGB.getTotalNumFrames()-1));
    gui.add(openCv);
    
    drawLines.setName("Draw Lines");
    drawLines.add(colorH.set("Hue", 100, 0, 255));
    drawLines.add(colorS.set("Saturation", 0, 0, 255));
    drawLines.add(colorB.set("Brightness", 255, 0, 255));
    drawLines.add(colorA.set("Alpha", 255, 0, 255));
    drawLines.add(lineWidth.set("Line Width", 0.1, 0.1, 5.0));
    gui.add(drawLines);
    
    if (bwIsFound) {
        useVideo.setName("Process Video");
        useVideo.add(bBWvideo.set("Use BW video", false));
        gui.add(useVideo);
    }
        
    gui.loadFromFile("settings.xml");
    if(_width == 680){gui.minimizeAll();}
    
    //openCv
    colorImg.allocate(320,240);
	grayImg.allocate(320,240);
    grayImg.set(1.0);
    
}

//--------------------------------------------------------------
void ofApp::update(){
	ofBackground(100,100,100);

    bool bNewFrameRGB = false;
    bool bNewFrameBW = false;

    vidRGB.update();
    bNewFrameRGB = vidRGB.isFrameNew();
    
    if (bwIsFound) {
        vidBW.update();
        bNewFrameBW = vidBW.isFrameNew();
    }

	if (bNewFrameRGB){
        
        if (vidRGB.getCurrentFrame() > startFrame && vidRGB.getCurrentFrame() < endFrame) {
            
            ofPixels pix;
            if (bNewFrameBW && bBWvideo) {
                pix = vidBW.getPixels();
            }
            else{
                pix = vidRGB.getPixels();
            }
            pix.resize(320, 240);
            colorImg.setFromPixels(pix);
            grayImg = colorImg;
            grayImg.threshold(threshold);
            contourFinder.findContours(grayImg, miniArea, maxArea, 1, false, false);
            
            int ratio = vidRGB.getWidth()/grayImg.getWidth();
            
            if (contourFinder.nBlobs > 0) {
                points.push_back(ofPoint(contourFinder.blobs[0].centroid.x*ratio+20, contourFinder.blobs[0].centroid.y*ratio+20));
            }
            pix.clear();
 
        }
        
        if (vidRGB.getCurrentFrame() == vidRGB.getTotalNumFrames()-1) {
            points.clear();
        }
        
	}

}

//--------------------------------------------------------------
void ofApp::draw(){
    int _width = 320;
    int _height = vidRGB.getHeight();
    
	// draw rgb video, processed image
	ofSetHexColor(0xffffff);
	vidRGB.draw(20,20);
	grayImg.draw(20, _height+40);

	// draw blob

	ofFill();
	ofSetHexColor(0x333333);
	ofDrawRectangle(_width+20,_height+40,320,240);
	ofSetHexColor(0xffffff);

    for (int i = 0; i < contourFinder.nBlobs; i++){
        ofSetLineWidth(1.0);
        contourFinder.blobs[0].draw(_width+20, _height+40);
    }
    
    // draw polyline on rgb video
    line.clear();
    line.addVertices(points);
    ofSetLineWidth(lineWidth);
    ofColor c = ofColor(0);
    c.setHsb(colorH, colorS, colorB);
    ofSetColor(c, colorA);
    line.draw();
    
    // draw gui
    gui.draw();
    
    //report
    ofSetHexColor(0xffffff);
    stringstream reportStr;
    reportStr << "Press 's' to save settings" << endl
    << "Press 'l' to load settings" << endl
    << "Totoal points: " << points.size() << endl
    << "Current video frame: " << vidRGB.getCurrentFrame() << "/" << vidRGB.getTotalNumFrames() << endl
    << "Fps: " << ofGetFrameRate();
    ofDrawBitmapString(reportStr.str(), gui.getPosition().x, gui.getPosition().y + gui.getHeight() + 20);
    
    if (bBWvideo) {
        ofDrawBitmapString("Based on BW video", 30, _height+40+230);
    }
    else {
        ofDrawBitmapString("Based on RGB video", 30, _height+40+230);

    }

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 's') {
        gui.saveToFile("settings.xml");
    }
    if(key == 'l') {
        gui.loadFromFile("settings.xml");
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
