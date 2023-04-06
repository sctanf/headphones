## Docker build environment
A simple script to build the Ploopy firmware inside a docker container.

### Usage
Install docker and run:

```
./build_in_docker.sh
```

This will pull a minimal Ubuntu Luna docker container, install cmake and the arm compiler and build the firmware and tools. The first time you build it will be a little slow.