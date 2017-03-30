/*
 *    glsl_mix.fs
 *    glsl_mix.osx
 *
 *    Created by Raphael Lemoine on 11/9/11.
 *    Copyright 2011 Alioscopy. All rights reserved.
 *
 ******************************************************************************/
#version 150

uniform sampler2D u_srcTextures0;
uniform sampler2D u_srcTextures1;
uniform sampler2D u_srcTextures2;
uniform sampler2D u_srcTextures3;
uniform sampler2D u_srcTextures4;
uniform sampler2D u_srcTextures5;
uniform sampler2D u_srcTextures6;
uniform sampler2D u_srcTextures7;

uniform float u_vpWidth;
uniform float u_vpHeight;
uniform float u_collimation;


/*    The quad's display position, for lobe tracking ((dispx * 3) + dispy).
 */
uniform    float            u_lobeOffset;

in vec2 uv;

out vec4 fragColor;

/*----------------------------------------------------------------------------*/

void main()
{

    vec4        colors[VIEWPOINTS + 2];
    int            vp;
    vec3        color;

    {
#if VIEWPOINTS == 5
        colors[0] = texture(u_srcTextures0, vec2(uv.x + u_collimation * 2./u_vpWidth, uv.y) );
        colors[1] = texture(u_srcTextures1, vec2(uv.x + u_collimation * 1./u_vpWidth, uv.y) );
        colors[2] = texture(u_srcTextures2, vec2(uv.x + u_collimation * 0./u_vpWidth, uv.y) );
        colors[3] = texture(u_srcTextures3, vec2(uv.x + u_collimation * -1./u_vpWidth, uv.y) );
        colors[4] = texture(u_srcTextures4, vec2(uv.x + u_collimation * -2./u_vpWidth, uv.y) );
#endif
#if VIEWPOINTS == 8
        colors[0] = texture(u_srcTextures0, vec2(uv.x + u_collimation * 3.5/u_vpWidth, uv.y) );
        colors[1] = texture(u_srcTextures1, vec2(uv.x + u_collimation * 2.5/u_vpWidth, uv.y) );
        colors[2] = texture(u_srcTextures2, vec2(uv.x + u_collimation * 1.5/u_vpWidth, uv.y) );
        colors[3] = texture(u_srcTextures3, vec2(uv.x + u_collimation * 0.5/u_vpWidth, uv.y) );
        colors[4] = texture(u_srcTextures4, vec2(uv.x + u_collimation * -0.5/u_vpWidth, uv.y) );
        colors[5] = texture(u_srcTextures5, vec2(uv.x + u_collimation * -1.5/u_vpWidth, uv.y) );
        colors[6] = texture(u_srcTextures6, vec2(uv.x + u_collimation * -2.5/u_vpWidth, uv.y) );
        colors[7] = texture(u_srcTextures7, vec2(uv.x + u_collimation * -3.5/u_vpWidth, uv.y) );
#endif // VIEWPOINTS == 8

        /*    While there are only n viewpoints, colors[] array size
         *    is (n + 2): we copy colors[0] (colors[1]) into
         *    colors[n] (colors[n + 1]) because we use (vp + 0), (vp + 1),
         *    and (vp + 2) offsets in colors[]. This way, we don't have to
         *    calculate '(vp + n) % VIEWPOINTS' at every subpixel.
         */
        colors[VIEWPOINTS] = colors[0];
        colors[VIEWPOINTS + 1] = colors[1];
    }

    /*    2: Calculate which viewpoints are needed by the fragment, taking into
     *    account its physical position on the screen (lobeOffset).
     */
    {
        float        xy;

        /*    xy is always positive.
         */
        xy = (gl_FragCoord.x * 3.0) - gl_FragCoord.y + u_lobeOffset + 0.5;

        /*    First viewpoint for this pixel.
         */
        vp = (VIEWPOINTS - 1) - int(mod(xy, float(VIEWPOINTS)));
    }

    /*    3: Get the R/G/B values from the right viewpoints.
     */
    {
        color = vec3(colors[vp + 2].r, colors[vp + 1].g, colors[vp + 0].b);
    }

    // Only black background for now
    vec3 bgnd = vec3(0,0,0);

    vec3 alphas = vec3(colors[vp + 2].a, colors[vp + 1].a, colors[vp + 0].a);
    color = mix(bgnd, color, alphas);

    /*    4: Store the final R/G/B values.
     */
    fragColor = vec4(color, 1.0);
}
