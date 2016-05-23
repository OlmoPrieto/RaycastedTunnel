/*#ifndef __WOLFENSTEIN__
#define __WOLFENSTEIN__

#include <cmath>
#include <string>
#include <vector>
#include <iostream>

#include <SFML/Graphics.hpp>
//#include <SFML/Window.hpp>

//place the example code below here:

typedef unsigned int uint32;
typedef unsigned char byte;

#define mapWidth 24
#define mapHeight 24

int worldMap[mapWidth][mapHeight] =
{
  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 4, 0, 0, 0, 0, 5, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 4, 0, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

struct ColorRGB {
  ColorRGB(){}
  ColorRGB(byte _R, byte _G, byte _B) {
    R = _R; G = _G; B = _B;
  }

  ColorRGB operator/(uint32 factor) {
    R /= factor; G /= factor; B /= factor;

    return *this;
  }

  byte R;
  byte G;
  byte B;
};

sf::Sprite sprite;
sf::Texture texture;
sf::Image image;
byte *data_array;

void clearImage() {
  memset(data_array, 1, 800 * 600 * 4);
}

void verLine(int x, int drawStart, int drawEnd, ColorRGB color) {
  for (int i = 0; i < drawStart; ++i) {
    int p = (x + 800 * i) * 4;
    data_array[p] = 0;
    data_array[p + 1] = 0;
    data_array[p + 2] = 0;
    data_array[p + 3] = 255;

    image.setPixel(x, i, sf::Color(data_array[p], data_array[p + 1], data_array[p + 2], data_array[p + 3]));
  }

  for (int i = drawEnd; i < 600; ++i) {
    int p = (x + 800 * i) * 4;
    data_array[p] = 0;
    data_array[p + 1] = 0;
    data_array[p + 2] = 0;
    data_array[p + 3] = 255;

    image.setPixel(x, i, sf::Color(data_array[p], data_array[p + 1], data_array[p + 2], data_array[p + 3]));
  }

  "vec4 diffuseLighting(in vec3 N, in PointLight l, in vec3 material_color, in vec3 frag_pos) {\n"
  "  float distance = length(l.position - frag_pos);\n"
  "  float diff = max(dot(N, normalize(l.position - frag_pos)), 0.0);\n"
  "  float attenuation = 1.0f / (l.constant_factor + l.linear_factor * distance +\n"
  "    l.quadratic_factor * (distance * distance));\n"
  "  return vec4(l.diffuse_color * diff * material_color * attenuation, 1.0f);\n"
  "}\n"

  for (int i = drawStart; i < drawEnd; ++i) {
    int p = (x + 800 * i) * 4;
    data_array[p] = color.R;
    data_array[p + 1] = color.G;
    data_array[p + 2] = color.B;
    data_array[p + 3] = 255;

    image.setPixel(x, i, sf::Color(data_array[p], data_array[p + 1], data_array[p + 2], data_array[p + 3]));
  }
}

enum Key {
  none,
  up,
  down,
  left,
  right,
};

int main(int , char *[])
{
  double posX = 22, posY = 12;  //x and y start position
  double dirX = -1, dirY = 0; //initial direction vector
  double planeX = 0, planeY = 0.66; //the 2d raycaster version of camera plane

  double time = 0; //time of current frame
  double oldTime = 0; //time of previous frame

  int w = 800, h = 600;

  Key key = none;

  data_array = (byte*)malloc(800 * 600 * 4);

  texture.create(800, 600);
  image.create(800, 600, data_array);

  texture.update(image);
  sprite.setTexture(texture);

  // create the window
  sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {

      if (event.type == sf::Event::Closed) {
        window.close();
      }

      if (event.type == sf::Event::KeyPressed) {

      }
      else {
        key = none;
      }
    }

    // INPUT
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
      key = up;
    }// else 
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
      key = down;
    }// else 
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
      key = left;
    }// else 
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
      key = right;
    }

    //speed modifiers
    double moveSpeed = 0.06f;// * 5.0; //the constant value is in squares/second
    double rotSpeed = 0.03f;// * 3.0; //the constant value is in radians/second

    //move forward if no wall in front of you
    if (key == up)
    {
      if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
      if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
    }
    //move backwards if no wall behind you
    if (key == down)
    {
      if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
      if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
    }
    //rotate to the right
    if (key == right)
    {
      //both camera direction and camera plane must be rotated
      double oldDirX = dirX;
      dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
      dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
      double oldPlaneX = planeX;
      planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
      planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
    }
    //rotate to the left
    if (key == left)
    {
      //both camera direction and camera plane must be rotated
      double oldDirX = dirX;
      dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
      dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
      double oldPlaneX = planeX;
      planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
      planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
    }

    // CALCULATE THE RAYS
    for (int x = 0; x < w; x++) {
      //calculate ray position and direction
      double cameraX = 2 * x / double(w) - 1; //x-coordinate in camera space
      double rayPosX = posX;
      double rayPosY = posY;
      double rayDirX = dirX + planeX * cameraX;
      double rayDirY = dirY + planeY * cameraX;
      //which box of the map we're in
      int mapX = int(rayPosX);
      int mapY = int(rayPosY);

      //length of ray from current position to next x or y-side
      double sideDistX;
      double sideDistY;

      //length of ray from one x or y-side to next x or y-side
      double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
      double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
      double perpWallDist;

      //what direction to step in x or y-direction (either +1 or -1)
      int stepX;
      int stepY;

      int hit = 0; //was there a wall hit?
      int side; //was a NS or a EW wall hit?
      //calculate step and initial sideDist
      if (rayDirX < 0) {
        stepX = -1;
        sideDistX = (rayPosX - mapX) * deltaDistX;
      }
      else {
        stepX = 1;
        sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
      }
      if (rayDirY < 0) {
        stepY = -1;
        sideDistY = (rayPosY - mapY) * deltaDistY;
      }
      else {
        stepY = 1;
        sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
      }
      //perform DDA
      while (hit == 0) {
        //jump to next map square, OR in x-direction, OR in y-direction
        if (sideDistX < sideDistY) {
          sideDistX += deltaDistX;
          mapX += stepX;
          side = 0;
        }
        else {
          sideDistY += deltaDistY;
          mapY += stepY;
          side = 1;
        }
        //Check if ray has hit a wall
        if (worldMap[mapX][mapY] > 0) {
          hit = 1;
        }
      }
      //Calculate distance projected on camera direction (oblique distance will give fisheye effect!)
      if (side == 0) {
        perpWallDist = (mapX - rayPosX + (1 - stepX) / 2) / rayDirX;
      }
      else {
        perpWallDist = (mapY - rayPosY + (1 - stepY) / 2) / rayDirY;
      }

      //Calculate height of line to draw on screen
      int lineHeight = (int)(h / perpWallDist);

      //calculate lowest and highest pixel to fill in current stripe
      int drawStart = -lineHeight / 2 + h / 2;
      if (drawStart < 0) {
        drawStart = 0;
      }

      int drawEnd = lineHeight / 2 + h / 2;
      if (drawEnd >= h) {
        drawEnd = h - 1;
      }

      //choose wall color
      ColorRGB color;
      switch (worldMap[mapX][mapY])
      {
      case 1:  color = ColorRGB(255, 0, 0);       break; //red
      case 2:  color = ColorRGB(0, 255, 0);       break; //green
      case 3:  color = ColorRGB(0, 0, 255);       break; //blue
      case 4:  color = ColorRGB(255, 255, 255);   break; //white
      default: color = ColorRGB(255, 255, 0);     break; //yellow
      }

      //give x and y sides different brightness
      if (side == 1) {
        color = color / 2;
      }

      //draw the pixels of the stripe as a vertical line
      verLine(x, drawStart, drawEnd, color);
    }

    texture.update(image);

    window.clear(sf::Color::Black);
    window.draw(sprite);
    window.display();
  }

  free(data_array);

  return 0;
}

#endif // __WOLFENSTEIN__*/