#version 330

uniform sampler2D u_s2EntryDepth_Ss;
uniform sampler2D u_s2JFA_Ns;

#ifdef AUTOSTEREO
uniform mat4 u_invProj;
uniform mat4 u_invWorldView;
#else  // AUTOSTEREO
uniform mat4 u_m4Cs_Ms;
#endif // AUTOSTEREO

uniform vec3 u_f3CameraPos_Ms;
uniform float u_fCSGAngleCos;
uniform vec3 u_f3ImageSpacingSize;
uniform vec3 u_imageSize;

out vec4 v_f4CSGInfos;

//-----------------------------------------------------------------------------

vec3 fragCoordsToNDC(in vec3 _f3FragPos_Ss);
vec4 ndcToSpecificSpacePosition(in vec3 _f3FragPos_Ns, in mat4 _m4Inverse);

//-----------------------------------------------------------------------------

// Returns true if the ray hits the cone, the origin is then moved to the intersection point.
bool rayConeIntersection(in vec3 _3fConeOri, in vec3 _f3ConeDir_N, in float _fAngle, inout vec3 _f3RayPos, in vec3 _f3RayDir_N)
{
    // Vector from the cone origin to the ray origin.
    vec3 f3ConeToRayDir = _f3RayPos - _3fConeOri;

    float fRayfConeDot = dot(_f3RayDir_N, _f3ConeDir_N);
    float fConeDot = dot(f3ConeToRayDir, _f3ConeDir_N);

    // Angle cosine between the cone direction and the origin to origin vector.
    float fConeAngle = dot(normalize(f3ConeToRayDir), normalize(_f3ConeDir_N));

    // Ensure that the ray origin is 'inside' the cone.
    if(fConeAngle < _fAngle && fConeAngle > 0)
    {
        return false;
    }

    // We're looking for a point P belonging to the ray and the cone, P should therefore verify the following equations:
    //     · ((P - _3fConeOri) /  length(P - _3fConeOri)) * _f3ConeDir_N = cos(fConeAngle)
    //     · P = _f3RayPos + t * _f3RayDir_N
    // When simplifying this system we end up with a second degree polynomial a * t² + b * t + c with the following
    // coefficients :
    float fSquareAngle = _fAngle * _fAngle;
    float a = fRayfConeDot * fRayfConeDot - fSquareAngle;
    float b = 2 * (fRayfConeDot * fConeDot - dot(_f3RayDir_N, f3ConeToRayDir) * fSquareAngle);
    float c = fConeDot * fConeDot - dot(f3ConeToRayDir, f3ConeToRayDir) * fSquareAngle;

    // Solving the polynomial is trivial.
    float fDelta = b * b - 4. * a * c;

    vec3 f3IntersectionPos;
    if(fDelta > 0)
    {
        float fSqrtfDelta = sqrt(fDelta);
        float t1 = (-b - fSqrtfDelta) / (2. * a);
        float t2 = (-b + fSqrtfDelta) / (2. * a);

        // We're looking for the point closest to the ray origin.
        float t = min(t1, t2);

        if (t < 0.)
        {
            return false;
        }

        f3IntersectionPos = _f3RayPos + t * _f3RayDir_N;

        // Check if we hit the cone or its 'shadow'
        // i.e. check if the intersection is 'in front' or 'behind' the cone origin.
        vec3 cp = f3IntersectionPos - _3fConeOri;
        float h = dot(cp, _f3ConeDir_N);

        if (h < 0.) // We hit the shadow ...
        {
            return false;
        }
    }
    else // No solution to the polynomial = no intersection
    {
        return false;
    }

    _f3RayPos = f3IntersectionPos;
    return true;
}

//-----------------------------------------------------------------------------

void main()
{
    // Compute ray entry and ray exit in volume space
    vec2 f3DepthEntryExitDis_Ns = texelFetch(u_s2EntryDepth_Ss, ivec2(gl_FragCoord.xy), 0).rg;

    float fEntryDepthDis_Ns =  f3DepthEntryExitDis_Ns.r;
    float fExitDepthDis_Ns  = -f3DepthEntryExitDis_Ns.g;

    if(fExitDepthDis_Ns == -1 || fExitDepthDis_Ns < fEntryDepthDis_Ns)
    {
        discard;
    }

    vec3 f3EntryPos_Ns = fragCoordsToNDC(vec3(gl_FragCoord.xy, fEntryDepthDis_Ns));
    vec3 f3ExitPos_Ns = fragCoordsToNDC(vec3(gl_FragCoord.xy, fExitDepthDis_Ns));

#ifdef AUTOSTEREO
    mat4 m4Cs_Ms = u_invWorldView * u_invProj;
#else
    mat4 m4Cs_Ms = u_m4Cs_Ms;
#endif

    vec3 f3EntryPos_Ms = ndcToSpecificSpacePosition(f3EntryPos_Ns, m4Cs_Ms).xyz;
    vec3 f3ExitPos_Ms  = ndcToSpecificSpacePosition(f3ExitPos_Ns, m4Cs_Ms).xyz;

    // Compute nearest mask position
    vec4 f4ClosetMaskPos_Ns = texelFetch(u_s2JFA_Ns, ivec2(gl_FragCoord.xy), 0);
    vec3 f3CclosetMaskPos_Ms = ndcToSpecificSpacePosition(f4ClosetMaskPos_Ns.xyz, m4Cs_Ms).xyz;

    // Use image spacing to take voxel anisotropy into account
    vec3 f3SpacingSize_N = normalize(u_f3ImageSpacingSize);

    // Compute the origin and the direction of the ray and the cone in the voxel spacing
    vec3 f3ConeOri_Vs  = f3CclosetMaskPos_Ms * f3SpacingSize_N;
    vec3 f3DoneDir_VsN = normalize(u_f3CameraPos_Ms * f3SpacingSize_N - f3ConeOri_Vs);
    vec3 f3RayOri_Vs   = f3EntryPos_Ms * f3SpacingSize_N;
    vec3 f3RayDir_VsN  = normalize(f3ExitPos_Ms * f3SpacingSize_N - f3EntryPos_Ms * f3SpacingSize_N);

    // Check if the ray hits the cone
    bool hit = rayConeIntersection(f3ConeOri_Vs, f3DoneDir_VsN, u_fCSGAngleCos, f3RayOri_Vs, f3RayDir_VsN);
    if(hit)
    {       
        // Set the ray coordinate to the model space
        vec3 f3RayPos_Ms = f3RayOri_Vs/f3SpacingSize_N;

        // Compute the ray depth in the image spacing's unit (typically millimeters for medical images).
        vec3 f3IimageSize_Ts = u_f3ImageSpacingSize * u_imageSize;
        vec3 f3ConeDir_Ts = normalize(u_f3CameraPos_Ms - f3CclosetMaskPos_Ms) * f3IimageSize_Ts;
        vec3 f3ConeToRayDir_Ts = f3RayPos_Ms * f3IimageSize_Ts - f3CclosetMaskPos_Ms * f3IimageSize_Ts;

        // Store the new ray position and depth into the texture
        float fDepthLen_Ts = dot(f3ConeDir_Ts, f3ConeToRayDir_Ts) / length(f3ConeDir_Ts);
        v_f4CSGInfos = vec4(f3RayPos_Ms, fDepthLen_Ts);
    }
    else
    {
        discard;
    }
}
