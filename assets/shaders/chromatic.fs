#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// uniforms
uniform sampler2D screen;

// uniform float offset
uniform float offset = 1.0 / 128.0;
uniform float strength = 1.0;

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
const vec3 chromaticAberrationKernel[9] = vec3[9](
vec3(0.0000000000000000000, 0.04416589065853191, 0.0922903086524308425), vec3(0.10497808951021347), vec3(0.0922903086524308425, 0.04416589065853191, 0.0000000000000000000),
vec3(0.0112445223775533675, 0.10497808951021347, 0.1987116566428735725), vec3(0.40342407932501833), vec3(0.1987116566428735725, 0.10497808951021347, 0.0112445223775533675),
vec3(0.0000000000000000000, 0.04416589065853191, 0.0922903086524308425), vec3(0.10497808951021347), vec3(0.0922903086524308425, 0.04416589065853191, 0.0000000000000000000)
);  

void main()
{
  vec3 color = vec3(0.0);

  for(int i = 0; i < 9; i++) {
    vec3 local = texture(screen, vs_texcoord + offsets[i]).rgb;
    color.x += local.x * (chromaticAberrationKernel[i].x / strength);
    color.y += local.y * (chromaticAberrationKernel[i].y / strength);
    color.z += local.z * (chromaticAberrationKernel[i].z / strength);
  }

  FragColor = vec4(color, 1.0);
}