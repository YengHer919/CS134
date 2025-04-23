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
//
#pragma once
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"

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
    void drawAxis(ofVec3f location);

    // Camera
    ofEasyCam cam;

    // Model
    ofxAssimpModelLoader model;
    float modelRotation;

    // Lights
    ofLight keyLight;
    ofLight fillLight;
    ofLight rimLight;
    ofLight bottomFill;  // Added fourth light for subtle bottom illumination

    // Ground plane
    ofPlanePrimitive plane;
    ofMaterial planeMaterial;

    // GUI elements
    ofxPanel gui;
    ofxFloatSlider keyLightIntensity;
    ofxFloatSlider fillLightIntensity;
    ofxFloatSlider rimLightIntensity;
    ofxFloatSlider ambientIntensity;
    ofxColorSlider keyLightColor;
    ofxColorSlider fillLightColor;
    ofxColorSlider rimLightColor;

    // Additional GUI for advanced controls
    ofxPanel advancedGui;
    ofxFloatSlider keySoftness;     // Controls area light size
    ofxFloatSlider fillSpread;      // Controls spotlight cutoff
    ofxFloatSlider rimConcentration; // Controls spotlight concentration
    ofxFloatSlider rotationSpeed;

    // State flags
    bool bModelLoaded;
    bool bPlaneLoaded;
    bool bFlipScene;
    bool bShowLights;
    bool bRotateModel;
    bool bShowAdvancedControls;
};