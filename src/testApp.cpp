//Created by Group: Jiteng Liu, Hongjia Zhang, Rui Li

#include "testApp.h"

/* Note on OS X, you must have this in the Run Script Build Phase of your project. 
where the first path ../../../addons/ofxLeapMotion/ is the path to the ofxLeapMotion addon. 

cp -f ../../../addons/ofxLeapMotion/libs/lib/osx/libLeap.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/libLeap.dylib"; install_name_tool -change ./libLeap.dylib @executable_path/libLeap.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/$PRODUCT_NAME";

   If you don't have this you'll see an error in the console: dyld: Library not loaded: @loader_path/libLeap.dylib
*/

//--------------------------------------------------------------
void testApp::setup(){

    ofSetFrameRate(60); //this is the frame rate
    ofSetVerticalSync(true); //syncs the refresh-rate
	ofSetLogLevel(OF_LOG_VERBOSE); //Sets the logging level so only messages above a certain priority are shown

	leap.open(); 

    //set the two lights position:
	l1.setPosition(200, 300, 50);
	l2.setPosition(-200, -200, 50);

	cam.setOrientation(ofPoint(-20, 0, 0));//this is the camera position

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    ball.loadImage("ball.png"); //the green ball that represents hand
}


//--------------------------------------------------------------
void testApp::update(){

	fingersFound.clear();
	

	//here is a simple example of getting the hands and using them to draw trails from the fingertips. 
	//the leap data is delivered in a threaded callback - so it can be easier to work with this copied hand data
	
	//if instead you want to get the data as it comes in then you can inherit ofxLeapMotion and implement the onFrame method. 
	//there you can work with the frame data directly. 



    //Option 1: Use the simple ofxLeapMotionSimpleHand - this gives you quick access to fingers and palms. 
    
    simpleHands = leap.getSimpleHands();
    
    if( leap.isFrameNew() && simpleHands.size() ){
    
        //mapping the ordinates of the hands by x,y,z three dimentional:
        leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
		leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        leap.setMappingZ(-150, 150, -200, 200);
        
        
        for(int i = 0; i < simpleHands.size(); i++){
        
            for(int j = 0; j < simpleHands[i].fingers.size(); j++){
                int id = simpleHands[i].fingers[j].id;
            
                //draw the polyline that the fingures created:
                ofPolyline & polyline = fingerTrails[id]; 
                ofPoint pt = simpleHands[i].fingers[j].pos;
                
                //if the distance between the last point and the current point is too big - lets clear the line 
                //this stops us connecting to an old drawing
                if( polyline.size() && (pt-polyline[polyline.size()-1] ).length() > 200){
                    polyline.clear(); 
                }
                
                //add our point to our trail
                polyline.addVertex(pt); 
                
                //store fingers seen this frame for drawing
                fingersFound.push_back(id);
            }
        }
    }
//

//     Option 2: Work with the leap data / sdk directly - gives you access to more properties than the simple approach  
//     uncomment code below and comment the code above to use this approach. You can also inhereit ofxLeapMotion and get the data directly via the onFrame callback. 
//    
//	vector <Hand> hands = leap.getLeapHands();
//	if( leap.isFrameNew() && hands.size() ){
//
//		//leap returns data in mm - lets set a mapping to our world space. 
//		//you can get back a mapped point by using ofxLeapMotion::getMappedofPoint with the Leap::Vector that tipPosition returns  
//		leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
//		leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
//		leap.setMappingZ(-150, 150, -200, 200);
//				
//		for(int i = 0; i < hands.size(); i++){
//            for(int j = 0; j < hands[i].fingers().count(); j++){
//				ofPoint pt; 
//			
//				const Finger & finger = hands[i].fingers()[j];
//								
//				//here we convert the Leap point to an ofPoint - with mapping of coordinates
//				//if you just want the raw point - use ofxLeapMotion::getofPoint 
//				pt = leap.getMappedofPoint( finger.tipPosition() );
//                								
//				//lets get the correct trail (ofPolyline) out of our map - using the finger id as the key 
//				ofPolyline & polyline = fingerTrails[finger.id()]; 
//				
//				//if the distance between the last point and the current point is too big - lets clear the line 
//				//this stops us connecting to an old drawing
//				if( polyline.size() && (pt-polyline[polyline.size()-1] ).length() > 50 ){
//					polyline.clear(); 
//				}
//				
//				//add our point to our trail
//				polyline.addVertex(pt); 
//				
//				//store fingers seen this frame for drawing
//				fingersFound.push_back(finger.id());
//			}
//		}	
//	}
//    

	//IMPORTANT! - tell ofxLeapMotion that the frame is no longer new.
	leap.markFrameAsOld();
    pointLight.setPosition((ofGetWidth()*.5)+ cos(ofGetElapsedTimef()*.5)*(ofGetWidth()*.3), ofGetHeight()/2, 500);
    pointLight2.setPosition((ofGetWidth()*.5)+ cos(ofGetElapsedTimef()*.15)*(ofGetWidth()*.3),
                            ofGetHeight()*.5 + sin(ofGetElapsedTimef()*.7)*(ofGetHeight()), -300);
    
    pointLight3.setPosition(
                            cos(ofGetElapsedTimef()*1.5) * ofGetWidth()*.5,
                            sin(ofGetElapsedTimef()*1.5f) * ofGetWidth()*.5,
                            cos(ofGetElapsedTimef()*.2) * ofGetWidth()
                            );
    
}

//--------------------------------------------------------------
void testApp::draw(){
	//ofDisableLighting();
    ofBackgroundGradient(ofColor(90, 90, 90), ofColor(30, 30, 30),  OF_GRADIENT_BAR);

	ofSetColor(200);
	ofDrawBitmapString("ofxLeapMotion - Example App\nLeap Connected? " + ofToString(leap.isConnected()), 20, 20);

	cam.begin();	
 
	
	ofEnableLighting();
//	l1.enable();
//	l2.enable();
    pointLight.enable();
    pointLight2.enable();
    pointLight3.enable();
    
    //ofMaterial: Anything graphical applied to the polygons
	m1.begin();
	m1.setShininess(1);
    
    //drawing the hand trails:
    for (int i =0 ; i<simpleHands.size(); i++) {
        ofPoint hand = simpleHands[i].handPos;
        ofPushMatrix();
        ofFill();
        ofSetColor(ofColor::springGreen);
        sphere.setPosition(hand);
        ofScale(3, 3);
        sphere.draw();
        ofPopMatrix();
        //ofSetColor(216, 117, 50, 255);
       // ball.draw(hand);
    }
    
	for(int i = 0; i < fingersFound.size(); i++){
		ofxStrip strip;
		int id = fingersFound[i];
		
		ofPolyline & polyline = fingerTrails[id];
        polyline.getSmoothed(10);
		strip.generate(polyline.getVertices(),
                       20, ofPoint(100, 100, 10) );
        //ofSetColor(255, 153, 153, 150);
		ofSetColor(255 - i * 20, i*10, i * 30);
		strip.getMesh().draw();
	}
	
//    l2.disable();
    
//    for(int i = 0; i < simpleHands.size(); i++){
//        simpleHands[i].debugDraw();
//    }

	m1.end();
	cam.end();
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
  
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
    
}

//--------------------------------------------------------------
void testApp::exit(){
    // let's close down Leap and kill the controller
    leap.close();
}
