#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>

#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <vector>

#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <fstream>
#include <sstream>
#include <string>


using namespace glm;
//셰이더 연결
GLuint LoadShaders(const std::string& vertex_file_path, const std::string& fragment_file_path)
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // vertex 셰이더 코드 읽기
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path.c_str(), std::ios::in);
    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }
    else {
        printf("Impossible to open %s. Are you in the right directory? Don't forget to read the FAQ!\n", vertex_file_path.c_str());
        getchar();
        return 0;
    }

	// fragment 셰이더 코드 읽기
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path.c_str(), std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }
    //성공여부
    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader: %s\n", vertex_file_path.c_str());
    char const* VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path.c_str());
    char const* FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

void Update_mesh(const GLuint& VAO, const std::vector<GLuint>& GLBuffers,
    std::vector<glm::vec3> Positions, //위치
    std::vector<glm::vec3> Normals, //법선
    std::vector<glm::vec3> Colors, //색상
    std::vector<unsigned int>& Indices) //인덱스
{
    glBindVertexArray(VAO); // vao 바인딩

    //정점 위치
    glBindBuffer(GL_ARRAY_BUFFER, GLBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, Positions.size() * sizeof(Positions[0]), &Positions[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //정점 법선
    glBindBuffer(GL_ARRAY_BUFFER, GLBuffers[1]);
    glBufferData(GL_ARRAY_BUFFER, Normals.size() * sizeof(Normals[0]), &Normals[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //정점 색상
    glBindBuffer(GL_ARRAY_BUFFER, GLBuffers[2]);
    glBufferData(GL_ARRAY_BUFFER, Colors.size() * sizeof(Colors[0]), &Colors[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //인덱스 EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLBuffers[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(Indices[0]), &Indices[0], GL_STATIC_DRAW);

    
    glBindVertexArray(0); // VAO 바인딩 해제
}


int Width = 512;
int Height = 512;


void resize_callback(GLFWwindow*, int w, int h)
{
    Width = w;
    Height = h;
    glViewport(0, 0, w, h);
}

// 구 생성 
void CreateSphereMesh(float rad, int width, int height, std::vector<vec3>& Positions, std::vector<vec3>& Normals, std::vector<unsigned int>& Indices) {
    Positions.clear();
    Normals.clear();
    Indices.clear();

 //몸통
    for (int j = 1; j < height - 1; ++j) {
        float theta = float(j) / (height - 1) * pi<float>();
        for (int i = 0; i < width; ++i) {
            float phi = float(i) / (width - 1) * pi<float>() * 2.0f;
            float x = sin(theta) * cos(phi);
            float y = cos(theta);
            float z = -sin(theta) * sin(phi);
            vec3 p = rad * vec3(x, y, z);
            Positions.push_back(p);
            Normals.push_back(normalize(p));
        }
    }

	// 북극 추가
    Positions.push_back(vec3(0, rad, 0));
    Normals.push_back(vec3(0, 1, 0));

	// 남극 추가
    Positions.push_back(vec3(0, -rad, 0));
    Normals.push_back(vec3(0, -1, 0));

    int topIndex = Positions.size() - 2;
    int bottomIndex = Positions.size() - 1;

   //사각형을 두개의 삼각형으로 
    for (int j = 0; j < height - 3; ++j) {
        for (int i = 0; i < width - 1; ++i) {
            int idx0 = j * width + i;
            int idx1 = (j + 1) * width + (i + 1);
            int idx2 = j * width + (i + 1);
            int idx3 = (j + 1) * width + i;

            //1번 삼각형
            Indices.push_back(idx0);
            Indices.push_back(idx1);
            Indices.push_back(idx2);
            //2번 삼각형
            Indices.push_back(idx0);
            Indices.push_back(idx3);
            Indices.push_back(idx1);
        }
    }

    // 북극 삼각형
    for (int i = 0; i < width - 1; ++i) {
        Indices.push_back(topIndex);
        Indices.push_back(i);
        Indices.push_back(i + 1);
    }

    // 남극 삼각형
    for (int i = 0; i < width - 1; ++i) {
        int base = (height - 3) * width;
        Indices.push_back(bottomIndex);
        Indices.push_back(base + i + 1);
        Indices.push_back(base + i);
    }
}


int main(int argc, char* argv[]) {
    GLFWwindow* window;
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int Width = 512, Height = 512;
    window = glfwCreateWindow(Width, Height, "OpenGL Viewer", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glewExperimental = true;
    if (glewInit() != GLEW_OK) return -1;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLuint shaderProgram = LoadShaders("Phong.vert", "Phong.frag"); //셰이더 로드

    std::vector<vec3> Positions, Normals, Colors;
    std::vector<unsigned int> Indices;
    CreateSphereMesh(1.0f, 32, 16, Positions, Normals, Indices);
    for (size_t i = 0; i < Positions.size(); ++i) Colors.push_back(vec3(0.0f));

    mat4 model = scale(translate(mat4(1.0f), vec3(0, 0, -7)), vec3(2.0f)); // 이동, 크기
    mat4 view = lookAt(vec3(0, 0, 0), vec3(0, 0, -1), vec3(0, 1, 0)); //카메라 방향
	mat4 projection = frustum(-0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 1000.0f); //투영 행렬

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    std::vector<GLuint> GLBuffers(4);
    glGenBuffers(4, &GLBuffers[0]);
    Update_mesh(VAO, GLBuffers, Positions, Normals, Colors, Indices);

    vec3 lightPos(-4.0f, 4.0f, -3.0f); 
    vec3 viewPos(0.0f);
    vec3 lightColor(1.0f);

    vec3 ka(0.0f, 1.0f, 0.0f);
    vec3 kd(0.0f, 0.5f, 0.0f);
    vec3 ks(0.5f, 0.5f, 0.5f);
    float shininess = 32.0f;

    //shader 변수 전달
    glUseProgram(shaderProgram);
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, &lightPos[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &viewPos[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, &lightColor[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "ka"), 1, &ka[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "kd"), 1, &kd[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "ks"), 1, &ks[0]);
    glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), shininess);
    //shader 사용
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //자원 해제
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(4, &GLBuffers[0]);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
