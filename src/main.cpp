#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>

#include <iostream>
#include <cstdlib>
#include <cmath>
#include "time.h"

#include "../Button/Button.h"
#include "../SkyBox/SkyBox.h"
#include "../Particle/snow.h"
#include "../Bezier/Curvemodel.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_fixed_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// window settings
const unsigned int SCR_WIDTH = 1400;
const unsigned int SCR_HEIGHT = 800;

enum type { BEZIER };
enum select { CHANGED, SCENE, NONE };

int mouseX, mouseY;
bool ifstart = false;
bool MouseDown = false, PointSelect = false, firstMouse=true;

bool notChange = true;
bool framedisplay = true;
bool framelock = false;
Curvemodel diymodel;

// mode selection
type Mode = BEZIER;
select Select = NONE;
bool ifreset = true, ifdisplay = false;

// button size
const int buttonWidth = 160, buttonHeight = 80, buttonOffsetX = 600, buttonOffsetY = 320, buttonDist = 100;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 20.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float totalTime = 0.0f;



int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Final_Project", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    //glEnable(GL_FRAMEBUFFER_SRGB);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_fixed_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);


    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("../Bezier/shader.vs", "../Bezier/shader.fs"), frameShader("../Bezier/edge.vs", "../Bezier/edge.fs");
    ourShader.use();
    ourShader.setFloat("material.shininess", 16.0f);
    ourShader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
    ourShader.setVec3("light.diffuse", 0.6f, 0.6f, 0.65f);
    ourShader.setVec3("light.specular", 0.8f, 0.8f, 0.8f);

    ourShader.setFloat("light.constant", 1.0f);
    ourShader.setFloat("light.linear", 0.05f);
    ourShader.setFloat("light.quadratic", 0.01f);
    //Shader TeapotShader("../Teacups/model.vs", "../Teacups/model.fs");
    

    // components initialization

    Button textureButton("../Button/texture.jpg", SCR_WIDTH, SCR_HEIGHT,
        buttonWidth, buttonHeight, buttonOffsetX, buttonOffsetY - 4 * buttonDist);
    //Button displayButton("../Button/display.jpg", SCR_WIDTH, SCR_HEIGHT,
        //buttonWidth, buttonHeight, buttonOffsetX, buttonOffsetY - 5 * buttonDist);


    SkyBox background;
    Snow Blossoms("../Particle/blossom.obj", BLIZZARD);


    GLuint VAO, VBO[3];
    GLuint VAO2, VBO2;
    GLuint VAO3, VBO3;

    glEnable(GL_DEPTH_TEST);
    glLineWidth(5.0);
    glPointSize(15.0);

    vector<float> pvalues;  //顶点坐标
    vector<float> tvalues;  //纹理坐标
    vector<float> nvalues;  //法线
    vector <float> fpvalues;//锚点坐标
    vector <float> apvalues;//锚点坐标

    int totalindex, findex;
   

    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    GLuint texture = diymodel.load_texture("../Bezier/clay.png");

    // lighting
    glm::vec3 lightPos(12.0f, 30.0f, 5.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        diymodel.remake();
        totalindex = diymodel.load_model(&pvalues, &tvalues, &nvalues);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(3, VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STREAM_DRAW);
        
        notChange = true;

        while (notChange && !glfwWindowShouldClose(window)) {
            // per-frame time logic
            // --------------------
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // input
            // -----
            processInput(window);

            if (notChange == false)break;

            // render
            // ------
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       

            // view/projection transformations
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            

            // world transformation
            glm::mat4 model;
            
            //model = glm::mat4(1.0f);
            //model = glm::translate(model, lightPos);
            //model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube

            if (Select == SCENE) {
                background.changeTexture();
                Select = CHANGED;
            }

            //Render the Scene(skybox)
            background.drawSkybox(view, projection);

            //Render the Buttons
            textureButton.drawButton();

            //Render the particle system
            Blossoms.SetWeather(true);
            Blossoms.Draw(view, projection, lightPos, camera);

            //Render the model
            ourShader.use();

            model = glm::mat4(1.0f);
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);
            ourShader.setMat4("model", model);

            ourShader.setVec3("viewPos", camera.Position);
            ourShader.setVec3("light.position", lightPos);

            glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glEnableVertexAttribArray(2);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            
            ourShader.setInt("s", 0);

            glDrawArrays(GL_TRIANGLES, 0, totalindex);


            //Render the frame
            findex = diymodel.load_frame(&fpvalues);
            glGenVertexArrays(1, &VAO2);
            glGenBuffers(1, &VBO2);
            glBindVertexArray(VAO2);
            glBindBuffer(GL_ARRAY_BUFFER, VBO2);
            glBufferData(GL_ARRAY_BUFFER, fpvalues.size() * 4, &fpvalues[0], GL_STREAM_DRAW);

            bool active = diymodel.load_active(&apvalues);
            if (active) {
                glGenVertexArrays(1, &VAO3);
                glGenBuffers(1, &VBO3);
                glBindVertexArray(VAO3);
                glBindBuffer(GL_ARRAY_BUFFER, VBO3);
                glBufferData(GL_ARRAY_BUFFER, apvalues.size() * 4, &apvalues[0], GL_STREAM_DRAW);
            }




            glDisable(GL_DEPTH_TEST);
            frameShader.use();
            frameShader.setMat4("projection", projection);
            frameShader.setMat4("view", view);
            frameShader.setMat4("model", model);
            frameShader.setVec3("color", glm::vec3(0.5, 0.5, 0.0));
            glBindBuffer(GL_ARRAY_BUFFER, VBO2);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glEnableVertexAttribArray(0);



            if (framedisplay)
                glDrawArrays(GL_LINE_STRIP, 0, findex);

            frameShader.setVec3("color", glm::vec3(1.0, 1.0, 1.0));

            if (framedisplay)
                glDrawArrays(GL_POINTS, 0, findex);


            if (active) {
                glBindBuffer(GL_ARRAY_BUFFER, VBO3);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
                glEnableVertexAttribArray(0);
                frameShader.setVec3("color", glm::vec3(0.0, 1.0, 0.0));
                if (framedisplay)
                    glDrawArrays(GL_POINTS, 0, 1);
            }


            glEnable(GL_DEPTH_TEST);



            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, mouse_fixed_callback);
    if(glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouse_callback);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        diymodel.save_file();
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        diymodel.load_from_file();
        notChange = false;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (SCR_HEIGHT / 2 - buttonOffsetY - buttonHeight / 2 + 4 * buttonDist <= lastY && lastY <= SCR_HEIGHT / 2 - buttonOffsetY + buttonHeight / 2 + 4 * buttonDist)
                    Select = SCENE;
     }
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouse_fixed_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = float(xpos);
        lastY = float(ypos);
        firstMouse = false;
    }

    float xoffset = float(xpos - lastX);
    float yoffset = float(lastY - ypos); // reversed since y-coordinates go from bottom to top

    lastX = float(xpos);
    lastY = float(ypos);

    if (MouseDown && !PointSelect)
        camera.ProcessMouseMovement(xoffset, yoffset);

    if (PointSelect) {
        diymodel.modify_point(xoffset, yoffset);
        notChange = false;
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        int point;
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            //左键：选择拖拽
            MouseDown = true;

            if (framedisplay) {
                point = diymodel.get_point(lastX, lastY, camera);
                if (point >= 0) {
                    cout << "click point " << point << "at" << lastX << "," << lastY << endl;
                    PointSelect = true;
                }
                else {
                    cout << "click no point " << "at" << lastX << "," << lastY << endl;
                    PointSelect = false;
                }
            }

            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            //中键,创建新节点
            if (framedisplay) {
                point = diymodel.get_line_start_point(lastX, lastY, camera);
                if (point >= 0) {
                    diymodel.split_point(point);
                    notChange = false;
                }
                else {

                }
            }
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            //右键，删除节点
            if (framedisplay) {
                point = diymodel.get_point(lastX, lastY, camera);
                if (point >= 0) {
                    diymodel.remove_point(point);
                    notChange = false;

                }
                else {

                }
            }
            break;
        default:
            return;
        }
    }
    else {
        MouseDown = false;
        PointSelect = false;
    }
    return;
}


// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
