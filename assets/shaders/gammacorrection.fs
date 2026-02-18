#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// uniforms
uniform sampler2D screen;

void main()
{
  vec3 color = texture(screen, vs_texcoord).rgb;
  vec3 diffuse_color = pow(texture(screen, vs_texcoord).rgb, vec3(2.2));

  FragColor = vec4(pow(color, vec3(0.45)) * diffuse_color * 10, 1.0);
}