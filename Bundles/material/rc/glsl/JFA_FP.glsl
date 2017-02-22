/* Perform a jump flooding step */

#version 330

/* Current pass index */
uniform float u_passIndex;
/* Total number of passes required log2(max(window.x), max(window.y)) */
uniform float u_nbPasses;

/* Input texture: either the initial texture of a previous ping-pong step */
/* Layout of the texture (fragCoord.x, fragCoord.y, rayPos.z, 1.0) */
uniform sampler2D u_inputTexture;

/* screen resolution: needed as gl_FragCoord is in window space */
uniform float u_viewportWidth;
uniform float u_viewportHeight;

/* Output fragment color */
out vec4 fragColor;

void main()
{
    vec2 coords = gl_FragCoord.xy / vec2(u_viewportWidth, u_viewportHeight);

    float stepwidth = exp2(u_nbPasses - u_passIndex - 1.0);

    /* as we are working with floating coordinates */
    /* we put the maximum distance to a reasonable amount */
    float dstMin = 10.0;
    vec2 closestPoint = vec2(0.0);
    vec2 currentSampleCoords = coords;

    /* Additional parameter : we store the depth of the closest point */
    float depthClosestPoint = 0.0;

    /* Get the 8 "neighboring" samples for this step */
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            /* Compute the coordinate of the current sample */
            vec2 neighborSampleCoords = (gl_FragCoord.xy + vec2(x,y) * stepwidth) / vec2(u_viewportWidth, u_viewportHeight);
            /* Get the value for the sample */
            vec4 neighborSampleValue = texture2D(u_inputTexture, neighborSampleCoords);

            /* Compute the distance from the current sample position */
            /* to the closest point we get for our neighboring sample */
            float d = length(neighborSampleValue.xy - currentSampleCoords);

            /* Ensure that the current neighbor as already been initialized with a closest neighboring sample (x != 0.0 && y != 0.0) */
            /* And that the distance is the closest */
            if ((neighborSampleValue.x != 0.0) && (neighborSampleValue.y != 0.0) && (d < dstMin))
            {
                dstMin = d;
                closestPoint = neighborSampleValue.xy;

                depthClosestPoint = neighborSampleValue.z;
            }
        }
    }

    /* Finally we store the coordiante of the closest point we find */
    /* We also store the distance to the closest point in the blue channel */
    fragColor = vec4(closestPoint, depthClosestPoint, dstMin);
}
