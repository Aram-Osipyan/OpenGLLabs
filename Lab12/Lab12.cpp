// Lab12.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
struct vec3 {
    GLfloat x;
    GLfloat y;
    GLfloat z;
public:
    vec3(GLfloat x, GLfloat y, GLfloat z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};
class Mesh {
public:
    Mesh();
    Mesh(std::string mesh_filename, const char* texture_filename = "image.jpg");

    void Draw();
    void Draw(vec3 lightpos, vec3 view);
    void IncXPos(float d);
    void IncYPos(float d);
    void Release();

private:
   
    struct vec2 {
        GLfloat x;
        GLfloat y;
    };
    struct Vertex {
        // position
        vec3 Position;
        // normal
        vec3 Normal;
        // texCoords
        vec2 TexCoords;

    };

    // ID шейдерной программы
    GLuint Program;

    // ID атрибута вершин
    GLint Attrib_vertex;

    // ID атрибута цвета
    GLint Attrib_color;

    // ID юниформ переменной перемешения
    GLint Unif_posX;
    GLint Unif_posY;
    GLint unifTexture;
    GLint unifLightPos;
    GLint unifViewVec;
    // ID Vertex Buffer Object
    GLuint VBO;
    GLuint VAO;
    GLuint IBO;
    
    std::vector<Vertex> vertices;
    std::vector<vec3> positions;
    std::vector<vec3> normals;
    std::vector<vec2> uvs;

    std::vector<unsigned int> indices;
    // ID текстуры
    GLint textureHandle;
    // SFML текстура
    sf::Texture textureData;

    float posX = 0.0f, posY = 0.0f;

    // Исходный код вершинного шейдера
    const char* VertexShaderSource = R"(
    #version 330 core
    uniform float positX;
    uniform float positY;
    uniform vec3 lightPos;
    uniform vec3 view;

    layout (location = 0)in vec3 coord;
    layout (location = 1)in vec3 normal;
    layout (location = 2)in vec2 uv;
    out vec2 textcoord;
    out float constant;
    void main() {       
        float x_angle = 1 + positX;
        float y_angle = 1 + positY;
        
        
        vec3 position = coord * mat3(
            1, 0, 0,
            0, cos(x_angle), -sin(x_angle),
            0, sin(x_angle), cos(x_angle)
        ) * mat3(
            cos(y_angle), 0, sin(y_angle),
            0, 1, 0,
            -sin(y_angle), 0, cos(y_angle)
        );
        vec3 normal2 = normal * mat3(
            1, 0, 0,
            0, cos(x_angle), -sin(x_angle),
            0, sin(x_angle), cos(x_angle)
        ) * mat3(
            cos(y_angle), 0, sin(y_angle),
            0, 1, 0,
            -sin(y_angle), 0, cos(y_angle)
        );
        
        gl_Position = vec4(position, 1.0);
        textcoord = uv;
        constant = dot(normal2,position-view);
    }
    )";

    // Исходный код фрагментного шейдера
    const char* FragShaderSource = R"(
        #version 330 core
        in vec2 textcoord;
        in float constant;
        out vec4 color;
        uniform sampler2D textureData;
        void main() {
            color = constant*texture(textureData, textcoord);
        }
        )";
    // Проверка ошибок OpenGL, если есть то вывод в консоль тип ошибки
    void checkOpenGLerror();

    // Функция печати лога шейдера
    void ShaderLog(unsigned int shader);

    void InitVBO();

    void InitShader();

    void InitTexture(const char* filename = "image.jpg");

    void Init();

    // Освобождение шейдеров
    void ReleaseShader();

    // Освобождение буфера
    void ReleaseVBO();

    void ReadVectorsFromFile(std::string filename);

    void SetupMesh(std::string filename);
};
int main()
{
    sf::Window window(sf::VideoMode(600, 600), "My OpenGL window", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);

    window.setActive(true);

    // Инициализация glew
    glewInit();

    //Object obj = Object("cube.obj");
    //Mesh obj = Mesh("cube.obj", "image.jpg");
    Mesh obj = Mesh("file.obj", "image.jpg");
    //Mesh obj = Mesh("triangle.obj");
    //Mesh obj = Mesh("FinalBaseMesh.obj");
    //Mesh obj = Mesh();
    // lighting
    vec3 lightPos(1.2f, 1.0f, 2.0f);
    vec3 view(1.2f, 1.0f, 2.0f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
            }
            // обработка нажатий клавиш
            else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case (sf::Keyboard::A): obj.IncXPos(-0.05f); break;
                case (sf::Keyboard::D): obj.IncXPos(0.05f); break;
                case (sf::Keyboard::W): obj.IncYPos(-0.05f); break;
                case (sf::Keyboard::S): obj.IncYPos(0.05f); break;
                default: break;
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        obj.Draw(lightPos, view);

        window.display();
    }

    obj.Release();
    return 0;
}

