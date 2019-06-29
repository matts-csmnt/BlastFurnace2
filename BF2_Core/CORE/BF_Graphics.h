#pragma once

#include <memory>
#include "../Graphics & Window/BF_Window.h"

class Graphics {
public:
	Graphics();
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	void init();
	void shutdown();

	void frame();

	const double queryTimer() const;

	const bool getExitFlag() const;

private:

	std::unique_ptr<Window> m_pWindow;

	//exit flag
	bool m_exit;
};