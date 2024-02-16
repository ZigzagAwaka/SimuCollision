#include "engine.hpp"


int window_width = 1000;
int window_height = 1000;

Camera camera(0); // 0=trackball, 1=freefly
int INITIAL_DISTANCE = 3000; // initial distance of camera
float PERSPEC_FAR = 100000.0f; // max distance for objects rendering, compared to the camera
bool lowConfig = false; // set this to true if you have a bad pc and needs the models to be less detailed

/* Main fonction of the engine */
void visusyssol(GLFWwindow* window, glimac::FilePath applicationPath);

/* Main structure containing every planets data */
PlanetInfo planetInfo;


// ============================================================
// CALLBACKS (user inputs)
// ============================================================

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    if(action == GLFW_PRESS) {
        switch(key) {
            case GLFW_KEY_P: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
            case GLFW_KEY_L: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
            case GLFW_KEY_F: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
            case GLFW_KEY_O: planetInfo.modifyDrawOrbit(); break;
            case GLFW_KEY_V: planetInfo.modifyChosenView(); break;
            case GLFW_KEY_W: camera.moveFront(-1.0); break;
            case GLFW_KEY_A: camera.moveLeft(1.0); break;
            case GLFW_KEY_S: camera.moveFront(1.0); break;
            case GLFW_KEY_D: camera.moveLeft(-1.0); break;
            case GLFW_KEY_T: camera.switchType(); break;
            case GLFW_KEY_KP_SUBTRACT: planetInfo.modifySpeed(-100.0); break;
            case GLFW_KEY_KP_ADD: planetInfo.modifySpeed(100.0); break;
            case GLFW_KEY_SPACE: planetInfo.pauseTime(); break;
            case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, 1); break;
            default: break;
        }
    }
}

static void mouse_button_callback(GLFWwindow* /*window*/, int /*button*/, int /*action*/, int /*mods*/) {}

static void scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset) {
    camera.moveFront(yoffset);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ||
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        camera.rotateLeft((xpos - (window_width/2)) / (window_width/2));
        camera.rotateUp(((window_height/2) - ypos) / (window_height/2));
    }
}

static void size_callback(GLFWwindow* /*window*/, int width, int height) {
    window_width  = width;
    window_height = height;
}


// ============================================================


int main(int argc, char** argv) {
    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

#ifdef __APPLE__
    /* We need to explicitly ask for a 3.3 context on Mac */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    /* Create a window and its OpenGL context */
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "VisuSysSol", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    /* Intialize glad (loads the OpenGL functions) */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    /* Hook input callbacks */
    glfwSetKeyCallback(window, &key_callback);
    glfwSetMouseButtonCallback(window, &mouse_button_callback);
    glfwSetScrollCallback(window, &scroll_callback);
    glfwSetCursorPosCallback(window, &cursor_position_callback);
    glfwSetWindowSizeCallback(window, &size_callback);

    std::srand(time(0));
    glEnable(GL_DEPTH_TEST);
    std::cout << "Launching... " << argc << " " << argv << std::endl;
    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;

    /* Launch engine */
    glimac::FilePath applicationPath(argv[0]);
    visusyssol(window, applicationPath);
    
    glfwTerminate();
    return 0;
}


void visusyssol(GLFWwindow* window, glimac::FilePath applicationPath) {
    ClassicProgram classicObj(applicationPath);
    StarProgram star(applicationPath);
    PlanetProgram planet(applicationPath);
    
    std::vector<GLuint> textureObjects = createTextureObjects(applicationPath.dirPath());
    std::vector<Model> models = createModels(lowConfig);

    while (!glfwWindowShouldClose(window)) { // main loop
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        std::vector<glm::mat4> matrix(3); // 0=ProjMatrix, 1=globalMVMatrix, 2=viewMatrix
        matrix[0] = glm::perspective(glm::radians(70.0f), float(window_width/window_height), 0.1f, PERSPEC_FAR);
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -1*INITIAL_DISTANCE));
        matrix[2] = camera.getViewMatrix();
        matrix[1] = camera.getGlobalMVMatrix(modelMatrix);

        drawEverything(&star, &planet, &classicObj, planetInfo, textureObjects, models, matrix); // main engine func
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glfwSwapBuffers(window); // Update the display
        glfwPollEvents(); // Poll for and process events
    }

    glDeleteTextures(textureObjects.size(), textureObjects.data());
    glDeleteBuffers(models.size(), getDataOfModels(models, 0));
    glDeleteVertexArrays(models.size(), getDataOfModels(models, 1));
}
