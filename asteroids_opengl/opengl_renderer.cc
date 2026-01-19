#include "opengl_renderer.h"
#include <cassert>
#include <fstream>
#include <span>
#include <utility>

#include "wavefront.h"

Material default_material = { {1.0f, 1.0f, 1.0f} };
std::vector<float> create_vertices(WavefrontImporter & wi) {
  std::vector<float> vertices;

  for (Face face : wi.get_faces() ) {
    for (ReferenceGroup group : face.reference_groups ) {
      for (size_t i = 0; i < 3; i++) {
        vertices.push_back( group.vertice[i]);
      }
      for (size_t i = 0; i < 3; i++) {
        vertices.push_back( group.normal[i] );
      }
      if (face.material == nullptr) face.material = &default_material;
      for (size_t i = 0; i < 3; i++) {
        vertices.push_back( face.material->ambient[i]);
      }
    }
  }
  return vertices;
}

SquareMatrix4df rotation_y(const float angle_degr) {
  const float angle = angle_degr * PI / 180.0f;
  return { { std::cos(angle), 0.0f, -std::sin(angle), 0.0f},
                               { 0.0f,            1.0f, 0.0f,             0.0f},
                               { std::sin(angle), 0.0f, std::cos(angle),  0.0f},
                               { 0.0f,            0.0f, 0.0f,             1.0f}
    };
}

SquareMatrix4df rotation_x(const float angle_degr) {
  const float angle = angle_degr * PI / 180.0f;
  return { { 1.0f, 0.0f,            0.0f,             0.0f},
           { 0.0f, std::cos(angle), -std::sin(angle), 0.0f},
           { 0.0f, std::sin(angle), std::cos(angle),  0.0f},
           { 0.0f, 0.0f,            0.0f,             1.0f}
    };
}

SquareMatrix4df rotation_z(const float angle_degr) {
  const float angle = angle_degr * PI / 180.0f;
  return { { std::cos(angle), -std::sin(angle), 0.0f, 0.0f},
           { std::sin(angle), std::cos(angle),  0.0f, 0.0f},
           { 0.0f,            0.0f,             1.0f, 0.0f},
           { 0.0f,            0.0f,             0.0f, 1.0f}
    };
}


// geometric data as in original game and game coordinates
std::vector<Vector2df> spaceship = {
  Vector2df{-6.0f,  3.0f},
  Vector2df{-6.0f, -3.0f},
  Vector2df{-10.0f, -6.0f},
  Vector2df{14.0f,  0.0f},
  Vector2df{-10.0f,  6.0f},
  Vector2df{ -6.0f,  3.0f}
};

std::vector<Vector2df> flame = { 
  Vector2df{-6, 3},
  Vector2df{-12, 0},
  Vector2df{-6, -3}
};

std::vector<Vector2df> torpedo_points = { 
  Vector2df{0, 0},
  Vector2df{0, 1}
};

std::vector<Vector2df> saucer_points = {
  Vector2df{-16, -6},
  Vector2df{16, -6}, 
  Vector2df{40, 6}, 
  Vector2df{-40, 6},
  Vector2df{-16, 18},
  Vector2df{16, 18},
  Vector2df{40, 6},
  Vector2df{16, -6},
  Vector2df{8, -18},
  Vector2df{-8, -18},
  Vector2df{-16, -6},
  Vector2df{-40, 6}
};

std::vector<Vector2df> asteroid_1 = {
  Vector2df{ 0, -12},
  Vector2df{16, -24},
  Vector2df{32, -12},
  Vector2df{24, 0},
  Vector2df{32, 12},
  Vector2df{8, 24}, 
  Vector2df{-16, 24}, 
  Vector2df{-32, 12}, 
  Vector2df{-32, -12}, 
  Vector2df{-16, -24},
  Vector2df{0, -12}
};

