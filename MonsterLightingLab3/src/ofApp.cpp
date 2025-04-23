//
//  Starter file for Lighting Experiments--  
//
//  Kevin M. Smith SJSU - CS134
//
//  This file consists of a 3-Point Lighting Setup  (classic stage lighting)
//
//  Uses the OF ofLight class which is an object interface to OpenGL lighting.
//
//  -- Experiment with different lighting types 
//  -- Experiment with different lighting parameters
//  -- Disable lights to see the effect of individual lights or combinations
//  -- Connect sliders from ofxGUI to make this easier.
// 
//
//   (c) Kevin M. Smith - 2018  - Do not redistribute code or models with
//       permission from the author. 
//

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofEnableDepthTest();
    ofEnableLighting();

    cam.setDistance(10);
    cam.setNearClip(.1);
    cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
    cam.disableMouseInput();

    // Create GUI for lighting control
    gui.setup("Lighting Controls");
    gui.add(keyLightIntensity.setup("Key Light", 1.0, 0.0, 2.0));
    gui.add(fillLightIntensity.setup("Fill Light", 0.6, 0.0, 2.0));
    gui.add(rimLightIntensity.setup("Rim Light", 1.2, 0.0, 2.0));
    gui.add(ambientIntensity.setup("Ambient", 0.2, 0.0, 1.0));
    gui.add(keyLightColor.setup("Key Color", ofColor(255, 253, 240), ofColor(0, 0, 0), ofColor(255, 255, 255)));
    gui.add(fillLightColor.setup("Fill Color", ofColor(180, 200, 255), ofColor(0, 0, 0), ofColor(255, 255, 255)));
    gui.add(rimLightColor.setup("Rim Color", ofColor(255, 230, 200), ofColor(0, 0, 0), ofColor(255, 255, 255)));
    // Setup enhanced 3-Light System

    // Key Light - Main illumination
    keyLight.setup();
    keyLight.enable();
    keyLight.setAreaLight(2, 2);  // Larger area light for softer shadows
    keyLight.setAmbientColor(ofFloatColor(0.2, 0.2, 0.2));
    keyLight.setDiffuseColor(ofFloatColor(1.0, 0.98, 0.94));  // Slightly warm white
    keyLight.setSpecularColor(ofFloatColor(1.0, 1.0, 1.0));
    keyLight.rotate(45, ofVec3f(0, 1, 0));
    keyLight.rotate(-45, ofVec3f(1, 0, 0));
    keyLight.setPosition(5, 7, 5);  // Higher position for dramatic lighting

    // Fill Light - Softer secondary light
    fillLight.setup();
    fillLight.enable();
    fillLight.setSpotlight();
    fillLight.setScale(.08);  // Slightly larger spotlight
    fillLight.setSpotlightCutOff(25);  // Wider cone
    fillLight.setSpotConcentration(80);  // Softer edge falloff
    fillLight.setAttenuation(1.5, .0008, .0005);  // More realistic attenuation
    fillLight.setAmbientColor(ofFloatColor(0.1, 0.12, 0.15));  // Slightly blue ambient
    fillLight.setDiffuseColor(ofFloatColor(0.7, 0.78, 1.0));  // Cool blue fill
    fillLight.setSpecularColor(ofFloatColor(0.8, 0.8, 0.9));
    fillLight.rotate(-15, ofVec3f(1, 0, 0));
    fillLight.rotate(-60, ofVec3f(0, 1, 0));  // More side fill
    fillLight.setPosition(-6, 4, 4);

    // Rim Light - Dramatic edge highlighting
    rimLight.setup();
    rimLight.enable();
    rimLight.setSpotlight();
    rimLight.setScale(.06);
    rimLight.setSpotlightCutOff(25);
    rimLight.setSpotConcentration(170);  // More concentrated for defined edge
    rimLight.setAttenuation(.2, .001, .0005);
    rimLight.setAmbientColor(ofFloatColor(0.1, 0.08, 0.06));  // Warm ambient
    rimLight.setDiffuseColor(ofFloatColor(1.0, 0.9, 0.8));  // Warm rim light
    rimLight.setSpecularColor(ofFloatColor(1.0, 0.9, 0.8));
    rimLight.rotate(170, ofVec3f(0, 1, 0));  // Slightly off-center
    rimLight.setPosition(0, 6, -7);

    // Subtle bottom fill light for more dimensional look
    bottomFill.setup();
    bottomFill.enable();
    bottomFill.setPointLight();
    bottomFill.setAttenuation(3, .05, .02);  // Quick falloff
    bottomFill.setAmbientColor(ofFloatColor(0.0, 0.0, 0.0));
    bottomFill.setDiffuseColor(ofFloatColor(0.15, 0.15, 0.2, 1.0));  // Very subtle
    bottomFill.setSpecularColor(ofFloatColor(0.0, 0.0, 0.0));  // No specular
    bottomFill.setPosition(0, -1, 0);  // Below the model

    // Create Ground Plane with more interesting material
    plane.set(20, 20, 12, 12);  // More subdivisions for better lighting
    plane.rotate(-90, ofVec3f(1, 0, 0));
    planeMaterial.setAmbientColor(ofFloatColor(0.15, 0.15, 0.15, 1.0));
    planeMaterial.setDiffuseColor(ofFloatColor(0.35, 0.35, 0.4, 1.0));  // Slightly blue-ish
    planeMaterial.setSpecularColor(ofFloatColor(0.2, 0.2, 0.25, 1.0));  // Reduced specular
    planeMaterial.setShininess(2);  // Less shiny, more matte

    // Load model with better positioning
    if (model.loadModel("monster.obj")) {
        model.setScaleNormalization(false);
        model.setPosition(0, 1, 0);  // Slightly raised above ground
        model.setScale(1.0, 1.0, 1.0);  // Adjust scale if needed
        bModelLoaded = true;
    }
    else cout << "Error: Can't load model:" << " geo/monster.obj" << endl;

    // Initialize scene state
    bFlipScene = false;
    bShowLights = true;
    bRotateModel = false;
    rotationSpeed = 0.5;
    modelRotation = 0;
}

