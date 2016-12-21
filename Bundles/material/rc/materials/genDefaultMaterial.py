#! /usr/bin/env python

from jinja2 import Environment, PackageLoader, Template
from pprint import pprint
import copy

def appendDict(dict1, dict2):
    res = { k: v[:] for k,v in dict1.items() }
    for k in dict2.keys():
        if k in dict1:
            res[k] += dict2[k][:]
        else:
            res[k] = dict2[k][:]
    return res

## Function that generate permutations of the configurations in parameters/
## It takes a base config A and combine it with all the subsequent configurations 1, 2, 3,...
## It will thus generate for instance [A,1] [A,2], [A,1,2], [A,3], [A,1,3], [A,2,3], [A,1,2,3]
def generatePermutations(baseConfig, *configs):
    permutations = [baseConfig]

    # For each configuration
    for currentConfig in configs:

        # Iterate over all permutations generated before the currentConfig
        # At the beginning, only baseConfig, then growing with the permutations we create
        newPermutations  = []

        for config in permutations:

            # Concatenate name
            currentPermutation  = []
            currentPermutation += [ config[0] + '+' + currentConfig[0] ]

            # Concatenate defines
            if config[1]:
                if currentConfig[1]:
                    currentPermutation += [config[1] + ',' + currentConfig[1]]
                else:
                    currentPermutation += [config[1]]
            else:
                if currentConfig[1]:
                    currentPermutation += [currentConfig[1]]

            # Concatenate attached shaders
            currentPermutation += [config[2] + currentConfig[2]]
            currentPermutation += [config[3] + currentConfig[3]]
            currentPermutation += [config[4] + currentConfig[4]]

            # Concatenate params
            paramsDict = appendDict(config[5], currentConfig[5])

            currentPermutation.append(paramsDict)

            newPermutations += [currentPermutation]

        permutations += newPermutations

    return permutations


###############################################################################
## Shader Parameters
###############################################################################

## Common parameters used in Lighting.glsl, used both at Vertex and Fragment stages
lightingParams = [ '// Lighting',
                   'param_named_auto u_cameraPos camera_position',
                   'param_named u_numLights int 1',
                   'param_named_auto u_lightDir light_position_array 10',
                   'param_named_auto u_lightAmbient ambient_light_colour',
                   'param_named_auto u_lightDiffuse light_diffuse_colour_array 10',
                   'param_named_auto u_lightSpecular light_specular_colour_array 10',
                   'param_named_auto u_ambient surface_ambient_colour',
                   'param_named_auto u_diffuse surface_diffuse_colour',
                   'param_named_auto u_specular surface_specular_colour',
                   'param_named_auto u_shininess surface_shininess']

## Common parameters used for ambient lighting
ambientParams = [ '// Ambient',
                  'param_named_auto u_ambient surface_ambient_colour',
                  'param_named_auto u_diffuse surface_diffuse_colour']

## Diffuse or negato textures parameters, they should be bound at different unit depending on OIT technique
texParams = ['// Diffuse texture', 'param_named u_texture int 0']

texAlphaParams = ['// Use alpha channel from the texture', 'param_named u_useTextureAlpha int 0']

## Parameters used for Negato.glsl
negatoParams = [ '// Negato',
                 'param_named u_minValue float 0.0',
                 'param_named u_maxValue float 1.0',
                 'param_named u_slice float 0',
                 'param_named u_orientation int 2']

## Diffuse needed alone in some cases
diffuseColorParams = ['// Diffuse color',
                      'param_named_auto u_diffuse surface_diffuse_colour']

## Per primitive color
ppColorParams = ['param_named u_colorPrimitiveTexture int 0',
                 'param_named u_colorPrimitiveTextureSize float2 0 0']

## 'Name', '#Define', 'Attached vp', 'Attached fp', 'useAdjacency [0|1]', {parameters dict}
cfgAmbient = ['Ambient', 'AMBIENT=1', '', '', '', { 'renderSceneVP' : ambientParams } ]