std::vector<Vector2df> asteroid_2 = {
  Vector2df{6, -6},
  Vector2df{32, -12},
  Vector2df{16, -24}, 
  Vector2df{0, -16}, 
  Vector2df{-16, -24}, 
  Vector2df{-24, -12},
  Vector2df{-16, -0}, 
  Vector2df{-32, 12}, 
  Vector2df{-16, 24}, 
  Vector2df{-8, 16}, 
  Vector2df{16, 24}, 
  Vector2df{32, 6}, 
  Vector2df{16, -6},
};

std::vector<Vector2df> asteroid_3 = {
  Vector2df{-16, 0}, 
  Vector2df{-32, 6}, 
  Vector2df{-16, 24}, 
  Vector2df{0, 6}, 
  Vector2df{0, 24}, 
  Vector2df{16, 24},
  Vector2df{32, 6}, 
  Vector2df{32, 6}, 
  Vector2df{16, -24}, 
  Vector2df{-8, -24}, 
  Vector2df{-32, -6}
};

std::vector<Vector2df> asteroid_4 = {
  Vector2df{8,0}, 
  Vector2df{32,-6}, 
  Vector2df{32, -12}, 
  Vector2df{8, -24}, 
  Vector2df{-16, -24}, 
  Vector2df{-8, -12}, 
  Vector2df{-32, -12}, 
  Vector2df{-32, 12}, 
  Vector2df{-16, 24}, 
  Vector2df{8, 16}, 
  Vector2df{16, 24}, 
  Vector2df{32, 12}, 
  Vector2df{8, 0}
};

std::vector<Vector2df> spaceship_debris = {
  Vector2df{-2, -1}, 
  Vector2df{-10, 7}, 
  Vector2df{3, 1}, 
  Vector2df{7, 8},
  Vector2df{0, 3}, 
  Vector2df{6, 1},
  Vector2df{3, -1}, 
  Vector2df{ -5, -7},
  Vector2df{0, -4}, 
  Vector2df{-6, -6},
  Vector2df{-2, 2}, 
  Vector2df{2, 5}
};
    
std::vector<Vector2df> spaceship_debris_direction = {
 Vector2df{-40, -23},
 Vector2df{50, 15},
 Vector2df{0, 45},
 Vector2df{60, -15}, 
 Vector2df{10, -52}, 
 Vector2df{-40, 30}
};

std::vector<Vector2df> debris_points = {
 Vector2df{-32, 32}, 
 Vector2df{-32, -16}, 
 Vector2df{-16, 0}, 
 Vector2df{-16, -32}, 
 Vector2df{-8, 24},
 Vector2df{8, -24}, 
 Vector2df{24, 32}, 
 Vector2df{24, -24}, 
 Vector2df{24, -32}, 
 Vector2df{32, -8}
};
        
std::vector<Vector2df> digit_0 = { {0,-8}, {4,-8}, {4,0}, {0,0}, {0, -8} };
std::vector<Vector2df> digit_1 = { {4,0}, {4,-8} };
std::vector<Vector2df> digit_2 = { {0,-8}, {4,-8}, {4,-4}, {0,-4}, {0,0}, {4,0}  };
std::vector<Vector2df> digit_3 = { {0,0}, {4, 0}, {4,-4}, {0,-4}, {4,-4}, {4, -8}, {0, -8}  };
std::vector<Vector2df> digit_4 = { {4,0}, {4,-8}, {4,-4}, {0,-4}, {0,-8}  };
std::vector<Vector2df> digit_5 = { {0,0}, {4,0}, {4,-4}, {0,-4}, {0,-8}, {4, -8}  };
std::vector<Vector2df> digit_6 = { {0,-8}, {0,0}, {4,0}, {4,-4}, {0,-4} };
std::vector<Vector2df> digit_7 = { {0,-8}, {4,-8}, {4,0} };
std::vector<Vector2df> digit_8 = { {0,-8}, {4,-8}, {4,0}, {0,0}, {0,-8}, {0, -4}, {4, -4} };
std::vector<Vector2df> digit_9 = { {4, 0}, {4,-8}, {0,-8}, {0, -4}, {4, -4} };
       
