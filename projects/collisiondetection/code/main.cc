//------------------------------------------------------------------------------
// main.cc
// (C) 2015-2018 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#pragma once
#include "config.h"
#include "../projects/collisiondetection/code/collisiondetectionapp.h"

int
main(int argc, const char** argv)
{
	Game::CollisionDetectionApp app;
	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();
	
}