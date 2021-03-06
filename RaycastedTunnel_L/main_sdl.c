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

static const uint32 width = 512;
static const uint32 height = 512;

static int end = 0;
static int cpu_mhz = 0;

static const float light_factor = 2000.0f;
static float fov = 30.0f;
static float cylinder_radius = 100.0f;
static float last_time = 0.0f;
static float inverse2PI = 1.0f / (PI * 2.0f);

static SDL_Surface *surface = NULL;
static uint32 *framebuffer = NULL;
static uint32* tex_ptr = NULL;

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

typedef struct {
  vec3 *tl;
  vec3 *tr;
  vec3 *bl;
  vec3 *br;
  float inverse_width;
  float inverse_height;
  float inverse_light_factor;
} inner_loop_params;

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
  return (grades * PI) / 180.0f;
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

static inline vec3 lerp(vec3 *a, vec3 *b, float alpha) {
  vec3 result;
  
  result.x = (a->x + alpha * (b->x - a->x));
  result.y = (a->y + alpha * (b->y - a->y));
  result.z = (a->z + alpha * (b->z - a->z));
  
  return result;
}

static inline uint32 GetPixel(uint32 x, uint32 y){
  return tex_ptr[x + width * y];
}

static inline void PutPixel(uint32 color, uint32 x, uint32 y) {
  framebuffer[x + width * y] = color;
}

static void ChronoShow (char* name, int computations){
  float ms = ChronoWatchReset();
  float cycles = ms * (1000000.0f/1000.0f) * (float)cpu_mhz;
  float cyc_per_comp = cycles / (float)computations;
  fprintf ( stdout, "%s: %f ms, %d cycles, %f cycles/iteration\n", name, ms, (int)cycles, cyc_per_comp);
}

// HOTSPOT: 4 calls per x loop, 4 * width / 8 calls per y loop
static void rayCalc(vec3 *r, float *r_depth, uint32 *u, uint32 *v) {
	float vec_len = sqrt(r->x * r->x + r->y * r->y);
	if (vec_len != 0.0f) {
	  float t = cylinder_radius / vec_len;
	
	  float angle = atan2(r->x, r->y);
    
	  *r_depth = r->z * t;

    // careful with roundf() !!
	  //uint32 _u = (uint32)roundf(*r_depth);
	  //uint32 _v = (uint32)roundf((angle * 511.0f) * inverse2PI);
    uint32 _u = (uint32)(*r_depth);
	  uint32 _v = (uint32)((angle * 511.0f) * inverse2PI);
    
	  *u = abs(_u); // abs for rays that go backwards
		*v = _v;
	  	  
	  //*u &= 511;
	  *v &= 511;
	}
}

