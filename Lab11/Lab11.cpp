// Рисует кубик в клеточку

//#define TASK2
//#define TASK1
#define TASK3

#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <vector>
# define PI           3.14159265358979323846f  /* pi */


#ifdef TASK2
// Переменные с индентификаторами ID
// ID шейдерной программы
GLuint Program;
// ID атрибута
GLint Attrib_vertex;
// ID Vertex Buffer Object
GLuint VBO;
// ID атрибута цвета
GLint Attrib_color;
// ID юниформ переменной цвета
GLint Unif_color;
// ID VBO цвета
GLuint VBO_color;
// ID юниформ переменной перемещения
GLint Unif_X;
GLint Unif_Y;

// Вершина
struct Vertex
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

float posX = 0.0f, posY = 0.0f;

// Исходный код вершинного шейдера
const char* VertexShaderSource = R"(
    #version 330 core

    
    uniform  float X;
    uniform  float Y;
    in vec3 vertexPosition;
    in vec4 color;

     
    out vec4 vert_color;
    
    void main() {
        
        float x_angle = 1.7;
        float y_angle = -3.0;
        
        
        vec3 position = vertexPosition * mat3(
            1, 0, 0,
            0, cos(x_angle), -sin(x_angle),
            0, sin(x_angle), cos(x_angle)
        )* mat3(
            cos(y_angle), 0, sin(y_angle),
            0, 1, 0,
            -sin(y_angle), 0, cos(y_angle)
        );

        position.x += X;
        position.y += Y;
        
        gl_Position = vec4(position, 1.0);
        vert_color = color;
    }
)";

// Исходный код фрагментного шейдера
const char* FragShaderSource = R"(
    #version 330 core

    
    in vec4 vert_color;

    
    out vec4 color;

    void main() {
        color = vert_color;
    }
)";


void Init();
void Draw();
void Release();

int main() {
    sf::Window window(sf::VideoMode(600, 600), "My OpenGL window", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);

    window.setActive(true);

    // Инициализация glew
    glewInit();

    Init();

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
                case (sf::Keyboard::A): posX -= 0.5; break;
                case (sf::Keyboard::D): posX += 0.5; break;
                case (sf::Keyboard::S): posY -= 0.5; break;
                case (sf::Keyboard::W): posY += 0.5; break;
                default: break;
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Draw();

        window.display();
    }

    Release();
    return 0;
}


// Проверка ошибок OpenGL, если есть то вывод в консоль тип ошибки
void checkOpenGLerror() {
    GLenum errCode;
    // Коды ошибок можно смотреть тут
    // https://www.khronos.org/opengl/wiki/OpenGL_Error
    if ((errCode = glGetError()) != GL_NO_ERROR)
        std::cout << "OpenGl error!: " << errCode << std::endl;
}

// Функция печати лога шейдера
void ShaderLog(unsigned int shader)
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


void InitVBO()
{
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO_color);
    // Вершины кубика
    Vertex triangle[] = {
        { 0.0f,  0.5f, 0.0f}, {-0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f},
        { 0.0f,  0.5f, 0.0f}, { 0.5f, -0.5f,  0.5f}, { 0.0f, -0.5f, -0.5f},
        { 0.0f,  0.5f, 0.0f}, { 0.0f, -0.5f, -0.5f}, { -0.5f,-0.5f,  0.5f},
        {-0.5f, -0.5f, 0.5f}, { 0.5f, -0.5f,  0.5f}, { 0.0f, -0.5f, -0.5f},

    };
    // Цвет треугольника
    float colors[12][4] = {
       { 1.0, 1.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 }, { 1.0, 0.0, 0.0, 1.0 },
       { 1.0, 1.0, 1.0, 1.0 }, { 1.0, 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 },
       { 1.0, 1.0, 1.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 },
       { 0.0, 0.0, 1.0, 1.0 }, { 1.0, 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 },
    };
    // Передаем вершины в буфер
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    checkOpenGLerror();
}


