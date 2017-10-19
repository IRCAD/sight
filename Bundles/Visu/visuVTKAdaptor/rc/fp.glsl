uniform sampler2D texture;   // input image
varying vec2  texCoord;
void propFuncFS(void)
{
  vec4 tmp_color;
  tmp_color = texture2D(texture,texCoord);
  gl_FragColor = tmp_color;
  gl_FragColor.r = tmp_color.b;
  gl_FragColor.b = tmp_color.r;
}

