#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>

const float PI = 3.14159265358979323846f;

float cameraAngleX = 30.0f;
float cameraAngleY = 0.0f;
float cameraDistance = 15.0f;
int lastMouseX, lastMouseY;
bool mouseLeftDown = false;
bool mouseRightDown = false;

float blackHoleMass = 1.0f;
float accretionDiskRotation = 0.0f;
float timeScale = 1.0f;

struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float r, g, b;
    float life;
    float orbitRadius;
    float orbitAngle;
    float orbitSpeed;
};

std::vector<Particle> particles;
const int NUM_PARTICLES = 2000;

void initParticles() {
    srand(static_cast<unsigned>(time(nullptr)));
    particles.clear();
    
    for (int i = 0; i < NUM_PARTICLES; i++) {
        Particle p;
        p.orbitRadius = 2.5f + (rand() % 100) / 20.0f;
        p.orbitAngle = (rand() % 360) * PI / 180.0f;
        p.orbitSpeed = 0.5f / sqrt(p.orbitRadius);
        
        float tilt = ((rand() % 100) / 100.0f - 0.5f) * 0.3f;
        p.x = p.orbitRadius * cos(p.orbitAngle);
        p.y = tilt * p.orbitRadius * 0.1f;
        p.z = p.orbitRadius * sin(p.orbitAngle);
        
        float temp = 1.0f - (p.orbitRadius - 2.5f) / 5.0f;
        temp = fmax(0.0f, fmin(1.0f, temp));
        
        if (temp > 0.8f) {
            p.r = 1.0f; p.g = 1.0f; p.b = 0.9f;
        } else if (temp > 0.5f) {
            p.r = 1.0f; p.g = 0.8f; p.b = 0.3f;
        } else if (temp > 0.2f) {
            p.r = 1.0f; p.g = 0.4f; p.b = 0.1f;
        } else {
            p.r = 0.8f; p.g = 0.2f; p.b = 0.1f;
        }
        
        p.life = 1.0f;
        particles.push_back(p);
    }
}

void updateParticles(float dt) {
    for (auto& p : particles) {
        p.orbitAngle += p.orbitSpeed * dt * timeScale;
        
        float gravitationalPull = blackHoleMass * 0.01f / (p.orbitRadius * p.orbitRadius);
        p.orbitRadius -= gravitationalPull * dt * timeScale;
        
        if (p.orbitRadius < 1.5f) {
            p.orbitRadius = 2.5f + (rand() % 100) / 20.0f;
            p.orbitAngle = (rand() % 360) * PI / 180.0f;
            p.orbitSpeed = 0.5f / sqrt(p.orbitRadius);
        }
        
        float tilt = sin(p.orbitAngle * 3.0f) * 0.1f;
        p.x = p.orbitRadius * cos(p.orbitAngle);
        p.y = tilt * p.orbitRadius * 0.15f;
        p.z = p.orbitRadius * sin(p.orbitAngle);
    }
}

void drawSphere(float radius, int slices, int stacks) {
    for (int i = 0; i < stacks; i++) {
        float lat0 = PI * (-0.5f + (float)i / stacks);
        float z0 = sin(lat0);
        float zr0 = cos(lat0);
        
        float lat1 = PI * (-0.5f + (float)(i + 1) / stacks);
        float z1 = sin(lat1);
        float zr1 = cos(lat1);
        
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; j++) {
            float lng = 2 * PI * (float)j / slices;
            float x = cos(lng);
            float y = sin(lng);
            
            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(radius * x * zr0, radius * y * zr0, radius * z0);
            
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(radius * x * zr1, radius * y * zr1, radius * z1);
        }
        glEnd();
    }
}

void drawBlackHole() {
    glPushMatrix();
    
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    drawSphere(1.0f, 50, 50);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (int i = 0; i < 5; i++) {
        float radius = 1.0f + i * 0.15f;
        float alpha = 0.3f - i * 0.05f;
        
        glColor4f(0.1f, 0.0f, 0.2f, alpha);
        drawSphere(radius, 30, 30);
    }
    
    glDisable(GL_BLEND);
    glPopMatrix();
}

