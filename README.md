## sysstat - System performance tools for the Linux operating system(enable SPDK support in iostat)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/4040/badge.svg)](https://scan.coverity.com/projects/sysstat-sysstat)
[![Build Status](https://travis-ci.org/sysstat/sysstat.svg?branch=master)](https://travis-ci.org/sysstat/sysstat)

### Introduction

This sysstat package is the same as standard sysstat. In particularly, the iostat of this sysstat can also be used to monitor SPDK managed devices. It can report CPU statistics and input/outpurt statistics for both Kernel and SPDK managed devices.

### Install

Clone SPDK repository with:

```
$ git clone https://github.com/spdk/spdk
```
Build the environment of SPDK and DPDK according to the SPDK guide.

Clone sysstat repository under the same folder as SPDK:

```
$ git clone https://github.com/YamboZhou/sysstat.git
```

Configure sysstat for your system:

```
$ cd sysstat
$ ./configure
```

Compile and install:

```
$ make
$ sudo make install
```
### Use Guide

After starting your SPDK application successfully, you can use iostat to check the state of devices. 

For example:

```
#Check read/write state of devices.
iostat

#Check IOPS and throughout of devices.
iostat -mtx 1

#Check state of the specified SPDK managed device.
iostat -d Nvme0n1
```
Note: the name of devices are the same as that specidied in SPDK configuration file. e.g., Nvme0n1, Malloc0

### Note

Now, this is just a initialized version, more works need to be done. At present, the iostat can just be used to monitor the IOPS and throughput of the SPDK managed devices on bdev layer.

To run the newly build sysstat, either explicitly run as ./iostat or add it in to $PATH to avoid invoke already installed version.

Please refer to https://github.com/sysstat/sysstat to get more information about sysstat.
