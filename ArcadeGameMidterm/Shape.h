#pragma once

#include "ofMain.h"
#include "Particle.h"

// Basic Shape class supporting matrix transformations and drawing.
// 
//

class Shape {
public:
	Shape() {


	}
	virtual void draw() {

		// draw a box by defaultd if not overridden
		//
		ofPushMatrix();
		ofMultMatrix(getTransform());
		ofDrawBox(defaultSize);
		ofPopMatrix();
	}

	virtual bool inside(glm::vec3 p) {
		return false;
	}

	glm::mat4 getTransform() {
		glm::mat4 T = glm::translate(glm::mat4(1.0), glm::vec3(pos));
		glm::mat4 R = glm::rotate(glm::mat4(1.0), glm::radians(rot), glm::vec3(0, 0, 1));
		glm::mat4 S = glm::scale(glm::mat4(1.0), scale);      // added this scale if you want to change size of object
		return T * R * S;
	}
	glm::vec3 pos;
	float rot = 0.0;    // degrees 
	glm::vec3 scale = glm::vec3(1, 1, 1);
	float defaultSize = 20.0;



};

class Pillar : public Shape {
public:
    vector<Particle> particles;
    int health = 5;
    bool hasSplit = false;
    enum Orientation { VERTICAL, HORIZONTAL };
    vector<ofVec2f> points; // Store the random points for the shape

    Pillar(Orientation orient = VERTICAL) {
        orientation = orient;
        width = 30;
        height = 30;

        // Set dimensions based on orientation
        if (orientation == VERTICAL) {
            height = ofGetHeight(); // Full screen height for vertical pillars
        }
        else {
            width = ofGetWidth(); // Full screen width for horizontal pillars
        }

        lifespan = 5000;
        birthtime = ofGetElapsedTimeMillis();
        color = ofColor::red;

        // Generate the random polygon points once during initialization
        generateRandomShape();
    }

    // Generate a random shape similar to a rectangle but with random points
    void generateRandomShape() {
        const int numPoints = 10;
        points.clear(); // Clear any existing points

        for (int i = 0; i < numPoints; i++) {
            float t = ofMap(i, 0, numPoints, 0, TWO_PI);
            float baseX = width / 2 * cos(t);
            float baseY = height / 2 * sin(t);

            float randX = ofRandom(-width * 0.2, width * 0.2);
            float randY = ofRandom(-height * 0.2, height * 0.2);

            if (orientation == VERTICAL) {
                randX = ofClamp(randX, -width * 0.1, width * 0.1);
                if (hasSplit) {
                    randY = ofClamp(baseY, -width * 0.1, width * 0.1);
                }
            }
            else {
                randY = ofClamp(randY, -height * 0.1, height * 0.1);
                if (hasSplit) {
                    randY = ofClamp(baseX, -width * 0.1, width * 0.1);
                }
            }

            points.push_back(ofVec2f(baseX + randX, baseY + randY));
        }
    }

    void draw() {
        ofSetColor(color);
        ofPushMatrix();
        ofMultMatrix(getTransform());

        // Draw the random polygon shape instead of a rectangle
        ofBeginShape();
        for (const auto& point : points) {
            ofVertex(point.x, point.y);
        }
        ofEndShape(true); // true to close the shape

        ofPopMatrix();
    }

    bool isAlive() {
        return (lifespan > 0) && (ofGetElapsedTimeMillis() - birthtime < lifespan);
    }

    void spawn(float time) {
        Particle particle;
        ofVec3f dir = ofVec3f(ofRandom(-1, 1), ofRandom(-1, 1), 0);
        float speed = 100;
        particle.velocity = dir.getNormalized() * speed;
        particle.position.set(pos);
        particle.birthtime = time;
        particles.push_back(particle);
    }

    vector<Particle> getParticles() {
        return particles;
    }

    float width, height;
    float lifespan;
    uint64_t birthtime;
    ofColor color;
    Orientation orientation;
};

// First, update the WarningPillar class in your ofApp.h file
class WarningPillar : public Pillar {
public:
	// Single constructor with orientation parameter
	WarningPillar(Pillar::Orientation orient) : Pillar(orient) {
		// Apply different scaling based on orientation
		if (orient == Pillar::VERTICAL) {
			width = width * 0.25;
		}
		else { // HORIZONTAL
			height = height * 0.25;
		}
		color = ofColor::yellow;
		birthtime = ofGetElapsedTimeMillis();
		storedOrientation = orient;
	}
	bool isWarningComplete() {
		return (ofGetElapsedTimeMillis() - birthtime > warningTime);
	}

	Pillar::Orientation storedOrientation;
	float warningTime = 2000;
};


vector<Pillar> pillars;
vector<WarningPillar> warningPillars;