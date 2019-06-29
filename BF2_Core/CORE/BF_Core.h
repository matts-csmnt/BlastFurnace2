#pragma once

#include <memory>
#include "BF_Graphics.h"
#include "BF_Scene.h"

class Core {
public:
	Core();
	Core(const Core&) = delete;
	Core& operator=(const Core&) = delete;

	~Core();

	void init();
	void shutdown();

	void run();

private:

	//Engine Components
	std::unique_ptr<Graphics>	m_pGraphics;
	std::unique_ptr<Scene>		m_pScene;

	//Timer
	double m_seconds;
	int64_t m_milliseconds;

	//Exit flag
	bool m_exit;
};