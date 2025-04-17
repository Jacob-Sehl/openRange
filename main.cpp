#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <iostream>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Camera and view settings
struct Camera {
    float x = 0.0f;
    float y = -500.0f;
    float z = 300.0f;
    float rotationX = 30.0f;  // Tilt angle (0 for side view, 30 for isometric)
    float rotationY = 0.0f;   // Rotation around Y axis (0 for north, 45 for NE)
    float distance = 500.0f;  // Distance from target
    float targetX = 0.0f;     // Camera target (ranger position)
    float targetY = 0.0f;
    float targetZ = 0.0f;
    
    void updatePosition() {
        // Calculate camera position based on angles and distance
        float radY = rotationY * M_PI / 180.0f;
        float radX = rotationX * M_PI / 180.0f;
        
        x = targetX - distance * sin(radY) * cos(radX);
        y = targetY - distance * cos(radY) * cos(radX);
        z = targetZ + distance * sin(radX);
    }
    
    void follow(float x, float y, float z) {
        targetX = x;
        targetY = y;
        targetZ = z;
        updatePosition();
    }
} camera;

bool isometricView = true;
float transitionSpeed = 2.0f; // Speed of camera angle transitions

// Ranger position and movement
float rangerX = 0.0f;
float rangerY = 0.0f;
float speed = 200.0f; // Pixels per second

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

void renderSprite(GLuint textureID, float x, float y) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Save current matrix
    glPushMatrix();
    
    // Move to sprite position, lifting it higher off the ground
    glTranslatef(x, y, 48.0f);  // Lift sprite up by 48 units
    
    // Calculate the vector from camera to sprite
    float dx = x - camera.x;
    float dy = y - camera.y;
    
    // Calculate rotation angle around Z axis (yaw)
    float angleY = atan2(dy, dx) * 180.0f / M_PI;
    
    // Rotate the sprite to face the camera, adjusting for the sprite's orientation
    glRotatef(90 + angleY - camera.rotationY, 0, 0, 1);  // Yaw rotation
    glRotatef(camera.rotationX, 1, 0, 0);  // Counter pitch rotation
    
    // Enable texturing
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Increased size for better visibility
    float size = 64.0f;  // Size remains the same
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex3f(-size/2, 0, -size);
    glTexCoord2f(1, 1); glVertex3f(size/2, 0, -size);
    glTexCoord2f(1, 0); glVertex3f(size/2, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(-size/2, 0, 0);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glDisable(GL_BLEND);
}

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  IMG_Init(IMG_INIT_PNG);

  SDL_Window *window = SDL_CreateWindow("openRange", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
                                        WINDOW_HEIGHT, SDL_WINDOW_OPENGL);

  SDL_GLContext glContext = SDL_GL_CreateContext(window);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (float)WINDOW_WIDTH/(float)WINDOW_HEIGHT, 0.1f, 1000.0f);

  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);

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
      else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
          case SDLK_SPACE:
            // Toggle between isometric and side view
            isometricView = !isometricView;
            break;
          case SDLK_LEFT:
            // Rotate camera left
            camera.rotationY -= 45.0f;
            break;
          case SDLK_RIGHT:
            // Rotate camera right
            camera.rotationY += 45.0f;
            break;
          case SDLK_UP:
            // Increase tilt
            camera.rotationX = std::min(camera.rotationX + 15.0f, 60.0f);
            break;
          case SDLK_DOWN:
            // Decrease tilt
            camera.rotationX = std::max(camera.rotationX - 15.0f, 0.0f);
            break;
        }
      }
    }

    // Calculate movement
    float moveX = 0.0f;
    float moveY = 0.0f;
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_W]) {
        if (isometricView) {
            moveY += speed * deltaTime;  // Inverted Y movement
        } else {
            moveY += speed * deltaTime;
        }
    }
    if (keys[SDL_SCANCODE_S]) {
        if (isometricView) {
            moveY -= speed * deltaTime;  // Inverted Y movement
        } else {
            moveY -= speed * deltaTime;
        }
    }
    if (keys[SDL_SCANCODE_A]) {
        if (isometricView) {
            moveX -= speed * deltaTime;
        } else {
            moveX -= speed * deltaTime;
        }
    }
    if (keys[SDL_SCANCODE_D]) {
        if (isometricView) {
            moveX += speed * deltaTime;
        } else {
            moveX += speed * deltaTime;
        }
    }
    
    // Apply camera rotation to movement
    if (isometricView) {
        float angle = camera.rotationY * M_PI / 180.0f;
        float rotatedX = moveX * cos(angle) - moveY * sin(angle);
        float rotatedY = moveX * sin(angle) + moveY * cos(angle);
        rangerX += rotatedX;
        rangerY += rotatedY;
    } else {
        rangerX += moveX;
        rangerY += moveY;
    }
    
    // Update camera to follow ranger
    camera.follow(rangerX, rangerY, 0);
    
    // Clear screen
    glClearColor(0.529f, 0.808f, 0.922f, 1.0f);  // Sky blue
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)WINDOW_WIDTH/(float)WINDOW_HEIGHT, 0.1f, 2000.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Set up camera
    gluLookAt(camera.x, camera.y, camera.z,     // Camera position
              camera.targetX, camera.targetY, camera.targetZ,  // Look at point (ranger position)
              0, 0, 1);                         // Up vector
    
    // Draw ground grid
    if (isometricView) {
        glDisable(GL_TEXTURE_2D);
        glColor4f(0.2f, 0.6f, 0.2f, 1.0f);  // Green ground
        glBegin(GL_QUADS);
        glVertex3f(-1000, -1000, -1);
        glVertex3f(1000, -1000, -1);
        glVertex3f(1000, 1000, -1);
        glVertex3f(-1000, 1000, -1);
        glEnd();
        
        // Grid lines
        glColor4f(0.3f, 0.7f, 0.3f, 1.0f);
        glBegin(GL_LINES);
        for (int i = -10; i <= 10; i++) {
            glVertex3f(i * 100.0f, -1000, 0);
            glVertex3f(i * 100.0f, 1000, 0);
            glVertex3f(-1000, i * 100.0f, 0);
            glVertex3f(1000, i * 100.0f, 0);
        }
        glEnd();
    }
    
    // Reset color
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    
    // Draw ranger
    renderSprite(rangerTex, rangerX, rangerY);
    
    SDL_GL_SwapWindow(window);
  }

  glDeleteTextures(1, &rangerTex);
  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();
  return 0;
}
