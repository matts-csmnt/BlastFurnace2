#pragma once

//fwd dec engine components
class Graphics;
class Scene;
//--

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
	Graphics*	m_pGraphics;
	Scene*		m_pScene;
};