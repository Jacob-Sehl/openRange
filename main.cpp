#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <iostream>

// Constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float CAMERA_ROTATION_ANGLE = 45.0f;  // Degrees to rotate camera
const float CAMERA_TILT_ANGLE = 15.0f;      // Degrees to tilt camera
const float MOVEMENT_SPEED = 200.0f;        // Pixels per second

// Camera and view settings
struct Camera {
    // Camera position
    float x = 0.0f;
    float y = -500.0f;
    float z = 300.0f;
    
    // Camera angles
    float rotationX = 30.0f;  // Tilt angle (0 for side view, 30 for isometric)
    float rotationY = 0.0f;   // Rotation around Y axis (0 for north, 45 for NE)
    
    // Camera properties
    float distance = 500.0f;  // Distance from target
    
    // Target position (ranger position)
    float targetX = 0.0f;
    float targetY = 0.0f;
    float targetZ = 0.0f;
    
    void updatePosition() {
        // Convert angles to radians
        float radY = rotationY * M_PI / 180.0f;
        float radX = rotationX * M_PI / 180.0f;
        
        // Calculate camera position based on angles and distance
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

// Game state
float rangerX = 0.0f;
float rangerY = 0.0f;
bool facingLeft = false;  // Track which way the ranger is facing

// Texture loading helper function
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
    
    // Use nearest neighbor filtering for pixel art
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    SDL_FreeSurface(surface);
    return true;
}

// Sprite rendering function
void renderSprite(GLuint textureID, float x, float y) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glPushMatrix();
    
    // Position sprite and lift it off the ground
    glTranslatef(x, y, 48.0f);
    
    // Always rotate sprite to face camera directly
    glRotatef(-camera.rotationY, 0, 0, 1);    // Counter-rotate against camera yaw
    glRotatef(-camera.rotationX, 1, 0, 0);    // Counter-rotate against camera pitch (reversed)
    
    // Draw textured quad
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    float size = 64.0f;
    glBegin(GL_QUADS);
    // Flip texture coordinates horizontally if facing left
    if (facingLeft) {
        glTexCoord2f(1, 1); glVertex3f(-size/2, 0, -size);
        glTexCoord2f(0, 1); glVertex3f(size/2, 0, -size);
        glTexCoord2f(0, 0); glVertex3f(size/2, 0, 0);
        glTexCoord2f(1, 0); glVertex3f(-size/2, 0, 0);
    } else {
        glTexCoord2f(0, 1); glVertex3f(-size/2, 0, -size);
        glTexCoord2f(1, 1); glVertex3f(size/2, 0, -size);
        glTexCoord2f(1, 0); glVertex3f(size/2, 0, 0);
        glTexCoord2f(0, 0); glVertex3f(-size/2, 0, 0);
    }
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glDisable(GL_BLEND);
}

// Draw the ground grid
void drawGround() {
    glDisable(GL_TEXTURE_2D);
    
    // Draw ground plane
    glColor4f(0.2f, 0.6f, 0.2f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-1000, -1000, -1);
    glVertex3f(1000, -1000, -1);
    glVertex3f(1000, 1000, -1);
    glVertex3f(-1000, 1000, -1);
    glEnd();
    
    // Draw grid lines
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

// Handle keyboard input for movement
void handleMovement(float deltaTime) {
    float moveX = 0.0f;
    float moveY = 0.0f;
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    // Get movement input
    if (keys[SDL_SCANCODE_W]) moveY += MOVEMENT_SPEED * deltaTime;
    if (keys[SDL_SCANCODE_S]) moveY -= MOVEMENT_SPEED * deltaTime;
    if (keys[SDL_SCANCODE_A]) moveX -= MOVEMENT_SPEED * deltaTime;
    if (keys[SDL_SCANCODE_D]) moveX += MOVEMENT_SPEED * deltaTime;
    
    // Apply camera rotation to movement
    // Invert the angle to make movement relative to camera view
    float angle = -camera.rotationY * M_PI / 180.0f;
    float rotatedX = moveX * cos(angle) - moveY * sin(angle);
    float rotatedY = moveX * sin(angle) + moveY * cos(angle);
    
    // Update facing direction based on movement
    if (rotatedX < 0) facingLeft = true;
    else if (rotatedX > 0) facingLeft = false;
    
    rangerX += rotatedX;
    rangerY += rotatedY;
}

int main() {
    // Initialize SDL and OpenGL
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    
    SDL_Window *window = SDL_CreateWindow("openRange",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        WINDOW_WIDTH, WINDOW_HEIGHT,
                                        SDL_WINDOW_OPENGL);
    
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    
    // Set up OpenGL projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)WINDOW_WIDTH/(float)WINDOW_HEIGHT, 0.1f, 2000.0f);
    
    // Enable texturing and depth testing
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    
    // Load textures
    GLuint rangerTex;
    if (!loadTexture(rangerTex, "../assets/ranger.png"))
        return -1;
    
    // Game loop variables
    bool running = true;
    SDL_Event event;
    Uint32 lastTicks = SDL_GetTicks();
    
    // Main game loop
    while (running) {
        // Calculate delta time
        Uint32 currentTicks = SDL_GetTicks();
        float deltaTime = (currentTicks - lastTicks) / 1000.0f;
        lastTicks = currentTicks;
        
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        camera.rotationY += CAMERA_ROTATION_ANGLE;
                        break;
                    case SDLK_RIGHT:
                        camera.rotationY -= CAMERA_ROTATION_ANGLE;
                        break;
                    case SDLK_UP:
                        camera.rotationX = std::min(camera.rotationX + CAMERA_TILT_ANGLE, 60.0f);
                        break;
                    case SDLK_DOWN:
                        camera.rotationX = std::max(camera.rotationX - CAMERA_TILT_ANGLE, 0.0f);
                        break;
                }
            }
        }
        
        // Update game state
        handleMovement(deltaTime);
        camera.follow(rangerX, rangerY, 0);
        
        // Clear screen
        glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Set up camera
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0f, (float)WINDOW_WIDTH/(float)WINDOW_HEIGHT, 0.1f, 2000.0f);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(camera.x, camera.y, camera.z,
                  camera.targetX, camera.targetY, camera.targetZ,
                  0, 0, 1);
        
        // Render scene
        drawGround();
        
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        renderSprite(rangerTex, rangerX, rangerY);
        
        SDL_GL_SwapWindow(window);
    }
    
    // Cleanup
    glDeleteTextures(1, &rangerTex);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    
    return 0;
}
