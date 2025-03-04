#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include <iostream>
#include <fstream>


GLint programID;

// 定义对象结构体
struct GameObject {
    glm::mat4 model;
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    int indexCount;
};

std::vector<GameObject> objects;
int selectedObject = 0;
glm::mat4 view;
glm::vec3 cameraPos = glm::vec3(3.0f, 3.0f, 3.0f);

//鼠标事件定义
bool mousePressed = false;
double lastX = 400, lastY = 300;
// 摄像机参数
float theta = 0.0f;    // 绕 Z 轴的旋转角度（弧度）
float radius = 5.0f;   // 摄像机到原点的距离
float height = 2.0f;   // 摄像机高度（Y 轴位置）
bool rightMousePressed = false;

// 添加新的函数声明
void createPlane(GameObject& obj);
void createCube(GameObject& obj);
void createPyramid(GameObject& obj);
void updateTransformation();
glm::mat4 calculate_view_matrix();

// 在sendDataToOpenGL中创建对象
void sendDataToOpenGL() {
    //glEnable(GL_DEPTH_TEST);

    // 创建平面（2D）
    GameObject plane;
    createPlane(plane);
    objects.push_back(plane);

    // 创建立方体（3D）
    GameObject cube;
    createCube(cube);
    objects.push_back(cube);

    // 创建金字塔（3D）
    GameObject pyramid;
    createPyramid(pyramid);
    objects.push_back(pyramid);

    // 初始化视图矩阵
    view = calculate_view_matrix();
}
// 创建平面数据
void createPlane(GameObject& obj) {
    float vertices[] = {
        // 位置          // 颜色
        -1.0f, 0.0f, -1.0f, 0.8f, 0.8f, 0.8f,
         1.0f, 0.0f, -1.0f, 0.8f, 0.8f, 0.8f,
         1.0f, 0.0f,  1.0f, 0.8f, 0.8f, 0.8f,
        -1.0f, 0.0f,  1.0f, 0.8f, 0.8f, 0.8f,
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &obj.VAO);
    glGenBuffers(1, &obj.VBO);
    glGenBuffers(1, &obj.EBO);

    glBindVertexArray(obj.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    obj.indexCount = 6;
    obj.model = glm::mat4(1.0f);
}

void createCube(GameObject& obj) {
    float vertices[] = {
        // 位置              // 颜色
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,
        -0.5f,  0.5f,  0.5f, 0.2f, 0.8f, 0.2f,
    };

    unsigned int indices[] = {
        0,1,2, 2,3,0,   // 前
        4,5,6, 6,7,4,   // 后
        0,4,7, 7,3,0,   // 左
        1,5,6, 6,2,1,   // 右
        3,2,6, 6,7,3,   // 上
        0,1,5, 5,4,0    // 下
    };

    glGenVertexArrays(1, &obj.VAO);
    glGenBuffers(1, &obj.VBO);
    glGenBuffers(1, &obj.EBO);

    glBindVertexArray(obj.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    obj.indexCount = sizeof(indices) / sizeof(unsigned int);
    obj.model = glm::mat4(1.0f);
}

// 创建金字塔（使用索引）
void createPyramid(GameObject& obj) {
    float vertices[] = {
        // 位置             // 颜色
        // 底面
        -0.5f, -0.5f, -0.5f, 1.0f, 0.5f, 0.2f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.5f, 0.2f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.5f, 0.2f,
        -0.5f, -0.5f,  0.5f, 1.0f, 0.5f, 0.2f,
        // 顶点
         0.0f,  0.5f,  0.0f, 0.8f, 0.2f, 0.5f
    };

    unsigned int indices[] = {
        // 底面
        0, 1, 2,
        2, 3, 0,
        // 侧面
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };

    glGenVertexArrays(1, &obj.VAO);
    glGenBuffers(1, &obj.VBO);
    glGenBuffers(1, &obj.EBO);

    glBindVertexArray(obj.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    obj.indexCount = sizeof(indices) / sizeof(unsigned int);
    obj.model = glm::mat4(1.0f);
}

void get_OpenGL_info() {
    // OpenGL information
    const GLubyte* name = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* glversion = glGetString(GL_VERSION);
    std::cout << "OpenGL company: " << name << std::endl;
    std::cout << "Renderer name: " << renderer << std::endl;
    std::cout << "OpenGL version: " << glversion << std::endl;
}

bool checkStatus(
    GLuint objectID,
    PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
    PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
    GLenum statusType)
{
    GLint status;
    objectPropertyGetterFunc(objectID, statusType, &status);
    if (status != GL_TRUE)
    {
        GLint infoLogLength;
        objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* buffer = new GLchar[infoLogLength];

        GLsizei bufferSize;
        getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
        std::cout << buffer << std::endl;

        delete[] buffer;
        return false;
    }
    return true;
}

bool checkShaderStatus(GLuint shaderID) {
    return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID) {
    return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

std::string readShaderCode(const char* fileName) {
    std::ifstream meInput(fileName);
    if (!meInput.good()) {
        std::cout << "File failed to load ... " << fileName << std::endl;
        exit(1);
    }
    return std::string(
        std::istreambuf_iterator<char>(meInput),
        std::istreambuf_iterator<char>()
    );
}

void installShaders() {
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* adapter[1];
    //adapter[0] = vertexShaderCode;
    std::string temp = readShaderCode("VertexShaderCode.glsl");
    adapter[0] = temp.c_str();
    glShaderSource(vertexShaderID, 1, adapter, 0);
    //adapter[0] = fragmentShaderCode;
    temp = readShaderCode("FragmentShaderCode.glsl");
    adapter[0] = temp.c_str();
    glShaderSource(fragmentShaderID, 1, adapter, 0);

    glCompileShader(vertexShaderID);
    glCompileShader(fragmentShaderID);

    if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
        return;

    programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    if (!checkProgramStatus(programID))
        return;

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    glUseProgram(programID);
}

void paintGL(void) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspectRatio = 1920 / 1080;

    // 透视投影矩阵（动态宽高比）
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    GLint modelLoc = glGetUniformLocation(programID, "model");
    GLint viewLoc = glGetUniformLocation(programID, "view");
    GLint projLoc = glGetUniformLocation(programID, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

    for (auto& obj : objects) {
        glBindVertexArray(obj.VAO);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &obj.model[0][0]);
        glDrawElements(GL_TRIANGLES, obj.indexCount, GL_UNSIGNED_INT, 0);
    }
}


// 键盘回调函数
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        glm::mat4& model = objects[selectedObject].model;

        switch (key) {
        case GLFW_KEY_W: model = glm::translate(model, glm::vec3(0, 0.1, 0)); break;
        case GLFW_KEY_S: model = glm::translate(model, glm::vec3(0, -0.1, 0)); break;
        case GLFW_KEY_A: model = glm::translate(model, glm::vec3(-0.1, 0, 0)); break;
        case GLFW_KEY_D: model = glm::translate(model, glm::vec3(0.1, 0, 0)); break;
        case GLFW_KEY_Q: model = glm::rotate(model, 0.1f, glm::vec3(0, 1, 0)); break;
        case GLFW_KEY_E: model = glm::rotate(model, -0.1f, glm::vec3(0, 1, 0)); break;
        case GLFW_KEY_Z: model = glm::scale(model, glm::vec3(1.1)); break;
        case GLFW_KEY_X: model = glm::scale(model, glm::vec3(0.9)); break;
        case GLFW_KEY_1: selectedObject = 0; break;
        case GLFW_KEY_2: selectedObject = 1; break;
        case GLFW_KEY_3: selectedObject = 2; break;
        }
    }
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        mousePressed = (action == GLFW_PRESS);
        glfwGetCursorPos(window, &lastX, &lastY);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            rightMousePressed = true;
            glfwGetCursorPos(window, &lastX, &lastY); // 记录按下时的位置
        }
        else if (action == GLFW_RELEASE) {
            rightMousePressed = false;
        }
    }
}

