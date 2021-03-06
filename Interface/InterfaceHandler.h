////========================================
// Class: InterfaceHandler
// Author: Hemmy
// Date: 20/06/2018
// Description:
// Pretty much the same as an interface node
// but is not attached to the event system
// This is so individual subsystems can have
// access to the mouse and keyboard without
// going through the event system.
// ========================================

#pragma once

#include <stdio.h>

#include "KeyboardInterface.h"
#include "MouseInterface.h"

class InterfaceHandler{
public:
	InterfaceHandler(GLFWwindow* window);
	~InterfaceHandler();
	
	void update();
	
	Vector2 getMouseRelativePos() const { return m_mouseInterface->getRelativePosition();}
	Vector2 getMouseAbsolutePos() const { return m_mouseInterface->getAbsolutePosition();}

	GLFWwindow* getWindowListener() const {return m_windowListener;}

	bool* getPressedKeys() const {return m_keyboardInterface->getPressedKeys();}
	bool* getHeldKeys() const {return m_keyboardInterface->getHeldHeys();}
	
private:
	
	GLFWwindow* 		m_windowListener;
	MouseInterface* 	m_mouseInterface;
	KeyboardInterface*	m_keyboardInterface;
};


