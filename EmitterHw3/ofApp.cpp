#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup(){
	ofImage image;
	if (!image.load("C:\\Users\\herye\\Downloads\\of_v0.12.0_vs_release\\apps\\myApps\\Hw3Emmitter\\bin\\data\\ship.png")) {
		cout << "Can't open image file" << endl;
	}
	image.resize(image.getWidth() / 10, image.getHeight() / 10);
	ofSetVerticalSync(true);

	// create an image for sprites being spawned by emitter
	//
	if (defaultImage.load("C:\\Users\\herye\\Downloads\\of_v0.12.0_vs_release\\apps\\myApps\\Hw3Emmitter\\bin\\data\\poop.png")) {
		imageLoaded = true;
		defaultImage.resize(defaultImage.getWidth() / 3, defaultImage.getHeight()/3);
	}
	else {
		cout << "Can't open image file" << endl;
//		ofExit();
	}

	ofSetBackgroundColor(ofColor::black);

	

	// create an array of emitters and set their position;
	//
	

	emitter = new AgentEmitter();  // C++ polymorphism
	emitter->pos = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0);
	emitter->drawable = true; 
	if (imageLoaded) emitter->setChildImage(defaultImage);
	emitter->setImage(image);
	emitter->start();

	emitter1 = new AgentEmitter();  // C++ polymorphism
	emitter1->pos = emitter->pos + glm::vec2(-60,-60);
	emitter1->drawable = true;
	if (imageLoaded) emitter1->setChildImage(defaultImage);
	emitter1->setImage(image);
	emitter1->start();


	emitter2 = new AgentEmitter();  // C++ polymorphism
	emitter2->pos = emitter->pos + glm::vec2(60, -60);
	emitter2->drawable = true;
	if (imageLoaded) emitter2->setChildImage(defaultImage);
	emitter2->setImage(image);
	emitter2->start();



	gui.setup();
	gui.add(rate.setup("rate", 1, 1, 10));
	gui.add(life.setup("life", 5, .1, 10));
	gui.add(velocity.setup("velocity", 200, -1000, 1000));
	gui.add(scale.setup("Scale", .1, .05, 1.0));
	gui.add(rotationSpeed.setup("Rotation Speed (deg/Frame)", 0, 0, 10));

	bHide = false;

}

//--------------------------------------------------------------
void ofApp::update() {
	
	glm::vec3 headingVec = heading(emitter) * glm::vec3(velocity);
	emitter->setVelocity(headingVec);
	emitter1->setVelocity(headingVec);
	emitter2->setVelocity(headingVec);
	

	emitter->setRate(rate);
	emitter->setLifespan(life * 1000);    // convert to milliseconds 
	emitter->update();
	emitter1->setRate(rate);
	emitter1->setLifespan(life * 1000);    // convert to milliseconds 
	emitter1->update();
	emitter2->setRate(rate);
	emitter2->setLifespan(life * 1000);    // convert to milliseconds 
	emitter2->update();

	for (int i = 0; i < emitter->sys->sprites.size(); i++) {

		// Get values from sliders and update sprites dynamically
		//
		float sc = scale;
		float rs = rotationSpeed;
		emitter->sys->sprites[i].scale = glm::vec3(sc, sc, sc);
		emitter->sys->sprites[i].rotationSpeed = rs;
	}
	for (int i = 0; i < emitter1->sys->sprites.size(); i++) {

		// Get values from sliders and update sprites dynamically
		//
		float sc = scale;
		float rs = rotationSpeed;
		emitter1->sys->sprites[i].scale = glm::vec3(sc, sc, sc);
		emitter1->sys->sprites[i].rotationSpeed = rs;
	}
	for (int i = 0; i < emitter2->sys->sprites.size(); i++) {

		// Get values from sliders and update sprites dynamically
		//
		float sc = scale;
		float rs = rotationSpeed;
		emitter2->sys->sprites[i].scale = glm::vec3(sc, sc, sc);
		emitter2->sys->sprites[i].rotationSpeed = rs;
	}

	if (up) {
		moveForward();
	}
	if (down) {
		moveBackward();
	}
	if (left) {
		emitter->rot -= 5;
		emitter1->rot -= 5;
		emitter2->rot -= 5;
	}
	if (right) {
		emitter->rot += 5;
		emitter1->rot += 5;
		emitter2->rot += 5;
	}
}


