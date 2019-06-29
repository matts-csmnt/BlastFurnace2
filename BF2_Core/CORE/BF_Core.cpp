#include "BF_Core.h"
#include "../Utils/BF_Error.h"

Core::Core() : m_exit(false)
{
}

Core::~Core()
{
}

void Core::init()
{
	//Setup Graphics module
	m_pGraphics = std::make_unique<Graphics>();
	
	if (!m_pGraphics)
	{
		panicF("Failed to create the Graphics module");
	}

	m_pGraphics->init();

	//Setup Scene Module
	m_pScene = std::make_unique<Scene>();

	if (!m_pScene)
	{
		panicF("Failed to create the Scene module");
	}

	m_pScene->init();
}

void Core::shutdown()
{
	//Shutdown Graphics module
	m_pGraphics->shutdown();

	//Shutdown Scene Module
	m_pScene->shutdown();
}

void Core::run()
{
	//Main Loop
	while (!m_exit) {

		const double t0s = m_pGraphics->queryTimer();

		m_pScene->update();

		m_pGraphics->frame();

		//update timers
		const double t1s = m_pGraphics->queryTimer();
		m_seconds = t1s - t0s;
		m_milliseconds = static_cast<uint64_t>(m_seconds * 1000.0);

		//check exit conditions...
		m_exit |= m_pGraphics->getExitFlag();
	}
}
