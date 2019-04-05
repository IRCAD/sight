#version 330

vec3 gradientNormal(vec3 _f3Pos_Ms, sampler3D _s3Image)
{
    ivec3 imgDimensions = textureSize(_s3Image, 0);
    vec3 h = 1. / vec3(imgDimensions);
    vec3 hx = vec3(h.x, 0, 0);
    vec3 hy = vec3(0, h.y, 0);
    vec3 hz = vec3(0, 0, h.z);

    return -normalize( vec3(
                (texture(_s3Image, _f3Pos_Ms + hx).r - texture(_s3Image, _f3Pos_Ms - hx).r),
                (texture(_s3Image, _f3Pos_Ms + hy).r - texture(_s3Image, _f3Pos_Ms - hy).r),
                (texture(_s3Image, _f3Pos_Ms + hz).r - texture(_s3Image, _f3Pos_Ms - hz).r)
    ));
}