std::vector< std::vector<Vector2df> * > vertice_data = {
  &spaceship, &flame,
  &torpedo_points, &saucer_points,
  &asteroid_1, &asteroid_2, &asteroid_3, &asteroid_4,
  &spaceship_debris, &spaceship_debris_direction,
  &debris_points,
  &digit_0, &digit_1, &digit_2, &digit_3, &digit_4, &digit_5, &digit_6, &digit_7, &digit_8, &digit_9 };                                  

size_t ship_vertices_count = 0;
size_t torpedo_vertices_count = 0;
size_t saucer_vertices_count = 0;
size_t asteroid_vertices_count = 0;


// class OpenGLView

  OpenGLView::OpenGLView(GLuint vbo, unsigned int shaderProgram, size_t vertices_size, GLuint mode)
    : shaderProgram(shaderProgram), vertices_size(vertices_size), mode(mode) {
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    if (mode == GL_TRIANGLES) {
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);

      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
      glEnableVertexAttribArray(1);

      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
      glEnableVertexAttribArray(2);
    } else {
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);
      // Set default color to white for 2D elements (which don't have color attributes)
      glVertexAttrib3f(1, 1.0f, 1.0f, 1.0f);
      // Set default normal
      glVertexAttrib3f(2, 0.0f, 0.0f, 1.0f);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  OpenGLView::~OpenGLView() {
   glDeleteVertexArrays(1, &vao);
  }

  void OpenGLView::render( SquareMatrix<float,4> & matrice) {
    debug(2, "render() entry...");
    glBindVertexArray(vao);
    glUseProgram(shaderProgram);
    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &matrice[0][0] );
    glDrawArrays(mode, 0, vertices_size );
    debug(2, "render() exit.");
  }

// class TypedBodyView

  TypedBodyView::TypedBodyView(TypedBody * typed_body, GLuint vbo, unsigned int shaderProgram, size_t vertices_size, float scale, GLuint mode,
               std::function<bool()> draw, std::function<void(TypedBodyView *)> modify)
        : OpenGLView(vbo, shaderProgram, vertices_size, mode),  typed_body(typed_body), scale(scale), draw(draw), modify(modify) {
  }
  
  SquareMatrix4df TypedBodyView::create_object_transformation(Vector2df direction, float angle, float scale) {
    SquareMatrix4df  translation= { {1.0f,        0.0f,          0.0f, 0.0f},
                                    {0.0f,        1.0f,          0.0f, 0.0f},
                                    {0.0f,        0.0f,          1.0f, 0.0f},
                                    {direction[0], direction[1], 0.0f, 1.0f}
                                  };
    SquareMatrix4df rotation = { { std::cos(angle),  std::sin(angle), 0.0f, 0.0f},
                                 {-std::sin(angle),  std::cos(angle), 0.0f, 0.0f},
                                 { 0.0f,             0.0f,            1.0f, 0.0f},
                                 { 0.0f,             0.0f,            0.0f, 1.0f}
                               };
    SquareMatrix4df  scaling = { { scale,    0.0f, 0.0f,   0.0f},
                                 {  0.0f,   scale, 0.0f,   0.0f},
                                 {  0.0f,    0.0f, scale,  0.0f},
                                 {  0.0f,    0.0f, 0.0f,   1.0f}
                               };                                 

    SquareMatrix4df transform = translation * rotation * scaling;
    if (mode == GL_TRIANGLES) {
      SquareMatrix4df correction = { { 1.0f,  0.0f, 0.0f, 0.0f},
                                     { 0.0f,  0.0f, 1.0f, 0.0f},
                                     { 0.0f, -1.0f, 0.0f, 0.0f},
                                     { 0.0f,  0.0f, 0.0f, 1.0f} };
      if (typed_body->get_type() == BodyType::spaceship) {
        correction = correction * rotation_y(90);
      } else if (typed_body->get_type() == BodyType::torpedo) {
        correction = correction * rotation_y(90);
      } else if (typed_body->get_type() == BodyType::saucer) {
        correction = correction * rotation_x(-90);
      }
      return transform * correction;
    }
    return transform;
  }

  void TypedBodyView::render( SquareMatrix<float,4> & world) {
    debug(2, "render() entry...");
    if ( draw() ) {
      modify(this);
      auto transform = world * create_object_transformation(typed_body->get_position(), typed_body->get_angle(), scale);
      OpenGLView::render(transform);
    }
    debug(2, "render() exit.");
  }
  
 TypedBody * TypedBodyView::get_typed_body() {
   return typed_body;
 }

 void TypedBodyView::set_scale(float scale) {
   this->scale = scale;
 }

