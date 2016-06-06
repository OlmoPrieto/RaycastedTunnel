#ifndef __MAIN__
#define __MAIN__

#include <cmath>

#include <SFML/Graphics.hpp>

#define PI 3.14159

typedef unsigned int uint32;
typedef unsigned char byte;

const uint32 width = 512;
const uint32 height = 512;

float light_factor = 2000.0f;

byte *data_array = NULL;
sf::Image image;
sf::Sprite sprite;
sf::Texture texture;

sf::Image image2;
sf::Texture texture2;

// [vec3]
struct vec3 {
  vec3(){}
  vec3(float _x, float _y, float _z) {
    x = _x; y = _y; z = _z;
  }

  vec3 operator-(const vec3 &other) {
    vec3 result;
    result.x = x - other.x;
    result.y = y - other.y;
    result.z = z - other.z;

    return result;
  }

  vec3 operator +(const vec3 &other) {
    vec3 result;
    result.x = x + other.x;
    result.y = y + other.y;
    result.z = z + other.z;

    return result;
  }

  vec3 operator / (float factor) {
    vec3 result;
    result.x = x / factor;
    result.y = y / factor;
    result.z = z / factor;

    return result;
  }

  vec3 operator * (float factor) {
    vec3 result;
    result.x = x * factor;
    result.y = y * factor;
    result.z = z * factor;

    return result;
  }

  vec3 operator = (const vec3 &other) {
    x = other.x;
    y = other.y;
    z = other.z;

    return *this;
  }

  float norm() {
    return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
  }

  void normalize() {
    float norm = this->norm();

    x /= norm;
    y /= norm;
    z /= norm;
  }

  float x;
  float y;
  float z;
};
// [vec3]
vec3 operator * (float f, const vec3 &vec) {
  vec3 result = vec;
  return result * f;
}

struct mat3 {
  mat3() {
    matrix[0] = 1;
    matrix[1] = 0;
    matrix[2] = 0;

    matrix[3] = 0;
    matrix[4] = 1;
    matrix[5] = 0;

    matrix[6] = 0;
    matrix[7] = 0;
    matrix[8] = 1;
  }

  float operator [](uint32 index) {
    return matrix[index];
  }

  vec3 operator * (const vec3 &vec) {
    vec3 result;

    result.x = matrix[0] * vec.x + matrix[1] * vec.y + matrix[2] * vec.z;
    result.y = matrix[3] * vec.x + matrix[4] * vec.y + matrix[5] * vec.z;
    result.z = matrix[6] * vec.x + matrix[7] * vec.y + matrix[8] * vec.z;

    return result;
  }

  float matrix[9];
};

// [RGBColor]
struct RGBColor {
  RGBColor() {}
  RGBColor(byte _R, byte _G, byte _B) {
    R = _R; G = _G; B = _B;
  }
  RGBColor(const sf::Color &sf_color) {
    R = sf_color.r;
    G = sf_color.g;
    B = sf_color.b;
  }
  
  RGBColor operator = (const sf::Color &sf_color) {
    R = sf_color.r;
    G = sf_color.g;
    B = sf_color.b;
  }

  RGBColor operator * (float f){
    RGBColor result;
    result.R = R * f;
    result.G = G * f;
    result.B = B * f;

    return result;
  }

  byte R;
  byte G;
  byte B;
};
// [RGBColor]


void putPixel(uint32 x, uint32 y, const RGBColor &color) {
  /*if (x > 511) {
    x = 0;
  }
  if (y > 511) {
    y = 0;
  }*/
  int p = (x + width * y) * 4;

  data_array[p] =     color.R;
  data_array[p + 1] = color.G;
  data_array[p + 2] = color.B;
  data_array[p + 3] = 255;

  image.setPixel(x, y, sf::Color(color.R, color.G, color.B, 255));
}

// LERP = (x * a) + (y * (1 - a))
vec3 lerp(vec3 a, vec3 b, float alpha){
  //return vec3((a * alpha) + (b * (1.0f - alpha)));
  return vec3(a + alpha * (b - a));
}

float angleToRadians(float a) {
  return a * PI / 180.0f;
}

