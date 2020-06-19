varying vec2  texCoord;
void propFuncVS(void)
{
   texCoord = gl_MultiTexCoord0.st;
   texCoord.y = 1.-texCoord.y;
   
  // normal transformation
  gl_Position = ftransform();
}