// class OpenGLRenderer

void OpenGLRenderer::createVbos() {
    vbos = new GLuint[vertice_data.size()];
    glGenBuffers(vertice_data.size(), vbos);


    // Load spacehip.obj, torpedo.obj, saucer.obj and asteroid.obj wavefront files
    std::ifstream ship_file("spaceship.obj");
    WavefrontImporter ship_importer(ship_file);
    ship_importer.parse();
    std::vector<float> ship_vertices = create_vertices(ship_importer);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, ship_vertices.size() * sizeof(float), ship_vertices.data(), GL_STATIC_DRAW);
    ship_vertices_count = ship_vertices.size() / 9;

    std::ifstream torpedo_file("torpedo.obj");
    WavefrontImporter torpedo_importer(torpedo_file);
    torpedo_importer.parse();
    std::vector<float> torpedo_vertices = create_vertices(torpedo_importer);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
    glBufferData(GL_ARRAY_BUFFER, torpedo_vertices.size() * sizeof(float), torpedo_vertices.data(), GL_STATIC_DRAW);
    torpedo_vertices_count = torpedo_vertices.size() / 9;

    std::ifstream saucer_file("saucer.obj");
    WavefrontImporter saucer_importer(saucer_file);
    saucer_importer.parse();
    std::vector<float> saucer_vertices = create_vertices(saucer_importer);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[3]);
    glBufferData(GL_ARRAY_BUFFER, saucer_vertices.size() * sizeof(float), saucer_vertices.data(), GL_STATIC_DRAW);
    saucer_vertices_count = saucer_vertices.size() / 9;

    std::ifstream asteroid_file("asteroid.obj");
    WavefrontImporter asteroid_importer(asteroid_file);
    asteroid_importer.parse();
    std::vector<float> asteroid_vertices = create_vertices(asteroid_importer);
    asteroid_vertices_count = asteroid_vertices.size() / 9;

    for (int i = 0; i < 4; i++) {
      glBindBuffer(GL_ARRAY_BUFFER, vbos[4 + i]);
      glBufferData(GL_ARRAY_BUFFER, asteroid_vertices.size() * sizeof(float), asteroid_vertices.data(), GL_STATIC_DRAW);
    }

    for (size_t i = 0; i < vertice_data.size(); i++) {
       if (i == 0 || i == 2 || i ==3 || (i >=4 && i <=7) ) {
          continue; // already created above
       }

       glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
       glBufferData(GL_ARRAY_BUFFER, vertice_data[i]->size() * sizeof( Vector2df ), vertice_data[i]->data(), GL_STATIC_DRAW);
    }
}

void OpenGLRenderer::create(Spaceship * ship, std::vector< std::unique_ptr<TypedBodyView> > & views) {
    debug(4, "create(Spaceship *) entry...");


    views.push_back(std::make_unique<TypedBodyView>(ship, vbos[0], shaderProgram, ship_vertices_count, 10.0f, GL_TRIANGLES,
                    [ship]() -> bool {return ! ship->is_in_hyperspace();}) // only show ship if outside hyperspace
                   );
    views.push_back(std::make_unique<TypedBodyView>(ship, vbos[1], shaderProgram, vertice_data[1]->size(), 1.0f, GL_LINE_LOOP,
                    [ship]() -> bool {return ! ship->is_in_hyperspace() && ship->is_accelerating();}) // only show flame if accelerating
                   );
  
  debug(4, "create(Spaceship *) exit.");
}

