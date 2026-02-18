#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// uniforms
uniform sampler2D screen;
uniform vec2 resolution;
uniform float time;

void main()
{
  vec2 uv = ((gl_FragCoord.xy / resolution.xy) - 0.5) * vec2(resolution.x / resolution.y, 1.0);

  float mdf = 0.1;
  float noise = (fract(sin(dot(uv, vec2(12.9898,78.233)*2.0)) * 43758.5453 * ((sin(time) * 0.5) + 1.0))) * 0.2;
  
  FragColor = texture(screen, vs_texcoord) - noise;
}