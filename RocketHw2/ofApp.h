#pragma once
#include "ofMain.h"
#include "ofxGui.h"

class Shape {
public:
	virtual void draw() {}
	virtual bool inside(glm::vec2 p) {
		return false;
	}
	glm::mat4 getTransform();
	// channels
	//
	glm::vec3 position = glm::vec3(100, 100, 0);
	glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0);
	float angle = 0.0; // degrees
	bool bFill = false;
};
class Circle : public Shape {
public:
	void draw();
	bool inside(glm::vec2 p) {
		glm::mat4 inv = glm::inverse(getTransform());
		glm::vec3 p2 = inv * glm::vec4(p, 0, 1);
		return (glm::length(p2 - glm::vec3(0, 0, 0)) < radius);
	}
	float radius = 50;
};
class ImageShape : public Shape {
public:
	void set(ofImage image) { this->image = image; }
	void draw();
	bool inside(glm::vec2 p);
	ofImage image;
};
class Triangle : public Shape {
public:
	void draw();
	bool inside(glm::vec2 p) {
		//Inverse of transform
		glm::mat4 inv = glm::inverse(getTransform());
		glm::vec3 p2 = inv * glm::vec4(p, 0, 1);

		// Compute vectors from p to each triangle vertex
		glm::vec2 v0 = glm::normalize(pos[0] - glm::vec2(p2.x, p2.y));
		glm::vec2 v1 = glm::normalize(pos[1] - glm::vec2(p2.x, p2.y));
		glm::vec2 v2 = glm::normalize(pos[2] - glm::vec2(p2.x, p2.y));

		// Compute angles between vectors
		float angle0 = glm::acos(glm::dot(v0, v1));
		float angle1 = glm::acos(glm::dot(v1, v2));
		float angle2 = glm::acos(glm::dot(v2, v0));

		float sumAngles = angle0 + angle1 + angle2;
		return glm::abs(sumAngles - glm::two_pi<float>()) < 0.01f;
	}
	float size = 50;
	glm::vec2 pos[3];
};
class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	glm::vec3 heading();
	void moveForward();
	void moveBackward();
	bool bDrag = false;
	glm::vec2 lastMousePos;
	Circle circle;
	Triangle triangle;
	ImageShape sprite;
	glm::vec3 delta;
	bool Heading = true;
	bool imageOn = true;

	ofxPanel gui;
	ofxButton header;
	ofxButton shape;
	ofxFloatSlider scale;
	void shapeButtonPressed();
	void headingButtonPressed();
};