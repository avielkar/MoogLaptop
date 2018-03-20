/************************************************************************
	StdAfx.h -- Precompiled header
************************************************************************/

#pragma once

#ifndef STDAFX
#define STDAFX

//MatlabTcpCommunication
#include "MatlabTcpCommunicator.h"
//#define _WINSOCK2API_

#include "Application.hpp"
#include "InputHandlers.hpp"
#include "RenderContext.hpp"
#include "ShaderManager.hpp"
#include "OculusVR.hpp"

//MatlabTcpCommunication
//#include "MatlabTcpCommunicator.h"

// wxWindows
#include <wx\wxprec.h>
#include <wx\glcanvas.h>

// C++ libs
#include <string>
#include <sstream>
#include <vector>
#include <hash_map>
#include <math.h>

// OpenGL
//#include <gl\glu.h>
#include <gl\glut.h>

// ComputerBoards
#include "cbw.h"

// Moog
#include <MoogCom.h>
#include <NumericalMethods.h>

// Matlab
#include <engine.h>
#include <MatlabRDX.h>

void Delay(double ms);

#endif