FROM phusion/baseimage:0.9.15
MAINTAINER Thibauld Favre <thibauld@fastmail.com>
RUN apt-get update
RUN apt-get install -y ruby2.0 aptitude
RUN aptitude -y install build-essential
RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
ADD run.rb /home/REACT/
ADD ./src/*.h /home/REACT/src/
ADD ./src/*.cpp /home/REACT/src/
ADD ./src/Makefile /home/REACT/src/
ADD ./algorun/*.rb /home/REACT/algorun/
ADD ./algorun/web/index.html /home/REACT/algorun/web/
ADD ./algorun/web/js /home/REACT/algorun/web/js/
ADD ./algorun/web/css /home/REACT/algorun/web/css/
RUN cd /home/REACT/
ENV CODE_HOME /home/REACT
RUN ["/usr/bin/ruby","/home/REACT/run.rb","make"]
RUN rm -f src/*.o
EXPOSE 8765
ENTRYPOINT ["/usr/bin/ruby","/home/REACT/run.rb","start"]
