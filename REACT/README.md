# Reverse Engineering with Evolutionary Computation Tools (REACT)

REACT is an Evolutionary Algorithm for Discrete Dynamical Systems' Optimization

## AUTHORS ##

Paola Vera-Licona & John J. McGee Virginia Bioinformatics Institute at Virginia Tech

## ABOUT ##

The inference of gene regulatory networks (GRNs) from system-level experimental observations is at the heart of systems biology due to its centrality in gaining insight into the complex regulatory mechanisms in cellular systems. This includes the inference of both the network topology and dynamic mathematical models.

This software contains a novel network inference algorithm within the algebraic framework of Boolean polynomial dynamical system (BPDS). The algorithm considers time series data, including that of perturbation experiments such as knock-out mutants and RNAi experiments. To infer the network topology and dynamic models, it allows for the incorporation of prior biological knowledge while being robust to significant levels of noise in the data used for inference. It uses an evolutionary algorithm for local optimization with an encoding of the mathematical models as BPDS.

More info: http://www.paola-vera-licona.net/Software/EARevEng/REACT.html

## HOW TO RUN REACT ##

REACT has been made available as a Docker container to make it easy to run anywhere without tedious technical manipulation. For more information about Docker, see http://docker.com
The REACT docker image is available here: https://registry.hub.docker.com/u/thibauld/react/

1. Install docker
See https://docs.docker.com/

2. Run the REACT docker image
sudo docker run -d -p 31331:8765 --name react thibauld/react
This will automatically pull the thibauld/react docker image from docker hub.
Note that you can pick an other port number than 31331

3. Test that the REACT docker image is running correctly
You can try to call the REACT docker container with the program api_call_test.rb located in the test directory:
Example: ruby test/api_call_test.rb test/test1/REACT_Input_Example1.json 0.0.0.0 31331
It should return you with an output json file

4. You can also access the Algorun web interface via your web browser
Direct your web browser to http://127.0.0.1:31331

5. [optional] to stop and delete the container
sudo docker kill react
sudo docker rm react

## HOW TO REBUILD THE DOCKER IMAGE ##

If you changed the REACT code, you need to rebuild the docker image:
cd superADAM/REACT
sudo docker build -t <your username>/react .
You may also have to update the Dockerfile to reflect changes you made in the REACT docker container.