void OpenGLRenderer::create(Saucer * saucer, std::vector< std::unique_ptr<TypedBodyView> > & views) {
  debug(4, "create(Saucer *) entry...");
  float scale = 20.0f;
  if ( saucer->get_size() == 0 ) {
    scale = 10.0f;
  }
  views.push_back(std::make_unique<TypedBodyView>(saucer, vbos[3], shaderProgram, saucer_vertices_count, scale, GL_TRIANGLES));
  debug(4, "create(Saucer *) exit.");
}


void OpenGLRenderer::create(Torpedo * torpedo, std::vector< std::unique_ptr<TypedBodyView> > & views) {
  debug(4, "create(Torpedo *) entry...");
  views.push_back(std::make_unique<TypedBodyView>(torpedo, vbos[2], shaderProgram, torpedo_vertices_count, 10.0f, GL_TRIANGLES)); 
  debug(4, "create(Torpedo *) exit.");
}

void OpenGLRenderer::create(Asteroid * asteroid, std::vector< std::unique_ptr<TypedBodyView> > & views) {
  debug(4, "create(Asteroid *) entry...");
  GLuint rock_vbo_index = 4 +  asteroid->get_rock_type();

  float scale = (asteroid->get_size() == 3 ? 30.0f : ( asteroid->get_size() == 2 ? 15.0f : 7.5f ));
 
  views.push_back(std::make_unique<TypedBodyView>(asteroid, vbos[rock_vbo_index], shaderProgram, asteroid_vertices_count, scale, GL_TRIANGLES)); 
  debug(4, "create(Asteroid *) exit.");
}

void OpenGLRenderer::create(SpaceshipDebris * debris, std::vector< std::unique_ptr<TypedBodyView> > & views) {
  debug(4, "create(SpaceshipDebris *) entry...");
  views.push_back(std::make_unique<TypedBodyView>(debris, vbos[10], shaderProgram, vertice_data[10]->size(), 0.1f, GL_POINTS,
            []() -> bool {return true;},
            [debris](TypedBodyView * view) -> void { view->set_scale( 0.2f * (SpaceshipDebris::TIME_TO_DELETE - debris->get_time_to_delete()));}));   
  debug(4, "create(SpaceshipDebris *) exit.");
}

void OpenGLRenderer::create(Debris * debris, std::vector< std::unique_ptr<TypedBodyView> > & views) {
  debug(4, "create(Debris *) entry...");
  views.push_back(std::make_unique<TypedBodyView>(debris, vbos[10], shaderProgram, vertice_data[10]->size(), 0.1f, GL_POINTS,
            []() -> bool {return true;},
            [debris](TypedBodyView * view) -> void { view->set_scale(Debris::TIME_TO_DELETE - debris->get_time_to_delete());}));   
  debug(4, "create(Debris *) exit.");
}

void OpenGLRenderer::createSpaceShipView() {
  spaceship_view = std::make_unique<OpenGLView>(vbos[0], shaderProgram, ship_vertices_count, GL_TRIANGLES);
}

void OpenGLRenderer::createDigitViews() {
  for (size_t i = 0; i < 10; i++ ) {
    digit_views[i] = std::make_unique<OpenGLView>(vbos[11 + i], shaderProgram, vertice_data[11 + i]->size(), GL_LINE_STRIP);
  }
}


