#include "BF_Core.h"

#include "BF_Graphics.h"
#include "BF_Scene.h"

Core::Core() : m_pGraphics(nullptr), m_pScene(nullptr)
{
}

Core::~Core()
{
}

void Core::init()
{
	//Setup Graphics module
	m_pGraphics = new Graphics();
	if (m_pGraphics)
	{
		
	}

	//Setup Scene Module
	m_pScene = new Scene();
	if (m_pScene)
	{

	}
}

void Core::shutdown()
{
}

void Core::run()
{
}
