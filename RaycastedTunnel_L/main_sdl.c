#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <SDL/SDL.h>
//#include <SDL_image.h>

#include "chrono.h"

#define PI 3.14159

// Textura

struct GimpScrFormat{
  int  w;
  int  h;
  int  bytes_per_pixel; /* 3:RGB, 4:RGBA */ 
  unsigned char pixel_data[4];
};
  
extern struct GimpScrFormat gimp_image;

typedef unsigned int uint32;
typedef unsigned char byte;

static int cpu_mhz = 0;
float last_time = 0.0f;

const uint32 width = 512;
const uint32 height = 512;

float light_factor = 2000.0f;
float fov = 30.0f;
float cylinder_radius = 100.0f;

SDL_Surface *surface = NULL;
uint32 *framebuffer = NULL;
uint32* tex_ptr = NULL;
int end = 0;

typedef struct {
  byte r;
  byte g;
  byte b;
} RGBColor;

typedef struct {
  float x;
  float y;
  float z;
} vec3;

static inline void copyVec3(vec3 *dst, vec3 *src) {
  dst->x = src->x;
  dst->y = src->y;
  dst->z = src->z;
}

typedef struct {
  float matrix[9];
} mat3;

static inline void setIdentity(mat3 *m) {
  m->matrix[0] = 1.0f;
  m->matrix[1] = 0.0f;
  m->matrix[2] = 0.0f;
  m->matrix[3] = 0.0f;
  m->matrix[4] = 1.0f;
  m->matrix[5] = 0.0f;
  m->matrix[6] = 0.0f;
  m->matrix[7] = 0.0f;
  m->matrix[8] = 1.0f;
}

static inline void mat3xvec3(mat3 *m, vec3 *v, vec3 *result) {
  result->x = m->matrix[0] * v->x + m->matrix[1] * v->y + m->matrix[2] * v->z;
  result->y = m->matrix[3] * v->x + m->matrix[4] * v->y + m->matrix[5] * v->z;
  result->z = m->matrix[6] * v->x + m->matrix[7] * v->y + m->matrix[8] * v->z;
}

static inline float gradesToRadians(float grades) {
  return grades * PI / 180.0f;
}

static inline void setXRotation(mat3 *m, float radians) {
  m->matrix[4] = cos(radians);
  m->matrix[5] = -sin(radians);
  m->matrix[7] = sin(radians);
  m->matrix[8] = cos(radians);
}

static inline void setYRotation(mat3 *m, float radians) {
  m->matrix[0] = cos(radians);
  m->matrix[2] = sin(radians);
  m->matrix[6] = -sin(radians);
  m->matrix[8] = cos(radians);
}

static inline void setZRotation(mat3 *m, float radians) {
  m->matrix[0] = cos(radians);
  m->matrix[1] = -sin(radians);
  m->matrix[3] = sin(radians);
  m->matrix[4] = cos(radians);
}

static void rayCalc(vec3 *r, float *r_depth, uint32 *u, uint32 *v) {
	float vec_len = sqrt(r->x * r->x + r->y * r->y);
	if (vec_len != 0.0f) {
	  float t = cylinder_radius / vec_len;
	
	  float angle = atan2(r->x, r->y);
	  
	  *r_depth = r->z * t;

	  int _u = *r_depth;
	  int _v = (angle * 511) / (PI * 2.0f);

	  *u = abs(_u);

	  if (_v < 0) {
		*v = 511 - abs(_v);
	  } else {
		*v = _v;
	  }
	  
	  *u &= 511;
	  *v &= 511;
	}
  //printf("%u - %u\n", *u, *v);
}

static inline vec3 lerp(vec3 *a, vec3 *b, float alpha) {
  vec3 result;
  
  result.x = (a->x + alpha * (b->x - a->x));
  result.y = (a->y + alpha * (b->y - a->y));
  result.z = (a->z + alpha * (b->z - a->z));
  
  return result;
}

static void CreateBuffer() {
  framebuffer = (uint32*)malloc(surface->w * surface->h * sizeof(uint32));
}

static void DestroyBuffer() {
  free(framebuffer);
}

static void CleanBuffer(){
  memset(surface, 0, width * height * sizeof(uint32));
}

static inline uint32 GetPixel(uint32 x, uint32 y){
  return tex_ptr[x + width * y];
}

static inline void PutPixel(uint32 color, uint32 x, uint32 y) {
  framebuffer[x + width * y] = color;
}