inline Mesh::Mesh() {
    Init();
}

inline Mesh::Mesh(std::string mesh_filename, const char* texture_filename) {
    InitShader();
    InitTexture(texture_filename);
    SetupMesh(mesh_filename);
    glEnable(GL_DEPTH_TEST);
}

inline void Mesh::Draw() {
    glUseProgram(Program);
    glUniform1f(Unif_posX, posX);
    glUniform1f(Unif_posY, posY);
    glActiveTexture(GL_TEXTURE0);
    // Обёртка SFML на opengl функцией glBindTexture
    sf::Texture::bind(&textureData);
    glUniform1i(unifTexture, 0);
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

}

inline void Mesh::Draw(vec3 lightpos, vec3 view) {
    glUseProgram(Program);
    glUniform1f(Unif_posX, posX);
    glUniform1f(Unif_posY, posY);
    glUniform3f(unifViewVec, view.x, view.y, view.z);
    glActiveTexture(GL_TEXTURE0);
    // Обёртка SFML на opengl функцией glBindTexture
    sf::Texture::bind(&textureData);
    glUniform1i(unifTexture, 0);
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

}

inline void Mesh::IncXPos(float d) {
    posX += d;
}

inline void Mesh::IncYPos(float d) {
    posY += d;
}

inline void Mesh::Release() {
    ReleaseShader();
    ReleaseVBO();
}

// Проверка ошибок OpenGL, если есть то вывод в консоль тип ошибки

inline void Mesh::checkOpenGLerror() {
    GLenum errCode;
    // Коды ошибок можно смотреть тут
    // https://www.khronos.org/opengl/wiki/OpenGL_Error
    if ((errCode = glGetError()) != GL_NO_ERROR)
        std::cout << "OpenGl error!: " << errCode << std::endl;
}

// Функция печати лога шейдера

inline void Mesh::ShaderLog(unsigned int shader)
{
    int infologLen = 0;
    int charsWritten = 0;
    char* infoLog;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
    if (infologLen > 1)
    {
        infoLog = new char[infologLen];
        if (infoLog == NULL)
        {
            std::cout << "ERROR: Could not allocate InfoLog buffer" << std::endl;
            exit(1);
        }
        glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
        std::cout << "InfoLog: " << infoLog << "\n\n\n";
        delete[] infoLog;
    }
}

inline void Mesh::InitVBO()
{
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };
    glGenVertexArrays(1, &VAO);
    glGenVertexArrays(1, &VBO);
    glGenBuffers(1, &IBO);
    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

