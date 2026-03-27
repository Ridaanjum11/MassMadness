#include <iostream>
#include <SFML/Graphics.hpp>

int main() {
    std::cerr << "Test 1: Before window creation" << std::endl;
    std::cerr.flush();
    
    try {
        sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800, 600)), "Test Window");
        std::cerr << "Test 2: Window created successfully" << std::endl;
        std::cerr.flush();
        
        window.setFramerateLimit(60);
        std::cerr << "Test 3: Framerate set" << std::endl;
        std::cerr.flush();
        
        for (int i = 0; i < 10; i++) {
            window.clear();
            window.display();
        }
        
        std::cerr << "Test 4: Completed loop" << std::endl;
        std::cerr.flush();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        std::cerr.flush();
        return 1;
    }
    
    std::cerr << "Test 5: Complete" << std::endl;
    std::cerr.flush();
    return 0;
}