static void CopyToSDL (unsigned int *dst, unsigned int *src,
                      uint32 w, uint32 h, uint32 stride){
  uint32 y = 0;
  for (y = 0; y < h; y++){
    unsigned int *src_line = src + y * w;
    unsigned int *dst_line = dst + y * stride;
    memcpy(dst_line, src_line, w * sizeof(uint32));
  }

}
static void ChronoShow (char* name, int computations)
{
  float ms = ChronoWatchReset();
  float cycles = ms * (1000000.0f/1000.0f) * (float)cpu_mhz;
  float cyc_per_comp = cycles / (float)computations;
  fprintf ( stdout, "%s: %f ms, %d cycles, %f cycles/iteration\n", name, ms, (int)cycles, cyc_per_comp);
}

static void LimitFramerate (int fps) 
{  
  static unsigned int frame_time = 0;
  unsigned int t = GetMsTime();
  unsigned int elapsed = t - frame_time;
  // El tiempo sobrante se "regala" al sistema operativo con la funcion POSIX sleep
  const unsigned int limit = 1000 / fps;  // 1000 miliseconds / framerate minimo deseado 
  if ( elapsed < limit)
    usleep (( limit - elapsed) * 1000); // arg in microseconds
  frame_time = GetMsTime();
}



int main(int argc, char **argv) {
  if ( argc < 2) { fprintf ( stderr, "I need the cpu speed in Mhz!\n"); exit(0);}
  cpu_mhz = atoi( argv[1]);
  assert(cpu_mhz > 0);
  fprintf ( stdout, "Cycle times for a %d Mhz cpu\n", cpu_mhz);

   // Init SDL and screen
  if (SDL_Init( SDL_INIT_VIDEO) < 0)  {
    fprintf(stderr, "Can't Initialise SDL: %s\n", SDL_GetError());
    exit(1);
  }
  if (0 == SDL_SetVideoMode(width, height, 32,  SDL_HWSURFACE | SDL_DOUBLEBUF)) {
    printf("Couldn't set %dx%dx32 video mode: %s\n", width, height, SDL_GetError());
    return 0;
  }

  surface = SDL_GetVideoSurface();
  CreateBuffer(); 
  
  tex_ptr = (uint32*)gimp_image.pixel_data;
  
  uint32 half_width = width / 2;
  uint32 half_height = height / 2;
  vec3 mouse_pos;
  
  mat3 rotation;
  float fov_r = gradesToRadians(fov * 0.5f);
  
  vec3 right_vector;
  vec3 left_vector;
  right_vector.x = sin(fov_r); right_vector.y = 0.0f; right_vector.z = cos(fov_r);
  left_vector.x = -sin(fov_r); left_vector.y = 0.0f; left_vector.z = cos(fov_r);
  
  setIdentity(&rotation);
  setXRotation(&rotation, fov_r);
  vec3 top_left;
  mat3xvec3(&rotation, &left_vector, &top_left);
  vec3 top_right;
  mat3xvec3(&rotation, &right_vector, &top_right);
  
  setIdentity(&rotation);
  setXRotation(&rotation, -fov_r);
  vec3 bottom_left;
  mat3xvec3(&rotation, &left_vector, &bottom_left);
  vec3 bottom_right;
  mat3xvec3(&rotation, &right_vector, &bottom_right);
  
  setIdentity(&rotation);
  
  LimitFramerate(60);
  while (!end) {
    SDL_Event event;

    // Lock screen to get access to the memory array
    SDL_LockSurface(surface);

    //----- Update
    float delta_time = SDL_GetTicks() / 1000;
    last_time = delta_time;
    
    // mouse_pos.x = GETFROMWINDOW();
    // mouse_pos.x /= half_width;
    
    //setYRotation(&rotation, mouse_pos.x);

    vec3 top_right_;
    mat3xvec3(&rotation, &top_right, &top_right_);
    vec3 top_left_;
    mat3xvec3(&rotation, &top_left, &top_left_);
    vec3 bottom_right_;
    mat3xvec3(&rotation, &bottom_right, &bottom_right_);
    vec3 bottom_left_;
    mat3xvec3(&rotation, &bottom_left, &bottom_left_);
    
    ChronoWatchReset();

    // INNER LOOP
for (int y = 0; y < height; y += 8) {
	float alpha = (float)y / (float)height;
      
    vec3 left = lerp(&top_left_, &bottom_left_, alpha);
    vec3 right = lerp(&top_right_, &bottom_right_, alpha);
      
	float delta_x = (right.x - left.x) / (float)width;
  float delta_y = (right.y - left.y) / (float)width;
  float delta_z = (right.z - left.z) / (float)width;
	  
  vec3 ray;
  copyVec3(&ray, &left);
	
	for (int x = 0; x < width; x += 8) {
    
    // Top right
		vec3 ray_tr; 
    ray_tr.x = ray.x + ((x + 8) * delta_x); 
    ray_tr.y = ray.y; 
    ray_tr.z = ray.z;

    // Top left
		vec3 ray_tl;
    ray_tl.x = ray.x + delta_x * x;
    ray_tl.y = ray.y;
    ray_tl.z = ray.z;
    
    // Bottom right
		vec3 ray_br; 
    ray_br.x = ray.x + ((x + 8) * delta_x);
    ray_br.y = ray.y + (8 * delta_y);
    ray_br.z = ray.z;
    
    // Bottom left
		vec3 ray_bl; 
    ray_bl.x = ray.x; 
    ray_bl.y = ray.y + (8 * delta_y);
    ray_bl.z = ray.z;
		
		float depth_tr = 0.0f;
		float depth_tl = 0.0f;
		float depth_br = 0.0f;
		float depth_bl = 0.0f;
		
		uint32 u_tr = 0; uint32 v_tr = 0;
		uint32 u_tl = 0; uint32 v_tl = 0;
		uint32 u_br = 0; uint32 v_br = 0;
		uint32 u_bl = 0; uint32 v_bl = 0;
		
		rayCalc(&ray_tr, &depth_tr, &u_tr, &v_tr);
		rayCalc(&ray_tl, &depth_tl, &u_tl, &v_tl);
		rayCalc(&ray_br, &depth_br, &u_br, &v_br);
		rayCalc(&ray_bl, &depth_bl, &u_bl, &v_bl);
		
		float luminance_tr = (light_factor - abs(depth_tr)) / light_factor;
		if (luminance_tr <= 0) {
			luminance_tr = 0.0f;
		}
		float luminance_tl = (light_factor - abs(depth_tl)) / light_factor;
		if (luminance_tl <= 0) {
			luminance_tl = 0.0f;
		}
		float luminance_br = (light_factor - abs(depth_br)) / light_factor;
		if (luminance_br <= 0) {
			luminance_br = 0.0f;
		}
		float luminance_bl = (light_factor - abs(depth_bl)) / light_factor;
		if (luminance_bl <= 0) {
			luminance_bl = 0.0f;
		}
		
		uint32 left_du = abs(u_bl - u_tl) / 8;
		uint32 left_dv = abs(v_bl - v_tl) / 8;
		uint32 right_du = abs(u_br - u_tr) / 8;
		uint32 right_dv = abs(v_br - v_tr) / 8;
    
		uint32 left_u = u_tl;
		uint32 left_v = v_tl;
		uint32 right_u = u_tr;
		uint32 right_v = v_tr;
		
		uint32 left_dlum = abs(luminance_bl - luminance_tl) / 8;
		uint32 right_dlum = abs(luminance_br - luminance_tr) / 8;
		uint32 left_lum = luminance_tl;
		uint32 right_lum = luminance_tr;
		
		uint32 du = 0, dv = 0;
		uint32 u = 0, v = 0;
		uint32 dlum = 0;
		uint32 lum = 0;
		
		for (int dy = 0; dy < 8; dy++) {
			du = abs(right_u - left_u) / 8;
			dv = abs(right_v - left_v) / 8;
			u = left_u;
			v = left_v;
			
			dlum = abs(right_lum - left_lum) / 8;
			lum = left_lum;
  
			for (int dx = 0; dx < 8; dx++) {
				// read texture
        // printf("%u - %u\n", u, v);
        v &= 511;
				uint32 color = GetPixel(u, v);
				//color *= lum;
				
				PutPixel(color, x + dx, y + dy);
				
				u += du;
				v += dv;
				
				lum += dlum;
				
				/*ray.x += delta_x;
				ray.y += delta_y;
				ray.z += delta_z;*/
			}
			
			left_u += left_du;
			left_v += left_dv;
			right_u += right_du;
			right_v += right_dv;
			
			left_lum += left_dlum;
			right_lum += right_dlum;
		}
	}
}
    //----- Update

    ChronoShow ( "INNER LOOP", surface->w * surface->h);

    //----- Draw
    CopyToSDL(surface->pixels, framebuffer, surface->w, surface->h, surface->pitch >> 2);
    
    SDL_UnlockSurface(surface);
    SDL_Flip(surface);
    //----- Draw

    // maybe not needed
    //CleanBuffer();

    // Recoger eventos de la ventana
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_MOUSEMOTION:
          break;
        case SDL_MOUSEBUTTONDOWN:
          //printf("Mouse button %d pressed at (%d,%d)\n",
          //   event.button.button, event.button.x, event.button.y);
          break;
        case SDL_QUIT:
          end = 1;
          break;
      }
    } // while(eventos)
  } // while(game_loop)

  SDL_Quit();
  DestroyBuffer();

  return 0;
}