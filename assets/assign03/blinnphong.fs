#version 410

struct Light {
  vec3 color;
  vec3 position;
};

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;
uniform sampler2D g_material;

uniform Light light;
uniform vec3 camera_position;

out vec4 FragLighting;

vec4 blinnphong(vec3 position, vec3 normal, vec3 material) 
{
  
  return vec4(0.0, 0.3, 0.7, 1.0); 
}

void main()
{
  FragLighting = vec4(light.color, 1.0);
}