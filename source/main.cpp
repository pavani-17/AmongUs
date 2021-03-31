#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
#include <random>


#define GLT_IMPLEMENTATION
#include "gltext.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAX 1000000


// Normal Vertex Shader
const char *vertexShaderSource ="#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "uniform mat4 model; \n"
    "uniform mat4 view; \n"
    "uniform mat4 projection; \n"
    "void main()\n"
    "{\n"
    "   gl_Position = model*vec4(aPos, 0.0f, 1.0f);\n"
    "}\0";


// Normal Fragment Shader
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec3 color_val;\n"
    "void main()\n"
    "{\n"
    "       FragColor = vec4(color_val, 1.0f);\n"
    "}\n\0";




const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

int score = 0;
int tasks = 0;
bool spinObj = 0, spinCam=0;
bool imposterMoving = false;
bool imposterDisplay = true;
bool button2Activated = false;
int imposDirection = 0;
int pos_x=0, pos_y = 0;
int impos_x=9, impos_y=9;
int button1_posx, button1_posy;
int button2_posx, button2_posy;
float obstacles[18], powerups[18];
int obstacles_x[3], obstacles_y[3], powerups_x[3], powerups_y[3];
bool obs_visible[3] = {true, true, true};
bool power_visible[3] = {true, true, true};
glm::vec3 obstaclePos[3];
glm::vec3 powerupsPos[3];

glm::vec3 objPosition = glm::vec3(-0.8f+0.25f/6, -0.8f+0.25/6, 0.0f);
glm::vec3 objNewPos = glm::vec3(-0.8f+0.25f/6, -0.8f+0.25/6, 0.0f);
glm::vec3 imposterPos = glm::vec3(0.7 + 0.25f/6, 0.7 + 0.25f/6, 0.0f );

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 button1Pos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 button2Pos = glm::vec3(0.0f, 0.0f, 0.0f);

int height = 10, width=10;
int grid[10][10];
int steps=0;
std::vector <int> path[100];


int cnt=0;
void carve_passages_from(int c_x, int c_y)
{
    char directions[] = {'N', 'S', 'E', 'W'};   
    std::shuffle(directions, directions+4, std::default_random_engine(rand()));

    int i;    
    int dx,dy;
    int d_val,o_val;
    int n_x,n_y;
    for(i=0;i<4;i++)
    {
       cnt++;
        switch(directions[i])
        {
            case 'N':
                    dx=0;
                    dy=-1;
                    d_val=1;
                    o_val =2;
                    break;
            
            case 'S':
                    dx=0;
                    dy=1;
                    d_val=2;
                    o_val=1;
                    break;
            
            case 'E':
                    dx=1;
                    dy=0;
                    d_val=4;
                    o_val=8;
                    break;
            
            case 'W':
                    dx=-1;
                    dy=0;
                    d_val=8;
                    o_val=4;
                    break;
        }
        n_x = c_x + dx;
        n_y = c_y + dy;

        if (n_x >=0 && n_x < width && n_y >=0 && n_y < height && grid[n_y][n_x] == 0)
        {
            grid[c_y][c_x] = grid[c_y][c_x] | d_val;
            grid[n_y][n_x] = grid[n_y][n_x] | o_val;
            carve_passages_from(n_x, n_y);
        }
    }
}

