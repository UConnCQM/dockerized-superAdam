# A dockerized version of superAdam

## Current status

The project is still under heavy development at this stage

## Overview of the project

dockerized-superAdam is an attempt to make a collection of independent software modules and algorithms work together in a decentralized yet simple manner using [Docker containers](https://github.com/docker) and [Docker compose](http://docs.docker.com/compose/).

The objective is to be able to create a workflow for data computation (based on the different software modules) that could be used by a 3rd party program.

### Context

The software modules included in superAdam has been published independently and thus are very heterogeneous. They have been implemented using different programming languages, require different dependencies and have different I/O formats.

### Architecture

In order to make these modules work together, it was decided to:

1. Standardize the I/O format of the modules (with JSON).
2. Wrap every module inside a Web Service that would receive the input JSON and send the resulting JSON output.
3. Isolate every module in a docker container
4. Leverage docker compose capabilities to describe and link the containers together
5. Describe the pipeline logic at runtime through the environment variables given to each container

For a brief overview of the architecture of dockerized-superAdam, you can refer to this [presentation](https://docs.google.com/presentation/d/1X3ue1tz9utoNJPKuOV3dvzedHcvJnR2eipfrIVjQXvw/pub?start=false&loop=false&delayms=3000).

### JSON I/O standardization

Module owners agreed on a standard JSON format that will be use to communicate between all modules.

#### Input

Here is a template (sort of) of the input json of a module:

```
{"task": {
        "method": {
                "type": <problemType>,
                "id": <moduleName>,
                "description": <string>,
                "arguments": [{
                        "name":"priorVer",
                        "value":[
                              [1, 2, 2, 2, 2],
                              [0, 1, 0, 1, 1],
                              [1, 1, 0, 0, 2],
                              [0, 1, 2, 0, 3]
                            ]
                        },
                        {
                        "name":"hamming",
                        "value":1.0
                        }]
                },
        "input": [{
                  "type":"model",
                  "description": "Gene Regulation of Mammalian Cortical Area Development Published in: Giacomantonio CE, Goodhill GJ (2010) A Boolean Model of the Gene Regulatory Network Underlying Mammalian Cortical Area Development. PLoS Comput Biol 6(9): e1000936",
                  "name": "Cortical_Area_Development",
                  "fieldCardinality": 2,
                  "version": "1.0",
                  "parameters": [{
                      "id": "k1",
                      "name": "param1",
                      "states": ["low", "high"]
                      }],
                  "updateRules": [{
                        "target":"x1",
                        "functions": [{
                            "inputVariables": ["x1","x2"],
                            "polynomialFunction": "x1*x2",
                            "booleanFunction": "x1 & (!x2 | x1)",
                            "transitionTable": [
                                      [[0,0],1],
                                      [[0,1],0],
                                      [[1,0],0],
                                      [[1,1],0]],
                            "score": 1.0
                          },
                          {
                            "inputVariables": ["x3","x5"],
                            "transitionTable": [
                                      [[0,0],1],
                                      [[0,1],0],
                                      [[1,0],0],
                          }]
                        }],
                  "variables": [{
                      "id": "x1",
                      "name": "Coup_fti",
                      "states": ["low", "high"]
                      },
                      {
                      "id": "x2",
                      "name": "Emx2",
                      "states": ["off", "on"]
                      }],
                  "variableScores": [{
                      "target": "x1",
                      "sources": [
                                { "source": "x1", "score": 1 },
                                { "source": "x2", "score": .45 },
                                { "source": "x3", "score": .56 }
                                ]
                          },
                          {
                      "target": "x2",
                      "sources": [
                                { "source": "x1", "score": .2 },
                                { "source": "x2", "score": .45 }
                                ]
                          },
                          {
                      "target": "x3",
                      "sources": [
                                { "source": "x2", "score": .45 },
                                { "source": "x3", "score": .56 }
                                ]
                      }],
                  "simlab": <arbitrary structure for use in front end, treat as private information>
                },
                {
                   "type": "timeSeries",
                   "timeSeriesData": [{
                          "name": "wildtype experiment 1",
                          "index": [],
                          "matrix": [
                              [1,0,0,0],
                              [0,1,0,1],
                              [1,1,0,0]
                            ]
                          },
                          {
                          "name": "wildtype experiment 2",
                          "index": [],
                          "matrix": [
                              [1,1,0,0],
                              [0,0,0,1],
                              [1,0,0,0]
                            ]
                          },
                          {
                          "name": "knockout experiment 1",
                          "index": [1],
                          "matrix": [
                              [0,0,0,0],
                              [0,0,0,1],
                              [0,1,0,0],
                              [0,0,0,1]
                            ]
                          },
                          {
                          "name": "knockout experiment 2",
                          "index": [3],
                          "matrix": [
                              [0,1,0,0],
                              [0,1,0,1],
                              [0,1,0,1],
                              [0,0,0,0]
                            ]
                          },
                          {
                          "name": "knockout experiment 3",
                          "index": [2],
                          "matrix": [
                              [1,0,0,0],
                              [0,0,0,1],
                              [1,0,0,1],
                              [0,0,0,0]
                            ]
                          }
                        ]
                      },
                {... <more inputs>
                }]
        }
}
```

##### About model

* description, name, version are free form documentation, version should go up in some form if the model is saved for the user, and is modified.
* variables must have update rules, in order.
* parameters are essentially variables which do not change value

##### About updateRules

* each "function" can contain polynomialFunction, booleanFunction (only in 2 state case), transitionTable.  They MUST represent the same abstract function.
* a model will usually contain exactly one function per target, but more are often allowed.

The intent is that "parameters" includes the parameters that are special to the given <moduleName> and the "input" fields are constant across all <problemType>.

#### Output

The json output consists of an array of your output objects. Please note that if you used a model as input, you should strongly consider returning the modified/enriched model as an output.

Here is a template (sort of) of the json output of a module:
```
{"output": [
        {
        "type":"model"
        <your input model enriched...>
        },
        {
        "type":"<some other type of output>"
        ....
        }
    ]
}
```
