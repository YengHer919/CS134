#pragma once

#include "Shape.h"
#include <math.h>


// Base class for a Sprite. Can be instantiated on it's own (deafult)
// or subclassed to create a custom sprite.  Includes Shape transformations.
// If the sprite has no image set on it, then a simple triangle will be drawn.
//

class Sprite : public Shape {
public:
	Sprite() {
		// default geometry (triangle) if no image attached.
		//
		verts.push_back(glm::vec3(-20, 30, 0));
		verts.push_back(glm::vec3(20, 30, 0));
		verts.push_back(glm::vec3(0, -30, 0));
	}
	
	// some functions for highlighting when selected
	//
	void draw() {
		ofSetColor(ofColor::yellow);
		ofPushMatrix();
		ofMultMatrix(getTransform());
		ofDrawTriangle(verts[0], verts[1], verts[2]);
		ofPopMatrix();
	}

	float age() {
		return (ofGetElapsedTimeMillis() - birthtime);
	}

	void setImage(ofImage img) {
		spriteImage = img;
		bShowImage = true;
		width = img.getWidth();
		height = img.getHeight();
	}

	virtual bool inside(const glm::vec3 p);
	virtual void update() {}

	bool insideTriangle(const glm::vec3 p);
	

	void setSelected(bool state) { bSelected = state; }
	void setHighlight(bool state) { bHighlight = state; }
	bool isSelected() { return bSelected; }
	bool isHighlight() { return bHighlight; }


	bool bHighlight = false;
	bool bSelected = false;
	bool bShowImage = false;

	glm::vec3 velocity = glm::vec3(0, 0, 0);
	float rotationSpeed = 0.0;
	float birthtime = 0; // elapsed time in ms
	float lifespan = -1;  //  time in ms
	string name =  "UnammedSprite";
	float width = 40;
	float height = 40;
	glm::vec3 acceleration;
	float damping = 0.99; 

	ofImage spriteImage;

	// default verts for polyline shape if no image on sprite
	//
	vector<glm::vec3> verts;

	virtual void onCollision(Pillar* other) {
		// Default collision behavior: destroy both sprites
		lifespan = 0;
		other->health -= 1;
	}
};

