#pragma once

#include <iostream>

class Manager {
public:
    virtual void startUp() = 0;
    virtual void shutDown() = 0;
};

class RenderManager : public Manager {
public:
    void startUp() override {
        std::cout << "RenderManager start up.\n";
    }

    void shutDown() override {
        std::cout << "RenderManager shut down.\n";
    }
};

class PhysicsManager : public Manager {
public:
    void startUp() override {
        std::cout << "PhysicsManager start up.\n";
    }

    void shutDown() override {
        std::cout << "PhysicsManager shut down.\n";
    }
};

class AnimationManager : public Manager {
public:
    void startUp() override {
        std::cout << "AnimationManager start up.\n";
    }

    void shutDown() override {
        std::cout << "AnimationManager shut down.\n";
    }
};

class TextureManager : public Manager {
public:
    void startUp() override {
        std::cout << "TextureManager start up.\n";
    }

    void shutDown() override {
        std::cout << "TextureManager shut down.\n";
    }
};

class VideoManager : public Manager {
public:
    void startUp() override {
        std::cout << "VideoManager start up.\n";
    }

    void shutDown() override {
        std::cout << "VideoManager shut down.\n";
    }
};

class MemoryManager : public Manager {
public:
    void startUp() override {
        std::cout << "MemoryManager start up.\n";
    }

    void shutDown() override {
        std::cout << "MemoryManager shut down.\n";
    }
};

class FileSystemManager : public Manager {
public:
    void startUp() override {
        std::cout << "FileSystemManager start up.\n";
    }

    void shutDown() override {
        std::cout << "FileSystemManager shut down.\n";
    }
};

class SimulatonManager : public Manager {
public:
    void startUp() override {
        std::cout << "SimulationManager start up.\n";
    }

    void shutDown() override {
        std::cout << "SimulationManager shut down.\n";
    }

    void run() {
        std::cout << "SimulationManager run.\n";
    }
};
