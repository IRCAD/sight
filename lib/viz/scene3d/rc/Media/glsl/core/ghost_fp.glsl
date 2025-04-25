#version 420

#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#include "rim_lighting.inc.glsl"

uniform mat4 u_view;

// Uniforms
uniform vec4 u_diffuse;
uniform vec3 u_cameraPos;

// Input semantics
layout(location = 0) in vec3 v_f3Normal_Ws;
layout(location = 1) in vec3 v_f3Position_Ws;

uniform vec3 u_rim_params;

// Compute illumination or forward illumination received from the vertex shader
vec4 getFragmentColor()
{
  vec4 color = u_diffuse;
  color.a = color.a * rim_lighting(v_f3Normal_Ws, v_f3Position_Ws, u_view, u_rim_params);

  return color;
}

// Compute alpha channel
float getFragmentAlpha()
{
  float color =  u_diffuse.a * rim_lighting(v_f3Normal_Ws, v_f3Position_Ws, u_view, u_rim_params);
  return color;
}

#include "Transparency.inc.glsl"

void main(void)
{
    processFragment();
}
