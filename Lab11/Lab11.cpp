

#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <vector>
# define PI           3.14159265358979323846f  /* pi */



// ?????????? ? ????????????????? ID
// ID ????????? ?????????
GLuint Program;

// ID ???????? ??????
GLint Attrib_vertex;

// ID ???????? ?????
GLint Attrib_color;

// ID ??????? ?????????? ???????????
GLint Unif_posX;
GLint Unif_posY;

// ID Vertex Buffer Object
GLuint VBO_position;

GLuint VBO_color;

// ???????
struct Vertex
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

float posX = 0.0f, posY = 0.0f;

// ???????? ??? ?????????? ???????
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

// ???????? ??? ???????????? ???????
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

    // ????????????? glew
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
            // ????????? ??????? ??????
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


// ???????? ?????? OpenGL, ???? ???? ?? ????? ? ??????? ??? ??????
void checkOpenGLerror() {
    GLenum errCode;
    // ???? ?????? ????? ???????? ???
    // https://www.khronos.org/opengl/wiki/OpenGL_Error
    if ((errCode = glGetError()) != GL_NO_ERROR)
        std::cout << "OpenGl error!: " << errCode << std::endl;
}

// ??????? ?????? ???? ???????
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
    // ??????? ?????? ????????????
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
    // ???????? ??????? ? ?????
    glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    checkOpenGLerror();
}


void InitShader() {
    // ??????? ????????? ??????
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    // ???????? ???????? ???
    glShaderSource(vShader, 1, &VertexShaderSource, NULL);
    // ??????????? ??????
    glCompileShader(vShader);
    std::cout << "vertex shader \n";
    ShaderLog(vShader);

    // ??????? ??????????? ??????
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    // ???????? ???????? ???
    glShaderSource(fShader, 1, &FragShaderSource, NULL);
    // ??????????? ??????
    glCompileShader(fShader);
    std::cout << "fragment shader \n";
    ShaderLog(fShader);

    // ??????? ????????? ? ??????????? ??????? ? ???
    Program = glCreateProgram();
    glAttachShader(Program, vShader);
    glAttachShader(Program, fShader);

    // ??????? ????????? ?????????
    glLinkProgram(Program);
    // ????????? ?????? ??????
    int link_ok;
    glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
    if (!link_ok)
    {
        std::cout << "error attach shaders \n";
        return;
    }

    // ?????????? ID ???????? ?????? ?? ????????? ?????????
    Attrib_vertex = glGetAttribLocation(Program, "coord");
    if (Attrib_vertex == -1)
    {
        std::cout << "could not bind attrib coord" << std::endl;
        return;
    }

    // ?????????? ID ???????? ?????
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
    // ????????????? ????????? ????????? ???????
    glUseProgram(Program);
    // ???????? ??????? ? ??????
    glUniform1f(Unif_posX, posX);
    glUniform1f(Unif_posY, posY);
    // ???????? ?????? ?????????
    glEnableVertexAttribArray(Attrib_vertex);
    glEnableVertexAttribArray(Attrib_color);
    // ?????????? VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
    glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    glVertexAttribPointer(Attrib_color, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // ????????? VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // ???????? ?????? ?? ??????????(??????)
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // ????????? ?????? ?????????
    glDisableVertexAttribArray(Attrib_vertex);
    glDisableVertexAttribArray(Attrib_color);
    // ????????? ????????? ?????????
    glUseProgram(0);
    checkOpenGLerror();
}


// ???????????? ????????
void ReleaseShader() {
    // ????????? ????, ?? ????????? ???????? ?????????
    glUseProgram(0);
    // ??????? ????????? ?????????
    glDeleteProgram(Program);
}

// ???????????? ??????
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



