#version 430

uniform vec2 resolution;
uniform float time;
uniform float imageprogress;
uniform sampler2D image;
uniform sampler2D nextimage;
out vec4 color;

void main()
{
	vec2 uv = gl_FragCoord.xy / resolution.xy;

  color = texture(image, uv) * (1. - imageprogress) +
          texture(nextimage, uv) * imageprogress;
}