void drawEventHorizon() {
    glPushMatrix();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    for (int ring = 0; ring < 20; ring++) {
        float radius = 1.2f + ring * 0.05f;
        float alpha = 0.15f * (1.0f - ring / 20.0f);
        
        float hue = fmod(accretionDiskRotation * 0.1f + ring * 0.1f, 1.0f);
        float r = fabs(sin(hue * PI * 2));
        float g = fabs(sin((hue + 0.33f) * PI * 2)) * 0.3f;
        float b = fabs(sin((hue + 0.66f) * PI * 2));
        
        glColor4f(r * 0.5f, g * 0.2f, b, alpha);
        
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 60; i++) {
            float angle = i * 2 * PI / 60;
            float distortion = 1.0f + 0.1f * sin(angle * 3 + accretionDiskRotation);
            glVertex3f(radius * distortion * cos(angle), 0.0f, radius * distortion * sin(angle));
        }
        glEnd();
    }
    
    glDisable(GL_BLEND);
    glPopMatrix();
}

void drawAccretionDisk() {
    glPushMatrix();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (const auto& p : particles) {
        float alpha = 0.8f * (1.0f - (p.orbitRadius - 2.5f) / 5.0f);
        alpha = fmax(0.2f, fmin(1.0f, alpha));
        
        float glow = 1.0f + 0.3f * sin(accretionDiskRotation * 2.0f + p.orbitAngle);
        glColor4f(p.r * glow, p.g * glow, p.b * glow, alpha);
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();
    
    glDisable(GL_BLEND);
    glPopMatrix();
}

void drawSpacetimeGrid() {
    glPushMatrix();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    int gridSize = 30;
    float gridSpacing = 0.8f;
    float gridOffset = gridSize * gridSpacing / 2.0f;
    
    glLineWidth(1.0f);
    
    for (int i = 0; i <= gridSize; i++) {
        glBegin(GL_LINE_STRIP);
        for (int j = 0; j <= gridSize; j++) {
            float x = i * gridSpacing - gridOffset;
            float z = j * gridSpacing - gridOffset;
            
            float dist = sqrt(x * x + z * z);
            float curvature = 0.0f;
            
            if (dist > 1.5f) {
                curvature = -blackHoleMass * 3.0f / (dist * dist);
                curvature = fmax(curvature, -4.0f);
            } else {
                curvature = -4.0f;
            }
            
            float alpha = 0.4f * (1.0f - fabs(curvature) / 4.0f);
            alpha = fmax(0.1f, alpha);
            
            float colorIntensity = 1.0f - dist / (gridSize * gridSpacing / 2.0f);
            colorIntensity = fmax(0.3f, colorIntensity);
            
            glColor4f(0.2f * colorIntensity, 0.5f * colorIntensity, 1.0f * colorIntensity, alpha);
            glVertex3f(x, curvature, z);
        }
        glEnd();
    }
    
    for (int j = 0; j <= gridSize; j++) {
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= gridSize; i++) {
            float x = i * gridSpacing - gridOffset;
            float z = j * gridSpacing - gridOffset;
            
            float dist = sqrt(x * x + z * z);
            float curvature = 0.0f;
            
            if (dist > 1.5f) {
                curvature = -blackHoleMass * 3.0f / (dist * dist);
                curvature = fmax(curvature, -4.0f);
            } else {
                curvature = -4.0f;
            }
            
            float alpha = 0.4f * (1.0f - fabs(curvature) / 4.0f);
            alpha = fmax(0.1f, alpha);
            
            float colorIntensity = 1.0f - dist / (gridSize * gridSpacing / 2.0f);
            colorIntensity = fmax(0.3f, colorIntensity);
            
            glColor4f(0.2f * colorIntensity, 0.5f * colorIntensity, 1.0f * colorIntensity, alpha);
            glVertex3f(x, curvature, z);
        }
        glEnd();
    }
    
    glDisable(GL_BLEND);
    glPopMatrix();
}

