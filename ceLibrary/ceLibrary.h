#ifndef CRIMSON_LIBRARY_H
#define CRIMSON_LIBRARY_H

#pragma once

#include "Window.h"
#include "Application.h"
#include "GMath.h"
#include "Math3D.h"
#include "MatrixGenerator.h"
#include "Manager.h"
#include "Singleton.h"
#include "SmartPointer.h"
#include "ReferenceCount.h"
//#include "Profiler.h"


class CLib 
{ 
public: 
     static void Init(); 
     static void Shutdown();     
	 
};


#endif