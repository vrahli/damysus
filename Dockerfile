# Use an official ocaml runtime as a parent image
# Install focal
FROM ubuntu:20.04

EXPOSE 80/tcp

# Set the working directory to /app
WORKDIR /app

# This is to avoid interactive questions from tzdata
ENV DEBIAN_FRONTEND=noninteractive

# install some requirements
RUN apt-get update \
    && apt-get install -y apt-utils git wget iptables libdevmapper1.02.1 \
    && apt-get install -y build-essential ocaml ocamlbuild automake autoconf libtool python-is-python3 libssl-dev git cmake perl \
    && apt-get install -y libcurl4-openssl-dev protobuf-compiler libprotobuf-dev debhelper reprepro unzip build-essential python \
    && apt-get install -y lsb-release software-properties-common \
    && apt-get install -y pkg-config libuv1-dev python3-matplotlib \
    && apt-get install -y emacs psmisc jq iproute2

# install a newer version of openssl
RUN wget https://www.openssl.org/source/openssl-1.1.1i.tar.gz \
    && tar -xvzf openssl-1.1.1i.tar.gz \
    && cd openssl-1.1.1i \
    && ./config --prefix=/usr no-mdc2 no-idea \
    && make \
    && make install

# clone sgx sdk
RUN mkdir /opt/intel \
    && cd /opt/intel \
    && git clone https://github.com/intel/linux-sgx \
    && cd linux-sgx \
    && git checkout -b sgx2.13 73b8b57aea306d1633cc44a2efc40de6f4217364
# this version seems to work on non-sgx machines...

# build sdk
RUN cd /opt/intel/linux-sgx \
    && make preparation
RUN cd /opt/intel/linux-sgx/external/toolset/ubuntu20.04 \
    && cp as /usr/local/bin \
    && cp ld /usr/local/bin \
    && cp ld.gold /usr/local/bin \
    && cp objdump /usr/local/bin
RUN cd /opt/intel/linux-sgx \
    && make sdk \
    && make sdk_install_pkg

# install SDK
RUN cd /opt/intel/linux-sgx/linux/installer/bin \
    && echo -e "no\n/opt/intel\n" | ./sgx_linux_x64_sdk_2.13.100.4.bin \
    && . /opt/intel/sgxsdk/environment

# build PSW
RUN mkdir /etc/init
RUN . /opt/intel/sgxsdk/environment && cd /opt/intel/linux-sgx && make psw
RUN . /opt/intel/sgxsdk/environment && cd /opt/intel/linux-sgx && make deb_psw_pkg
RUN . /opt/intel/sgxsdk/environment && cd /opt/intel/linux-sgx && make deb_local_repo
#RUN add-apt-repository "deb [trusted=yes arch=amd64] file:/opt/intel/linux-sgx/linux/installer/deb/sgx_debian_local_repo focal main"; exit 0
RUN echo "deb [trusted=yes arch=amd64] file:/opt/intel/linux-sgx/linux/installer/deb/sgx_debian_local_repo focal main" >> /etc/apt/sources.list
RUN echo "# deb-src [trusted=yes arch=amd64] file:/opt/intel/linux-sgx/linux/installer/deb/sgx_debian_local_repo focal main" >> /etc/apt/sources.list
RUN apt update

# install psw
RUN apt-get install -y libsgx-launch libsgx-urts libsgx-epid libsgx-quote-ex libsgx-dcap-ql

RUN cd /opt/intel/linux-sgx/external/dcap_source/QuoteVerification/sgxssl/Linux \
    && ./build_openssl.sh \
    && make all \
    && make install

# Copy the current directory contents into the container at /app
COPY Makefile       /app
COPY experiments.py /app
COPY enclave.token  /app
COPY App            /app/App
COPY Enclave        /app/Enclave
#COPY salticidae     /app/salticidae

RUN cd /app \
    && git clone https://github.com/Determinant/salticidae.git \
    && cd salticidae \
    && cmake . -DCMAKE_INSTALL_PREFIX=. \
    && make \
    && make install \
    && pwd


#RUN . /opt/intel/sgxsdk/environment && make


CMD ["/bin/bash"]
#ENTRYPOINT ["pwd"]


#### Once docker has been installed, run the following so that sudo won't be needed:
####     sudo groupadd docker
####     sudo gpasswd -a $USER docker
####     -> log out and back in, and then:
####     sudo service docker restart
#### build container: docker build -t damysus .
#### run container: docker run -td --expose=8000-9999 --network="host" --name damysus0 damysus
#### (alternatively) run container in interactive mode: docker container run -it damysus /bin/bash
#### docker exec -t damysus0 bash -c "source /opt/intel/sgxsdk/environment; make"