void OpenGLRenderer::renderFreeShips(SquareMatrix4df & matrice) {
  constexpr float FREE_SHIP_X = 128;
  constexpr float FREE_SHIP_Y = 64;
  const float PIf = static_cast<float> ( PI );
  Vector2df position = {FREE_SHIP_X, FREE_SHIP_Y};
  SquareMatrix4df rotation = {   { std::cos(-PIf / 2.0f),  std::sin(-PIf / 2.0f), 0.0f, 0.0f},
                                 {-std::sin(-PIf / 2.0f),  std::cos(-PIf / 2.0f), 0.0f, 0.0f},
                                 { 0.0f,                 0.0f,                1.0f, 0.0f},
                                 { 0.0f,                 0.0f,                0.0f, 1.0f}
                               };
  for (int i = 0; i < game.get_no_of_ships(); i++) {
    SquareMatrix4df  translation= { {1.0f,        0.0f,         0.0f, 0.0f},
                                    {0.0f,        1.0f,         0.0f, 0.0f},
                                    {0.0f,        0.0f,         1.0f, 0.0f},
                                    {position[0], position[1],  0.0f, 1.0f} };
    SquareMatrix4df render_matrice = matrice * translation * rotation;
    spaceship_view->render( render_matrice );
    position[0] += 20.0;
  }
}

void OpenGLRenderer::renderScore(SquareMatrix4df & matrice) {
  constexpr float SCORE_X = 128 - 48;
  constexpr float SCORE_Y = 48 - 4;
  

  long long score = game.get_score();
  int no_of_digits = 0;
  if (score > 0) {
    no_of_digits = std::trunc( std::log10( score ) ) + 1;
  }

  Vector2df position = {SCORE_X + 20.0f * no_of_digits,  SCORE_Y};  
  do {
    int d = score % 10;
    score /= 10;
    SquareMatrix4df scale_translation= { {4.0f,        0.0f,         0.0f, 0.0f},
                                         {0.0f,        4.0f,         0.0f, 0.0f},
                                         {0.0f,        0.0f,         1.0f, 0.0f},
                                         {position[0], position[1],  0.0f, 1.0f} };
    SquareMatrix4df render_matrice = matrice * scale_translation;
    digit_views[d]->render( render_matrice );
    no_of_digits--;
    position[0] -= 20;

  } while (no_of_digits > 0);
}


void OpenGLRenderer::create_shader_programs() {

    static const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec3 incolor;\n"
        "layout (location = 2) in vec3 innormal;\n"
        "out vec3 color;\n"
        "out vec4 normal;\n"
        "uniform mat4 model;\n"
        "void main()\n"
        "{\n"
        "gl_Position = model * vec4(position, 1.0);\n"
        "color = incolor;\n"
        "normal = normalize( model * vec4(innormal, 0.0));\n"
        "}\0";

    // direction to light source is hard coded: (0,1,-4)
    // Lambertian shading used for vertices of triangle
    // cause during rasterization colors are interpolated, the result is Gouraud-Shading
    static const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 outColor;\n"
    "in vec3 color;\n"
    "in vec4 normal;\n"
    "void main () {\n"
    "  outColor = vec4(color * (0.3 + 0.7 * max(0.0, dot(normal, normalize( vec4(0.0, 1.0, -4.0, 0.0))))) , 1.0);\n"
    "}\n\0";

    // build and compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        error( std::string("vertex shader compilation failed") + infoLog);
    }
    // build and compiler fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        error( std::string("fragment shader compilation failed") + infoLog);
    }
    

    // link both shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        error( std::string("linking shader programs failed") + infoLog);
    }
}


bool OpenGLRenderer::init() {
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
    error( std::string("Could not initialize SDL. SDLError: ") + SDL_GetError() );
  } else {
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
    if( window == nullptr ) {
      error( std::string("Could not create Window. SDLError: ") + SDL_GetError() );
    } else {
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG );
      SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
      SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

      context = SDL_GL_CreateContext(window);
      
      GLenum err = glewInit(); // to be called after OpenGL render context is created
      if (GLEW_OK != err) {
        error( "Could not initialize Glew. Glew error message: " );
        error( glewGetErrorString(err) );
      }
      debug(1, "Using GLEW Version: ");
      debug(1, glewGetString(GLEW_VERSION) );
      glEnable(GL_DEPTH_TEST);

      SDL_GL_SetSwapInterval(1);

      create_shader_programs();
      createVbos();
      createSpaceShipView();
      createDigitViews();



      return true;
    }
  }
  return false;
}