void InitShader() {
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

    // Вытягиваем ID атрибута из собранной программы
    const char* attr_name = "vertexPosition";
    Attrib_vertex = glGetAttribLocation(Program, attr_name);
    if (Attrib_vertex == -1)
    {
        std::cout << "could not bind attrib " << attr_name << std::endl;
        return;
    }

    // Вытягиваем ID атрибута цвета
    Attrib_color = glGetAttribLocation(Program, "color");
    if (Attrib_color == -1)
    {
        std::cout << "could not bind attrib color" << std::endl;
        return;
    }

    const char* unifX_name = "X";
    Unif_X = glGetUniformLocation(Program, unifX_name);
    if (Unif_X == -1)
    {
        std::cout << "could not bind uniform " << unifX_name << std::endl;
        return;
    }


    const char* unifY_name = "Y";
    Unif_Y = glGetUniformLocation(Program, unifY_name);
    if (Unif_Y == -1)
    {
        std::cout << "could not bind uniform " << unifY_name << std::endl;
        return;
    }

    checkOpenGLerror();
}

void Init() {
    InitShader();
    InitVBO();
    // Включаем проверку глубины
    glEnable(GL_DEPTH_TEST);
}


void Draw() {
    // Устанавливаем шейдерную программу текущей
    glUseProgram(Program);

    glUniform1f(Unif_X, posX);
    glUniform1f(Unif_Y, posY);
    // Включаем массив атрибутов
    glEnableVertexAttribArray(Attrib_vertex);
    glEnableVertexAttribArray(Attrib_color);
    // Подключаем VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Указывая pointer 0 при подключенном буфере, мы указываем что данные в VBO
    glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Подключаем VBO_color
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    glVertexAttribPointer(Attrib_color, 4, GL_FLOAT, GL_FALSE, 0, 0);
    // Отключаем VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Передаем данные на видеокарту(рисуем)
    glDrawArrays(GL_TRIANGLES, 0, 12);
    // Отключаем массив атрибутов
    glDisableVertexAttribArray(Attrib_vertex);
    glDisableVertexAttribArray(Attrib_color);
    // Отключаем шейдерную программу
    glUseProgram(0);
    checkOpenGLerror();
}


// Освобождение шейдеров
void ReleaseShader() {
    // Передавая ноль, мы отключаем шейдрную программу
    glUseProgram(0);
    // Удаляем шейдерную программу
    glDeleteProgram(Program);
}

// Освобождение буфера
void ReleaseVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VBO_color);
}

void Release() {
    ReleaseShader();
    ReleaseVBO();
}
#endif // !TASK2

#ifdef TASK1
// Переменные с индентификаторами ID
// ID шейдерной программы
GLuint Program;

// ID атрибута вершин
GLint Attrib_vertex;

// ID атрибута цвета
GLint Attrib_color;

// ID юниформ переменной перемешения
GLint Unif_posX;
GLint Unif_posY;

// ID Vertex Buffer Object
GLuint VBO_position;

GLuint VBO_color;

// Вершина
struct Vertex
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

float posX = 0.0f, posY = 0.0f;

// Исходный код вершинного шейдера
const char* VertexShaderSource = R"(
    #version 330 core

    
    uniform float positX;
    uniform float positY;
    in vec3 coord;
    in vec4 color; 
    
    
    out vec4 vert_color;

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

        
        gl_Position = vec4(position, 1.0);
        vert_color = color;
    }
)";

// Исходный код фрагментного шейдера
const char* FragShaderSource = R"(
    #version 330 core

    
    in vec4 vert_color;

    
    out vec4 color;

    void main() {
        color = vert_color;
    }
)";


void Init();
void Draw();
void Release();


int main() {
    sf::Window window(sf::VideoMode(600, 600), "My OpenGL window", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);

    window.setActive(true);

    // Инициализация glew
    glewInit();

    Init();

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
                case (sf::Keyboard::A): posX -= 0.05; break;
                case (sf::Keyboard::D): posX += 0.05; break;
                case (sf::Keyboard::W): posY += 0.05; break;
                case (sf::Keyboard::S): posY -= 0.05; break;
                default: break;
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Draw();

        window.display();
    }

    Release();
    return 0;
}


