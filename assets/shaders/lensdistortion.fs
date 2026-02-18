#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// uniforms
uniform sampler2D screen;
uniform vec2 resolution;

void main()
{
  vec2 uv = (gl_FragCoord.xy / resolution.xy) - vec2(0.5);
  float uvatan = atan(uv.x, uv.y);
  float uvsqrt = sqrt(dot(uv, uv));
  uvsqrt *= uvsqrt;

  FragColor = texture(screen, vec2(0.5) + vec2(sin(uvatan), cos(uvatan)) * uvsqrt);
}