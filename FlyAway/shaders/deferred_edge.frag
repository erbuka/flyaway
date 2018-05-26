#version 330

uniform sampler2D in_Texture;
uniform vec2 in_Resolution;

const float vkernel[9] = float[9](
  1.0, 2.0, 1.0,
  0.0, 0.0, 0.0,
  -1.0, -2.0, -1.0
);

const float hkernel[9] = float[9](
  1.0, 0.0, -1.0,
  2.0, 0.0, -2.0,
  1.0, 0.0, -1.0
);

in vec2 fs_TexCoord;

float fade(float t)
{
  return t * t * t * (t * (t * 6 - 15) + 10);
}
    
void main()
{
    
  vec2 step = vec2(1.0) / in_Resolution.xy;
  vec4 v = vec4(0.0);
  vec4 h = vec4(0.0);
    
  for(int x = 0; x < 3; x++)
    for(int y = 0; y < 3; y++)
    {
      vec2 s = vec2(x - 1, y - 1) * step;
   	  v += vkernel[y * 3 + x] * texture2D(in_Texture, fs_TexCoord + s);
    }
    
  for(int x = 0; x < 3; x++)
    for(int y = 0; y < 3; y++)
    {
    	vec2 s = vec2(x - 1, y - 1) * step;
     	h += hkernel[y * 3 + x] * texture2D(in_Texture, fs_TexCoord + s);
    }

  vec4 sobel = sqrt((h * h) + (v * v));

  gl_FragData[0] = vec4(vec3(1.0) - max(sobel.r, max(sobel.g, sobel.b)), 1.0);
}
