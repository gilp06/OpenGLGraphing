#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "Shader.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Parser.hpp"

const char* vertex_source = R"glsl(
    #version 150 core
    in vec2 position;
	uniform mat4 proj;
	//uniform mat4 trans;
    void main()
    {
        gl_Position = proj * vec4(position, 0.0, 1.0);
    }
	)glsl";

const char* fragment_source = R"glsl(
    #version 150 core
    uniform vec4 incolor;
	out vec4 color;
    void main()
    {
        color = incolor;
    }
	)glsl";



//CURRENT MAIN CONTROLS

float VIEW_X_MIN = -10;
float VIEW_X_MAX = 10;
float VIEW_Y_MIN = -10;
float VIEW_Y_MAX = 10;

int window_width = 600;
int window_height = 600;
float settings_ratio = 0.2f;



int NUMPOINTS = 1000;

const int POW = 1;
const float COEFFICIENT = 3.0f;

float offsetX{0.0f};
float offsetY{0.0f};

float mouseX;
float mouseY;
float yValAtCursor;
float base_speed = 5.0f;





void process_input(GLFWwindow* window, float dt);
glm::vec2 CursorToOrtho(float x, float y, float min_x, float max_x, float min_y, float max_y);
void set_vertices(GLuint program, const std::string &rpn, GLuint &vao, GLuint &vbo);
void set_axis_vertices(GLuint program,  GLuint &vao, GLuint &vbo);
void set_position_vertices(GLuint program, GLuint &vao, GLuint &vbo);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    glm::vec2 mouse_pos = CursorToOrtho(xpos, ypos, VIEW_X_MIN, VIEW_X_MAX, VIEW_Y_MIN, VIEW_Y_MAX);
    mouseX = mouse_pos.x;
    mouseY = mouse_pos.y;
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    window_width = width;
    window_height = height;
    glViewport(width * settings_ratio,0,width * (1.0f - settings_ratio), height);
}

