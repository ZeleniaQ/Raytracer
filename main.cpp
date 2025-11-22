#include "scene.hpp"
#include "renderer.hpp"


int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./raytracer <scene.txt>\n";
        return 1;
    }
    Scene scene;
    if (!scene.loadFromFile(argv[1])) {
        std::cerr << "Failed to load scene." << std::endl;
        return 1;
    }
    Renderer renderer(scene);
    renderer.render();
    return 0;
}