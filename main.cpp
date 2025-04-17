#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <iostream>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Ranger position
float rangerX = 0.0f;
float rangerY = 0.0f;
float speed = 100.0f; // Pixels per second

float lastTime = 0.0f;

bool loadTexture(GLuint &textureID, const char *filepath) {
  SDL_Surface *surface = IMG_Load(filepath);
  if (!surface) {
    std::cerr << "Failed to load image: " << IMG_GetError() << "\n";
    return false;
  }
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  GLint mode = surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
  glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode,
               GL_UNSIGNED_BYTE, surface->pixels);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST); // pixel look
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  SDL_FreeSurface(surface);
  return true;
}

void renderRanger(GLuint textureID) {
  glBindTexture(GL_TEXTURE_2D, textureID);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 1);
  glVertex2f(-16, -16);
  glTexCoord2f(1, 1);
  glVertex2f(16, -16);
  glTexCoord2f(1, 0);
  glVertex2f(16, 16);
  glTexCoord2f(0, 0);
  glVertex2f(-16, 16);
  glEnd();

  glDisable(GL_BLEND);
}

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  IMG_Init(IMG_INIT_PNG);

  SDL_Window *window = SDL_CreateWindow("Cowboy Cam", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
                                        WINDOW_HEIGHT, SDL_WINDOW_OPENGL);

  SDL_GLContext glContext = SDL_GL_CreateContext(window);
  glewInit();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2,
          WINDOW_HEIGHT / 2, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_TEXTURE_2D);

  GLuint rangerTex;
  if (!loadTexture(rangerTex, "../assets/ranger.png"))
    return -1;

  bool running = true;
  SDL_Event event;

  Uint32 lastTicks = SDL_GetTicks();

  while (running) {
    Uint32 currentTicks = SDL_GetTicks();
    float deltaTime = (currentTicks - lastTicks) / 1000.0f;
    lastTicks = currentTicks;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        running = false;
    }

    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_W])
      rangerY += speed * deltaTime;
    if (keys[SDL_SCANCODE_S])
      rangerY -= speed * deltaTime;
    if (keys[SDL_SCANCODE_A])
      rangerX -= speed * deltaTime;
    if (keys[SDL_SCANCODE_D])
      rangerX += speed * deltaTime;

    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(-rangerX, -rangerY, 0); // Camera follows the cowboy

    glPushMatrix();
    glTranslatef(rangerX, rangerY, 0);
    renderRanger(rangerTex);
    glPopMatrix();

    SDL_GL_SwapWindow(window);
  }

  glDeleteTextures(1, &rangerTex);
  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();
  return 0;
}
