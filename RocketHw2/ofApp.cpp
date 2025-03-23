#include "ofApp.h"
// getTransform - get shapes transform from channels
//
glm::mat4 Shape::getTransform() {
	glm::mat4 T = glm::translate(glm::mat4(1.0), position);
	glm::mat4 R = glm::rotate(glm::mat4(1.0), glm::radians(angle), glm::vec3(0,
		0, 1));
	glm::mat4 S = glm::scale(glm::mat4(1.0), scale);
	return (T * R * S);
}
void Circle::draw() {
	ofSetCircleResolution(50);
	if (bFill)
		ofFill();
	else ofNoFill();
	ofPushMatrix();
	ofMultMatrix(getTransform());
	ofDrawCircle(glm::vec2(0, 0), radius);
	ofDrawLine(glm::vec2(0.0), glm::vec2(0, radius));
	ofPopMatrix();
}

void Triangle::draw() {
	// Set the triangle vertices relative to origin (0,0)
	pos[0] = glm::vec2(0, -size);
	pos[1] = glm::vec2(-size, size);
	pos[2] = glm::vec2(size, size);

	// Apply transformation matrix
	ofPushMatrix();
	ofMultMatrix(getTransform());
	ofDrawTriangle(pos[0], pos[1], pos[2]);
	ofPopMatrix();
}
void ImageShape::draw() {
	ofPushMatrix();
	ofMultMatrix(getTransform());
	image.draw(-image.getWidth() / 2.0, -image.getHeight() / 2.0);
	ofPopMatrix();
}
bool ImageShape::inside(glm::vec2 p) {
	glm::mat4 inv = glm::inverse(getTransform());
	glm::vec3 p2 = inv * glm::vec4(p, 0, 1);
	cout << p2 << endl;
	// Get image width and height
	float imgWidth = image.getWidth()/2;
	float imgHeight = image.getHeight()/2;

	// Check if p2 is within the image bounds
	bool withinX = (p2.x >= -imgWidth && p2.x <= imgWidth);
	bool withinY = (p2.y >= -imgHeight && p2.y <= imgHeight);

	return withinX && withinY;
}
// Calculate heading vector by rotating "up" vector to current angle of shape
//
// Special note, the -30 is purely for astetics to match the image
glm::vec3 ofApp::heading() {
	if(imageOn){
		glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(sprite.angle - 30),
			glm::vec3(0, 0, 1));
		return glm::normalize(rot * glm::vec4(glm::vec3(0, -1, 0), 1));
	}
	else {
		glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(triangle.angle),
			glm::vec3(0, 0, 1));
		return glm::normalize(rot * glm::vec4(glm::vec3(0, -1, 0), 1));
	}
}
// Example of how to move 10 units along the heading
//
void ofApp::moveForward() {
	// use: r(t) = o + dt;
	//
	sprite.position = sprite.position + heading() * 10.0;
	triangle.position = triangle.position + heading() * 10.0;
}
void ofApp::moveBackward() {
	// use: r(t) = o + dt;
	//
	sprite.position = sprite.position - heading() * 10.0;
	triangle.position = triangle.position - heading() * 10.0;

}
//--------------------------------------------------------------
void ofApp::setup() {
	header.addListener(this, &ofApp::headingButtonPressed);
	shape.addListener(this, &ofApp::shapeButtonPressed);

	gui.setup();
	gui.add(header.setup("heading"));
	gui.add(shape.setup("shape"));
	gui.add(scale.setup("size", 5, 1, 10));
	
	ofSetBackgroundColor(ofColor::black);
	ofImage image;
	if (!image.load("C:\\Users\\herye\\Downloads\\of_v0.12.0_vs_release\\apps\\myApps\\Hw2_Rocket2_0\\bin\\data\\ship.png")) {
		cout << "can't load image" << endl;
		ofExit(0);
	}
	float scaleX = ofGetWidth() / 7.0f;
	float scaleY = ofGetHeight() / 7.0f;

	// Resize the image
	image.resize(scaleX, scaleY);

	sprite.set(image);
	sprite.position = glm::vec3(ofGetWidth() / 2.0 + 100, ofGetHeight() / 2.0,
		0);
	sprite.angle = 45;
	
}
//--------------------------------------------------------------
void ofApp::update() {
	// animate parameters over time
	//
	/*float s = 10.0 * abs(sin(ofGetFrameNum() / 100.0));
	circle.scale = glm::vec3(s, s, s);
	circle.angle += .5;*/
	// sprite.angle += 1.0;
	triangle.scale = glm::vec3(scale);
	sprite.scale = glm::vec3(scale);
}
//--------------------------------------------------------------
void ofApp::draw() {
	// circle.draw();
	if (imageOn) {
		sprite.draw();
	}else{
		ofSetColor(ofColor::white);
		triangle.draw();
	}
	// draw heading to test math
	// use: r(t) = o + dt
	ofSetColor(ofColor::lightBlue);
	float scaleValue = static_cast<float>(scale);
	if(Heading){
		ofDrawLine(sprite.position, sprite.position + heading() * 200.0 * scaleValue);
	}
	gui.draw();
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case OF_KEY_UP:
		moveForward();
		break;
	case 'f':
		ofToggleFullscreen();
		break;
	default:
		break;
	case OF_KEY_LEFT:
		sprite.angle -= 5; // Decrease rotation (rotate left)
		triangle.angle -= 5; // Decrease rotation (rotate left)
		break;
	case OF_KEY_RIGHT:
		sprite.angle += 5; // Increase rotation (rotate right)
		triangle.angle += 5; // Decrease rotation (rotate left)
		break;
	case OF_KEY_DOWN:
		moveBackward();
		break;
	}
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	if (bDrag) {
		sprite.position = glm::vec3(x, y, 0.0) + delta;
		triangle.position = glm::vec3(x, y, 0.0) + delta;
	}
}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	glm::vec2 pos = glm::vec2(x, y);
	/*if (circle.inside(pos)) {
	lastMousePos = pos;
	}*/
	if (sprite.inside(pos)){
		delta = sprite.position - glm::vec3(x, y, 0.0);
		cout << "inside sprite" << endl;
		bDrag = true;
	}
	else if (triangle.inside(pos)) {
		delta = triangle.position - glm::vec3(x, y, 0.0);
		cout << "inside triangle" << endl;
		bDrag = true;
	}
	else {
		cout << "outside" << endl;
	}
}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bDrag = false;
}
//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {
}
//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {
}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
}


void ofApp::shapeButtonPressed() {
	if(imageOn){
		triangle.position = sprite.position;
		triangle.scale = sprite.scale;
		triangle.angle = sprite.angle - 30;
	}else{
		sprite.position = triangle.position;
		sprite.scale = triangle.scale;
		sprite.angle = triangle.angle + 30;
	}	
	imageOn = !imageOn;
}

void ofApp::headingButtonPressed() {
	Heading = !Heading;
}