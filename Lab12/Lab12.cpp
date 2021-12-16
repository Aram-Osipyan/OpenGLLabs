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

#include "Mesh.h"
int main()
{
#pragma region FONG_SHADERS
    const char* FongVertexShaderSource = R"(
    #version 330 core
    uniform float positX;
    uniform float positY;

    layout (location = 0)in vec3 coord;
    layout (location = 1)in vec3 normal;
    layout (location = 2)in vec2 uv;

    out vec2 textcoord;
    out vec3 Normal;  
    out vec3 FragPos;

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
        Normal = normal2;
        FragPos = position;
    }
    )";

    // Исходный код фрагментного шейдера
    const char* FongFragShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        in vec2 textcoord;
        in vec3 Normal;  
        in vec3 FragPos;  
  
        uniform vec3 lightPos = vec3(0,0,-1); 
        uniform vec3 view;
        uniform sampler2D textureData;

        void main() {
        vec3 lightColor = vec3(1,0.5,0.5);
        vec3 objectColor = texture(textureData, textcoord).rgb;
            // ambient
            float ambient = 0.1;
            //vec3 ambient = ambientStrength * lightColor;
  	
            // diffuse 
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diffuse = max(dot(norm, lightDir), 0.0);
            //vec3 diffuse = diff * lightColor;
    
            // specular
            float specularStrength = 0.7;
            vec3 viewDir = normalize(view - FragPos);
            vec3 reflectDir = reflect(lightDir, norm);  
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            //vec3 specular = specularStrength * spec * lightColor;  
            //vec3 specular = vec3(0,0,0);
            float specular = specularStrength * spec ;  
            vec3 result = (ambient + diffuse + specular) * lightColor * objectColor;
            FragColor = vec4(result, 1.0);
        }
        )";
#pragma endregion

#pragma region TOON_SHADERS
    const char* ToonShadingVertexShaderSource = R"(
    #version 330 core
    uniform float positX;
    uniform float positY;

    layout (location = 0)in vec3 coord;
    layout (location = 1)in vec3 normal;
    layout (location = 2)in vec2 uv;

    out vec2 textcoord;
    out vec3 Normal;  
    out vec3 FragPos;

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
        Normal = normal2;
        FragPos = position;
    }
    )";

    // Исходный код фрагментного шейдера
    const char* ToonShadingFragShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        in vec2 textcoord;
        in vec3 Normal;  
        in vec3 FragPos;  
  
        uniform vec3 lightPos = vec3(0,0,-1); 
        uniform vec3 view;
        uniform sampler2D textureData;

        void main() {

            vec4 position = vec4(FragPos,0);
            float _UnlitOutlineThickness = 0.5;
            float _LitOutlineThickness = 0.5;
            vec4 _OutlineColor = vec4(1,1,1,1);
            vec3 normalDirection = normalize(Normal);
            float _Shininess = 0.5;
            vec4 _SpecColor = vec4(0.5,0.5,0.5,1);
            vec4 _LightColor0 = vec4(1,1,0,1);
            vec4 _UnlitColor = vec4(1,0,0,1);
            float _DiffuseThreshold = 0.5;
            vec4 _Color = texture(textureData, textcoord);
            vec3 viewDirection = normalize(view);
            vec3 lightDirection;
            float attenuation;
            vec4 _WorldSpaceLightPos0 = vec4(lightPos,0.0);
            if (0.0 == _WorldSpaceLightPos0.w) // directional light?
            {
               attenuation = 1.0; // no attenuation
               lightDirection = normalize(vec3(_WorldSpaceLightPos0));
            } 
            else // point or spot light
            {
               vec3 vertexToLightSource = vec3(_WorldSpaceLightPos0 - position);
               float distance = length(vertexToLightSource);
               attenuation = 1.0 / distance; // linear attenuation 
               lightDirection = normalize(vertexToLightSource);
            }
            
            // default: unlit 
            vec3 fragmentColor = vec3(_UnlitColor); 
            
            // low priority: diffuse illumination
            if (attenuation * max(0.0, dot(normalDirection, lightDirection))  >= _DiffuseThreshold)
            {
               fragmentColor = vec3(_LightColor0) * vec3(_Color); 
            }
            
            // higher priority: outline
            if (dot(viewDirection, normalDirection) 
               < mix(_UnlitOutlineThickness, _LitOutlineThickness, 
               max(0.0, dot(normalDirection, lightDirection))))
            {
               fragmentColor = 
                  vec3(_LightColor0) * vec3(_OutlineColor); 
            }
            
            // highest priority: highlights
            if (dot(normalDirection, lightDirection) > 0.0 
               // light source on the right side?
               && attenuation *  pow(max(0.0, dot(
               reflect(-lightDirection, normalDirection), 
               viewDirection)), _Shininess) > 0.5) 
               // more than half highlight intensity? 
            {
               fragmentColor = _SpecColor.a 
                  * vec3(_LightColor0) * vec3(_SpecColor)
                  + (1.0 - _SpecColor.a) * fragmentColor;
            }
 
            FragColor = vec4(fragmentColor, 1.0);
        }
        )";
#pragma endregion

#pragma region BIDIRECTIONAL_LIGHT_SHADERS
    const char* BiDirectionalVertexShaderSource = R"(
    #version 330 core
    uniform float positX;
    uniform float positY;

    layout (location = 0)in vec3 coord;
    layout (location = 1)in vec3 normal;
    layout (location = 2)in vec2 uv;

    out vec2 textcoord;
    out vec3 Normal;  
    out vec3 FragPos;

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
        Normal = normal2;
        FragPos = position;
    }
    )";

    // Исходный код фрагментного шейдера
    const char* BiDirectionalFragShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        in vec2 textcoord;
        in vec3 Normal;  
        in vec3 FragPos;  
  
        uniform vec3 lightPos = vec3(0,0,-1); 
        uniform vec3 view;
        uniform sampler2D textureData;

        void main() {
            vec4 color1 = vec4(0.5, 0.0, 0.0,1);
            vec4 color2 = vec4(0.5, 0.5, 0.0,1);

            vec3 n2 = normalize(Normal);
            vec3 l2 = normalize(view);
            vec4 diff = color1 * max( dot(n2,l2),0) + color2 * max(dot(n2,-l2),0);
            FragColor = diff * texture(textureData, textcoord);

        }
        )";
#pragma endregion //BIDIRECTIONAL_LIGHT_SHADERS

    
    sf::Window window(sf::VideoMode(600, 600), "My OpenGL window", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);

    window.setActive(true);

    // Инициализация glew
    glewInit();

    //Object obj = Object("cube.obj");
    //Mesh obj = Mesh("file.obj", "image.jpg");
    //Mesh obj = Mesh("file.obj","pink.jpg", FongVertexShaderSource, FongFragShaderSource);
    //Mesh obj = Mesh("file.obj","pink.jpg", ToonShadingVertexShaderSource, ToonShadingFragShaderSource);
    Mesh obj = Mesh("file.obj", "pink.jpg", BiDirectionalVertexShaderSource, BiDirectionalFragShaderSource);
    //Mesh obj = Mesh();
    // lighting
    vec3 lightPos(1.2f, 1.0f, 0.0f);
    vec3 view(0.5f, 0.0f, 0.0f);

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