int main() {
  // create the window
  sf::RenderWindow window(sf::VideoMode(width, height), "RaycastedTunnel");

  data_array = (byte*)malloc(width * height * 4);

  texture.create(width, height);
  image.create(width, height, data_array);
  texture.update(image);
  sprite.setTexture(texture);

  image2.loadFromFile("resources/lava.png");
  const byte *image2_data = image2.getPixelsPtr();

  float fov = 30.0f;
  float plane_dist = (0.5f * (float)width) / tan(fov / 2.0f);
  float cylinder_radius = 100.0f;

  mat3 rotation;
  // X axis
  /*rotation.matrix[4] = cos(fov * 0.5f);
  rotation.matrix[5] = -sin(fov * 0.5f);
  rotation.matrix[7] = sin(fov * 0.5f);
  rotation.matrix[8] = cos(fov * 0.5f);*/

  // Y axis
  /*rotation.matrix[0] = cos(fov * 0.5f);
  rotation.matrix[2] = sin(fov * 0.5f);
  rotation.matrix[6] = -sin(fov * 0.5f);
  rotation.matrix[8] = cos(fov * 0.5f);*/

  // Z axis
  /*rotation.matrix[0] = cos_y;
  rotation.matrix[1] = -sin_y;
  rotation.matrix[3] = sin_y;
  rotation.matrix[4] = cos_y;*/

  /*vec3 top_left(-1.0f, 1.0f, plane_dist);
  vec3 top_right(1.0f, 1.0f, plane_dist);
  vec3 bottom_left(-1.0f, -1.0f, plane_dist);
  vec3 bottom_right(1.0f, -1.0f, plane_dist);*/
  
  float fov_r = angleToRadians(fov * 0.5f);

  vec3 right_vector(sin(fov_r), 0.0f, cos(fov_r));
  vec3 left_vector(-sin(fov_r), 0.0f, cos(fov_r));

  rotation.matrix[4] = cos(fov_r);
  rotation.matrix[5] = -sin(fov_r);
  rotation.matrix[7] = sin(fov_r);
  rotation.matrix[8] = cos(fov_r);
  vec3 top_left(rotation * left_vector);
  vec3 top_right(rotation * right_vector);

  // X axis
  rotation.matrix[4] = cos(-fov_r);
  rotation.matrix[5] = -sin(-fov_r);
  rotation.matrix[7] = sin(-fov_r);
  rotation.matrix[8] = cos(-fov_r);
  vec3 bottom_left(rotation * left_vector);
  vec3 bottom_right(rotation * right_vector);

  rotation = mat3();

  int half_width = width / 2;
  int half_height = height / 2;

  sf::Clock c;
  sf::Time time = c.getElapsedTime();
  sf::Time last_time = c.getElapsedTime();
  float dt = 0.0f;

  vec3 mouse_pos;



  while (window.isOpen()) {

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    // [INPUT]
    //
    // [INPUT]



    // [UPDATE]
    //
    
    time = c.getElapsedTime();
    dt = time.asMilliseconds() - last_time.asMilliseconds();
    printf("dt: %f\n", dt);
    last_time = time;
    float cos_y = cos(angleToRadians(time.asMilliseconds() * 0.01f));
    float sin_y = sin(angleToRadians(time.asMilliseconds() * 0.01f));
    

    mouse_pos.x = sf::Mouse::getPosition(window).x;
    //mouse_pos.y = sf::Mouse::getPosition(window).y;

    // Normalize mouse pos
    mouse_pos.x /= half_width; // -> normalized with half width so we can do a full circle
    //mouse_pos.y /= height;   // -> not useful


    // Y axis
    rotation.matrix[0] = cos(mouse_pos.x);
    rotation.matrix[2] = sin(mouse_pos.x);
    rotation.matrix[6] = sin(mouse_pos.x) * (-1);
    rotation.matrix[8] = cos(mouse_pos.x);

    // Y axis
    //rotation.matrix[0] = cos_y;
    //rotation.matrix[2] = sin_y;
    //rotation.matrix[6] = -sin_y;
    //rotation.matrix[8] = cos_y ;


    vec3 top_right_ = rotation * top_right;
    vec3 top_left_ = rotation * top_left;
    vec3 bottom_right_ = rotation * bottom_right;
    vec3 bottom_left_ = rotation * bottom_left;

    for (int y = 0; y < height; y++) {
      float alpha = ((float)y) / (float)height;

      vec3 left = lerp(top_left_, bottom_left_, alpha);
      vec3 right = lerp(top_right_, bottom_right_, alpha);

      /*vec3 left = lerp(top_left, bottom_left, alpha);
      vec3 right = lerp(top_right, bottom_right, alpha);*/

      float delta_x = (right.x - left.x) / (float)width;
      float delta_y = (right.y - left.y) / (float)width;
      float delta_z = (right.z - left.z) / (float)width;

      vec3 ray = left;

      for (int x = 0; x < width; x++) {   
        float vec_len = sqrt(ray.x * ray.x + ray.y * ray.y);
        if (vec_len != 0.0f) {
          float t = cylinder_radius / vec_len;
        
          float angle = atan2(ray.x, ray.y);
          
          float depth = ray.z * t;

          int _u = depth;
          int _v = (angle * 511) / (PI * 2.0f);

          uint32 u;
          uint32 v;

          u = abs(_u);
          //v = abs(_v);

          if (_v < 0) {
            v = 511 - abs(_v);
          } else {
            v = _v;
          }
          //v = abs(_v);
          u &= 511;
          v &= 511;

          RGBColor color = image2.getPixel(u, v);
          float luminance = (light_factor - abs(depth)) / light_factor;
          if (luminance <= 0) {
            luminance = 0.0f;
          }

          color = color * luminance;

          putPixel((uint32)(x), (uint32)(y), color);
        }

        ray.x += delta_x;
        ray.y += delta_y;
        ray.z += delta_z;
      }
    }
    // [UPDATE]


    // [DRAW]
    window.clear(sf::Color::Black);
    texture.update(image);
    window.draw(sprite);
    window.display();
    // [DRAW]
  }

  free(data_array);

  return 0;
}

#endif // __MAIN__