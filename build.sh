#!/bin/sh

mkdir -p ./bin
mkdir -p ./data
cd Feeder; make clean; make; cd ..
cd Logger; make clean; make; cd ..
cd Background; make clean; make; cd ..
cd Black; make clean; make; cd ..
cd ColorResults; make clean; make; cd ..
cd DepthChart; make clean; make; cd ..
cd IntersectResults; make clean; make; cd ..
cd IntersectWith; make clean; make; cd ..
cd Lit; make clean; make; cd ..
cd PixelFactory; make clean; make; cd ..
cd Reflection; make clean; make; cd ..
cd Shader; make clean; make; cd ..
cd Writer; make clean; make; cd ..
cd libRayTracer2014; make clean; make; cd ..
