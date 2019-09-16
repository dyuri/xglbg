#version 430

#define NUMBARS 64

uniform vec2 resolution;
uniform float time;
uniform float imageprogress;
uniform sampler2D image;
uniform sampler2D nextimage;
uniform sampler1D fft;

out vec4 color;

void main()
{
	vec2 uv = gl_FragCoord.xy / resolution.xy;

  vec4 bg = texture(image, uv) * (1. - imageprogress) +
             texture(nextimage, uv) * imageprogress;

  float f = .5 * (texelFetch(fft, int(uv.x * NUMBARS), 0).r - .5);
  
  color = vec4(f) + bg;
}
