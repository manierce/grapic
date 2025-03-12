#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <sstream>
#include <map>

#include <iostream>
#include <fstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

GLint programID;

// �������ṹ��
struct GameObject {
    glm::mat4 model;
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    int indexCount;
};
// �������ݽṹ��λ�� + �������� + ���ߣ�
struct VertexData {
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<GameObject> objects;
int selectedObject = 0;
glm::mat4 view;
glm::vec3 cameraPos = glm::vec3(3.0f, 3.0f, 3.0f);

//����¼�����
bool mousePressed = false;
double lastX = 400, lastY = 300;
// ���������
float theta = 0.0f;    // �� Z �����ת�Ƕȣ����ȣ�
float radius = 5.0f;   // �������ԭ��ľ���
float height = 2.0f;   // ������߶ȣ�Y ��λ�ã�
bool rightMousePressed = false;

// ����µĺ�������
void createPlane(GameObject& obj);
void createCube(GameObject& obj);
void createPyramid(GameObject& obj);
void createFromOBJ(GameObject& obj, const std::string& path);
void updateTransformation();
glm::mat4 calculate_view_matrix();

// ��sendDataToOpenGL�д�������
void sendDataToOpenGL() {
    //glEnable(GL_DEPTH_TEST);

    // ����ƽ�棨2D��
    GameObject plane;
    createPlane(plane);
    objects.push_back(plane);

    // ���������壨3D��
    GameObject cube;
    createCube(cube);
    objects.push_back(cube);

    // ������������3D��
    GameObject pyramid;
    createPyramid(pyramid);
    objects.push_back(pyramid);

    // ���ģ��4����OBJ�ļ����أ�
    GameObject obj4;
    createFromOBJ(obj4, "obj.txt"); // ȷ��·����ȷ
    objects.push_back(obj4);
    // ��ʼ����ͼ����
    view = calculate_view_matrix();
}
// ����ƽ������
void createPlane(GameObject& obj) {
    float vertices[] = {
        // λ��          // ��ɫ
        -100.0f, -1.0f, -100.0f, 0.8f, 0.8f, 0.8f,
         100.0f, -1.0f, -100.0f, 0.8f, 0.8f, 0.8f,
         100.0f, -1.0f,  100.0f, 0.8f, 0.8f, 0.8f,
        -100.0f, -1.0f,  100.0f, 0.8f, 0.8f, 0.8f,
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

    // λ������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // ��ɫ����
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    obj.indexCount = 6;
    obj.model = glm::mat4(1.0f);
}

void createCube(GameObject& obj) {
    float vertices[] = {
        // λ��              // ��ɫ
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
        0,1,2, 2,3,0,   // ǰ
        4,5,6, 6,7,4,   // ��
        0,4,7, 7,3,0,   // ��
        1,5,6, 6,2,1,   // ��
        3,2,6, 6,7,3,   // ��
        0,1,5, 5,4,0    // ��
    };

    glGenVertexArrays(1, &obj.VAO);
    glGenBuffers(1, &obj.VBO);
    glGenBuffers(1, &obj.EBO);

    glBindVertexArray(obj.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // λ������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // ��ɫ����
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    obj.indexCount = sizeof(indices) / sizeof(unsigned int);
    obj.model = glm::mat4(1.0f);
}

// ������������ʹ��������
void createPyramid(GameObject& obj) {
    float vertices[] = {
        // λ��             // ��ɫ
        // ����
        -0.5f, -0.5f, -0.5f, 1.0f, 0.5f, 0.2f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.5f, 0.2f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.5f, 0.2f,
        -0.5f, -0.5f,  0.5f, 1.0f, 0.5f, 0.2f,
        // ����
         0.0f,  0.5f,  0.0f, 0.8f, 0.2f, 0.5f
    };

    unsigned int indices[] = {
        // ����
        0, 1, 2,
        2, 3, 0,
        // ����
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

//����obj
void createFromOBJ(GameObject& obj, const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open file " << path << std::endl;
        return;
    }

    // ��ʱ�洢ԭʼ����
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;

    // ���ն������������
    std::vector<VertexData> vertices;
    std::vector<unsigned int> indices;
    std::map<std::string, unsigned int> vertexToIndex;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            // ������λ�ã�v x y z
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (type == "vt") {
            // �����������꣺vt u v
            glm::vec2 tex;
            iss >> tex.x >> tex.y;
            texCoords.push_back(tex);
        }
        else if (type == "vn") {
            // �����ߣ�vn nx ny nz
            glm::vec3 norm;
            iss >> norm.x >> norm.y >> norm.z;
            normals.push_back(norm);
        }
        else if (type == "f") {
            // �����棺f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
            for (int i = 0; i < 3; ++i) {
                std::string vertexStr;
                iss >> vertexStr;
                std::vector<std::string> parts = split(vertexStr, '/');

                // ����������֧�ָ���������ȱʧ�
                int posIdx = 0, texIdx = -1, normIdx = -1;
                if (!parts[0].empty()) posIdx = std::stoi(parts[0]);
                if (parts.size() > 1 && !parts[1].empty()) texIdx = std::stoi(parts[1]);
                if (parts.size() > 2 && !parts[2].empty()) normIdx = std::stoi(parts[2]);

                // ��������������� -1 ��ʾ���һ��Ԫ�أ�
                posIdx = (posIdx < 0) ? positions.size() + posIdx : posIdx - 1;
                if (texIdx != -1) texIdx = (texIdx < 0) ? texCoords.size() + texIdx : texIdx - 1;
                if (normIdx != -1) normIdx = (normIdx < 0) ? normals.size() + normIdx : normIdx - 1;

                // ��϶�������
                VertexData vertex;
                vertex.position = positions[posIdx];
                if (texIdx >= 0 && texIdx < texCoords.size()) vertex.texCoord = texCoords[texIdx];
                if (normIdx >= 0 && normIdx < normals.size()) vertex.normal = normals[normIdx];

                // ����Ƿ��Ѵ�����ͬ����
                std::string key = vertexStr;
                if (vertexToIndex.find(key) == vertexToIndex.end()) {
                    vertexToIndex[key] = static_cast<unsigned int>(vertices.size());
                    vertices.push_back(vertex);
                }
                indices.push_back(vertexToIndex[key]);
            }
        }
    }

    // ת��Ϊ OpenGL ���õĶ������飨λ�� + �������� + ���ߣ�
    std::vector<float> vertexBuffer;
    for (const auto& v : vertices) {
        vertexBuffer.push_back(v.position.x);
        vertexBuffer.push_back(v.position.y);
        vertexBuffer.push_back(v.position.z);
        vertexBuffer.push_back(v.texCoord.x);
        vertexBuffer.push_back(v.texCoord.y);
        vertexBuffer.push_back(v.normal.x);
        vertexBuffer.push_back(v.normal.y);
        vertexBuffer.push_back(v.normal.z);
    }

    // ���� OpenGL ������
    glGenVertexArrays(1, &obj.VAO);
    glGenBuffers(1, &obj.VBO);
    glGenBuffers(1, &obj.EBO);

    glBindVertexArray(obj.VAO);

    // ��� VBO
    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), vertexBuffer.data(), GL_STATIC_DRAW);

    // ��� EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // ���ö�������ָ��
    // λ�����ԣ�0��
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // �����������ԣ�1��
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // �������ԣ�2��
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    obj.indexCount = indices.size();
    obj.model = glm::mat4(1.0f);

    std::cout << "Loaded OBJ-TXT model: " << path
        << " (" << vertices.size() << " vertices, "
        << indices.size() / 3 << " faces)" << std::endl;
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

    // ͸��ͶӰ���󣨶�̬��߱ȣ�
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


// ���̻ص�����
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        glm::mat4& model = objects[selectedObject].model;

        switch (key) {
        case GLFW_KEY_W: model = glm::translate(model, glm::vec3(0, 0.1, 0)); break;
        case GLFW_KEY_S: model = glm::translate(model, glm::vec3(0, -0.1, 0)); break;
        case GLFW_KEY_A: model = glm::translate(model, glm::vec3(-0.1, 0, 0)); break;
        case GLFW_KEY_D: model = glm::translate(model, glm::vec3(0.1, 0, 0)); break;
        case GLFW_KEY_F: model = glm::translate(model, glm::vec3(0, 0, 0.1)); break;
        case GLFW_KEY_B: model = glm::translate(model, glm::vec3(0, 0, -0.1)); break;
        case GLFW_KEY_Q: model = glm::rotate(model, 0.5f, glm::vec3(0, 1, 0)); break;
        case GLFW_KEY_E: model = glm::rotate(model, -0.5f, glm::vec3(0, 1, 0)); break;
        case GLFW_KEY_Z: model = glm::scale(model, glm::vec3(1.1)); break;
        case GLFW_KEY_X: model = glm::scale(model, glm::vec3(0.9)); break;
        case GLFW_KEY_1: selectedObject = 0; break;
        case GLFW_KEY_2: selectedObject = 1; break;
        case GLFW_KEY_3: selectedObject = 2; break;
        case GLFW_KEY_4: selectedObject = 3; break;
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
            glfwGetCursorPos(window, &lastX, &lastY); // ��¼����ʱ��λ��
        }
        else if (action == GLFW_RELEASE) {
            rightMousePressed = false;
        }
    }
}

glm::mat4 calculate_view_matrix() {
    // ���ݼ�������������λ��
    float x = radius * cos(theta);
    float z = radius * sin(theta);
    glm::vec3 cameraPos = glm::vec3(x, height, z);

    // ����ʼ����ԭ�㣬up ����Ϊ Y ��
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
    
    if (rightMousePressed) {
        // ����ƫ����
        float xoffset = xpos - lastX;
        float yoffset = ypos - lastY;
        lastX = xpos;
        lastY = ypos;

        // ����������
        float sensitivity = 0.002f;
        theta -= xoffset * sensitivity; // ˮƽ�ƶ�������ת�Ƕ�
        height += yoffset * 0.01f;       // ��ֱ�ƶ����Ƹ߶�

        // ���Ƹ߶ȷ�Χ����ѡ��
        if (height < 0.1f) height = 0.1f;
        if (height > 10.0f) height = 10.0f;
    }
    view = calculate_view_matrix();
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    radius -= yoffset * 0.5f;
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
    glfwSetScrollCallback(window, scroll_callback);
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