// Проверка ошибок OpenGL, если есть то вывод в консоль тип ошибки
void checkOpenGLerror() {
    GLenum errCode;
    // Коды ошибок можно смотреть тут
    // https://www.khronos.org/opengl/wiki/OpenGL_Error
    if ((errCode = glGetError()) != GL_NO_ERROR)
        std::cout << "OpenGl error!: " << errCode << std::endl;
}

// Функция печати лога шейдера
void ShaderLog(unsigned int shader)
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


void InitVBO()
{
    glGenBuffers(1, &VBO_position);
    glGenBuffers(1, &VBO_color);
    // Вершины нашего треугольника
    Vertex triangle[] = {
        { -0.5, -0.5, +0.5 }, { -0.5, +0.5, +0.5 }, { +0.5, +0.5, +0.5 },
        { +0.5, +0.5, +0.5 }, { +0.5, -0.5, +0.5 }, { -0.5, -0.5, +0.5 },
        { -0.5, -0.5, -0.5 }, { +0.5, +0.5, -0.5 }, { -0.5, +0.5, -0.5 },
        { +0.5, +0.5, -0.5 }, { -0.5, -0.5, -0.5 }, { +0.5, -0.5, -0.5 },

        { -0.5, +0.5, -0.5 }, { -0.5, +0.5, +0.5 }, { +0.5, +0.5, +0.5 },
        { +0.5, +0.5, +0.5 }, { +0.5, +0.5, -0.5 }, { -0.5, +0.5, -0.5 },
        { -0.5, -0.5, -0.5 }, { +0.5, -0.5, +0.5 }, { -0.5, -0.5, +0.5 },
        { +0.5, -0.5, +0.5 }, { -0.5, -0.5, -0.5 }, { +0.5, -0.5, -0.5 },

        { +0.5, -0.5, -0.5 }, { +0.5, -0.5, +0.5 }, { +0.5, +0.5, +0.5 },
        { +0.5, +0.5, +0.5 }, { +0.5, +0.5, -0.5 }, { +0.5, -0.5, -0.5 },
        { -0.5, -0.5, -0.5 }, { -0.5, +0.5, +0.5 }, { -0.5, -0.5, +0.5 },
        { -0.5, +0.5, +0.5 }, { -0.5, -0.5, -0.5 }, { -0.5, +0.5, -0.5 },
    };

    float colors[36][4] = {
        { 1.0, 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 },
        { 0.0, 0.0, 1.0, 1.0 }, { 1.0, 0.0, 1.0, 1.0 }, { 1.0, 0.0, 0.0, 1.0 },
        { 0.0, 1.0, 1.0, 1.0 }, { 1.0, 1.0, 0.0, 1.0 }, { 0.5, 0.5, 0.0, 1.0 },
        { 1.0, 1.0, 0.0, 1.0 }, { 0.0, 1.0, 1.0, 1.0 }, { 0.0, 0.5, 0.5, 1.0 },

        { 0.5, 0.5, 0.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 },
        { 0.0, 0.0, 1.0, 1.0 }, { 1.0, 1.0, 0.0, 1.0 }, { 0.5, 0.5, 0.0, 1.0 },
        { 0.0, 1.0, 1.0, 1.0 }, { 1.0, 0.0, 1.0, 1.0 }, { 1.0, 0.0, 0.0, 1.0 },
        { 1.0, 0.0, 1.0, 1.0 }, { 0.0, 1.0, 1.0, 1.0 }, { 0.0, 0.5, 0.5, 1.0 },

        { 0.0, 0.5, 0.5, 1.0 }, { 1.0, 0.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 },
        { 0.0, 0.0, 1.0, 1.0 }, { 1.0, 1.0, 0.0, 1.0 }, { 0.0, 0.5, 0.5, 1.0 },
        { 0.0, 1.0, 1.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 }, { 1.0, 0.0, 0.0, 1.0 },
        { 0.0, 1.0, 0.0, 1.0 }, { 0.0, 1.0, 1.0, 1.0 }, { 0.5, 0.5, 0.0, 1.0 }
    };
    // Передаем вершины в буфер
    glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    checkOpenGLerror();
}


