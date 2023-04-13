from ubuntu:lunar
RUN apt-get update && apt install -y cmake gcc-arm-none-eabi git python3 g++
RUN git config --global --add safe.directory "*"
ENTRYPOINT ["/headphones/firmware/docker/entrypoint.sh"]