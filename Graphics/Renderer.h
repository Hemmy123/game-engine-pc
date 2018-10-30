//
//  Renderer.h
//  Graphics
//
//  Created by Hemmy on 30/05/2018.
//  Copyright � 2018 Hemmy. All rights reserved.
//

#pragma once

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <stdio.h>

#include "MasterRenderer.h"
#include "RenderObject.h"
#include "Camera.h"

#include <vector>

using std::vector;

enum Projection {
	Orthographic,
	Perspective,

};

class Renderer : public MasterRenderer {
public:
	Renderer(int height, int width);
	~Renderer();

	void defaultGLSettings();


	/// This needs to be called after creating a Renderer. This is due
	/// to GLFW requireing a window to get user input. Maybe code should
	/// be restructured to avoid this?
	void createCamera(InterfaceHandler* ih);

	// ---------- Rendering ---------- //

	/// Carries out the whole rendering process, drawing to scene FBO,
	/// add post processing and then presenting the scene to the screen.
	void renderScene();

	/// Draws a renderObject onto whatever FBO is currently bound.
	void drawRenderObject(const RenderObject &o);

	void drawAllRenderObjects();

	void changeProjection(Projection proj);
	// ---------- Updating ---------- //

	/// The 'master' update method which updates everything in the scene
	/// and then calls the render methods to draw to the screen.
	void update(float msec);

	/// Updates the camera and view matrix, and then updates the
	/// Objects via the updateRenderObjects method.
	void updateScene(float msec);

	/// Loops through all renderObjects and updates their world transform
	void updateRenderObjects(float msec);

	/// Update the matrices being pushing to shaders via uniforms
	void updateShaderMatrices();


	// ---------- Getters/Setters ---------- //

	/// Checks if the objects are transparent or not and then adds
	/// them into the relevant vector
	void setRenderObjects(vector<RenderObject*> renderObjects);


	vector<RenderObject*> getOpaqueObjects() 		const { return m_opaqueObjects; }
	vector<RenderObject*> getTransparentObjectS() 	const { return m_transparentObjects; };

	void addRenderObject(RenderObject* renderObject);

	GLint getWidth()const { return WIDTH; }
	GLint getHeight()const { return HEIGHT; }

	GLFWwindow* getWindow() const { return m_window; }

	Camera* getCamera()		const { return m_camera; }

	Shader* getCurrentShader()  { return m_currentShader; }

	// ---------- Post processing test methods ---------- //

	void setCurrentShader(Shader* s) { m_currentShader = s; }

	// ---------- Misc ---------- //
	bool checkWindow() { return glfwWindowShouldClose(m_window); }

	void setShaderLight(Shader* shader, Light &light);

protected:



	// ---------- Functions ---------- //



	/// Presents the scene to the screen (after all post processing and
	/// extras are finished.
	void presentScene();

	Shader* m_currentShader;
	Shader* m_sceneShader;

	// ---------- Fields ---------- //
	float m_dt;

	// -- RenderObjects to render
	vector<RenderObject*> m_opaqueObjects;
	vector<RenderObject*> m_transparentObjects;

	Vector4 m_clearColour;

	// ----- View and Matrices ----- //
	float m_viewDistance = 1000;
	float m_fov = 45.0f;
	float m_aspectRatio;

	Camera* m_camera;




};




