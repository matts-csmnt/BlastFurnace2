#pragma once

class Scene {
public:
	Scene();
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	void init();
	void shutdown();

	void update();

private:

};