FROM phusion/baseimage:0.9.15
MAINTAINER Thibauld Favre <thibauld@fastmail.com>
RUN apt-get update
RUN apt-get install -y ruby2.0 perl cpanminus libgetopt-euclid-perl libjson-perl build-essential
RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
RUN cpanm JSON::Parse
ADD run.rb /home/SDDS/
ADD ./src /home/SDDS/src/
ADD ./algorun/*.rb /home/SDDS/algorun/
ADD ./algorun/web/index.html /home/SDDS/algorun/web/
ADD ./algorun/web/js /home/SDDS/algorun/web/js/
ADD ./algorun/web/css /home/SDDS/algorun/web/css/
RUN cd /home/SDDS/
ENV SDDS_HOME /home/SDDS/
EXPOSE 8765
ENTRYPOINT ["/usr/bin/ruby","/home/SDDS/run.rb","start"]
