Stochastic Discrete Dynamical System

=== AUTHORS ===

Seda Arat

=== ABOUT ===

SDDS module simulates the average trajectory for each variable out of numberofSimulations trajectories deterministically or stochastically.

=== HOW TO RUN REACT ===

SDDS has been made available as a Docker container to make it easy to run anywhere without tedious technical manipulation. For more information about Docker, see http://docker.com
The SDDS docker image is available here: https://registry.hub.docker.com/u/thibauld/sdds/

1. Install docker
See https://docs.docker.com/

2. Run the SDDS docker image
sudo docker run -d -p 31331:8765 --name sdds thibauld/sdds
This will automatically pull the thibauld/sdds docker image from docker hub.
Note that you can pick an other port number than 31331

3. Test that the SDDS docker image is running correctly
You can try to call the SDDS docker container with the program api_call_test.rb located in the test directory:
Example: ruby test/api_call_test.rb doc/sample-input.json 0.0.0.0 31331
It should return you with an output json file

4. You can also access the Algorun web interface via your web browser
Direct your web browser to http://127.0.0.1:31331

5. [optional] to stop and delete the container
sudo docker kill sdds
sudo docker rm sdds

=== HOW TO REBUILD THE DOCKER IMAGE ===

If you changed the REACT code, you need to rebuild the docker image:
cd superADAM/SDDS
sudo docker build -t <your username>/sdds .
You may also have to update the Dockerfile to reflect changes you made in the SDDS docker container.
