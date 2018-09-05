#version 330

layout(location = 0) out vec4 frontDepth;
layout(location = 1) out vec4 backDepth;
layout(location = 2) out vec4 frontColor;
layout(location = 3) out vec4 backColor;
layout(location = 4) out vec4 frontAlpha;
layout(location = 5) out vec4 backAlpha;

/** Init shader :
  * Draw a front render of object
  **/
void main()
{
    frontDepth = vec4(-gl_FragCoord.z);
    backDepth = vec4(gl_FragCoord.z);
    frontColor = vec4(0.);
    backColor = vec4(0.);
    frontAlpha = vec4(0.);
    backAlpha = vec4(0.);
}
