#version 410

precision mediump float;

out vec4 FragColor;

struct Light {
  vec3 color;
  vec3 position;
};

struct Palette {
  vec3 color1;
  vec3 color2;
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
uniform Palette pal;
uniform Material material;
uniform sampler2D texture2d; //2D texture sampler
uniform sampler2D zaToon; //2D toon texture

vec3 toonShading(vec3 normal, vec3 frag_pos) 
{
  normal = normalize(normal);
  vec3 view_dir = normalize(camera - frag_pos); 
  vec3 light_dir = normalize(light.position - frag_pos); 
  vec3 half_dir = normalize(light_dir + view_dir); 

  // apply material 
  //float NdotL = max(dot(normal, light_dir), 0.0);
  float NdotL = (dot(normal, light_dir) + 1.0f) * 0.5;
  float NdotH = max(dot(normal, half_dir), 0.0);

  vec3 diffuse = NdotL * material.diffuse;
  vec3 specular = vec3(0.0);
  if (NdotL > 0.0)
      specular = pow(NdotH, material.shininess) * material.specular;

  vec3 gradient = texture(zaToon, vec2(NdotL, NdotL)).rgb;
  vec3 out_color = mix(pal.color2, pal.color1, gradient);

  vec3 lighting = diffuse + specular; 
  //return (NdotL) * gradient; 
  return out_color; 
}

void main()
{
  vec3 lighting = toonShading(vs_normal, vs_position);
  vec3 ambient = material.ambient * material.color;

  vec3 final_color = ambient + lighting * lighting;
  vec4 texture = texture(texture2d, vs_texcoord);
  FragColor = vec4(lighting, 1.0);
}