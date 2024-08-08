//------------------------------------------------------------------------------
// main.cc
// (C) 2015-2018 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#pragma once
#include "config.h"
#include "../projects/unconstraineddynamics/code/unconstraineddynamicsapp.h"

int
main(int argc, const char** argv)
{
	Game::UnconstrainedDynamicsApp app;
	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();
	
}