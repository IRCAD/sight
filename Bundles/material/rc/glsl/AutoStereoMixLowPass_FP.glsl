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



/*    The quad's display position, for lobe tracking ((dispx * 3) + dispy).
 */
uniform    float            u_lobeOffset;

in vec2 uv;

/*----------------------------------------------------------------------------*/

void main()
{

    vec4        colors[VIEWPOINTS + 2];
    int            vp;
    vec3        color;

    {

        colors[0] = 0.5*texture(u_srcTextures0, uv) + 0.25*texture(u_srcTextures0, vec2(uv.x+0.5/1920.0,uv.y)) + 0.25*texture(u_srcTextures0, vec2(uv.x-0.5/1920.0,uv.y));
        colors[1] = 0.5*texture(u_srcTextures1, uv) + 0.25*texture(u_srcTextures1, vec2(uv.x+0.5/1920.0,uv.y)) + 0.25*texture(u_srcTextures1, vec2(uv.x-0.5/1920.0,uv.y));
#if (VIEWPOINTS > 2)
        colors[2] = 0.5*texture(u_srcTextures2, uv)+ 0.25*texture(u_srcTextures2, vec2(uv.x+0.5/1920.0,uv.y)) + 0.25*texture(u_srcTextures2, vec2(uv.x-0.5/1920.0,uv.y));
#endif    /*(VIEWPOINTS > 2)    */
#if (VIEWPOINTS > 3)
        colors[3] = 0.5*texture(u_srcTextures3, uv)+ 0.25*texture(u_srcTextures3, vec2(uv.x+0.5/1920.0,uv.y)) + 0.25*texture(u_srcTextures3, vec2(uv.x-0.5/1920.0,uv.y));
#endif    /*(VIEWPOINTS > 3)    */
#if (VIEWPOINTS > 4)
        colors[4] = 0.5*texture(u_srcTextures4, uv)+ 0.25*texture(u_srcTextures4, vec2(uv.x+0.5/1920.0,uv.y)) + 0.25*texture(u_srcTextures4, vec2(uv.x-0.5/1920.0,uv.y));
#endif    /*(VIEWPOINTS > 4)    */
#if (VIEWPOINTS > 5)
        colors[5] = 0.5*texture(u_srcTextures5, uv)+ 0.25*texture(u_srcTextures5, vec2(uv.x+0.5/1920.0,uv.y)) + 0.25*texture(u_srcTextures5, vec2(uv.x-0.5/1920.0,uv.y));
#endif    /*(VIEWPOINTS > 5)    */
#if (VIEWPOINTS > 6)
        colors[6] = 0.5*texture(u_srcTextures6, uv)+ 0.25*texture(u_srcTextures6, vec2(uv.x+0.5/1920.0,uv.y)) + 0.25*texture(u_srcTextures6, vec2(uv.x-0.5/1920.0,uv.y));
#endif    /*(VIEWPOINTS > 6)    */
#if (VIEWPOINTS > 7)
        colors[7] = 0.5*texture(u_srcTextures7, uv)+ 0.25*texture(u_srcTextures7, vec2(uv.x+0.5/1920.0,uv.y)) + 0.25*texture(u_srcTextures7, vec2(uv.x-0.5/1920.0,uv.y));
#endif    /*(VIEWPOINTS > 7)    */

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

    /*    4: Store the final R/G/B values.
     */
    gl_FragColor = vec4(color, 1.0);
    return;
}
