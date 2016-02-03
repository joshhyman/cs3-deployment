FROM ubuntu:latest

# Install the C and C++ compilers, then clean up.
RUN apt-get update && apt-get install --auto-remove -y make g++ libboost-all-dev && apt-get clean autoclean && apt-get autoremove -y && rm -rf /tmp/* /var/lib/{apt,dpkg,cache,log}

WORKDIR /usr/src/webserver
COPY webserver /usr/src/webserver

RUN make

EXPOSE 1080:1080
CMD ["./server_main", "config_file"]