//--------------------------------------------------------------
void ofApp::draw(){
	emitter->draw();
	emitter1->draw();
	emitter2->draw();
	if (!bHide) {
		gui.draw();
	}
}


//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
//	cout << "mouse( " << x << "," << y << ")" << endl;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if (bDrag) {
		emitter->pos = glm::vec3(x, y, 0.0) + delta;
		emitter1->pos = glm::vec3(x, y, 0.0) + delta1;
		emitter2->pos = glm::vec3(x, y, 0.0) + delta2;
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	glm::vec3 pos = glm::vec3(x, y, 0);
	if (emitter->inside(pos)) {
		delta = emitter->pos - glm::vec3(x, y, 0.0);
		delta1 = emitter1->pos - glm::vec3(x, y, 0.0);
		delta2 = emitter2->pos - glm::vec3(x, y, 0.0);
		cout << "inside sprite" << endl;
		bDrag = true;
	}
	else if (emitter1->inside(pos)) {
		delta = emitter->pos - glm::vec3(x, y, 0.0);
		delta1 = emitter1->pos - glm::vec3(x, y, 0.0);
		delta2 = emitter2->pos - glm::vec3(x, y, 0.0);
		cout << "inside sprite" << endl;
		bDrag = true;
	}
	else if (emitter2->inside(pos)) {
		delta = emitter->pos - glm::vec3(x, y, 0.0);
		delta1 = emitter1->pos - glm::vec3(x, y, 0.0);
		delta2 = emitter2->pos - glm::vec3(x, y, 0.0);
		cout << "inside sprite" << endl;
		bDrag = true;
	}
	else {
		cout << "outside" << endl;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	bDrag = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

glm::vec3 ofApp::heading(Emitter *emitters) {
	glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(emitters->rot -30),
		glm::vec3(0, 0, 1));
	return glm::normalize(rot * glm::vec4(glm::vec3(0, -1, 0), 1));
}

void ofApp::moveForward() {
	// use: r(t) = o + dt;
	//
	emitter->pos = emitter->pos + heading(emitter) * 10.0;
	emitter1->pos = emitter1->pos + heading(emitter1) * 10.0;
	emitter2->pos = emitter2->pos + heading(emitter2) * 10.0;
}
void ofApp::moveBackward() {
	// use: r(t) = o + dt;
	//
	emitter->pos = emitter->pos - heading(emitter) * 10.0;
	emitter1->pos = emitter1->pos - heading(emitter1) * 10.0;
	emitter2->pos = emitter2->pos - heading(emitter2) * 10.0;
}

void ofApp::keyPressed(int key) {
	switch (key) {
	case 'C':
	case 'c':
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		bHide = !bHide;
		break;
	case 'r':
		break;
	case 's':
		break;
	case 'u':
		break;
	case OF_KEY_ALT:
		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	case OF_KEY_UP:
		up = true;
		break;
	case OF_KEY_LEFT:
		left = true;
		break;
	case OF_KEY_RIGHT:
		right = true;
		break;
	case OF_KEY_DOWN:
		down = true;
		break;
	}
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
	case OF_KEY_UP:
		up = false;
		break;
	case OF_KEY_LEFT:
		left = false;
		break;
	case OF_KEY_RIGHT:
		right = false;
		break;
	case OF_KEY_DOWN:
		down = false;
		break;
	case OF_KEY_ALT:
		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_SHIFT:
		break;
	}
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