inline void Mesh::InitShader() {
    // Создаем вершинный шейдер
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    // Передаем исходный код
    glShaderSource(vShader, 1, &VertexShaderSource, NULL);
    // Компилируем шейдер
    glCompileShader(vShader);
    std::cout << "vertex shader \n";
    ShaderLog(vShader);

    // Создаем фрагментный шейдер
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    // Передаем исходный код
    glShaderSource(fShader, 1, &FragShaderSource, NULL);
    // Компилируем шейдер
    glCompileShader(fShader);
    std::cout << "fragment shader \n";
    ShaderLog(fShader);

    // Создаем программу и прикрепляем шейдеры к ней
    Program = glCreateProgram();
    glAttachShader(Program, vShader);
    glAttachShader(Program, fShader);

    // Линкуем шейдерную программу
    glLinkProgram(Program);
    // Проверяем статус сборки
    int link_ok;
    glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
    if (!link_ok)
    {
        std::cout << "error attach shaders \n";
        return;
    }

    // Вытягиваем ID атрибута вершин из собранной программы
    Attrib_vertex = glGetAttribLocation(Program, "coord");
    if (Attrib_vertex == -1)
    {
        std::cout << "could not bind attrib coord" << std::endl;
        return;
    }

    const char* unifX_name = "positX";
    Unif_posX = glGetUniformLocation(Program, unifX_name);
    if (Unif_posX == -1)
    {
        std::cout << "could not bind uniform " << unifX_name << std::endl;
        return;
    }

    const char* unifY_name = "positY";
    Unif_posY = glGetUniformLocation(Program, unifY_name);
    if (Unif_posY == -1)
    {
        std::cout << "could not bind uniform " << unifY_name << std::endl;
        return;
    }
    unifTexture = glGetUniformLocation(Program, "textureData");
    if (unifTexture == -1)
    {
        std::cout << "could not bind uniform textureData" << std::endl;
        return;
    }
    unifLightPos = glGetUniformLocation(Program, "lightPos");
    if (unifTexture == -1)
    {
        std::cout << "could not bind uniform lightPos" << std::endl;
        return;
    }
    unifViewVec = glGetUniformLocation(Program, "view");
    if (unifTexture == -1)
    {
        std::cout << "could not bind uniform view" << std::endl;
        return;
    }
    checkOpenGLerror();
}

inline void Mesh::InitTexture(const char* filename)
{
    // Загружаем текстуру из файла
    if (!textureData.loadFromFile(filename))
    {
        // Не вышло загрузить картинку
        return;
    }
    // Теперь получаем openGL дескриптор текстуры
    textureHandle = textureData.getNativeHandle();
}

inline void Mesh::Init() {
    InitShader();
    InitVBO();
    glEnable(GL_DEPTH_TEST);
}

// Освобождение шейдеров

inline void Mesh::ReleaseShader() {
    // Передавая ноль, мы отключаем шейдрную программу
    glUseProgram(0);
    // Удаляем шейдерную программу
    glDeleteProgram(Program);
}

// Освобождение буфера

inline void Mesh::ReleaseVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO);
}

inline void Mesh::ReadVectorsFromFile(std::string filename) {
    std::ifstream infile(filename);
    std::string descr;
    while (infile >> descr) {
        if (descr == "v")
        {
            float x, y, z;
            infile >> x >> y >> z;
            positions.push_back({ x,y,z });
        }
        else if (descr == "vt")
        {
            float x, y;
            infile >> x >> y;
            uvs.push_back({ x,y });
        }
        else if (descr == "vn")
        {
            float x, y, z;
            infile >> x >> y >> z;
            normals.push_back({ x,y,z });
        }
        else if (descr == "f")
        {
            // get triangle
            for (size_t i = 0; i < 3; i++)
            {
                int p_ind;
                char c;
                // read vertex index
                infile >> p_ind;
                infile >> c;
                // read uv index
                int uv_ind;

                infile >> uv_ind;
                infile >> c;
                // read normal index
                int n_ind;

                infile >> n_ind;

                vertices.push_back({ positions[p_ind - 1],normals[n_ind - 1],uvs[uv_ind - 1] });
            }
        }
    }

    indices = std::vector<unsigned int>(vertices.size());

    for (size_t i = 0; i < indices.size(); i++)
    {
        indices[i] = i;
    }
}

inline void Mesh::SetupMesh(std::string filename)
{
    ReadVectorsFromFile(filename);
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), &positions[0], GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
    checkOpenGLerror();
}
