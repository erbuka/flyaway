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
  vec4 result = vec4(0.0);
    
  for(int x = 0; x < 3; x++)
    for(int y = 0; y < 3; y++)
    {
      vec2 s = vec2(x - 1, y - 1) * step;
   	  result += vkernel[y * 3 + x] * texture2D(in_Texture, fs_TexCoord + s);
    }
    
  for(int x = 0; x < 3; x++)
    for(int y = 0; y < 3; y++)
    {
    	vec2 s = vec2(x - 1, y - 1) * step;
     	result += hkernel[y * 3 + x] * texture2D(in_Texture, fs_TexCoord + s);
    }

  /*
  result = abs(result / 8.0);
  float avg = 1.0 - max(result.x, max(result.y, result.z));
  avg = avg > 0.2 ? (avg - 0.2) / 0.8 : 0.0;
  */
  
  float avg = (1.0 - clamp(abs(result.r) + abs(result.g) + abs(result.b), 0.0, 1.0)) > 0.8 ? 1.0 : 0.0;
  
  /*
  float k = (1.0 - clamp(abs(result.r) + abs(result.g) + abs(result.b), 0.0, 1.0));
  float avg = k > 0.8 ? fade((k - 0.8) / 0.2) : 0.0;
  */
  gl_FragData[0] = vec4(avg, avg, avg, 1.0);
}