cfgFlat = ['Flat', 'FLAT=1', 'Lighting_VP', '', '', {  'renderSceneVP' : lightingParams } ]

cfgGouraud = ['Gouraud', '', 'Lighting_VP', '', '', { 'renderSceneVP' : lightingParams } ]

cfgPixelLit = ['PixelLit', 'PIXEL_LIT=1', '', 'Lighting_FP', '', { 'defaultFP' : lightingParams,
                                                                   'depthPeelingFP' : lightingParams,
                                                                   'dualDepthPeelingFP' : lightingParams,
                                                                   'HT_weight_blendFP' : lightingParams,
                                                                   'weighted_blendFP' : lightingParams } ]

cfgEdgeNormal = ['Edge_Normal', 'EDGE_NORMAL=1', '', '', '', { } ]

cfgVertexColor = ['VT', 'VERTEX_COLOR=1', '', '', '', { } ]

cfgDiffuseTex = ['DfsTex', 'DIFFUSE_TEX=1', '', '', '', { 'defaultFP' : texParams + texAlphaParams,
                                                          'depthPeelingFP' : texParams + texAlphaParams,
                                                          'dualDepthPeelingFP' : texParams + texAlphaParams,
                                                          'HT_weight_blendFP' : texParams + texAlphaParams,
                                                          'weighted_blendFP' : texParams + texAlphaParams } ]

cfgTriangles = ['Triangles', 'TRIANGLES=1', '', '', '', { 'renderSceneGP' : [], } ]
cfgQuad = ['Quad', 'QUAD=1', '', '', '1', { } ]
cfgTetra = ['Tetra', 'TETRA=1', '', '', '1', { } ]

cfgPerPrimitiveColor = ['PPColor', 'PER_PRIMITIVE_COLOR=1', '', '', '', { 'renderSceneGP' : ppColorParams} ]

## Configurations for vertex programs
## Basis are the different lighting techniques, and optional are vertex color and diffuse texture
configsListVP = []

configsListVP += generatePermutations(cfgAmbient, cfgVertexColor, cfgDiffuseTex)
configsListVP += generatePermutations(cfgFlat, cfgVertexColor, cfgDiffuseTex)
configsListVP += generatePermutations(cfgGouraud, cfgVertexColor, cfgDiffuseTex)
configsListVP += generatePermutations(cfgPixelLit, cfgVertexColor, cfgDiffuseTex)

## Configurations for fragment programs
## Base are the different lighting techniques, and optional are vertex color and diffuse texture
configsListFP = []

configsListFP += generatePermutations(cfgAmbient, cfgVertexColor, cfgDiffuseTex)
configsListFP += generatePermutations(cfgFlat, cfgVertexColor, cfgDiffuseTex)
configsListFP += generatePermutations(cfgGouraud, cfgVertexColor, cfgDiffuseTex)
configsListFP += generatePermutations(cfgPixelLit, cfgVertexColor, cfgDiffuseTex)

configsListFP += [cfgEdgeNormal]

## Configurations for geometry programs
## Base are the different primitives types to generate, and optional are vertex color, diffuse texture and per-primitive
## color
configsListGP = []

configsListGP += generatePermutations(cfgTriangles, cfgVertexColor, cfgDiffuseTex, cfgPerPrimitiveColor)
configsListGP += generatePermutations(cfgQuad, cfgVertexColor, cfgDiffuseTex, cfgPerPrimitiveColor)
configsListGP += generatePermutations(cfgTetra, cfgVertexColor, cfgDiffuseTex, cfgPerPrimitiveColor)

#pprint(configsListVP)

env = Environment(loader=PackageLoader('genDefaultMaterial', 'genTemplates'),trim_blocks=True)
template = env.get_template('Default.program.tpl')
template.stream(configsVP=configsListVP, configsFP=configsListFP).dump('scene/Default.program')

template = env.get_template('R2VB.program.tpl')
template.stream(configsVP=configsListVP, configsGP=configsListGP).dump('core/R2VB.program')
