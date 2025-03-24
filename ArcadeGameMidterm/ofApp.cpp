#include "ofApp.h"
#include <stdio.h>
#include <math.h>

//--------------------------------------------------------------
void ofApp::setup() {
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
	if (!gameOver) {
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

		uint64_t currentTime = ofGetElapsedTimeMillis();

		// Warning pillar spawning logic
		if (currentTime - lastPillarSpawnTime > nextPillarSpawnTime) {
			spawnPillar();
			lastPillarSpawnTime = currentTime;
		}

		if (currentTime >= nextPillarSpawnTime && nextPillarSpawnTime > 0) {
			// Convert all ready warning pillars to real pillars
			for (int i = warningPillars.size() - 1; i >= 0; i--) {
				if (warningPillars[i].isWarningComplete()) {
					// Create a real pillar at the same position
					Pillar p(warningPillars[i].storedOrientation);
					p.pos = warningPillars[i].pos;
					p.lifespan = fmin(10000, p.lifespan + difficulty);
					pillars.push_back(p);
					// Remove the warning pillar
					warningPillars.erase(warningPillars.begin() + i);
				}
			}
		}

		// Remove any expired warning pillars
		for (int i = warningPillars.size() - 1; i >= 0; i--) {
			if (warningPillars[i].isWarningComplete()) {
				warningPillars.erase(warningPillars.begin() + i);
			}
		}

		for (int i = pillars.size() - 1; i >= 0; i--) {
			if (!pillars[i].isAlive()) {
				pillars.erase(pillars.begin() + i);
			}
		}

		// Check for collisions between emitter and pillars
		for (int i = 0; i < pillars.size(); i++) {
			bool collision = false;
			float emitterRadius = 10;

			if (pillars[i].orientation == Pillar::VERTICAL) {
				// For vertical pillars, check x distance
				float xDist = abs(emitter->pos.x - pillars[i].pos.x);
				float yDist = abs(emitter->pos.y - pillars[i].pos.y);

				collision = (xDist < (pillars[i].width / 2 + emitterRadius)) &&
					(yDist < (pillars[i].height / 2 + emitterRadius));
			}
			else {
				// For horizontal pillars, check y distance
				float xDist = abs(emitter->pos.x - pillars[i].pos.x);
				float yDist = abs(emitter->pos.y - pillars[i].pos.y);

				collision = (xDist < (pillars[i].width / 2 + emitterRadius)) &&
					(yDist < (pillars[i].height / 2 + emitterRadius));
			}

			if (collision) {
				if (currentTime - hit > 2000) {
					if (emitter->color == ofColor::blue) {
						emitter->color = ofColor::yellow;
					}
					else if (emitter->color == ofColor::yellow) {
						emitter->color = ofColor::red;
					}
					else if (emitter->color == ofColor::red) {
						cout << "==========================================" << endl;
						cout << "                GAME OVER!                " << endl;
						cout << "==========================================" << endl;
						cout << "  Press 'c' to continue or 'r' to restart!" << endl;
						cout << "==========================================" << endl;
						gameOver = true;
					}
					hit = currentTime;
				}
			}
		}
	}
}


//--------------------------------------------------------------
void ofApp::draw() {
	emitter->draw();
	// Draw emitter
	emitter->draw();

	// Draw pillars
	for (int i = 0; i < warningPillars.size(); i++) {
		warningPillars[i].draw();
	}
	for (int i = 0; i < pillars.size(); i++) {
		pillars[i].draw();
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

void Emitter::integrate() {
	float fr = 1.0 / 60.0;
	pos += speed * fr;
	speed += acceleration * fr;
	speed *= damping;
}

glm::vec3 ofApp::heading(Emitter* emitters) {
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
		if (gameOver) {
			gameOver = false;
			emitter->color = ofColor::blue;
		}
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		break;
	case 'r':
		if (gameOver) {
			gameOver = false;
			emitter->color = ofColor::blue;
			emitter->pos = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0);
			emitter->speed = glm::vec3(0);
			emitter->acceleration = glm::vec3(0);
			pillars.clear();
			warningPillars.clear();
			difficulty = 0;
		}
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
		emitter->setRate(0);
		break;
	}
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

void ofApp::spawnPillar() {
	// Randomly decide if this pillar will be vertical or horizontal
	bool isVertical = (ofRandom(2) < 1);
	Pillar::Orientation orientation = isVertical ? Pillar::VERTICAL : Pillar::HORIZONTAL;

	// Create a warning pillar first
	WarningPillar wp(orientation);

	float screenWidth = ofGetWidth();
	float screenHeight = ofGetHeight();

	if (isVertical) {
		// Vertical pillar: random x position, centered vertically
		float randomX = ofRandom(screenWidth);
		wp.pos = glm::vec3(randomX, screenHeight / 2, 0);
	}
	else {
		// Horizontal pillar: random y position, centered horizontally
		float randomY = ofRandom(screenHeight);
		wp.pos = glm::vec3(screenWidth / 2, randomY, 0);
	}

	// Schedule the actual pillar to spawn after the warning
	wp.warningTime = fmax(1000, wp.warningTime - difficulty);

	if (wp.warningTime == 1000) {
		nextPillarSpawnTime = fmax(2000, 2000 + wp.warningTime - difficulty);
	}else{
		nextPillarSpawnTime = 2000 + wp.warningTime;
	}
	warningPillars.push_back(wp);

	difficulty += fmin(1000, difficulty + 600);
}
