
//-----------------------------------------------------------------------------

// Ray - box intersection. Returns the intersection point along the ray.
// This version doesn't handle the case where there are no intersection.
// It also only returns the intersection with the front face, be it in front (if t > 0) or behind (if < 0) the ray.
float rayAxisAlignedBoxIntersection(in vec3 _f3RayPos, in vec3 _f3RayDir,
                                   in vec3 _f3AxisAlignedBoxMin, in vec3 _f3AxisAlignedBoxMax)
{
    vec3 t0 = (_f3AxisAlignedBoxMin - _f3RayPos) / _f3RayDir;
    vec3 t1 = (_f3AxisAlignedBoxMax - _f3RayPos) / _f3RayDir;

    vec3 tmin = min(t0, t1);
    float t   = max(tmin.x, max(tmin.y, tmin.z));

    return t;
}

//-----------------------------------------------------------------------------

