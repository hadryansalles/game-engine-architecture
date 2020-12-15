#include "Managers.h"
#include "StackAllocator.h"
#include "AlignedAllocator.h"

/*
 * "Many of the singletons are statically allocated objects (globals). 
 *  It's not always pretty, but it gets the job done." 
 *  Jason Gregory about Naughty Dog's Game Engine.
 */

// Declare engine systems in any order
RenderManager     gRenderManager;
PhysicsManager    gPhysicsManager;
AnimationManager  gAnimationManager;
TextureManager    gTextureManager;
VideoManager      gVideoManager;
MemoryManager     gMemoryManager;
FileSystemManager gFileSystemManager;
SimulatonManager  gSimulationManager;

int main(int argc, const char* argv) {
   
    // Start up engine systems in the correct order
    gMemoryManager.startUp();
    gFileSystemManager.startUp();
    gVideoManager.startUp();
    gTextureManager.startUp();
    gRenderManager.startUp();
    gAnimationManager.startUp();
    gPhysicsManager.startUp();
    gSimulationManager.startUp();

    // Run the game
    gSimulationManager.run();

    // Shut everything down, in reverse order
    gSimulationManager.shutDown();
    gPhysicsManager.shutDown();
    gAnimationManager.shutDown();
    gRenderManager.shutDown();
    gTextureManager.shutDown();
    gVideoManager.shutDown();
    gFileSystemManager.shutDown();
    gMemoryManager.shutDown();

    return 0;
}