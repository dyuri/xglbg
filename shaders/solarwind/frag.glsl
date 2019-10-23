#version 430

#define R .4
#define PI 3.1415927
#define SPEED 1.2
#define DIST .25
#define GLOW 1.9
#define ROTSPEED .02
#define LAYERS 20.
#define PHI - PI / 9.
#define SS(a,b,t) smoothstep(a,b,t)

uniform vec2 resolution;
uniform float time;
uniform float dayprogress;
uniform sampler1D samples;
uniform sampler1D fft;
uniform int nosound;
uniform sampler2D image;
uniform sampler2D nextimage;
uniform float imageprogress;

out vec4 color;

mat2 rotate2d(float _angle){
    return mat2(cos(_angle),-sin(_angle),
                sin(_angle),cos(_angle));
}

float mspeed() {
  return SPEED * (.2 + .8 * (1. - float(nosound)));
}

float gspeed(float x) {
    x = fract(x) * 100.;
    return mspeed() + .5 * texelFetch(fft, int(abs(x)), 0).r;
}

vec2 random2(vec2 st){
    st = vec2( dot(st,vec2(127.1,311.47)),
               dot(st,vec2(269.5,183.33)) );
    return -1.0 + 2.0*fract(sin(st)*43758.553123);
}

// Value Noise by Inigo Quilez - iq
// https://www.shadertoy.com/view/lsf3WH
float noise(vec2 st) {
    st *= 63.;
    vec2 i = floor(st);
    vec2 f = fract(st);

    vec2 u = f*f*(3.0-2.0*f);

    return mix( mix( dot( random2(i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ),
                     dot( random2(i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
                mix( dot( random2(i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ),
                     dot( random2(i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);
}

float windTexture(vec3 uvw, float speed) {
    vec2 st = vec2(
        uvw.xy /
        	(1. + uvw.z) +
        vec2(uvw.z + time * speed * .1 * (uvw.z + .2),
             uvw.z));
    
    return pow(noise(st) * 3.2, 2.5);
}

float wind(vec3 uvw) {
    float col = 0.;
    float dist = length(uvw);
    float circle = SS(-.02, .01, 1. - dist);

    // rotate
    uvw.xy = uvw.xy * rotate2d(PHI);
    
    // glow
    float d = max(0., DIST - max(0., dist - 1.)) / DIST;
    float gx = max(0.2, uvw.x);
    float glowspeed = gspeed(uvw.y);
    
    // "around" planet
    uvw.y *= 1. + 
        SS(0., 1. + abs(uvw.z) / R, max(0., -uvw.x * dist)) + // after
        SS(0., .3 + abs(uvw.z) / R, max(0., uvw.x * dist)); // before
    
    col = SS(.1, 1., windTexture(uvw, mspeed())) *
        (1. - circle); // don't draw into the planet
	
    return (1. + d * gx * GLOW * glowspeed) * col + d * gx * gx * GLOW * glowspeed;
}

vec3 planet(vec2 uv, float wcol, float speed) {
    vec3 uvw = vec3(uv, sqrt(1. - dot(uv, uv)));

    // "hot air"
    vec2 hot = vec2(-cos(PHI), sin(PHI)) * (wcol - .5) * speed * .1;
	
    vec2 longLat = vec2(
        (atan(uvw.x, uvw.z) / PI + 1.0) * 0.5,
        (asin(uvw.y) / PI + 0.5)
    );
    
    vec4 surface = 
        sqrt(uvw.z) * // sphere
        (imageprogress * texture(nextimage, longLat - hot + ROTSPEED * vec2(time, 0.)) + 
         (1. - imageprogress) * texture(image, longLat - hot + ROTSPEED * vec2(time, 0.))) * // sphere mapped texture
        max(.2, ((cos(-PHI) * uvw.x + .5) + (sin(-PHI) * uvw.y + .5) + uvw.z) / 2.); // sun direction
        
   	return surface.rgb;
}

void main()
{
    // normalized pixel coordinates (0, 0 in the center)
    vec2 uv = gl_FragCoord.xy / resolution.xy -.5;
    uv.x *= resolution.x / resolution.y;

    uv = uv / R;
    
    vec3 uvw = vec3(0.);
    float wcol = 0.;
    
    for (float w = 0.01; w < 1.5; w += 1.5 / LAYERS) {
    	uvw = vec3(uv, w);
    	wcol += wind(uvw) / LAYERS;
    }
    
    vec3 col = vec3(wcol * 1.2, wcol * wcol, wcol * wcol * wcol * .8) + max(vec3(0.), .5 * planet(uv, wcol, mspeed()));
    
    float vignette = min(.4 * (3.2 - length(uv)), 1.);

    color = vec4(vignette * col, 1.);
}
