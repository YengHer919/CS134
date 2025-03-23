#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup(){
	ofSetBackgroundColor(ofColor::black);
	// create an array of emitters and set their position;
	//
	emitter = new AgentEmitter();  // C++ polymorphism
	emitter->pos = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0);
	emitter->drawable = true; 
	emitter->setRate(0);
	emitter->setLifespan(life * 1000);    // convert to milliseconds 
	emitter->start();

}

//--------------------------------------------------------------
void ofApp::update() {
	glm::vec3 headingVec = heading(emitter) * glm::vec3(velocity);
	emitter->setVelocity(headingVec);
	emitter->update();
	emitter->integrate();

	glm::vec3 pos = emitter->pos;
	float screenWidth = ofGetWidth();
	float screenHeight = ofGetHeight();

	// Check X position
	if (pos.x < 0) {
		emitter->pos = glm::vec3(screenWidth, pos.y, pos.z);
	}
	else if (pos.x > screenWidth) {
		emitter->pos = glm::vec3(0, pos.y, pos.z);
	}

	// Check Y position
	if (pos.y < 0) {
		emitter->pos = glm::vec3(pos.x, screenHeight, pos.z);
	}
	else if (pos.y > screenHeight) {
		emitter->pos = glm::vec3(pos.x, 0, pos.z);
	}

	for (int i = 0; i < emitter->sys->sprites.size(); i++) {
		emitter->sys->sprites[i].scale = glm::vec3(scale, scale, scale);
		emitter->sys->sprites[i].rotationSpeed = rotationSpeed;
	}


	if (emitter->acceleration != glm::vec3(0)) {
		if (emitter->acceleration.x > 0.003) emitter->acceleration.x *= 0.8;
		else if (emitter->velocity.x > 0.003) emitter->acceleration.x *= 0.8;
		if (emitter->acceleration.y > 0.003) emitter->acceleration.y *= 0.8;
		else if (emitter->velocity.y > 0.003) emitter->acceleration.y *= 0.8;
		if (emitter->acceleration.z > 0.003) emitter->acceleration.z *= 0.8;
		else if (emitter->velocity.z > 0.003) emitter->acceleration.z *= 0.8;
	}

	if (up) {
		moveForward();
	}
	if (down) {
		moveBackward();
	}
	if (left) {
		emitter->rot -= 5;
	}
	if (right) {
		emitter->rot += 5;
	}
}


//--------------------------------------------------------------
void ofApp::draw(){
	emitter->draw();
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
//	cout << "mouse( " << x << "," << y << ")" << endl;
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

void Emitter::integrate() {
	float fr = 1.0 / 60.0;
	pos += speed * fr;
	speed += acceleration * fr;
	speed *= damping;
}

glm::vec3 ofApp::heading(Emitter *emitters) {
	glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(emitters->rot),
		glm::vec3(0, 0, 1));
	return glm::normalize(rot * glm::vec4(glm::vec3(0, -1, 0), 1));
}

void ofApp::moveForward() {
	glm::vec3 thrustVector = heading(emitter) * 10.0f;
	emitter->acceleration += thrustVector;
	if (glm::length(emitter->acceleration) > 50.0f) {
		emitter->acceleration = glm::normalize(emitter->acceleration) * 50.0f;
	}
	emitter->speed += thrustVector;
	if (glm::length(emitter->speed) > 200.0f) {
		emitter->speed = glm::normalize(emitter->speed) * 200.0f;
	}
}

void ofApp::moveBackward() {
	glm::vec3 thrustVector = heading(emitter) * 10.0f;
	emitter->acceleration -= thrustVector;
	if (glm::length(emitter->acceleration) > 50.0f) {
		emitter->acceleration = glm::normalize(emitter->acceleration) * 50.0f;
	}
	emitter->speed -= thrustVector;
	if (glm::length(emitter->speed) > 200.0f) {
		emitter->speed = glm::normalize(emitter->speed) * 200.0f;
	}
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
	case ' ':
		emitter->setRate(rate);;
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
	case ' ':
		emitter->setRate(0 );
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

void ofApp::spawnPillar() {
	Pillar p;

	// Set random position at the edge of the screen
	int side = ofRandom(4); // 0: top, 1: right, 2: bottom, 3: left
	float screenWidth = ofGetWidth();
	float screenHeight = ofGetHeight();

	switch (side) {
	case 0: // top
		p.pos = glm::vec3(ofRandom(screenWidth), 0, 0);
		break;
	case 1: // right
		p.pos = glm::vec3(screenWidth, ofRandom(screenHeight), 0);
		break;
	case 2: // bottom
		p.pos = glm::vec3(ofRandom(screenWidth), screenHeight, 0);
		break;
	case 3: // left
		p.pos = glm::vec3(0, ofRandom(screenHeight), 0);
		break;
	}

	pillars.push_back(p);
}
