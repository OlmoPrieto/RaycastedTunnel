#ifndef __MAIN__
#define __MAIN__

#include <cmath>

#include <SFML/Graphics.hpp>

#define PI2 3.14159 * 2

typedef unsigned int uint32;
typedef unsigned char byte;

const uint32 width = 512;
const uint32 height = 512;

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

  byte R;
  byte G;
  byte B;
};
// [RGBColor]


void putPixel(uint32 x, uint32 y, const RGBColor &color) {
  /*if (x > 511) {
    x = 0;
  }*/
  /*if (y > 511) {
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

int main() {
  // create the window
  sf::RenderWindow window(sf::VideoMode(width, height), "RaycastedTunnel");

  data_array = (byte*)malloc(width * height * 4);

  texture.create(width, height);
  image.create(width, height, data_array);
  texture.update(image);
  sprite.setTexture(texture);

  image2.loadFromFile("resources/hello_kitty.jpg");
  const byte *image2_data = image2.getPixelsPtr();

  float fov = 45.0f;
  float plane_dist = (0.5f * (float)width) / tan(fov / 2.0f);
  float cylinder_radius = 10.0f;

  vec3 top_left(-1.0f, 1.0f, plane_dist);
  vec3 top_right(1.0f, 1.0f, plane_dist);
  vec3 bottom_left(-1.0f, -1.0f, plane_dist);
  vec3 bottom_right(1.0f, -1.0f, plane_dist);

  int half_width = width / 2;
  int half_height = height / 2;

  //float step = 0.015625f;
  float step = 0.0055625f;

  mat3 rotation;
  sf::Clock c;
  sf::Time time = c.getElapsedTime();

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
    float cos_y = cos(time.asMilliseconds() * 0.0001f);
    float sin_y = sin(time.asMilliseconds() * 0.0001f);
    // X axis
    /*rotation.matrix[4] = cos_y;
    rotation.matrix[5] = -sin_y;
    rotation.matrix[7] = sin_y;
    rotation.matrix[8] = cos_y;*/
    
    // Y axis
    rotation.matrix[0] = cos_y;
    rotation.matrix[2] = sin_y;
    rotation.matrix[6] = -sin_y;
    rotation.matrix[8] = cos_y;

    // Z axis
    /*rotation.matrix[0] = cos_y;
    rotation.matrix[1] = -sin_y;
    rotation.matrix[3] = sin_y;
    rotation.matrix[4] = cos_y;*/

    /*vec3 top_left(-1.0f, 1.0f, plane_dist);
    vec3 top_right(1.0f, 1.0f, plane_dist);
    vec3 bottom_left(-1.0f, -1.0f, plane_dist);
    vec3 bottom_right(1.0f, -1.0f, plane_dist);*/

    for (float i = top_left.y; i >= bottom_left.y; i -= step) {
      // LERP = (x * a) + (y * (1 - a))
      /*
        LERP left = top_left bottom_left
        LERP right = top_right bottom_right
        delta_x = (right.x - left.x ) / w;
        delta_y = (right.y - left.y) / w;
        delta_z = (right.z - left.z) / w;
      */

      // what lerp() does here is just substract i in the corresponding coordinate
      vec3 left = lerp(top_left, bottom_left, 1.0f - (i * 0.5f + 0.5f));
      vec3 right = lerp(top_right, bottom_right, 1.0f - (i * 0.5f + 0.5f));

      float delta_x = (right.x - left.x) / width;
      float delta_y = (right.y - left.y) / width;
      float delta_z = (right.z - left.z) / width;

      for (float j = top_left.x; j <= top_right.x; j += step) {
        //vec3 pixel = vec3(j * half_width, i * half_height, plane_dist);
        vec3 ray = left;
        
        ray.x += delta_x;
        ray.y += delta_y;
        ray.z += delta_z;

        vec3 d = ray / ray.norm();
        
        float t = cylinder_radius / sqrt(d.x * d.x + d.y * d.y);

        float angle = atan2(d.x * t, d.y * t);
        
        int _u = d.z * 511;
        int _v = (angle * 511) / (3.14159f * 2.0f);
        
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

        RGBColor color = image2.getPixel(u, v);

        putPixel((uint32)(j * half_width + half_width), (uint32)(i * (-half_height) + half_height), color);
        //putPixel((uint32)(ray.x * half_width + half_width), (uint32)(ray.y * (-half_height) + half_height), color);
      }
    }

    // "tunel" effect
    /*color.R = (color.R * (1.0f + d.z));
    color.G = (color.G * (1.0f + d.z));
    color.B = (color.B * (1.0f + d.z));*/

    /*if (d.z < -0.6f) {
    color.R = 0;
    color.G = 0;
    color.B = 0;
    }*/
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