/* tile positions in world coordinates
   used to draw objects seemless between boundary
  +---+---+---+   
  | 5 | 7 | 2 |
  +---+---+---+
  | 4 | 0 | 1 |
  +---+---+---+
  | 6 | 8 | 3 |
  +---+---+---+
*/

void OpenGLRenderer::render() {
  debug(2, "render() entry...");
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // transformation to canonical view and from left handed to right handed coordinates
  const auto w = static_cast<float>(window_width);
  const auto h = static_cast<float>(window_height);
  SquareMatrix4df world_transformation =
                         SquareMatrix4df{
                           { 2.0f / w,           0.0f,            0.0f,  0.0f},
                           {     0.0f,     -2.0f / h,            0.0f,  0.0f}, // (negative, because we have a left handed world coord. system)
                           {     0.0f,               0.0f,  2.0f / w,  0.0f},
                           {    -1.0f,               1.0f,            0.0f,  1.0f}
                         };

  glClearColor ( 0.0, 0.0, 0.0, 1.0 );
  glClear ( GL_COLOR_BUFFER_BIT );
  
  debug(2, "remove views for deleted objects");

  // remove all views for typed bodies that have to be deleted 
  erase_if(views, []( std::unique_ptr<TypedBodyView> & view) { return view->get_typed_body()->is_marked_for_deletion();}); 

  auto new_bodies = game.get_physics().get_recently_added_bodies();
  for (Body2df * body : new_bodies) {
    assert(body != nullptr);
    TypedBody * typed_body = static_cast<TypedBody *>(body);
    auto type = typed_body->get_type();
    if (type == BodyType::spaceship) {
      create( static_cast<Spaceship *>(typed_body), views );
    } else if (type == BodyType::torpedo ) {
      create( static_cast<Torpedo *>(typed_body), views );
    } else  if (type == BodyType::asteroid) {
      create( static_cast<Asteroid *>(typed_body), views );
    } else if (type == BodyType::saucer) {
      create( static_cast<Saucer *>(typed_body), views );
    } else if (type == BodyType::spaceship_debris ) {
      create( static_cast<SpaceshipDebris *>(typed_body), views );
    } else if (type == BodyType::debris) {
      create( static_cast<Debris *>(typed_body), views );
    }
  }

  SquareMatrix4df view_matrix = world_transformation;
  Spaceship * ship = game.get_ship();
  if (ship != nullptr) {
    Vector2df position = ship->get_position();
    SquareMatrix4df camera_translation = { {1.0f, 0.0f, 0.0f, 0.0f},
                                           {0.0f, 1.0f, 0.0f, 0.0f},
                                           {0.0f, 0.0f, 1.0f, 0.0f},
                                           {w / 2.0f - position[0], h / 2.0f - position[1], 0.0f, 1.0f} };
    view_matrix = world_transformation * camera_translation;
  }

  debug(2, "render all views");
  for (auto & view : views) {
    for (int i = -1; i <= 1; ++i) {
      for (int j = -1; j <= 1; ++j) {
        SquareMatrix4df tile_translation = { {1.0f, 0.0f, 0.0f, 0.0f},
                                             {0.0f, 1.0f, 0.0f, 0.0f},
                                             {0.0f, 0.0f, 1.0f, 0.0f},
                                             {i * w, j * h, 0.0f, 1.0f} };
        SquareMatrix4df tiled_view_matrix = view_matrix * tile_translation;
        view->render(tiled_view_matrix);
      }
    }
  }
  
  renderFreeShips(world_transformation);
  renderScore(world_transformation);

  SDL_GL_SwapWindow(window);
  debug(2, "render() exit.");
}

void OpenGLRenderer::exit() {
  views.clear();
  glDeleteBuffers(vertice_data.size(), vbos);
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow( window );
  SDL_Quit();
}