void path_generator()
{
   int dist[height][width][height][width];
   int i,j,k,l,m,n;

   for(i=0;i<height*width;i++)
   {
      for(j=0;j<height*width;j++)
      {
         path[i].push_back(0);
      }
   }

   for(i=0;i<height;i++)
   {
      for(j=0;j<width;j++)
      {
         for(k=0;k<height;k++)
         {
            for(l=0;l<width;l++)
            {
               dist[i][j][k][l] = MAX;
               if(i==k && j==l)
               {
                  dist[i][j][k][l] = 0;
                  path[i*height+j][k*height+l] = k*height+l;
               }
            }
         }
      }
   }

   for(i=0;i<height;i++)
   {
      for(j=0;j<width;j++)
      {
         if(i!=0)
         {
            if(grid[i][j] & 1)
            {
               dist[i][j][i-1][j] = 1;
               dist[i-1][j][i][j] = 1;
               path[i*height+j][(i-1)*height+j] = (i-1)*height+j;
               path[(i-1)*height+j][i*height+j] = i*height + j;
            }
         }
         if(i!=height-1)
         {
            if(grid[i][j] & 2)
            {
               dist[i][j][i+1][j] = 1;
               dist[i+1][j][i][j] = 1;
               path[i*height+j][(i+1)*height+j] = (i+1)*height+j;
               path[(i+1)*height+j][i*height+j] = i*height+j;
            }
         }

         if(j!=0)
         {
            if(grid[i][j] & 8)
            {
               dist[i][j][i][j-1] = 1;
               dist[i][j-1][i][j] = 1;
               path[i*height+j][i*height+j-1] = i*height + j - 1;
               path[i*height + j-1][i*height+j] = i*height + j;
            }
         }

         if(j!=width-1)
         {
            if(grid[i][j] & 4)
            {
               dist[i][j][i][j+1] = 1;
               dist[i][j+1][i][j] = 1;
               path[i*height+j][i*height+j+1] = i*height + j + 1;
               path[i*height+j+1][i*height+j] = i*height+j;
            }
         }
      }
   }
   // k = m,n
   for(m=0;m<height;m++)
   {
      for(n=0;n<width;n++)
      {
         // i = k,l
         for(k=0;k<height;k++)
         {
            for(l=0;l<width;l++)
            {
               // j = i,j
               for(i=0;i<height;i++)
               {
                  for(j=0;j<width;j++)
                  {
                     if(dist[k][l][i][j] > dist[k][l][m][n] + dist[m][n][i][j])
                     {
                        dist[k][l][i][j] = dist[k][l][m][n] + dist[m][n][i][j];
                        path[k*height+l][i*height+j] = path[k*height+l][m*height+n];
                     }
                  }
               }
            }
         }
      }
   }   
}


bool collide(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
   bool temp1=false, temp2=false;
   float d1, d2;

   d1 = (x1 - x3)*(y4 - y3) - (y1 - y3)*(x4-x3);
   d2 = (x2 - x3)*(y4 - y3) - (y2 - y3)*(x4-x3);

   if(!((d1<0 && d2<0) || (d1>0 && d2>0)))
   {
      temp1 = true;
   }

   d1 = (x3 - x1)*(y2 - y1) - (y3 - y1)*(x2 - x1);
   d2 = (x4 - x1)*(y2 - y1) - (y4 - y1)*(x2 - x1);


   if(!((d1<0 && d2<0) || (d1>0 && d2>0)))
   {
      temp2 = true;
   }

   if(temp1 && temp2)
   {
      return true;
   }
   else
   {
      return false;
   }
   
}

