#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

// uniforms
uniform sampler2D wave_tex; //2D texture sampler
uniform sampler2D wave_spec; //2D texture sampler
uniform sampler2D wave_warp; //2D texture sampler
uniform vec3 camera;

uniform float time;
uniform vec3 water_color;
uniform float scale = 5.0;

void main()
{
  // offset texcoords in some direction:
  vec2 dir = vec2(1.0, 0.0);
  vec2 uv = vs_texcoord + vec2(time * dir);
  uv.x += 0.01 * sin(uv.x * 3.5 + time);
  uv.y += -0.35 * sin(uv.y * 1.5 + time);
  
  vec4 sample1 = texture(wave_tex, uv * 1.0);
  vec4 sample2 = texture(wave_tex, uv * 1.2);

  // warp:
  vec2 warp_uv = vs_texcoord * scale;
  vec2 warp_scroll = vec2(0.5, 0.5) * time;
  vec2 warp = texture(wave_warp, warp_uv + warp_scroll).xy;
  warp = (warp * 2.0) - 1.0;

  // albedo:
  vec2 albedo_uv = vs_texcoord * scale;
  vec4 albedo = texture(wave_tex, albedo_uv + warp).rgba;

  vec3 final_color = water_color + vec3(albedo.a);

  // specular/shimmer:
  vec2 spec_uv = vs_texcoord * 1.0;
  vec2 spec_scroll = vec2(0.5, 0.5) * time;

  vec3 object_color = vec3((sample1 * 0.75) - (sample2 * 0.25));
  //texture(texture2d, uv).rgb;

  vec3 spec_sample1 = texture(wave_spec, spec_uv + vec2(0.5, 0.5) * time).rgb;
  vec3 spec_sample2 = texture(wave_spec, spec_uv + vec2(-0.5, -0.5) * time).rgb;
  vec3 spec = spec_sample1 + spec_sample2;

  // fresnel:
  float fresnel = dot(normalize(camera), vec3(0.0, 1.0, 0.0));

  const vec3 kBright = vec3(0.299, 0.587, 0.114);
  float brightness = dot(spec, kBright);

  if (brightness <= 0.5 || brightness > 95.0) {
    final_color = mix(final_color, final_color + spec, fresnel);
  }

  FragColor = vec4(final_color, 1.0);
}