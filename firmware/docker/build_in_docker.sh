#!/bin/bash
CHECKOUT_ROOT=`git rev-parse --show-toplevel`
docker build -t ploopy-headphones:0.1 .
docker run -v $CHECKOUT_ROOT:/headphones -u "$(id -u):$(id -g)" ploopy-headphones:0.1