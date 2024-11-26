#include <SFML/Window.hpp>
//#include <SFML/OpenGL.hpp> 
#include <SFML/Graphics.hpp>
#include <GL/glew.h>      
#include <GL/gl.h>       
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// ID шейдерной программы
GLuint Program;
// ID атрибутов
GLint Attrib_vertex;
GLint Attrib_color;
// ID буферов
GLuint VBO_Vertex, VBO_Color;

// Структура для координат вершины
struct Vertex {
    GLfloat x, y; // Координаты вершины
};

// Структура для цвета вершины
struct Color {
    GLfloat r, g, b; // Цвет вершины
    Color(GLfloat newr, GLfloat newg, GLfloat newb) {
        r = newr;
        g = newg;
        b = newb;
    }
};

// Исходный код вершинного шейдера
const char* VertexShaderSource = R"(
#version 330 core
in vec2 coord;        //  координаты вершины
in vec3 vertColor;    // цвет вершины
out vec3 fragColor;   // во фрагментный шейдер

void main() {
    gl_Position = vec4(coord, 0.0, 1.0); // положение вершины на экране
    fragColor = vertColor;              // цвет дальше
}
)";

// Исходный код фрагментного шейдера
const char* FragShaderSource = R"(
#version 330 core
in vec3 fragColor;  // цвет из вершинного шейдера
out vec4 color;

void main() {
    color = vec4(fragColor, 1.0); // используем интерполированный цвет
}
)";

void ShaderLog(unsigned int shader) {
    int infologLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
    if (infologLen > 1) {
        int charsWritten = 0;
        std::vector<char> infoLog(infologLen);
        glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog.data());
        std::cout << "InfoLog: " << infoLog.data() << std::endl;
    }
}

//Vertex fig;
void InitVBO() {
    const float pi = 3.14159265358979323846f;
    const float r = 0.8f;

    //квадрат
    /*Vertex fig[6] = {
        {-0.5f, -0.5f},
        {0.5f, -0.5f},
        {-0.5f, 0.5f},

        {-0.5f, 0.5f},
        {0.5f, 0.5f},
        {0.5f, -0.5f},
    };

    Color colors[6] = {
    {0.529f, 0.808f, 0.922f},
    {1.0f, 0.874f, 0.0f},
    {0.314f, 0.784f, 0.471f},
    {1.0f, 0.412f, 0.706f},
    {0.6f, 0.4f, 0.8f},
    {0.2f, 0.4f, 0.7f}
    };*/

    //пятиугольник
   /* Vertex fig[5];
    Color colors[5] = {
    {0.529f, 0.808f, 0.922f}, 
    {1.0f, 0.874f, 0.0f},     
    {0.314f, 0.784f, 0.471f}, 
    {1.0f, 0.412f, 0.706f},   
    {0.6f, 0.4f, 0.8f}        
    };
    for (int i = 0; i < 5; i++) {
        double angle = 2 * pi * i / 5;
        fig[i].y = r * cos(angle);
        fig[i].x = r * sin(angle);
    }*/

    //веер
    const int num_segments = 6; 
    Vertex fig[num_segments + 2];
    fig[0] = { 0.0f, 0.0f }; 
    for (int i = 0; i <= num_segments; i++) {
        float angle = 3.1415926f * i / num_segments;
        fig[i + 1] = { cos(angle), sin(angle) };
    }
    Color colors[8] = {
    {0.529f, 0.808f, 0.922f}, 
    {1.0f, 0.874f, 0.0f},     
    {0.314f, 0.784f, 0.471f}, 
    {1.0f, 0.412f, 0.706f},   
    {0.6f, 0.4f, 0.8f},        
    {0.75f, 0.784f, 0.471f},
    {1.0f, 0.24f, 0.606f},   
    {0.2f, 0.4f, 0.7f}        
    };


    // Создаем буфер для координат вершин
    glGenBuffers(1, &VBO_Vertex);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Vertex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fig), fig, GL_STATIC_DRAW);

    // Создаем буфер для цветов
    glGenBuffers(1, &VBO_Color);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
}

void InitShader() {
    // Создаем вершинный шейдер
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &VertexShaderSource, NULL);
    glCompileShader(vShader);
    ShaderLog(vShader);

    // Создаем фрагментный шейдер
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &FragShaderSource, NULL);
    glCompileShader(fShader);
    ShaderLog(fShader);

    // Создаем шейдерную программу и прикрепляем шейдеры
    Program = glCreateProgram();
    glAttachShader(Program, vShader);
    glAttachShader(Program, fShader);
    glLinkProgram(Program);

    // Проверяем статус сборки
    int link_ok;
    glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        std::cerr << "Error: Could not link shader program." << std::endl;
        return;
    }

    // Получаем атрибуты
    Attrib_vertex = glGetAttribLocation(Program, "coord");
    if (Attrib_vertex == -1) {
        std::cerr << "Error: Could not bind attribute 'coord'." << std::endl;
        return;
    }

    Attrib_color = glGetAttribLocation(Program, "vertColor");
    if (Attrib_color == -1) {
        std::cerr << "Error: Could not bind attribute 'vertColor'." << std::endl;
        return;
    }
}

void Init() {
    InitShader();
    InitVBO();
}

void Draw() {
    glUseProgram(Program);

    // Подключаем буфер с координатами
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Vertex);
    glEnableVertexAttribArray(Attrib_vertex);
    glVertexAttribPointer(Attrib_vertex, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Подключаем буфер с цветами
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Color);
    glEnableVertexAttribArray(Attrib_color);
    glVertexAttribPointer(Attrib_color, 3, GL_FLOAT, GL_FALSE, sizeof(Color), (void*)0);

    // Рисуем пятиугольник
    glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

    // Отключаем атрибуты
    glDisableVertexAttribArray(Attrib_vertex);
    glDisableVertexAttribArray(Attrib_color);

    glUseProgram(0);
}

void ReleaseShader() {
    glUseProgram(0);
    glDeleteProgram(Program);
}

void ReleaseVBO() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO_Vertex);
    glDeleteBuffers(1, &VBO_Color);
}

void Release() {
    ReleaseShader();
    ReleaseVBO();
}

int main() {
    sf::Window window(sf::VideoMode(600, 600), "Gradient Pentagon", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);
    window.setActive(true);
    glewInit();
    Init();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            else if (event.type == sf::Event::Resized) glViewport(0, 0, event.size.width, event.size.height);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Draw();
        window.display();
    }

    Release();
    return 0;
}
