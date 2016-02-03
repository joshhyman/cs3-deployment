FROM ubuntu:latest

# Install the C and C++ compilers, then clean up.
RUN apt-get update && apt-get install --auto-remove -y gcc g++ && apt-get clean autoclean && apt-get autoremove -y && rm -rf /tmp/* /var/lib/{apt,dpkg,cache,log}

WORKDIR /usr/src/hello
COPY mybinary.cc /usr/src/hello/mybinary.cc

RUN gcc mybinary.cc -o mybinary

CMD ["./mybinary"]