// Handle Input
void processInput(GLFWwindow *window, float vertices[], int len)
{
   bool press=false;
   float speed = 0.01f;
   if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
   {
      objNewPos.y += speed;
      press = true;
   }
   if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
   {
      objNewPos.y -= speed;
      press = true;
   }
   if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
   {
      objNewPos.x -= speed;
      press = true;
   }
   if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
   {
      objNewPos.x += speed;
      press = true;
   }

   if(press)
   {
      float d1, d2;
      int i;
      float x,y;
      bool collision = false;
      bool temp1=false, temp2=false;
      for(i=0;i<len;i+=4)
      {
         x =-0.08f/6;
         y =-0.1f/6;

         if(collide(vertices[i], vertices[i+1], vertices[i+2], vertices[i+3], objNewPos.x + x, objNewPos.y + y, objPosition.x + x, objPosition.y+y))
         {
            collision = true;
            break;
         }         

         x = 0.35f/6;
         y =-0.1f/6;

         if(collide(vertices[i], vertices[i+1], vertices[i+2], vertices[i+3], objNewPos.x + x, objNewPos.y + y, objPosition.x + x, objPosition.y+y))
         {
            collision = true;
            break;
         }

         x = 0.35f/6;
         y = 0.5f/6;

         if(collide(vertices[i], vertices[i+1], vertices[i+2], vertices[i+3], objNewPos.x + x, objNewPos.y + y, objPosition.x + x, objPosition.y+y))
         {
            collision = true;
            break;
         }

         x = -0.08f/6;
         y = 0.5f/6;

         if(collide(vertices[i], vertices[i+1], vertices[i+2], vertices[i+3], objNewPos.x + x, objNewPos.y + y, objPosition.x + x, objPosition.y+y))
         {
            collision = true;
            break;
         }

         // Rectangle Sides
         
         if(collide(vertices[i], vertices[i+1], vertices[i+2], vertices[i+3], objNewPos.x + x, objNewPos.y + y, objNewPos.x, objNewPos.y))
         {
            collision = true;
            break;
         }

         if(collide(vertices[i], vertices[i+1], vertices[i+2], vertices[i+3], objNewPos.x + x, objNewPos.y + y, objNewPos.x + 0.5f/6, objNewPos.y + 0.5f/6))
         {
            collision = true;
            break;
         }

         x=0.35f/6;
         y=-0.1f/6;

         if(collide(vertices[i], vertices[i+1], vertices[i+2], vertices[i+3], objNewPos.x + x, objNewPos.y + y, objNewPos.x, objNewPos.y))
         {
            collision = true;
            break;
         }

         if(collide(vertices[i], vertices[i+1], vertices[i+2], vertices[i+3], objNewPos.x + x, objNewPos.y + y, objNewPos.x + 0.5f/6, objNewPos.y + 0.5f/6))
         {
            collision = true;
            break;
         }
      }
      if(!collision)
      {
         objPosition.x = objNewPos.x;
         objPosition.y = objNewPos.y;

      }
      else
      {
         objNewPos.x = objPosition.x;
         objNewPos.y = objPosition.y;
      }
   }

   pos_x = ((objPosition.x)+0.8)*6;
   pos_y = ((objPosition.y)+0.8)*6;

   impos_x = ((imposterPos.x)+0.8)*6;
   impos_y = ((imposterPos.y)+0.8)*6;

   if(pos_x == button1_posx && pos_y == button1_posy && imposterDisplay)
   {
      imposterDisplay = false;
      tasks++;
   }

   if(pos_x == button2_posx && pos_y == button2_posy && !button2Activated)
   {
      button2Activated = true;
      tasks++;
   }

   if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
   {
      glfwSetWindowShouldClose(window, true);
   }

   if(button2Activated)
   {
      int i;
      for(i=0;i<3;i++)
      {
         if(obstacles_x[i] == pos_x && obstacles_y[i] == pos_y && obs_visible[i])
         {
            score -= 1;
            obs_visible[i] = false;
         }

         if(powerups_x[i] == pos_x && powerups_y[i] == pos_y && power_visible[i])
         {
            score += 5;
            power_visible[i] = false;
         }
      }
   }

   if(imposterDisplay)
   {
      if(!imposterMoving)
      {
         int dest = path[impos_x*height + impos_y][pos_x*height+pos_y];
         if(dest == impos_x*height + impos_y)
         {
            return;
         }
         else if(dest == impos_x*height + impos_y + 1)
         {
            imposDirection = 1; // Up
         }
         else if(dest == impos_x*height + impos_y - 1)
         {
            imposDirection = -1; //Down
         }
         else if(dest == (impos_x-1)*height + impos_y)
         {
            imposDirection = -2; //Left
         }
         else
         {
            imposDirection = 2; // Right
         }
         switch(imposDirection)
         {
            case 1:
                  imposterPos.y += 1.0f/480;
                  break;

            case -1:
                  imposterPos.y -= 1.0f/480;
                  break;
            
            case 2:
                  imposterPos.x += 1.0f/480;
                  break;
            
            case -2:
                  imposterPos.x -= 1.0f/480;
                  break;
         }
         imposterMoving = true;
         steps = 1;
      }
      else
      {
         steps++;
         switch(imposDirection)
         {
            case 1:
                  imposterPos.y += 1.0f/480;
                  // impos_y +=1;
                  break;

            case -1:
                  imposterPos.y -= 1.0f/480;
                  // impos_y -=1;
                  break;
            
            case 2:
                  imposterPos.x += 1.0f/480;
                  // impos_x += 1;
                  break;
            
            case -2:
                  imposterPos.x -= 1.0f/480;
                  // impos_x -= 1;
                  break;
         }
         if(steps == 80)
         {
            imposterMoving = false;
         }
      }
   }
   

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

namespace {
   void errorCallback(int error, const char* description) {
      fprintf(stderr, "GLFW error %d: %s\n", error, description);
   }

   GLFWwindow* initialize() {
      int glfwInitRes = glfwInit();
      if (!glfwInitRes) {
         fprintf(stderr, "Unable to initialize GLFW\n");
         return nullptr;
      }

      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

      GLFWwindow* window = glfwCreateWindow(1000, 1000, "InitGL", nullptr, nullptr);
      if (!window) {
         fprintf(stderr, "Unable to create GLFW window\n");
         glfwTerminate();
         return nullptr;
      }

      glfwMakeContextCurrent(window);
      glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

      int gladInitRes = gladLoadGL();
      if (!gladInitRes) {
         fprintf(stderr, "Unable to initialize glad\n");
         glfwDestroyWindow(window);
         glfwTerminate();
         return nullptr;
      }

      return window;
   }
}



int main(int argc, char* argv[]) {

   srand(time(NULL));
   int i,j;

   glfwSetErrorCallback(errorCallback);

   GLFWwindow* window = initialize();
   if (!window) {
      return 0;
   }
   gltInit();


   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_int_distribution<> distrib(0, 9);

   button1_posx = distrib(gen);
   button1_posy = distrib(gen);

   button1Pos.x = (float)button1_posx/6 - 0.8f;
   button1Pos.y = (float)button1_posy/6 - 0.8f;

   button2_posx = distrib(gen);
   button2_posy = distrib(gen);

   button2Pos.x = (float)button2_posx/6 - 0.8f;
   button2Pos.y = (float)button2_posy/6 - 0.8f;

   for(i=0;i<3;i++)
   {
      obstacles_x[i] = distrib(gen);
      obstacles_y[i] = distrib(gen);

      obstaclePos[i].x = (float)obstacles_x[i]/6 - 0.8f;
      obstaclePos[i].y = (float)obstacles_y[i]/6 - 0.8f;
      obstaclePos[i].z = 0;

      powerups_x[i] = distrib(gen);
      powerups_y[i] = distrib(gen);

      powerupsPos[i].x = (float)powerups_x[i]/6 - 0.8f;
      powerupsPos[i].y = (float)powerups_y[i]/6 - 0.8f;
      powerupsPos[i].z = 0;
   }


   // compile vertex shader
   unsigned int vertexShader;
   vertexShader = glCreateShader(GL_VERTEX_SHADER);
   
   glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
   glCompileShader(vertexShader);

   int success;
   char infoLog[512];
   glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
   if(!success)
   {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      fprintf(stderr, "Unable to Compile Vertex Shader\n");
      fprintf(stderr, "%s\n", infoLog);
      return 0;
   }

   // compile computer shader
   unsigned int fragmentShader;
   fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
   glCompileShader(fragmentShader);
   glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
   if(!success)
   {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      fprintf(stderr, "Unable to Compile Fragment Shader\n");
      fprintf(stderr, "%s\n", infoLog);
      return 0;
   }

   for(i=0;i<height;i++)
   {
      for(j=0;j<width;j++)
      {
         grid[i][j]=0;
      }
   }
   carve_passages_from(0,0);
   path_generator();



   unsigned int shaderProgram;
   shaderProgram = glCreateProgram();

   glAttachShader(shaderProgram, vertexShader);
   glAttachShader(shaderProgram, fragmentShader);
   glLinkProgram(shaderProgram);

   glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
   if(!success)
   {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      fprintf(stderr, "Unable to Link Shaders\n");
      fprintf(stderr, "%s\n", infoLog);
      return 0;
   }

   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);

   std::vector <std::pair<float, float>> vertex_vec;
   

   for(i=0;i<height;i++)
   {
      for(j=0;j<width;j++)
      {
         if((grid[i][j] & 1) == 0)
         {
            vertex_vec.push_back(std::make_pair(i,j));
            vertex_vec.push_back(std::make_pair(i,j+1));
         }

         if((grid[i][j] & 2) == 0)
         {
            vertex_vec.push_back(std::make_pair(i+1,j));
            vertex_vec.push_back(std::make_pair(i+1,j+1));
         }

         if((grid[i][j] & 8) == 0)
         {
            vertex_vec.push_back(std::make_pair(i,j));
            vertex_vec.push_back(std::make_pair(i+1,j));
         }

         if((grid[i][j] & 4) == 0)
         {
            vertex_vec.push_back(std::make_pair(i,j+1));
            vertex_vec.push_back(std::make_pair(i+1,j+1));
         }
      }
   }
   int len = vertex_vec.size();
   float vertices[len*2];
   for(i=0;i<len;i++)
   {
      vertices[2*i] = vertex_vec[i].first/6 - 0.8;
      vertices[2*i+1] = vertex_vec[i].second/6 - 0.8;
   }

   float character[] = {
      // Body
      0.0f, 0.0f,
      0.0f, 0.4f/6,
      0.3f/6, 0.4f/6,
      0.0f, 0.0f,
      0.3f/6, 0.0f,
      0.3f/6, 0.4f/6,

      // Leg 1
      0.0f, 0.0f,
      0.1f/6, -0.1f/6,
      0.1f/6, 0.0,
      0.0f, 0.0f,
      0.1f/6, -0.1f/6,
      0.0,  -0.1f/6,

      //Leg 2
      0.3f/6, 0.0f,
      0.2f/6, -0.1f/6,
      0.2f/6, 0.0f,
      0.3f/6, 0.0f,
      0.3f/6, -0.1f/6,
      0.2f/6, -0.1f/6,

      //bag
      0.0f, 0.05f/6,
      0.0f, 0.35f/6,
      -0.08f/6, 0.35f/6,
      0.0f, 0.05f/6,
      -0.08f/6, 0.05f/6,
      -0.08f/6, 0.35f/6,
   };

   float visor_ar[] = {
      0.1f/6, 0.15f/6,
      0.35f/6, 0.32f/6,
      0.1f/6, 0.32f/6,
      0.1f/6, 0.15f/6,
      0.35f/6, 0.32f/6,
      0.35f/6, 0.15f/6,
   };
   
   // for(i=0;i<12;i++)
   // {
   //    character[i] = character[i]/6 - 0.8;
   // }

   int triangleAmount = 20; //# of triangles used to draw circle
    
    GLfloat radius = 1.0f/24; //radius
    GLfloat twicePi = 2.0f * 22/7;

   float circle[2*triangleAmount+2];
   float semi_circle[2*triangleAmount+2];
    
   for(i = 0; i <= triangleAmount;i++) { 
      circle[2*i] = 1.0f/12+ radius * cos(i *  twicePi / triangleAmount);
      circle[2*i+1] = 1.0f/12 +radius * sin(i * twicePi / triangleAmount);
   }

   radius = 0.15f/6;

   for(i = 0; i <= triangleAmount;i++) { 
      semi_circle[2*i] = 0.15f/6 + radius * cos(i *  twicePi / triangleAmount);
      semi_circle[2*i+1] = 0.35f/6 +radius * sin(i * twicePi / triangleAmount);
   }




   unsigned int VBO, VAO;
   unsigned int VAO1, VBO1;
   unsigned int circ, circ_A;
   unsigned int semi_circ, semi_circ_A;
   unsigned int visor, visor_A;

   glGenVertexArrays(1, &circ_A);
   glGenBuffers(1, &circ);

   glGenVertexArrays(1, &visor_A);
   glGenBuffers(1, &visor);

   glGenVertexArrays(1, &semi_circ_A);
   glGenBuffers(1, &semi_circ);
   
   glGenVertexArrays(1, &VAO);
   glGenBuffers(1, &VBO);
   glGenVertexArrays(1, &VAO1);
   glGenBuffers(1, &VBO1);

   glBindVertexArray(VAO);

   glBindBuffer(GL_ARRAY_BUFFER, VBO);

   
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,GL_STATIC_DRAW);

   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   glBindVertexArray(VAO1);

   glBindBuffer(GL_ARRAY_BUFFER, VBO1);

   
   glBufferData(GL_ARRAY_BUFFER, sizeof(character), character,GL_STATIC_DRAW);

   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   glBindVertexArray(circ_A);
   glBindBuffer(GL_ARRAY_BUFFER, circ);   
   glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle,GL_STATIC_DRAW);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   glBindVertexArray(semi_circ_A);
   glBindBuffer(GL_ARRAY_BUFFER, semi_circ);   
   glBufferData(GL_ARRAY_BUFFER, sizeof(semi_circle), semi_circle,GL_STATIC_DRAW);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   glBindVertexArray(visor_A);
   glBindBuffer(GL_ARRAY_BUFFER, visor);   
   glBufferData(GL_ARRAY_BUFFER, sizeof(visor_ar), visor_ar,GL_STATIC_DRAW);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   
   glm::mat4 projection;
   projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);

   glm::mat4 view;
   view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

   glUseProgram(shaderProgram);

   glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);

   glEnable(GL_DEPTH_TEST);

   char str[1000];

   GLTtext *text = gltCreateText();

  
   while (!glfwWindowShouldClose(window)) {

      glUseProgram(shaderProgram);


      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      processInput(window, vertices, 2*len);
   

      int viewLoc = glGetUniformLocation(shaderProgram, "view");
      glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

      int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
      glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

      int modelLoc = glGetUniformLocation(shaderProgram, "model"); 
      glm::mat4 model = glm::mat4(1.0f);
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

      color = glm::vec3(0.0f, 0.0f, 0.0f);
      int colorLoc = glGetUniformLocation(shaderProgram, "color_val");
      glUniform3fv(colorLoc, 1, glm::value_ptr(color));
      

      glBindVertexArray(VAO);
      glDrawArrays(GL_LINES, 0, len);

      model = glm::translate(model, objPosition);
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

      color = glm::vec3(0.5f, 0.5f, 0.5f);
      glUniform3fv(colorLoc, 1, glm::value_ptr(color));


      glBindVertexArray(visor_A);
      glDrawArrays(GL_TRIANGLES, 0, 6);

      color = glm::vec3(0.0f, 1.0f, 1.0f);
      glUniform3fv(colorLoc, 1, glm::value_ptr(color));

      
      glBindVertexArray(0);  

      

      glBindVertexArray(VAO1);
      glDrawArrays(GL_TRIANGLES, 0, 24);

      glBindVertexArray(semi_circ_A);
      glDrawArrays(GL_TRIANGLE_FAN, 0, triangleAmount+1);


      if(imposterDisplay)
      {
         model = glm::mat4(1.0f);
         model = glm::translate(model, imposterPos);
         glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

         color = glm::vec3(0.5f, 0.5f, 0.5f);
         glUniform3fv(colorLoc, 1, glm::value_ptr(color));


         glBindVertexArray(visor_A);
         glDrawArrays(GL_TRIANGLES, 0, 6);

         color = glm::vec3(0.9f, 0.32f, 0.5f);
         glUniform3fv(colorLoc, 1, glm::value_ptr(color));

         glBindVertexArray(VAO1);
         glDrawArrays(GL_TRIANGLES, 0, 24);

         glBindVertexArray(semi_circ_A);
         glDrawArrays(GL_TRIANGLE_FAN, 0, triangleAmount+1);

      }
      
      color = glm::vec3(1.0f, 0.0f, 0.0f);
      glUniform3fv(colorLoc, 1, glm::value_ptr(color));

      glBindVertexArray(circ_A);
      model = glm::mat4(1.0f);
      model = glm::translate(model, button1Pos);
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
      glDrawArrays(GL_TRIANGLE_FAN, 0, triangleAmount);

      if(button2Activated)
      {
         color = glm::vec3(0.0f, 0.0f, 0.0f);
         glUniform3fv(colorLoc, 1, glm::value_ptr(color));

         for(i=0;i<3;i++)
         {
            if(obs_visible[i])
            {
               model = glm::mat4(1.0f);
               model = glm::translate(model, obstaclePos[i]);
               glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

               glDrawArrays(GL_TRIANGLE_FAN, 0, triangleAmount);
            }
         }

         color = glm::vec3(1.0f, 1.0f, 0.0f);
         glUniform3fv(colorLoc, 1, glm::value_ptr(color));

         for(i=0;i<3;i++)
         {
            if(power_visible[i])
            {
               model = glm::mat4(1.0f);
               model = glm::translate(model, powerupsPos[i]);
               glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

               glDrawArrays(GL_TRIANGLE_FAN, 0, triangleAmount);
            }
         }
      }

      color = glm::vec3(0.0f, 1.0f, 0.0f);
      glUniform3fv(colorLoc, 1, glm::value_ptr(color));

      model = glm::mat4(1.0f);
      model = glm::translate(model, button2Pos);
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
      glDrawArrays(GL_TRIANGLE_FAN, 0, triangleAmount);

      sprintf(str, "Score: %d\nTasks: %d/2\nLight: On\nTime: \n",score,tasks);

      gltSetText(text, str);


      gltBeginDraw();
      gltColor(0.0f, 1.0f, 1.0f, 0.0f);
      gltDrawText2D(text, 0.8f, 0.8f, 1);
      gltEndDraw();



      glfwSwapBuffers(window);
      glfwPollEvents();
   }
   // Finish drawing text

   // Deleting text
   gltDeleteText(text);

   // Destroy glText
   gltTerminate();

   glDeleteVertexArrays(1, &VAO);
   glDeleteBuffers(1, &VBO);
   glDeleteVertexArrays(1, &VAO1);
   glDeleteBuffers(1, &VBO1);
   glDeleteProgram(shaderProgram);

   glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}

