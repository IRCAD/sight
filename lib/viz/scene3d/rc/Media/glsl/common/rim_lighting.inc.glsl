float rim_lighting(vec3 _f3NormalDirWs, vec3 _f3PosWs, mat4 _view, vec3 _rim_params)
{
    vec3 n = normalize(mat3(_view) * _f3NormalDirWs);
    vec3 p = vec3(_view * vec4(_f3PosWs, 1.0));
    vec3 v = normalize(-p);
    float cos_vn = dot(v, n);
    float vdn = cos_vn < 0. ? 0.0 : 1.0 - cos_vn;

    float color = smoothstep(_rim_params.x, _rim_params.y, vdn);
    return color * _rim_params.z + 1.0 - _rim_params.z;
}