void drawStars() {
    glPushMatrix();
    glPointSize(1.5f);
    glBegin(GL_POINTS);
    
    srand(12345);
    for (int i = 0; i < 500; i++) {
        float theta = (rand() % 360) * PI / 180.0f;
        float phi = (rand() % 180 - 90) * PI / 180.0f;
        float radius = 50.0f;
        
        float x = radius * cos(phi) * cos(theta);
        float y = radius * sin(phi);
        float z = radius * cos(phi) * sin(theta);
        
        float brightness = 0.5f + (rand() % 50) / 100.0f;
        glColor3f(brightness, brightness, brightness * 1.1f);
        glVertex3f(x, y, z);
    }
    
    glEnd();
    glPopMatrix();
}

void drawGravitationalLensing() {
    glPushMatrix();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    for (int ring = 0; ring < 10; ring++) {
        float radius = 1.5f + ring * 0.3f;
        float alpha = 0.1f * (1.0f - ring / 10.0f);
        
        glColor4f(1.0f, 0.8f, 0.5f, alpha);
        
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 100; i++) {
            float angle = i * 2 * PI / 100;
            float wobble = 1.0f + 0.05f * sin(angle * 5 + accretionDiskRotation * 0.5f);
            
            float x = radius * wobble * cos(angle);
            float y = radius * wobble * sin(angle) * 0.3f;
            float z = radius * wobble * sin(angle) * cos(cameraAngleX * PI / 180.0f);
            
            glVertex3f(x, y, z);
        }
        glEnd();
    }
    
    glDisable(GL_BLEND);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    float camX = cameraDistance * sin(cameraAngleY * PI / 180.0f) * cos(cameraAngleX * PI / 180.0f);
    float camY = cameraDistance * sin(cameraAngleX * PI / 180.0f);
    float camZ = cameraDistance * cos(cameraAngleY * PI / 180.0f) * cos(cameraAngleX * PI / 180.0f);
    
    gluLookAt(camX, camY, camZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    
    drawStars();
    drawSpacetimeGrid();
    drawBlackHole();
    drawEventHorizon();
    drawAccretionDisk();
    drawGravitationalLensing();
    
    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    float ratio = (float)w / (float)h;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int value) {
    float dt = 0.016f;
    
    accretionDiskRotation += dt * timeScale;
    updateParticles(dt);
    
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
        case 'q':
        case 'Q':
            exit(0);
            break;
        case ' ':
            timeScale = (timeScale > 0.0f) ? 0.0f : 1.0f;
            break;
        case 'r':
        case 'R':
            initParticles();
            blackHoleMass = 1.0f;
            cameraAngleX = 30.0f;
            cameraAngleY = 0.0f;
            cameraDistance = 15.0f;
            break;
        case '+':
        case '=':
            blackHoleMass += 0.1f;
            if (blackHoleMass > 3.0f) blackHoleMass = 3.0f;
            break;
        case '-':
        case '_':
            blackHoleMass -= 0.1f;
            if (blackHoleMass < 0.5f) blackHoleMass = 0.5f;
            break;
    }
    glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseLeftDown = true;
            lastMouseX = x;
            lastMouseY = y;
        } else {
            mouseLeftDown = false;
        }
    }
    
    if (button == 3) {
        cameraDistance -= 0.5f;
        if (cameraDistance < 5.0f) cameraDistance = 5.0f;
    }
    if (button == 4) {
        cameraDistance += 0.5f;
        if (cameraDistance > 30.0f) cameraDistance = 30.0f;
    }
    
    glutPostRedisplay();
}

void mouseMotion(int x, int y) {
    if (mouseLeftDown) {
        cameraAngleY += (x - lastMouseX) * 0.5f;
        cameraAngleX += (y - lastMouseY) * 0.5f;
        
        if (cameraAngleX > 89.0f) cameraAngleX = 89.0f;
        if (cameraAngleX < -89.0f) cameraAngleX = -89.0f;
        
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

void initOpenGL() {
    glClearColor(0.0f, 0.0f, 0.02f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Black Hole Simulation - 3D Spacetime");
    
    initOpenGL();
    initParticles();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutTimerFunc(0, timer, 0);
    
    glutMainLoop();
    
    return 0;
}
