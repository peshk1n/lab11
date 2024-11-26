//#include <some_library_header.h>

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>

using namespace std;
// ID шейдерной программы
GLuint Program;
// ID атрибута
GLint Attrib_vertex;
// ID Vertex Buffer Object
GLuint VBO;

GLint Uniform_color;

struct Vertex {
	GLfloat x;
	GLfloat y;
};

// Исходный код вершинного шейдера
const char* VertexShaderSource = R"(
 #version 330 core
 in vec2 coord;
 void main() {
 gl_Position = vec4(coord, 0.0, 1.0);
 }
)";

// Исходный код фрагментного шейдера
//const char* FragShaderSource = R"(
// #version 330 core
// out vec4 color;
// void main() {
// color = vec4(0, 1, 0, 1);
// }
//)";

const char* FragShaderSource = R"(
#version 330 core
uniform vec4 color; 
void main() {
    gl_FragColor = color; 
}
)";

void ShaderLog(unsigned int shader)
{
	int infologLen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
	if (infologLen > 1)
	{
		int charsWritten = 0;
		std::vector<char> infoLog(infologLen);
		glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog.data());
		std::cout << "InfoLog: " << infoLog.data() << std::endl;
	}
}

void InitShader() {
	// Создаем вершинный шейдер
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	// Передаем исходный код
	glShaderSource(vShader, 1, &VertexShaderSource, NULL);
	// Компилируем шейдер
	glCompileShader(vShader);
	cout << "vertex shader \n";
	// Функция печати лога шейдера
	ShaderLog(vShader);
	// Создаем фрагментный шейдер
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Передаем исходный код
	glShaderSource(fShader, 1, &FragShaderSource, NULL);
	// Компилируем шейдер
	glCompileShader(fShader);
	std::cout << "fragment shader \n";
	// Функция печати лога шейдера
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
	if (!link_ok) {
		std::cout << "error attach shaders \n";
		return;
	}
	// Вытягиваем ID атрибута из собранной программы
	const char* attr_name = "coord"; //имя в шейдере
	Attrib_vertex = glGetAttribLocation(Program, attr_name);
	if (Attrib_vertex == -1) {
		std::cout << "could not bind attrib " << attr_name << std::endl;
		return;
	}

	// Вытягиваем ID uniform-переменной
	const char* uniform_name = "color"; // имя в шейдере
	Uniform_color = glGetUniformLocation(Program, uniform_name);
	if (Uniform_color == -1) {
		std::cout << "could not bind uniform " << uniform_name << std::endl;
		return;
	}
	//checkOpenGLerror();
}
void Draw() {
	glUseProgram(Program); // Устанавливаем шейдерную программу текущей

	glUniform4f(Uniform_color, 0.0f, 0.5f, 1.0f, 1.0f); 

	glEnableVertexAttribArray(Attrib_vertex); // Включаем массив атрибутов
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Подключаем VBO
   // сообщаем OpenGL как он должен интерпретировать вершинные данные.
	glVertexAttribPointer(Attrib_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Отключаем VBO
	//glDrawArrays(GL_TRIANGLES, 0, 3); // Передаем данные на видеокарту(рисуем)
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
	glDisableVertexAttribArray(Attrib_vertex); // Отключаем массив атрибутов
	glUseProgram(0); // Отключаем шейдерную программу

	//checkOpenGLerror();
}


void InitVBO2() {
	glGenBuffers(1, &VBO);
	// Центр веера и несколько точек по кругу
	const int num_segments = 6; // Количество сегментов
	Vertex fan[num_segments + 2];
	fan[0] = { 0.0f, 0.0f }; // Центр веера
	for (int i = 0; i <= num_segments; i++) {
		float angle =  3.1415926f * i / num_segments;
		fan[i + 1] = { cos(angle), sin(angle) };
	}
	// Передаем вершины в буфер
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fan), fan, GL_STATIC_DRAW);

}
void InitVBO3() {

	const float pi = 3.14159265358979323846f;
	const float r = 0.8f;
	glGenBuffers(1, &VBO);
	Vertex pentagon[5];
	for (int i = 0; i < 5; i++) {
		double angle = 2 * pi * i / 5;
		pentagon[i].y = r * cos(angle);
		pentagon[i].x = r * sin(angle);
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pentagon), pentagon, GL_STATIC_DRAW);
}
void InitVBO1() {

	
	glGenBuffers(1, &VBO);
	Vertex square[6] = {
	{ -0.5f, -0.5f },
	{ 0.5f, -0.5f },
	{ -0.5f, 0.5f },

	{ -0.5f, 0.5f },
	{ 0.5f, 0.5f },
	{ 0.5f, -0.5f }
	};
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);
}

void Init1() {
	// Шейдеры
	InitShader();
	// Вершинный буфер
	InitVBO1();
}
void Init2() {
	// Шейдеры
	InitShader();
	// Вершинный буфер
	InitVBO2();
}
void Init3() {
	// Шейдеры
	InitShader();
	// Вершинный буфер
	InitVBO3();
}
// Освобождение буфера
void ReleaseVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);
}

// Освобождение шейдеров
void ReleaseShader() {
	// Передавая ноль, мы отключаем шейдерную программу
	glUseProgram(0);
	// Удаляем шейдерную программу
	glDeleteProgram(Program);
}
void Release() {
	// Шейдеры
	ReleaseShader();
	// Вершинный буфер
	ReleaseVBO();
}

int main() {
	sf::Window window(sf::VideoMode(600, 600), "My OpenGL window", sf::Style::Default, sf::ContextSettings(24));
	window.setVerticalSyncEnabled(true);
	window.setActive(true);
	glewInit();
	int a =1;
	if(a==1)
		Init1();
	if(a==2)
		Init2();
	if (a == 3)
		Init3();
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) { window.close(); }
			else if (event.type == sf::Event::Resized) { glViewport(0, 0, event.size.width, event.size.height); }
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Draw();
		window.display();
	}
	Release();
	return 0;
}

