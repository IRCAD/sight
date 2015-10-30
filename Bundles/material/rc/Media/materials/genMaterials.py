#! /usr/bin/env python

from jinja2 import Environment, PackageLoader, Template
from pprint import pprint

def generatePermutations(baseConfig, *configs):
    permutations = [baseConfig]

    # For each configuration
    for currentConfig in configs:

        # Iterate over all permutations generated before the currentConfig
        # At the beginning, only baseConfig, then growing wih the permutations we create
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

        permutations += [currentPermutation]
    return permutations

env = Environment(loader=PackageLoader('genMaterials', 'templates'),trim_blocks=True)
template = env.get_template('Common.program.tpl')

###############################################################################
## Shader Parameters
###############################################################################

## Common parameters used in Lighting.glsl, used both for Vertex and Fragment stages
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
                                                   'defaultFP' : texParams,
                                                   'depthPeelingFP' : dpTexParams,
                                                   'dualDepthPeelingFP' : ddpTexParams,
                                                   'HT_weight_blendFP' : htwbTexParams,
                                                   'weighted_blendFP' : dpTexParams } ]

cfgGouraud = ['Gouraud', '', 'Lighting_VP', '', { 'renderSceneVP' : lightingParams,
                                                  'defaultFP' : texParams,
                                                  'depthPeelingFP' : dpTexParams,
                                                  'dualDepthPeelingFP' : ddpTexParams,
                                                  'HT_weight_blendFP' : htwbTexParams,
                                                  'weighted_blendFP' : dpTexParams } ]

cfgPixelLit = ['PixelLit', 'PIXEL_LIT=1', '', 'Lighting_FP', { 'renderSceneVP' : [],
                                                               'defaultFP' : lightingParams + texParams,
                                                               'depthPeelingFP' : lightingParams + dpTexParams,
                                                               'dualDepthPeelingFP' : lightingParams + ddpTexParams,
                                                               'HT_weight_blendFP' : lightingParams + htwbTexParams,
                                                               'weighted_blendFP' : lightingParams + dpTexParams } ]

cfgEdgeNormal = ['Edge_Normal', 'EDGE_NORMAL=1', '', '', { 'defaultFP' : [],
                                                           'depthPeelingFP' : [],
                                                           'dualDepthPeelingFP' : [],
                                                           'HT_weight_blendFP' : [],
                                                           'weighted_blendFP' : [] } ]

cfgNegato = ['Negato', 'NEGATO=1', '', 'Negato_FP', { 'defaultFP' : negatoParams + texParams + diffuseColorParams,
                                                      'depthPeelingFP' : negatoParams + dpTexParams,
                                                      'dualDepthPeelingFP' : negatoParams + ddpTexParams,
                                                      'HT_weight_blendFP' : negatoParams + htwbTexParams + diffuseColorParams,
                                                      'weighted_blendFP' : negatoParams + dpTexParams  + diffuseColorParams} ]

cfgVertexColor = ['VT', 'VERTEX_COLOR=1', '', '', { 'renderSceneVP' : [],
                                                    'defaultFP' : [],
                                                    'depthPeelingFP' : [],
                                                    'dualDepthPeelingFP' : [],
                                                    'HT_weight_blendFP' : [],
                                                    'weighted_blendFP' : [] } ]
configsListVP = []

configsListVP += generatePermutations(cfgFlat, cfgVertexColor)
configsListVP += generatePermutations(cfgGouraud, cfgVertexColor)
configsListVP += generatePermutations(cfgPixelLit, cfgVertexColor)

configsListFP = []

configsListFP += generatePermutations(cfgFlat, cfgVertexColor)
configsListFP += generatePermutations(cfgGouraud, cfgVertexColor)
configsListFP += generatePermutations(cfgPixelLit, cfgVertexColor)

configsListFP += [cfgEdgeNormal]
configsListFP += [cfgNegato]

#pprint(configsListVP)

template.stream(configsVP=configsListVP, configsFP=configsListFP).dump('Common.program')