int main(int argc, char** argv)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    const char* glsl_version = "#version 150 core";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Graphing Calculator", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glViewport(window_width * settings_ratio,0, window_width * (1.0f - settings_ratio), window_height);
    glEnable(GL_LINE_SMOOTH);
    glPointSize(2.0f);

    float dt = 0.0f;
    float lastFrame = 0.0f;

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui::GetVersion();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    GLuint graph_vao;
    glGenVertexArrays(1, &graph_vao);

    GLuint graph_vbo;
    glGenBuffers(1, &graph_vbo);

    GLuint axis_vao;
    glGenVertexArrays(1, &axis_vao);

    GLuint axis_vbo;
    glGenVertexArrays(1, &axis_vao);

    GLuint pos_vao;
    glGenVertexArrays(1, &pos_vao);

    GLuint pos_vbo;
    glGenVertexArrays(1, &pos_vbo);



    Shader line_shader;
    line_shader.Compile(vertex_source, fragment_source);
    line_shader.Use();

    glm::vec4 line_color = {1.0, 0.0, 0.0, 1.0};
    glm::vec4 bg_color = {0.0, 0.0, 0.0, 1.0};
    glm::vec4 axis_color = {1.0, 1.0, 1.0, 1.0};
    std::string expression = "x 2 ^";
    float line_width = 1.0f;

    set_vertices(line_shader.ID, expression, graph_vao, graph_vbo);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        float current_frame = glfwGetTime();
        dt = current_frame - lastFrame;
        lastFrame = current_frame;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        /* Render here */
        glLineWidth(line_width);
        glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        process_input(window, dt);

        float x_values[2] = {VIEW_X_MIN, VIEW_X_MAX};
        float y_values[2] = {VIEW_Y_MIN, VIEW_Y_MAX};


        glm::mat4 projection = glm::ortho<float>(VIEW_X_MIN, VIEW_X_MAX,  VIEW_Y_MIN, VIEW_Y_MAX, -1.0f, 1.0f);
        line_shader.SetUniformMatrix4fv("proj", projection);

        set_axis_vertices(line_shader.ID, axis_vao,axis_vbo);
        line_shader.SetUniform4f("incolor", axis_color);
        glDrawArrays(GL_LINE_STRIP, 0, 5);

        line_shader.SetUniform4f("incolor", line_color);
        set_vertices(line_shader.ID, expression, graph_vao, graph_vbo);
        glDrawArrays(GL_LINE_STRIP, 0, NUMPOINTS);











        ImGui::SetNextWindowPos(ImVec2(0,0));
        ImGui::SetNextWindowSize(ImVec2(window_width * settings_ratio, window_height));
        ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
        if(ImGui::TreeNode("Settings"))
        {
            ImGui::SliderInt("Num Pts", &NUMPOINTS, 1, 100000, "%d", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat("Line Thickness", &line_width, 1.0f, 10.0f, "%.1f");
            ImGui::InputFloat("Viewport Speed", &base_speed, 1.0f);
            if(ImGui::TreeNode("Color Options"))
            {
                ImGui::ColorPicker4("Line Color", glm::value_ptr(line_color));
                ImGui::ColorPicker4("Background Color", glm::value_ptr(bg_color));
                ImGui::ColorPicker4("Axis Color", glm::value_ptr(axis_color));
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        ImGui::Text("Viewport: ");
        if(ImGui::InputFloat2("X Range: ", x_values))
        {
            VIEW_X_MIN = x_values[0];
            VIEW_X_MAX = x_values[1];
        }
        if(ImGui::InputFloat2("Y Range: ", y_values))
        {
            VIEW_Y_MIN = y_values[0];
            VIEW_Y_MAX = y_values[1];
        }
        ImGui::InputText("Y=", &expression);
        ImGui::Text("(%.1f, %.1f)", mouseX, yValAtCursor);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}



void process_input(GLFWwindow* window, float dt)
{
    float speed = base_speed * dt;
    if(glfwGetKey(window, GLFW_KEY_LEFT))
    {
        VIEW_X_MIN -= speed;
        VIEW_X_MAX -= speed;
    }
    if(glfwGetKey(window, GLFW_KEY_RIGHT)) {
        VIEW_X_MIN += speed;
        VIEW_X_MAX += speed;
    }
    if(glfwGetKey(window, GLFW_KEY_UP))
    {
        VIEW_Y_MIN += speed;
        VIEW_Y_MAX += speed;
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN))
    {
        VIEW_Y_MIN -= speed;
        VIEW_Y_MAX -= speed;
    }
}


glm::vec2 CursorToOrtho(float x, float y, float min_x, float max_x, float min_y, float max_y)
{
    float newX = x, newY = y;
    newX -= window_width * settings_ratio;
    if(newX < 0)
    {
        newX = 0;
    }

    float converted_x = min_x + (max_x-min_x) * (newX / (window_width * (1.0-settings_ratio)));
    float converted_y = min_y + (max_y-min_y) * (newY / (window_height));


    return {converted_x, converted_y};
}



void set_vertices(GLuint program, const std::string &rpn, GLuint &vao, GLuint &vbo) {
    yValAtCursor = Parser::RPNToFloat(rpn, mouseX);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    float vertices[NUMPOINTS * 2];
    for(int i = 0; i < NUMPOINTS * 2; i++)
    {
        if(i % 2 == 0)
        {
            const int value_range = (VIEW_X_MAX) - (VIEW_X_MIN);
            float value_gap = (float) value_range / (NUMPOINTS * 2);
            vertices[i] = (VIEW_X_MIN) + i * value_gap;
        }
        else
        {
            auto x = vertices[i - 1];
            vertices[i] = Parser::RPNToFloat(rpn, x);
        }
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "position"), 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
}

void set_axis_vertices(GLuint program, GLuint &vao, GLuint &vbo) {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    float vertices[10];

    vertices[0] = VIEW_X_MIN;
    vertices[1] = 0;
    vertices[2] = VIEW_X_MAX;
    vertices[3] = 0;
    vertices[4] = 0;
    vertices[5] = 0;
    vertices[6] = 0;
    vertices[7] = VIEW_Y_MAX;
    vertices[8] = 0;
    vertices[9] = VIEW_Y_MIN;

    glBufferData(GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

    GLint pos_attribute = glGetAttribLocation(program, "position");
    glVertexAttribPointer(pos_attribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
}

void set_position_vertices(GLuint program, GLuint &vao, GLuint &vbo) {
    glBindVertexArray(vao);
}