glm::mat4 calculate_view_matrix() {
    // 根据极坐标计算摄像机位置
    float x = radius * cos(theta);
    float z = radius * sin(theta);
    glm::vec3 cameraPos = glm::vec3(x, height, z);

    // 焦点始终在原点，up 方向为 Y 轴
    return glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mousePressed) {
        double dx = xpos - lastX;
        double dy = ypos - lastY;
        lastX = xpos;
        lastY = ypos;

        glm::mat4& model = objects[selectedObject].model;
        model = glm::rotate(model, (float)dx * 0.01f, glm::vec3(0, 1, 0));
        model = glm::rotate(model, (float)dy * 0.01f, glm::vec3(1, 0, 0));
    }
    static double lastX = 400, lastY = 300; // 初始位置

    if (rightMousePressed) {
        // 计算偏移量
        float xoffset = xpos - lastX;
        float yoffset = ypos - lastY;
        lastX = xpos;
        lastY = ypos;

        // 控制灵敏度
        float sensitivity = 0.005f;
        theta -= xoffset * sensitivity; // 水平移动控制旋转角度
        height += yoffset * 0.01f;       // 垂直移动控制高度

        // 限制高度范围（可选）
        if (height < 0.1f) height = 0.1f;
        if (height > 10.0f) height = 10.0f;

        // 更新视图矩阵
        view = calculate_view_matrix();
    }
    view = calculate_view_matrix();
}

void initializedGL(void) {
    // run only once
    glewExperimental = GL_TRUE;
    glewInit();
    get_OpenGL_info();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    sendDataToOpenGL();
    installShaders();
}

int main(int argc, char* argv[]) {
    GLFWwindow* window;

    /* Initialize the glfw */
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    /* glfw: configure; necessary for MAC */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    /* do not allow resizing */
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1920, 1080, "Assignment 1", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback); // TODO
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    // TODO: mouse callback, etc.

    /* Initialize the glew */
    if (GLEW_OK != glewInit()) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    get_OpenGL_info();
    initializedGL();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        paintGL();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
