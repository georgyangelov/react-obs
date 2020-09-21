#!/bin/sh

rm -rf obs-plugin/generated

JS_OUT_DIR="controller/src/generated"
mkdir -p "$JS_OUT_DIR"

CPP_OUT_DIR="obs-plugin/generated"
mkdir -p "$CPP_OUT_DIR"

PROTOC_GEN_TS_PATH="./controller/node_modules/.bin/protoc-gen-ts"

protoc \
    -I=. \
    --cpp_out="$CPP_OUT_DIR" \
    --plugin="protoc-gen-ts=${PROTOC_GEN_TS_PATH}" \
    --js_out=import_style=commonjs,binary:"$JS_OUT_DIR" \
    --ts_out="$JS_OUT_DIR" \
    ./protocol.proto
