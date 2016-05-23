#ifndef __MAIN__
#define __MAIN__

#include <cmath>

#include <SFML/Graphics.hpp>

#define PI2 3.14159 * 2

typedef unsigned int uint32;
typedef unsigned char byte;

const uint32 width = 800;
const uint32 height = 600;

byte *data_array = NULL;
sf::Image image;
sf::Sprite sprite;
sf::Texture texture;

sf::Image image2;
sf::Texture texture2;

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

  vec3 operator / (float factor) {
    vec3 result;
    result.x = x / factor;
    result.y = y / factor;
    result.z = z / factor;

    return result;
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

void putPixel(uint32 x, uint32 y, const RGBColor &color) {
  int p = (x + width * y) * 4;

  data_array[p] =     color.R;
  data_array[p + 1] = color.G;
  data_array[p + 2] = color.B;
  data_array[p + 3] = 255;

  image.setPixel(x, y, sf::Color(color.R, color.G, color.B, 255));
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

  float fov = 60.0f;
  float plane_dist = (width / 2.0f) / tan(fov / 2.0f);
  float cylinder_radius = 10.0f;

  int half_width = width / 2;
  int half_height = height / 2;

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
    for (int i = -half_height; i < half_height; ++i) {
      for (int j = -half_width; j < half_width; ++j) {
        vec3 pixel = vec3(j, i, plane_dist);

        vec3 d = pixel / pixel.norm();

        float t = cylinder_radius / sqrt(d.x * d.x + d.y * d.y);
        
        float angle = atan2(d.x, d.y);

        uint32 u = abs(d.z * 511);
        uint32 v = abs((angle * 511) / (3.14159 * 2));
        
        u *= 2;
        v *= 2;
        
        RGBColor color = image2.getPixel(u, v);

        putPixel(j + half_width, i + half_height, color);
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