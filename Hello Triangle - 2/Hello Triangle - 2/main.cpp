#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// 창 크기 설정
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//각 shader GLSL 코드
const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n\0";


int main(){
    
    // glfw : 초기화, 설정
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // MAC OSX
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // glfw window 생성
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if(window ==NULL){
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // OpenGL 함수 포인터 로드
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }
    
    
    
    //vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // 컴파일 에러 체크
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }
    //fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    //  컴파일 에러 체크
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // 링킹 에러 체크
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }
    //링크 했으니 shader 객체는 제거
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // 정점 데이터( vertex data) 세팅
    float vertices[] = {
         0.5f,  0.5f, 0.0f, //우상단
         0.5f, -0.5f, 0.0f, //우하단
        -0.5f, -0.5f, 0.0f, //좌하단
        -0.5f,  0.5f, 0.0f, //좌상단
    };
    
    unsigned int indices[] = {
        0, 1, 3,    // 1st triangle
        1, 2, 3     // 2nd triangle
    };
    unsigned int VBO, VAO, EBO; //VBO를 통해 메모리를 관리한다. VAO를 통해 효율적인 렌더링을 수행한다. EBO 를 통해 어떤 정점을 사용해 렌더링할지 결정할 인덱스를 정한다.
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); //VBO에게 unique 한 ID 부여
    glGenBuffers(1, &EBO);
    
    //VAO 바인드 후 vertex buffer 를 세팅하고, vertex attribute 를 구성한다.
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //생성된 버퍼를 GL_ARRAY_BUFFER 유형으로 바인드
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //초기화 한 정점들을 버퍼의 메모리에 복사(메모리 할당, 데이터 저장)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    //vertex attribute location 이 0, vec3 타입이니까 3, 정규화 NO, stride 는 float(4byte) 3개, 12도 가능, tightly packed 되어있다면 0으로도 가능, offset 은 0 void* 형으로 형변환.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3* sizeof(float), (void*)0);
    //vertex attribute 사용할 수 있게
    glEnableVertexAttribArray(0);
    
    // glVertexAttribPointer 함수로 VBO 를 vertex attribute 의 바인드 된 VBO로 등록한 후, 안전하게 언바인드 할 수 있다.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    // EBO 를 VAO 언바인드 전에 언바인드 해버리면 EBO 를 못쓴다.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // VAO 를 언바인드 할 수 있고 그 뒤에 다른 VAO 호출들이 이 VAO 를 우연히 수정하지는 않지만 가끔씩 발생한다.
    // 다른 VAO 를 수정하려면 glBindVertexArray 함수를 호출해야한다. 어쨌든 일반적으로 VAO 들이나 VBO 들을 필수적인 상황이 아니면 언바인드 하지 않는다.
    glBindVertexArray(0);
    
    //wireframe 모드
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    
    //렌더링 루프
    while(!glfwWindowShouldClose(window)){
        
        processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); //현재 VAO 가 1개만 있기 때문에 매번 바인딩 할 필요는 없지만 체계적인 코드를 위해 작성
        //glDrawArrays(GL_TRIANGLES, 0, 6); //EBO 안쓸때
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //EBO 사용시 이 함수
        //glBindVertexArray(0); //매번 언바인드 할 필요 없다.
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    
    glfwTerminate();
    return 0;
}
    
    void framebuffer_size_callback(GLFWwindow* window, int width, int height ){
        glViewport(0, 0, width, height);
    }
    
    void processInput(GLFWwindow* window) {
        if( glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }
    
