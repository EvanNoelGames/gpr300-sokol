#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// uniforms
uniform sampler2D screen;

// uniform float offset
uniform float offset = 1.0 / 600.0;
uniform float strength = 0.5;

vec2 offsets[9]
 = vec2[](
  vec2(-offset, offset), // top left
  vec2(0.0, offset), // top center
  vec2(offset, offset), // top right
  
  vec2(-offset, 0.0), // center left
  vec2(0.0, 0.0), // center
  vec2(0.0, offset), // center right

  vec2(-offset, -offset), // bottom left
  vec2(0.0, -offset), // bottom center
  vec2(offset, -offset) // bottom right
);
const float kernal[9] = float[](
  0.0, -1.0, 0.0,
  -1.0, 5.0, -1.0,
  0.0, -1.0, 0.0
);

void main()
{
  vec3 color = vec3(0.0);

  for(int i = 0; i < 9; i++) {
    vec3 local = vec3(texture(screen, vs_texcoord + offsets[i]));
    color += local * (kernal[i] / strength);
  }

  FragColor = vec4(color, 1.0);
}