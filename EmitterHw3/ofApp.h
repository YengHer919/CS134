#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Emitter.h"
#include "Shape.h"

class Agent : public Sprite {
public:
	Agent() {
		Sprite::Sprite();
//		cout << "in Agent Constuctor" << endl;
	}
};

class AgentEmitter : public Emitter {
public:
	void AgentEmitter::spawnSprite() {
//		cout << "in AgentEmitter::spawnSprite" << endl;
		Agent sprite;
		if (haveChildImage) sprite.setImage(childImage);
		sprite.velocity = velocity;
		sprite.lifespan = lifespan;
		sprite.pos = pos;
		sprite.birthtime = ofGetElapsedTimeMillis();
		sys->add(sprite);
	}
	void moveSprite(Sprite *sprite) {
		Emitter::moveSprite(sprite);

		// Align path of travel to velocity 
		//
		glm::vec3 v = glm::normalize(sprite->velocity);
		glm::vec3 u = glm::vec3(0, -1, 0);
		float a = glm::orientedAngle(u, v, glm::vec3(0, 0, 1));
		sprite->rot =  glm::degrees(a);
	}
};


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void moveForward();
		void moveBackward();
		glm::vec3 heading(Emitter* emitter);
		bool bDrag = false;
		glm::vec3 delta;
		glm::vec3 delta1;
		glm::vec3 delta2;

		Emitter  *emitter = NULL;
		Emitter  *emitter1 = NULL;
		Emitter  *emitter2 = NULL;

		ofImage defaultImage;
		ofVec3f mouse_last;
		bool imageLoaded;
		bool up;
		bool down;
		bool left;
		bool right;
		// Some basic UI
		//
		bool bHide;
		ofxFloatSlider rate;
		ofxFloatSlider life;
		ofxFloatSlider velocity;
		ofxLabel screenSize;
		ofxFloatSlider scale;
		ofxFloatSlider rotationSpeed = 3.0;


		ofxPanel gui;
};
