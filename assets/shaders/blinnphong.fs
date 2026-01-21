#version 410

precision mediump float;

out vec4 FragColor;

struct Light {
  vec3 color;
  vec3 position;
};

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  vec3 color;
  float shininess;
  float alpha;
};

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

// uniforms
uniform vec3 camera;
uniform Light light;
uniform Material material;
uniform sampler2D texture2d; //2D texture sampler

vec3 blinnphong(vec3 normal, vec3 frag_pos) 
{
  normal = normalize(normal);
  vec3 view_dir = normalize(camera - frag_pos); 
  vec3 light_dir = normalize(light.position - frag_pos); 
  vec3 half_dir = normalize(light_dir + view_dir); 

  // apply material 
  float NdotL = max(dot(normal, light_dir), 0.0); 
  float NdotH = max(dot(normal, half_dir), 0.0);

  vec3 diffuse = NdotL * material.diffuse;
  vec3 specular = vec3(0.0);
  if (NdotL > 0.0)
      specular = pow(NdotH, material.shininess) * material.specular;

  vec3 lighting = diffuse + specular; 
  return lighting * light.color; 
}

void main()
{
  vec3 lighting = blinnphong(vs_normal, vs_position);
  vec3 ambient = material.ambient * material.color;

  vec3 final_color = ambient + lighting * lighting;
  vec4 texture = texture(texture2d, vs_texcoord);
  FragColor = vec4(final_color, 1.0) * texture;
}