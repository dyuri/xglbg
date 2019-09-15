# xglbg
Live background wallpaper in opengl for X (i3). It's made for shaders from [Shadertoy.com](shadertoy.com).

Started as a fork of [liveW](https://github.com/dgranosa/liveW.git), but I've removed a lot of features I don't need, and rewrote some parts.

# Requirements:
 - Pulseaudio
 - FFTW
 - Compton (optional)
 
# Compiling:
```
git clone https://github.com/dyuri/xglbg
cd xglbg
make
```

# Using:
```
$ ./xglbg -s _source_ -p _shader_name_ -i _image_theme_
```

_source_ is name of Pulseaudio device which you can obtain with:
```
$ pacmd list-sources | grep "name:"
  name: <alsa_output.pci-0000_22_00.3.analog-stereo.monitor>
  name: <alsa_input.pci-0000_22_00.3.analog-stereo>
```
You are intrested in one with .monitor on the end.

_shader_name_ is the name of the shaders folder inside `/shaders/` in the configuration directory.

_image_theme_ is the name of the image theme folder inside `/images/` in the configuration directory. The `png` images in that directory will be loaded in alphabetical order based on the time of the day (`dayprogress`) and passed to the shader as `image` and `nextimage`. For example if you have 24 images (`image00.png`, `image01.png` ... `image23.png`) then at 10:20 `image10.png` will be loaded as `image` and `image11.png` will be `nextimage`.

The configuration directory is `$XDG_CONFIG_HOME/xglbg` which is usually `$HOME/.config/xglbg`.

Example: ./xglbg -s alsa_output.pci-0000_22_00.3.analog-stereo.monitor -p equalizer -i chaos
```
$ ./xglbg -h
  Usage: xglbg <options>                                                      
  Options:                                                                    
     -h Print help
     -d Turn debug on
     -k Plasma window setup
     -w Window mode
     -g Window geometry WIDTHxHEIGHT 
     -o Position of window relative to top left corner TOPxLEFT (default 0x0)
     -t Transparency (default 1.0)
     -p shader name in shaders folder
     -i image theme in images folder
     -f FPS (default 30)
     -D Dynamic FPS - use 1 fps if there is no sound
     -Y Use only youtube thumbnail for album art
     -s Pulseaudio device source
        Specify using the name from "pacmd list-sources | grep "name:""
```

# Multi monitor setup:
In most multi monitor configurations all monitors are part of one X11 screen so maybe `xglbg` will be strached through all monitors.
To set `xglbg` to draw on one monitor use arguments -g (set width and height of window) and -o (distance of top-left corner of window and top-left corner of screen).

## Example:
If you have two monitors vertically placed with resolution 1920x1200.

First monitor: `./xglbg -g 1920x1200`

Second monitor: `./xglbg -g 1920x1200 -o 1920x0`

# Using shaders from Shadertoy:

  1. Create directory inside shaders/ and copy shader code in file frag.glsl inside created folder.
  2. Add to begining of the shader:
   ```glsl
   #version 430
   uniform vec2 resolution;
   uniform float time;
   uniform float dayprogress; // progress of day - from 0.0 to 1.0
   uniform sampler1D samples;
   uniform sampler1D fft;
   uniform sampler2D image; // background image
   out vec4 color;
   ```
  3. Rename possible colission between variables names from above inside code in something else.
  4. Replace:
     - iResolution -> resolution
     - fragCoord -> gl_FragCoord
     - fragColor -> color
     - iTime -> time
     - void mainImage( in, out ) -> void main()
  5. If using music input replace code:
     - texture(iChannel0, vec2(coordX, coordY)).x;
     - If coordY is smaller then 0.5 replace with:
       - texture(fft, coordX).x;
     - otherwise
       - texture(samples, coordX).x;
