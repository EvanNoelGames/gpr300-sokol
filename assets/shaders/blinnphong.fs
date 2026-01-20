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

vec3 blinnphong(vec3 normal, vec3 frag_pos, vec3 light_pos) {
  vec3 view_dir = normalize(camera - frag_pos);
  vec3 light_dir = normalize(light.position - frag_pos);
  vec3 reflect_dir = reflect(light_dir, vs_normal);
  vec3 half_dir = normalize(light_dir + view_dir);

  // apply material 
  float NdotL = max(dot(normal, light_dir), 0.0);
  float NdotH = max(dot(normal, half_dir), 0.0);

  vec3 diffuse = NdotL * material.diffuse;
  float specular = pow(NdotH, material.shininess) * material.alpha;
  vec3 lighting = vec3(diffuse) + vec3(specular);

  return lighting * light.color;
}

void main()
{
  vec3 lighting = blinnphong(vs_normal, vs_position, light.position) + material.ambient * 0.5;
  vec3 object_color = vs_normal.rgb * 0.5 + 0.5;
  vec3 final_color = object_color * lighting;

  FragColor = vec4(final_color, 1.0);
}