void InitShader() {
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

    // Вытягиваем ID атрибута цвета
    Attrib_color = glGetAttribLocation(Program, "color");
    if (Attrib_color == -1)
    {
        std::cout << "could not bind attrib color" << std::endl;
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

    checkOpenGLerror();
}

void Init() {
    InitShader();
    InitVBO();
    glEnable(GL_DEPTH_TEST);
}


void Draw() {
    // Устанавливаем шейдерную программу текущей
    glUseProgram(Program);
    // Передаем юниформ в шейдер
    glUniform1f(Unif_posX, posX);
    glUniform1f(Unif_posY, posY);
    // Включаем массив атрибутов
    glEnableVertexAttribArray(Attrib_vertex);
    glEnableVertexAttribArray(Attrib_color);
    // Подключаем VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
    glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    glVertexAttribPointer(Attrib_color, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // Отключаем VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Передаем данные на видеокарту(рисуем)
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // Отключаем массив атрибутов
    glDisableVertexAttribArray(Attrib_vertex);
    glDisableVertexAttribArray(Attrib_color);
    // Отключаем шейдерную программу
    glUseProgram(0);
    checkOpenGLerror();
}


// Освобождение шейдеров
void ReleaseShader() {
    // Передавая ноль, мы отключаем шейдрную программу
    glUseProgram(0);
    // Удаляем шейдерную программу
    glDeleteProgram(Program);
}

// Освобождение буфера
void ReleaseVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO_position);
    glDeleteBuffers(1, &VBO_color);
}

void Release() {
    ReleaseShader();
    ReleaseVBO();
}
#endif // TASK1


#ifdef TASK3
// Переменные с индентификаторами ID
// ID шейдерной программы
GLuint Program;
// ID атрибута
GLint Attrib_vertex;
// ID юниформ переменной цвета


GLint Unif_X;
GLint Unif_Y;

GLuint VBO;
// Вершина
struct Vertex
{
    GLfloat x;
    GLfloat y;
};

float triangle_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
float RX = 1;
float RY = 1;

// Исходный код вершинного шейдера
const char* VertexShaderSource = R"(
    #version 330 core
    uniform  float X;
    uniform  float Y;
    in vec2 coord; 
    out vec4 col;

    vec3 hsv2rgb(vec3 c)
    {
        vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
        vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
        return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
    }
    vec3 rgb2hsv(vec3 c)
    {
        vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
        vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
        vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

        float d = q.x - min(q.w, q.y);
        float e = 1.0e-10;
        return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
    }
    void main() {
        gl_Position = vec4(coord, 0.0, 1.0);
        float sat = distance(gl_Position.x,gl_Position.y);
        float hue = asin(gl_Position.x/sat);
        vec3 hsv = vec3(hue,sat,1);
        col = vec4(hsv2rgb(hsv),1);
    }
)";

// Исходный код фрагментного шейдера
const char* FragShaderSource = R"(
    #version 330 core
    in vec4 col; 
    out vec4 color;
    void main() {
        color = col;
    }
)";


void Init();
void Draw();
void Release();

// Инкремент цветовой компоненты цвета
void incColor(int index) {
    triangle_color[index] += 0.1;
    if (triangle_color[index] > 1)
        triangle_color[index] = 1;
}

// Декремент цветовой компоненты цвета
void decColor(int index) {
    triangle_color[index] -= 0.1;
    if (triangle_color[index] < 0)
        triangle_color[index] = 0;
}

void incXRad(int i) {
    RX += 0.1f*i;
    std::cout << "RX : " << RX<<std::endl;
}

