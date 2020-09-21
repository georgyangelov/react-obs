#!/bin/sh

mkdir -p controller/src/generated

protoc -I=. --cpp_out=obs-plugin/generated --js_out=import_style=commonjs,binary:controller/src/generated ./protocol.proto