//--------------------------------------------------------------
void ofApp::update() {
    // Update light colors and intensities from GUI
    keyLight.setDiffuseColor(ofFloatColor(
        keyLightColor.getFillColor().r / 255.0 * keyLightIntensity,
        keyLightColor.getFillColor().g / 255.0 * keyLightIntensity,
        keyLightColor.getFillColor().b / 255.0 * keyLightIntensity
    ));

    fillLight.setDiffuseColor(ofFloatColor(
        fillLightColor.getFillColor().r / 255.0 * fillLightIntensity,
        fillLightColor.getFillColor().g / 255.0 * fillLightIntensity,
        fillLightColor.getFillColor().b / 255.0 * fillLightIntensity
    ));

    rimLight.setDiffuseColor(ofFloatColor(
        rimLightColor.getFillColor().r / 255.0 * rimLightIntensity,
        rimLightColor.getFillColor().g / 255.0 * rimLightIntensity,
        rimLightColor.getFillColor().b / 255.0 * rimLightIntensity
    ));

    // Update ambient for all lights
    float ambient = ambientIntensity;
    keyLight.setAmbientColor(ofFloatColor(ambient, ambient, ambient));
    fillLight.setAmbientColor(ofFloatColor(ambient * 0.8, ambient * 0.85, ambient));
    rimLight.setAmbientColor(ofFloatColor(ambient * 0.9, ambient * 0.85, ambient * 0.8));

    // Rotate model if enabled
    if (bRotateModel) {
        modelRotation += rotationSpeed;
        if (modelRotation >= 360) modelRotation = 0;
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackground(ofColor(10, 10, 15));  // Dark blue-black for better contrast

    // Draw GUI
    ofDisableDepthTest();
    gui.draw();
    ofEnableDepthTest();
    cam.begin();

    ofPushMatrix();

    if (bFlipScene) {
        ofRotateDeg(180, 1, 0, 0);
    }

    // Draw lights if enabled
    if (bShowLights) {
        keyLight.draw();
        fillLight.draw();
        rimLight.draw();
        // Don't draw bottomFill - usually hidden
    }

    // Draw the model with rotation
    if (bModelLoaded) {
        ofPushMatrix();
        ofTranslate(0, 1, 0);  // Keep model slightly above ground
        ofRotateYDeg(modelRotation);

        model.drawFaces();
        
        ofPopMatrix();
    }

    // Draw the ground plane
    planeMaterial.begin();
    plane.draw();
    planeMaterial.end();

    ofPopMatrix();
    cam.end();

    // Draw status info
    ofSetColor(255);}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    switch (key) {
    case 'C':
    case 'c':
        if (cam.getMouseInputEnabled()) cam.disableMouseInput();
        else cam.enableMouseInput();
        break;
    case 'F':
    case 'f':
        ofToggleFullscreen();
        break;
    case 'R':
    case 'r':
        break;
    case 'W':
    case 'w':
        break;
    case 'L':
    case 'l':
        bShowLights = !bShowLights;
        break;
    case 'U':
    case 'u':
        bFlipScene = !bFlipScene;
        break;
    case '1':
        keyLight.enable();
        break;
    case '!':
        keyLight.disable();
        break;
    case '2':
        fillLight.enable();
        break;
    case '@':
        fillLight.disable();
        break;
    case '3':
        rimLight.enable();
        break;
    case '#':
        rimLight.disable();
        break;
    case '4':
        bottomFill.enable();
        break;
    case '$':
        bottomFill.disable();
        break;
    default:
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

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}