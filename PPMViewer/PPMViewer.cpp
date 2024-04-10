#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <SDL.h>
#ifdef ENSCRYPTEN
#include <SDL2.h>
#include <SDL/SDL.h>
#include <emscripten.h>
#endif

std::vector<std::string> split(std::string content, std::string delimiter = "\r\n") {
    std::vector<std::string> lines;
    size_t previous = 0;
    while ((previous = content.find(delimiter)) != std::string::npos) {
        lines.push_back(content.substr(0, previous));
        content.erase(0, previous + delimiter.size());
    }
    lines.push_back(content);
    return lines;
}

int main(int argc, char* argv[]) {

    const char* ppmFilePath = "images/image.ppm";
    std::ifstream ppmFile(ppmFilePath, std::ios::binary);

    if (!ppmFile.is_open()) {
        std::cerr << "Failed to open the PPM file." << std::endl;
        return 1;
    }

    // Read PPM header
    std::string magic;
    int width, height, maxColor;
    ppmFile >> magic >> width >> height >> maxColor;

    if (magic != "P3" || maxColor != 255) {
        std::cerr << "Unsupported PPM format. Only P3 with max color 255 is supported." << std::endl;
        return 1;
    }

    // Create an SDL window to display the image
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to Init video." << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("PPM Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);

    // Read pixel data
    std::vector<uint8_t> pixelData(width * height * 3);
    std::string content;
    std::getline(ppmFile, content);
    std::vector<uint8_t> p;

    for (auto j = 0; j < width; j++) {
        for (auto i = 0; i < height; i++) {
            std::getline(ppmFile, content);

            //std::cout << content << "\n";

            std::vector<std::string> pixel = split(content, " ");
            for (auto& k : pixel) {
                p.push_back(std::stoi(k));
                //std::cout << stoi(k) << " " <<  "\n";
            }
        }
    }

    // Update texture with pixel data
    SDL_UpdateTexture(texture, nullptr, p.data(), width * 3);

    SDL_Event event;
    bool quit = false;

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);

    
    while (!quit) {
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}