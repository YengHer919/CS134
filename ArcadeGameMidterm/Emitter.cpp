#include "ofApp.h"
//----------------------------------------------------------------------------------
//
// This example code demonstrates the use of an "Emitter" class to emit Sprites
// and set them in motion. The concept of an "Emitter" is taken from particle
// systems (which we will cover next week).
//
// The Sprite class has also been upgraded to include lifespan, velocity and age
// members.   The emitter can control rate of emission and the current velocity
// of the particles. In this example, there is no acceleration or physics, the
// sprites just move simple frame-based animation.
//
// The code shows a way to attach images to the sprites and optional the
// emitter (which is a point source) can also have an image.  If there are
// no images attached, a placeholder rectangle is drawn.
// Emitters  can be placed anywhere in the window. In this example, you can drag
// it around with the mouse.
//
// OF has an add-in called ofxGUI which is a very simple UI that is useful for
// creating sliders, buttons and fields. It is not recommended for commercial 
// game development, but it is useful for testing.  The "h" key will hide the GUI
// 
// If you want to run this example, you need to use the ofxGUI add-in in your
// setup.
//
//
//  Kevin M. Smith - CS 134 SJSU




//  Add a Sprite to the Sprite System
//
void SpriteList::add(Sprite s) {
	sprites.push_back(s);
}

// Remove a sprite from the sprite system. Note that this function is not currently
// used. The typical case is that sprites automatically get removed when the reach
// their lifespan.
//
void SpriteList::remove(int i) {
	sprites.erase(sprites.begin() + i);
}


//  Update the SpriteSystem by checking which sprites have exceeded their
//  lifespan (and deleting).  Also the sprite is moved to it's next
//  location based on velocity and direction.
//
void SpriteList::update() {
	if (sprites.size() == 0) return;
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (s != sprites.end()) {
		if (s->lifespan != -1 && s->age() > s->lifespan) {
			//			cout << "deleting sprite: " << s->name << endl;
			tmp = sprites.erase(s);
			s = tmp;
		}
		else s++;
	}
}

//  Render all the sprites
//
void SpriteList::draw() {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].draw();
	}
}

Emitter::Emitter() {
	sys = new SpriteList();
	init();
}


void Emitter::init() {
	lifespan = 3000;    // default milliseconds
	started = false;
	speed = glm::vec3(0, 0, 0);
	lastSpawned = 0;
	rate = 1;    // sprites/sec
	haveChildImage = false;
	haveImage = false;
	velocity = ofVec3f(100, 100, 0);
	drawable = true;
	width = 50;
	height = 50;
}



//  Draw the Emitter if it is drawable. In many cases you would want a hidden emitter
//
//
void Emitter::draw() {

	// draw the Emitter itself 
	// note: set drawable=false if you want the emitter to be invisible
	//
	if (drawable) {

		ofPushMatrix();
		ofMultMatrix(getTransform());

		if (haveImage) {
			image.draw(-image.getWidth() / 2.0, -image.getHeight() / 2.0);
		}
		else {
			ofSetColor(color);
			ofDrawTriangle(
				-width / 2, height / 2,       // Bottom left point
				width / 2, height / 2,        // Bottom right point
				0, -height+4 / 2                // Top center point
			);
		}

		ofPopMatrix();
	}

	// draw sprite system
	//
	sys->draw();
}

//  Update the Emitter. If it has been started, spawn new sprites with
//  initial velocity, lifespan, birthtime.
//
void Emitter::update() {
	if (!started) return;

	float time = ofGetElapsedTimeMillis();

	if ((time - lastSpawned) > (1000.0 / rate)) {
		// call virtual to spawn a new sprite
		//
		spawnSprite();
		lastSpawned = time;
	}
	// update sprite list
	//
	sys->update();

	
	for (int i = 0; i < sys->sprites.size(); i++) {
		moveSprite(&sys->sprites[i]);
	}
	
}

void Emitter::moveSprite(Sprite* sprite) {
	// Replace the current position update with an integration approach
	float fr = 1.0 / 60.0;
	sprite->pos += 3 * sprite->velocity * fr;
	sprite->velocity += sprite->acceleration * fr;
	sprite->velocity *= sprite->damping;  
	sprite->rot += sprite->rotationSpeed;
	// Screen wrapping - if outside of screen, appear at other side
	float screenWidth = ofGetWidth();
	float screenHeight = ofGetHeight();

	// Wrap horizontally
	if (sprite->pos.x < 0) sprite->pos.x = screenWidth;
	else if (sprite->pos.x > screenWidth) sprite->pos.x = 0;

	// Wrap vertically
	if (sprite->pos.y < 0) sprite->pos.y = screenHeight;
	else if (sprite->pos.y > screenHeight) sprite->pos.y = 0;
}

// virtual function to move sprite (can be overloaded)
//
//void Emitter::moveSprite(Sprite *sprite) {
//    sprite->pos += sprite->velocity / ofGetFrameRate();
//	sprite->rot += sprite->rotationSpeed;
//}

// virtual function to spawn sprite (can be overloaded)
//
void Emitter::spawnSprite() {
	Sprite sprite;
	if (haveChildImage) sprite.setImage(childImage);
	sprite.velocity = velocity;
	sprite.acceleration = acceleration;  // Add this
	sprite.damping = damping;  // Add this
	sprite.lifespan = lifespan;
	sprite.pos = pos;
	sprite.birthtime = ofGetElapsedTimeMillis();
	sys->add(sprite);
}

// Start/Stop the emitter.
//
void Emitter::start() {
	started = true;
	lastSpawned = ofGetElapsedTimeMillis();
}

void Emitter::stop() {
	started = false;
}


void Emitter::setLifespan(float life) {
	lifespan = life;
}

void Emitter::setVelocity(const glm::vec3 v) {
	velocity = v;
}

void Emitter::setChildImage(ofImage img) {
	childImage = img;
	haveChildImage = true;
}

void Emitter::setImage(ofImage img) {
	image = img;
	haveImage = true;
}

void Emitter::setRate(float r) {
	rate = r;
}
