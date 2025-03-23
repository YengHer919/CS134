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
		sprite.velocity = glm::normalize(velocity) * 300.0f;
		sprite.lifespan = lifespan;
		sprite.pos = pos;
		sprite.birthtime = ofGetElapsedTimeMillis();
		sys->add(sprite);
	}
	void moveSprite(Sprite *sprite) {
		Emitter::moveSprite(sprite);
		glm::vec3 v = glm::normalize(sprite->velocity);
		glm::vec3 u = glm::vec3(0, -1, 0);
		float a = glm::orientedAngle(u, v, glm::vec3(0, 0, 1));
		sprite->rot =  glm::degrees(a);
	}
};

class Pillar : public Shape {
public:
	Pillar() {
		width = 30;
		height = 100;
		lifespan = 5000; // 5 seconds in milliseconds
		birthtime = ofGetElapsedTimeMillis();
		color = ofColor::red;
	}

	void draw() {
		ofPushMatrix();
		ofMultMatrix(getTransform());
		ofSetColor(color);
		ofDrawRectangle(-width / 2, -height / 2, width, height);
		ofPopMatrix();
	}

	bool isAlive() {
		return (ofGetElapsedTimeMillis() - birthtime < lifespan);
	}

	float width, height;
	float lifespan;
	uint64_t birthtime;
	ofColor color;
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
		void spawnPillar();
		glm::vec3 heading(Emitter* emitter);

		Emitter  *emitter = NULL;
		
		ofVec3f mouse_last;
		bool imageLoaded;
		bool up;
		bool down;
		bool left;
		bool right;
		float rate = 5;
		float life = 5;
		float velocity = 100;
		float scale = 0.3;
		float rotationSpeed = 3.0;
		vector<Pillar> pillars;
		float pillarSpawnTime = 2000; // milliseconds between pillar spawns
		uint64_t lastPillarSpawnTime = 0;
};