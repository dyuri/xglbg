#version 430

#define NUMBARS 100

uniform vec2 resolution;
uniform float time;
uniform float imageprogress;
uniform int nosound;
uniform sampler2D image;
uniform sampler2D nextimage;
uniform sampler1D fft;

out vec4 color;

void main()
{
	vec2 uv = gl_FragCoord.xy / resolution.xy;

  vec4 bg = texture(image, uv) * (1. - imageprogress) +
             texture(nextimage, uv) * imageprogress;

  float f = texelFetch(fft, int(uv.x * NUMBARS), 0).r;
  float fc = .5 * (f - .5);
  
  color = vec4(vec3(1. - step(f, uv.y)) * fc, 1.0) * (1. - float(nosound)) + bg;
}
