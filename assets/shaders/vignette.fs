#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// uniforms
uniform sampler2D screen;
uniform vec2 resolution;

void main()
{
  vec3 color = texture(screen, vs_texcoord).rgb;
  vec2 uv = ((gl_FragCoord.xy / resolution.xy) - 0.5) * vec2(resolution.x / resolution.y, 1.0);

  color -= vec3(length(uv)) * 0.5;
  FragColor = vec4(color, 1.0);
}