void incYRad(int i) {
    RY += 0.1f * i;
    std::cout << "RY : " << RY<<std::endl;
}

int main() {
    sf::Window window(sf::VideoMode(600, 600), "My OpenGL window", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);

    window.setActive(true);

    // Инициализация glew
    glewInit();

    Init();

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
                case (sf::Keyboard::A): incXRad(-1); break;
                case (sf::Keyboard::D): incXRad(1); break;
                case (sf::Keyboard::W): incYRad(1); break;
                case (sf::Keyboard::S): incYRad(-1); break;
                default: break;
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Draw();

        window.display();
    }

    Release();
    return 0;
}


// Проверка ошибок OpenGL, если есть то вывод в консоль тип ошибки
void checkOpenGLerror() {
    GLenum errCode;
    // Коды ошибок можно смотреть тут
    // https://www.khronos.org/opengl/wiki/OpenGL_Error
    if ((errCode = glGetError()) != GL_NO_ERROR)
        std::cout << "OpenGl error!: " << errCode << std::endl;
}

// Функция печати лога шейдера
void ShaderLog(unsigned int shader)
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

int n = 100;

void InitVBO()
{
    glGenBuffers(1, &VBO);
    // Вершины нашего треугольника
    std::vector<Vertex> vertexes;

    float step = 2 * PI / n;
    for (size_t i = 1; i < 30*n; i++)
    {
        float t0 = (i-1) * step;
        float t1 = i * step;
        vertexes.push_back({ 0,0 });
        vertexes.push_back({RX*cos(t0),RY * sin(t0) });
        vertexes.push_back({ RX * cos(t1),RY * sin(t1) });
    }

    // Передаем вершины в буфер
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, n*30, vertexes.data(), GL_STATIC_DRAW);
    checkOpenGLerror();
}


void InitShader() {
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

    // Вытягиваем ID атрибута из собранной программы
    const char* attr_name = "coord";
    Attrib_vertex = glGetAttribLocation(Program, attr_name);
    if (Attrib_vertex == -1)
    {
        std::cout << "could not bind attrib " << attr_name << std::endl;
        return;
    }

    // Вытягиваем ID юниформ
    const char* unifX_name = "X";
    Unif_X = glGetUniformLocation(Program, unifX_name);
    if (Unif_X == -1)
    {
        std::cout << "could not bind uniform " << unifX_name << std::endl;
        return;
    }


    const char* unifY_name = "Y";
    Unif_Y = glGetUniformLocation(Program, unifY_name);
    if (Unif_Y == -1)
    {
        std::cout << "could not bind uniform " << unifY_name << std::endl;
        return;
    }
    checkOpenGLerror();
}

void Init() {
    InitShader();
    InitVBO();
}


void Draw() {
    // Устанавливаем шейдерную программу текущей
    glUseProgram(Program);
    // Передаем юниформ в шейдер
    
    //glUniform1f(Unif_X, RX);
    //glUniform1f(Unif_Y, RY);
    // Включаем массив атрибутов
    glEnableVertexAttribArray(Attrib_vertex);
    // Подключаем VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Указывая pointer 0 при подключенном буфере, мы указываем что данные в VBO
    glVertexAttribPointer(Attrib_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0);
    // Отключаем VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Передаем данные на видеокарту(рисуем)
    glDrawArrays(GL_TRIANGLE_STRIP, 0, n*30);
    // Отключаем массив атрибутов
    glDisableVertexAttribArray(Attrib_vertex);
    // Отключаем шейдерную программу
    glUseProgram(0);
    checkOpenGLerror();
}


// Освобождение шейдеров
void ReleaseShader() {
    // Передавая ноль, мы отключаем шейдрную программу
    glUseProgram(0);
    // Удаляем шейдерную программу
    glDeleteProgram(Program);
}

// Освобождение буфера
void ReleaseVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO);
}

void Release() {
    ReleaseShader();
    ReleaseVBO();
}
#endif // TASK3
