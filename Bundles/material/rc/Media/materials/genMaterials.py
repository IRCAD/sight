#! /usr/bin/env python

from jinja2 import Environment, PackageLoader, Template
from pprint import pprint

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

            # Concatenate params
            dict = {}
            for key, param, in config[4].iteritems():
                dict[key] = param + currentConfig[4][key]
            currentPermutation.append(dict)

            newPermutations += [currentPermutation]

        permutations += newPermutations

    return permutations

env = Environment(loader=PackageLoader('genMaterials', 'templates'),trim_blocks=True)
template = env.get_template('Common.program.tpl')

###############################################################################
## Shader Parameters
###############################################################################

## Common parameters used in Lighting.glsl, used both at Vertex and Fragment stages
lightingParams = [ 'param_named_auto u_cameraPos camera_position',
                   'param_named_auto u_lightDir light_position_array 2',
                   'param_named_auto u_ambient surface_ambient_colour',
                   'param_named_auto u_diffuse surface_diffuse_colour',
                   'param_named_auto u_specular surface_specular_colour',
                   'param_named_auto u_shininess surface_shininess']

## Diffuse or negato textures parameters, they should be bind at different unit depending on OIT technique
texParams = ['param_named u_texture int 0']
dpTexParams = ['param_named u_texture int 1']
ddpTexParams = ['param_named u_texture int 4']
htwbTexParams = ['param_named u_texture int 2']

## Parameters used for Negato.glsl
negatoParams = [ 'param_named u_minValue float 0.0',
                 'param_named u_maxValue float 1.0',
                 'param_named u_slice float 0',
                 'param_named u_orientation int 2']

## Diffuse needed alone in some cases
diffuseColorParams = ['param_named_auto u_diffuse surface_diffuse_colour']

cfgFlat = ['Flat', 'FLAT=1', 'Lighting_VP', '', {  'renderSceneVP' : lightingParams,
                                                   'defaultFP' : [],
                                                   'depthPeelingFP' : [],
                                                   'dualDepthPeelingFP' : [],
                                                   'HT_weight_blendFP' : [],
                                                   'weighted_blendFP' : [] } ]

cfgGouraud = ['Gouraud', '', 'Lighting_VP', '', { 'renderSceneVP' : lightingParams,
                                                  'defaultFP' : [],
                                                  'depthPeelingFP' : [],
                                                  'dualDepthPeelingFP' : [],
                                                  'HT_weight_blendFP' : [],
                                                  'weighted_blendFP' : [] } ]

cfgPixelLit = ['PixelLit', 'PIXEL_LIT=1', '', 'Lighting_FP', { 'renderSceneVP' : [],
                                                               'defaultFP' : lightingParams,
                                                               'depthPeelingFP' : lightingParams,
                                                               'dualDepthPeelingFP' : lightingParams,
                                                               'HT_weight_blendFP' : lightingParams,
                                                               'weighted_blendFP' : lightingParams } ]

cfgEdgeNormal = ['Edge_Normal', 'EDGE_NORMAL=1', '', '', { 'defaultFP' : [],
                                                           'depthPeelingFP' : [],
                                                           'dualDepthPeelingFP' : [],
                                                           'HT_weight_blendFP' : [],
                                                           'weighted_blendFP' : [] } ]

cfgNegato = ['Negato', 'NEGATO=1', '', 'Negato_FP', { 'defaultFP' : negatoParams + diffuseColorParams + texParams,
                                                      'depthPeelingFP' : negatoParams + dpTexParams,
                                                      'dualDepthPeelingFP' : negatoParams + ddpTexParams,
                                                      'HT_weight_blendFP' : negatoParams + diffuseColorParams + htwbTexParams,
                                                      'weighted_blendFP' : negatoParams + diffuseColorParams + dpTexParams} ]

cfgVertexColor = ['VT', 'VERTEX_COLOR=1', '', '', { 'renderSceneVP' : [],
                                                    'defaultFP' : [],
                                                    'depthPeelingFP' : [],
                                                    'dualDepthPeelingFP' : [],
                                                    'HT_weight_blendFP' : [],
                                                    'weighted_blendFP' : [] } ]

cfgDiffuseTex = ['DfsTex', 'DIFFUSE_TEX=1', '', '', { 'renderSceneVP' : [],
                                                      'defaultFP' : texParams,
                                                      'depthPeelingFP' : dpTexParams,
                                                      'dualDepthPeelingFP' : ddpTexParams,
                                                      'HT_weight_blendFP' : htwbTexParams,
                                                      'weighted_blendFP' : dpTexParams } ]

configsListVP = []

configsListVP += generatePermutations(cfgFlat, cfgVertexColor, cfgDiffuseTex)
configsListVP += generatePermutations(cfgGouraud, cfgVertexColor, cfgDiffuseTex)
configsListVP += generatePermutations(cfgPixelLit, cfgVertexColor, cfgDiffuseTex)

configsListGP = []

configsListGP += [cfgFlat]
configsListGP += [cfgGouraud]
configsListGP += [cfgPixelLit]

configsListFP = []

configsListFP += generatePermutations(cfgFlat, cfgVertexColor, cfgDiffuseTex)
configsListFP += generatePermutations(cfgGouraud, cfgVertexColor, cfgDiffuseTex)
configsListFP += generatePermutations(cfgPixelLit, cfgVertexColor, cfgDiffuseTex)

configsListFP += [cfgEdgeNormal]
configsListFP += [cfgNegato]

#pprint(configsListVP)

template.stream(configsVP=configsListVP, configsGP=configsListGP, configsFP=configsListFP).dump('Common.program')
