FROM phusion/baseimage:0.9.15
MAINTAINER Thibauld Favre <thibauld@fastmail.com>
RUN apt-get update
RUN apt-get install -y ruby2.0 perl cpanminus libgetopt-euclid-perl libjson-perl build-essential
RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
RUN cpanm JSON::Parse
ADD run.rb /home/RandomSampling/
ADD ./src /home/RandomSampling/src/
ADD ./algorun/*.rb /home/RandomSampling/algorun/
ADD ./algorun/web/index.html /home/RandomSampling/algorun/web/
ADD ./algorun/web/js /home/RandomSampling/algorun/web/js/
ADD ./algorun/web/css /home/RandomSampling/algorun/web/css/
RUN cd /home/RandomSampling/
ENV RS_HOME /home/RandomSampling/
EXPOSE 8765
ENTRYPOINT ["/usr/bin/ruby","/home/RandomSampling/run.rb","start"]