static void InnerLoop(inner_loop_params *params) {
  for (int y = 0; y < height; y += 8) {
    float alpha = (float)y * params->inverse_height;
        
    vec3 left = lerp(params->tl, params->bl, alpha);
    vec3 right = lerp(params->tr, params->br, alpha);
        
	  float delta_x = (right.x - left.x) * params->inverse_width;
	  float delta_y = (right.y - left.y) * params->inverse_width;
    float delta_z = (right.z - left.z) * params->inverse_width;
    
    
    float alpha2 = ((float)y + 8.0f) * params->inverse_height;     
      
    vec3 left2 = lerp(params->tl, params->bl, alpha2);
    vec3 right2 = lerp(params->tr, params->br, alpha2);  
    
    float delta_y_x = (right2.x - left2.x) * params->inverse_width;
	  float delta_y_y = (right2.y - left2.y) * params->inverse_width;
    float delta_y_z = (right2.z - left2.z) * params->inverse_width; 
      
    vec3 ray = left;
    //copyVec3(&ray, &left);

	  for (int x = 0; x < width; x += 8) {
      
    	// Top right
      vec3 ray_tr;
    	ray_tr.x = ray.x + ((float)(x + 8) * delta_x);
    	ray_tr.y = ray.y + ((float)(x + 8) * delta_y);
    	ray_tr.z = ray.z + ((float)(x + 8) * delta_z);
      
    	// Top left
    	vec3 ray_tl;
      ray_tl.x = ray.x + ((float)(x) * delta_x);
      ray_tl.y = ray.y + ((float)(x) * delta_y);
      ray_tl.z = ray.z + ((float)(x) * delta_z);
      
      // Bottom right
    	vec3 ray_br; 
      ray_br.x = left2.x + ((float)(x + 8) * delta_y_x);
      ray_br.y = left2.y + ((float)(x + 8) * delta_y_y);
      ray_br.z = left2.z + ((float)(x + 8) * delta_y_z);
      
      // Bottom left
    	vec3 ray_bl;
    	ray_bl.x = left2.x + ((float)(x) * delta_y_x);
    	ray_bl.y = left2.y + ((float)(x) * delta_y_y);
      ray_bl.z = left2.z + ((float)(x) * delta_y_z);
      
      
    	float depth_tr = 10000.0f;
    	float depth_tl = 10000.0f;
    	float depth_br = 10000.0f;
    	float depth_bl = 10000.0f;

    	uint32 u_tr = 0; uint32 v_tr = 0;
    	uint32 u_tl = 0; uint32 v_tl = 0;
    	uint32 u_br = 0; uint32 v_br = 0;
    	uint32 u_bl = 0; uint32 v_bl = 0;

    	rayCalc(&ray_tr, &depth_tr, &u_tr, &v_tr);
    	rayCalc(&ray_tl, &depth_tl, &u_tl, &v_tl);
    	rayCalc(&ray_br, &depth_br, &u_br, &v_br);
    	rayCalc(&ray_bl, &depth_bl, &u_bl, &v_bl);

    	float luminance_tr = (light_factor - abs(depth_tr)) * params->inverse_light_factor;
    	if (luminance_tr <= 0.0f) {
		    luminance_tr = 0.0f;
    	}
    	float luminance_tl = (light_factor - abs(depth_tl)) * params->inverse_light_factor;
    	if (luminance_tl <= 0.0f) {
      	luminance_tl = 0.0f;
   	  }
    	float luminance_br = (light_factor - abs(depth_br)) * params->inverse_light_factor;
    	if (luminance_br <= 0.0f) {
    		luminance_br = 0.0f;
    	}
    	float luminance_bl = (light_factor - abs(depth_bl)) * params->inverse_light_factor;
    	if (luminance_bl <= 0.0f) {
      	luminance_bl = 0.0f;
    	}
        
      float left_du = ((float)u_bl - (float)u_tl) * 0.125f;
      float left_dv = ((float)v_bl - (float)v_tl) * 0.125f;
      float right_du = ((float)u_br - (float)u_tr) * 0.125f;
      float right_dv = ((float)v_br - (float)v_tr) * 0.125f;

      float left_u = (float)u_tl;
      float left_v = (float)v_tl;
      float right_u = (float)u_tr;
      float right_v = (float)v_tr;
      
      float left_dlum = (luminance_bl - luminance_tl) * 0.125f;
      float right_dlum = (luminance_br - luminance_tr) * 0.125f;
      float left_lum = luminance_tl;
      float right_lum = luminance_tr;

      float du = 0.0f, dv = 0.0f;
      uint32 u = 0, v = 0;
      float dlum = 0.0f;
      float lum = 0.0f;
      
      // HOTSPOT: 64 iterations each x loop, 64 * width / 8 iterations each y loop
      for (int dy = 0; dy < 8; dy++) {
	      du = (right_u - left_u) * 0.125f;
	      dv = (right_v - left_v) * 0.125f;
	      //u = (uint32)roundf(left_u);
	      //v = (uint32)roundf(left_v);
	      u = (uint32)left_u;
	      v = (uint32)left_v;
	
	      dlum = (right_lum - left_lum) * 0.125f;
	      lum = left_lum;
        
	      for (int dx = 0; dx < 8; dx++) {
        	u &= 511;
        	v &= 511;
	      	
		      uint32 color = GetPixel(u, v);

	      	unsigned char color_r = (0x000000ff & color);
	      	color_r = (unsigned char)((float)color_r * lum);
	      	unsigned char color_g = (0x0000ff00 & color) >> 8;
	      	color_g = (unsigned char)((float)color_g * lum);
	      	unsigned char color_b = (0x00ff0000 & color) >> 16;
	      	color_b = (unsigned char)((float)color_b * lum);
	      	unsigned char color_a = (0xff000000 & color) >> 24; 
		
		      color = (0xff000000 & (color_a << 24)) | 
              	       (0x00ff0000 & (color_b << 16)) |
        			         (0x0000ff00 & (color_g << 8))  |
              		     (0x000000ff & (color_r));		

	      	PutPixel(color, x + dx, y + dy);
		      				    
	      	u = (uint32)roundf((float)u + du);
	      	v = (uint32)roundf((float)v + dv);
		      
	      	lum += dlum;
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
}   

static void CreateBuffer() {
  framebuffer = (uint32*)malloc(surface->w * surface->h * sizeof(uint32));
}

static void DestroyBuffer() {
  free(framebuffer);
}

/*static void CleanBuffer(){
  memset(surface, 0, width * height * sizeof(uint32));
}*/

static void CopyToSDL (unsigned int *dst, unsigned int *src,
                      uint32 w, uint32 h, uint32 stride){
  uint32 y = 0;
  for (y = 0; y < h; y++){
    unsigned int *src_line = src + y * w;
    unsigned int *dst_line = dst + y * stride;
    memcpy(dst_line, src_line, w * sizeof(uint32));
  }

}

static void LimitFramerate (int fps) {  
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
  //uint32 half_height = height / 2;
  vec3 mouse_pos;
  mouse_pos.x = 0.0f;
  mouse_pos.y = 0.0f;
  mouse_pos.z = 0.0f;
  
  mat3 rotation;
  float fov_r = gradesToRadians(fov * 0.5f);
  float inverse_width = 1.0f / (float)width;
  float inverse_height = 1.0f / (float)height;
  float inverse_light_factor = 1.0f / light_factor;
  
  inner_loop_params params;
  
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
    
    int x = 0, y = 0;
    SDL_GetMouseState(&x, &y);

    mouse_pos.x = x;
    mouse_pos.x /= half_width;
    mouse_pos.y = y;
    
    setIdentity(&rotation);
    setYRotation(&rotation, mouse_pos.x);

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
    params.tl = &top_left_;
    params.tr = &top_right_;
    params.bl = &bottom_left_;
    params.br = &bottom_right_;
    params.inverse_width = inverse_width;
    params.inverse_height = inverse_height;
    params.inverse_light_factor = inverse_light_factor;
    InnerLoop(&params);
    //----- Update
    ChronoShow ("INNER LOOP", surface->w * surface->h);

    ChronoWatchReset();
    //----- Draw
    CopyToSDL(surface->pixels, framebuffer, surface->w, surface->h, surface->pitch >> 2);
    ChronoShow ("CopyToSDL", surface->w * surface->h);
    
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
          // printf("Mouse button %d pressed at (%d,%d)\n",
          // event.button.button, event.button.x, event.button.y);
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
