FROM ubuntu:16.04

#Run upgrades
RUN apt-get update

#Install basic packages
RUN apt-get -y install git wget make libncurses-dev flex bison gperf python python-serial vim curl cmake g++ lcov

RUN mkdir -p /root/esp

#Install xtensa toolchain
WORKDIR /root/esp/
RUN wget https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-61-gab8375a-5.2.0.tar.gz
RUN tar -xzf xtensa-esp32-elf-linux64-1.22.0-61-gab8375a-5.2.0.tar.gz
RUN rm xtensa-esp32-elf-linux64-1.22.0-61-gab8375a-5.2.0.tar.gz
ENV PATH=$PATH:/root/esp/xtensa-esp32-elf/bin

#intall esp-idf
WORKDIR /root/esp/
RUN git clone https://github.com/espressif/esp-idf.git
ENV IDF_PATH=/root/esp/esp-idf

WORKDIR /
