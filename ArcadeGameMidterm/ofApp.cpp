#include "ofApp.h"
#include <stdio.h>
#include <math.h>

//--------------------------------------------------------------
void ofApp::setup() {
	// create an array of emitters and set their position;
	//
	emitter = new AgentEmitter();  // C++ polymorphism
	emitter->pos = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0);
	emitter->drawable = true;
	emitter->setRate(0);
	emitter->setLifespan(life * 1000);    // convert to milliseconds 
	emitter->start();
	playerhit.load("hit_sound.wav");   // Sound for player getting hit
	collide.load("collide_sound.wav");  // Sound for bullet collision
	thrust.load("Thrust.wav");
	image.load("C:\\Users\\herye\\Downloads\\of_v0.12.0_vs_release\\apps\\myApps\\MidtermArcadeGame\\bin\\data\\background.jpg");
}

//--------------------------------------------------------------

void ofApp::update() {
	if (!gameOver) {
		glm::vec3 headingVec = heading(emitter) * glm::vec3(velocity);
		emitter->setVelocity(headingVec);
		emitter->update();
		emitter->integrate();
		emitter->rotate();

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


		if (emitter->acceleration != glm::vec3(0) && !(up || down)) {
			if (emitter->acceleration.x > 0.003) emitter->acceleration.x *= 0.8;
			else if (emitter->velocity.x > 0.003) emitter->acceleration.x *= 0.8;
			if (emitter->acceleration.y > 0.003) emitter->acceleration.y *= 0.8;
			else if (emitter->velocity.y > 0.003) emitter->acceleration.y *= 0.8;
			if (emitter->acceleration.z > 0.003) emitter->acceleration.z *= 0.8;
			else if (emitter->velocity.z > 0.003) emitter->acceleration.z *= 0.8;
		}
		if (!(right || left)) {
			emitter->rotSpeed *= 0.9;
			emitter->rotAcc *= 0.8;
			if (abs(emitter->rotSpeed) < 0.1) emitter->rotSpeed = 0;
			if (abs(emitter->rotAcc) < 0.1) emitter->rotAcc = 0;
		}

		if (up) {
			moveForward();
		}
		if (down) {
			moveBackward();
		}
		if (left) {
			emitter->rotSpeed -= 5;
			emitter->rotAcc -= 5;
		}
		if (right) {
			emitter->rotSpeed += 5;
			emitter->rotAcc += 5;
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

		// Check for collisions between emitter and pillars
		for (Pillar& currPillar : pillars) {
			if (!currPillar.hasSplit && currPillar.health <= 0) {
				float time = ofGetElapsedTimeMillis();
				for (int i = 0; i < 12; i++) {
					currPillar.spawn(time);
				}
				explosion = currPillar.getParticles();
				boom = true;
				currPillar.color = ofColor::black;
				splitPillar(currPillar);
			}
			if (currPillar.hasSplit && currPillar.health == 0) {
				score += 1;
				currPillar.color = ofColor::black;
			}
			if (currPillar.color != ofColor::black) {
				bool collision = false;
				float xDist = abs(emitter->pos.x - currPillar.pos.x);
				float yDist = abs(emitter->pos.y - currPillar.pos.y);
				collision = (xDist < (currPillar.width / 2 + emitterRadius)) && (yDist < (currPillar.height / 2 + emitterRadius));
				if (collision) {
					if (currentTime - hit > 2000) {
						// Play hit sound when player collides
						playerhit.play();

						if (emitter->color == ofColor::blue) {
							emitter->color = ofColor::yellow;
						}
						else if (emitter->color == ofColor::yellow) {
							emitter->color = ofColor::red;
						}
						else if (emitter->color == ofColor::red) {
							gameOver = true;
							playerhit.stop();
							collide.stop();
						}
						hit = currentTime;
					}
				}

				for (int j = 0; j < emitter->sys->sprites.size(); j++) {
					Sprite* currentSprite = &emitter->sys->sprites[j];
					float xDist = abs(currentSprite->pos.x - currPillar.pos.x);
					float yDist = abs(currentSprite->pos.y - currPillar.pos.y);

					if ((xDist < (currPillar.width / 2 + spriteRadius)) && (yDist < (currPillar.height / 2 + spriteRadius))) {
						// Play collision sound when bullet hits a pillar
						collide.play();
						currentSprite->onCollision(&currPillar);
						bullethit = currentTime;
					}
				}
			}
		}
		if (playerhit.isPlaying() && (currentTime - hit >= 2000)) {
			playerhit.stop();
		}
		if (collide.isPlaying() && (currentTime - bullethit >= 500)) {
			collide.stop();
		}

		// Remove any expired warning pillars
		for (auto it = warningPillars.rbegin(); it != warningPillars.rend();) {
			if (it->isWarningComplete()) {
				it = decltype(it)(warningPillars.erase(std::next(it).base())); // Convert safely
			}
			else {
				++it;
			}
		}

		for (auto it = pillars.rbegin(); it != pillars.rend();) {
			if (!it->isAlive() || it->color == ofColor::black) {
				it = decltype(it)(pillars.erase(std::next(it).base())); // Convert to base iterator
			}
			else {
				++it;
			}
		}

		if (boom) {
			updateParticles();
			if (explosion.empty()) {
				boom = false;
			}
		}
	}
}


//--------------------------------------------------------------
void ofApp::draw() {
	ofSetColor(250, 250, 250);
	image.draw(0, 0, ofGetWidth(), ofGetHeight());
	for (int i = 0; i < explosion.size(); i++) {
		explosion[i].draw();
	}
	// Draw pillars
	for (int i = 0; i < warningPillars.size(); i++) {
		warningPillars[i].draw();
	}
	for (int i = 0; i < pillars.size(); i++) {
		pillars[i].draw();
	}

	ofSetColor(255, 255, 255); // White text
	ofDrawBitmapString("SCORE: " + ofToString(score), 20, 30);

	emitter->draw();

	if (gameOver) {
		ofSetColor(255, 255, 255); // White text
		// Game over message
		ofDrawBitmapString("GAME OVER!", (ofGetWidth() / 2) - 50, (ofGetHeight() / 2) - 20);
		ofDrawBitmapString("Final Score: " + ofToString(score), (ofGetWidth() / 2) - 75, (ofGetHeight() / 2));
		ofDrawBitmapString("Press 'c' to continue or 'r' to restart!", (ofGetWidth() / 2) - 150, (ofGetHeight() / 2) + 20);
		ofSetColor(255, 0, 0); // Red text

		// Optional decorative lines
		ofSetLineWidth(2);
		ofDrawLine((ofGetWidth() / 2) - 200, ofGetHeight() / 2 - 100, (ofGetWidth() / 2) + 200, ofGetHeight() / 2 - 100);
		ofDrawLine((ofGetWidth() / 2) - 200, ofGetHeight() / 2 + 100, (ofGetWidth() / 2) + 200, ofGetHeight() / 2 + 100);
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

void Emitter::rotate() {
	rot += rotSpeed * fr;
	rot += rotAcc * fr;
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
			score = 0;
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
		if (!gameOver) {
			thrust.play();
		}
		break;
	case OF_KEY_LEFT:
		left = true;
		break;
	case OF_KEY_RIGHT:
		right = true;
		break;
	case OF_KEY_DOWN:
		if (!gameOver) {
			thrust.play();
		}
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
		thrust.stop();
		break;
	case OF_KEY_LEFT:
		left = false;
		break;
	case OF_KEY_RIGHT:
		right = false;
		break;
	case OF_KEY_DOWN:
		thrust.stop();
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
	}
	else {
		nextPillarSpawnTime = 2000 + wp.warningTime;
	}
	warningPillars.push_back(wp);

	difficulty += fmin(1000, difficulty + 600);
}

void ofApp::splitPillar(Pillar& currPillar) {
	// Split vertically
	if (currPillar.orientation == Pillar::VERTICAL) {
		float newHeight = currPillar.height / 3.0f;
		float splitSpacing = newHeight * 0.5f;  // Space between split pillars

		for (int i = 0; i < 3; i++) {
			Pillar newPillar(Pillar::VERTICAL);

			// Reduce width slightly
			newPillar.width = 15;
			newPillar.height = newHeight;

			// Calculate new vertical position
			float verticalOffset = (i - 1) * (newHeight + splitSpacing);
			newPillar.pos = glm::vec3(
				currPillar.pos.x,
				currPillar.pos.y + verticalOffset,
				currPillar.pos.z
			);

			// Set health and lifespan
			newPillar.health = 2;
			newPillar.lifespan = 2500;  // Reduced lifespan
			newPillar.hasSplit = true;

			// Slight color variation
			newPillar.color = ofColor::orange;

			pillars.push_back(newPillar);
		}
	}
	// Split horizontally
	else {
		float newWidth = currPillar.width / 3.0f;
		float splitSpacing = newWidth * 0.5f;  // Space between split pillars

		for (int i = 0; i < 3; i++) {
			Pillar newPillar(Pillar::HORIZONTAL);

			// Reduce height slightly
			newPillar.height = 15;
			newPillar.width = newWidth;

			// Calculate new horizontal position
			float horizontalOffset = (i - 1) * (newWidth + splitSpacing);
			newPillar.pos = glm::vec3(
				currPillar.pos.x + horizontalOffset,
				currPillar.pos.y,
				currPillar.pos.z
			);

			// Set health and lifespan
			newPillar.health = 2;
			newPillar.lifespan = 2500;  // Reduced lifespan
			newPillar.hasSplit = true;

			// Slight color variation
			newPillar.color = ofColor::orange;

			pillars.push_back(newPillar);
		}
	}
	// Mark original pillar as split
	currPillar.hasSplit = true;
	currPillar.color = ofColor::black;
	currPillar.lifespan = 0;  // Immediately expire original pillar
}

void ofApp::updateParticles() {
	if (explosion.size() == 0) return;
	vector<Particle>::iterator p = explosion.begin();
	vector<Particle>::iterator tmp;
	while (p != explosion.end()) {
		if (p->lifespan != -1 && p->age() > p->lifespan) {
			tmp = explosion.erase(p);
			p = tmp;
		}
		else p++;
	}
	// integrate all the particles in the store
	//
	for (int i = 0; i < explosion.size(); i++) {
		explosion[i].integrate();
	}
}
