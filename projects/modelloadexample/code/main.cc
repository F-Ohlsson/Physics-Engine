//------------------------------------------------------------------------------
// main.cc
// (C) 2015-2018 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#pragma once
#include "config.h"
#include "../projects/modelloadexample/code/modelloadapp.h"

int
main(int argc, const char** argv)
{
	Game::ModelLoadApp app;
	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();
	
}