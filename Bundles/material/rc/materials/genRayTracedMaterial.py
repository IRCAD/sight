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
            currentPermutation += [ config[0] + '_' + currentConfig[0] ]

            # Concatenate defines
            if config[1]:
                if currentConfig[1]:
                    currentPermutation += [config[1] + ',' + currentConfig[1]]
                else:
                    currentPermutation += [config[1]]
            else:
                if currentConfig[1]:
                    currentPermutation += [currentConfig[1]]

            # Concatenate params
            paramsDict = appendDict(config[2], currentConfig[2])

            currentPermutation.append(paramsDict)

            newPermutations += [currentPermutation]

        permutations += newPermutations

    return permutations


###############################################################################
## Shader Parameters
###############################################################################

# See RayTracedVolume.material.tpl for Volume Illumination parameters.
# They are dynamically set due to texture_unit numbers conflicts with IDVR's.

preIntParams = ['// PreIntegration',
                'param_named u_min float 0',
                'param_named u_max float 0']

## 'Name', '#Define', {parameters dict}
cfgDefault = ['', '', { }]

cfgAO            = ['AmbientOcclusion', 'AMBIENT_OCCLUSION=1', { }]
cfgColorBleeding = ['ColorBleeding',    'COLOR_BLEEDING=1',    { }]
cfgShadows       = ['Shadows',          'SHADOWS=1',           { }]

cfgPreIntegration = ['PreIntegrated', 'PREINTEGRATION=1', { 'preIntParams' : preIntParams }]

cfgMImP  = ['MImP', 'IDVR=1', { }]
cfgAImC  = ['AImC', 'IDVR=2', { }]

## Configurations for fragment programs and materials
configsListFP = []

configsListFP += generatePermutations(cfgDefault, cfgAO, cfgColorBleeding, cfgShadows, cfgPreIntegration, cfgMImP, cfgAImC)

env = Environment(loader=PackageLoader('genRayTracedMaterial', 'genTemplates'),trim_blocks=True)
template = env.get_template('RayTracedVolume.material.tpl')
template.stream(configsFP=configsListFP).dump('core/RayTracedVolume.material')

env = Environment(loader=PackageLoader('genRayTracedMaterial', '../compositors/genTemplates'),trim_blocks=True)
template = env.get_template('VolumeRayTracing.compositor.tpl')
template.stream(configsFP=configsListFP).dump('../compositors/core/VolumeRayTracing.compositor')