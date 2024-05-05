## PanoProjector — a fast and low-memory panorama converter

PanoProjector can convert a spherical panorama in the usual equirectangular
projection to cube faces.

## Usage

Extract a face:

```
pano-projector face --face=front sphere.jpg front.jpg
```

Create a tiled pyramid for use by Pannellum:

```
pano-projector pyramid sphere.jpg out_dir
```

Create a tiled pyramid for a single face:

```
pano-projector pyramid sphere.jpg --face=front out_dir
```

See `pano-projector --help` for more information about options.

## Performance

For a 32000x16000 px source image on Intel Core i7-9750H.

Single face extraction:

|                  | User time (s) | Wall time (s) | Max RSS (MiB) |
|------------------|---------------|---------------|---------------|
| Hugin's nona     | 67.90         | 13.54         | 1874          |
| PanoProjector    | 3.82          | 3.90          | 188           |

Single face, pyramid with 512x512px tiles:

|                  | User time (s) | Wall time (s) | Max RSS (MiB) |
|------------------|---------------|---------------|---------------|
| PanoProjector    | 4.11          | 4.32          | 189           |

All faces, pyramid with 512x512 px tiles:

|                  | User time (s) | Wall time (s) | Max RSS (MiB) |
|------------------|---------------|---------------|---------------|
| Pannellum/nona   | 517.19        | 129.24        | 1875          |
| PanoProjector    | 21.30         | 21.87         | 1471          |

By default, nona uses multiple threads, so the user time exceeds the wall time
by a factor of the average number of cores. PanoProjector is single-threaded.
It instead relies on just being fast.

Also, by breaking up the job into separate faces, it is possible to parallelize
by running multiple instances of the tool. We plan to generate faces on demand,
when the user requests them.

## Installation

PanoProjector is written in C++ and requires C++20. It uses a CMake build
system. It depends on libjpeg and Boost program_options.

When it is compiled for an x86-64 architecture, the default build options
require a processor equivalent to Intel Skylake or later.

On Debian or similar:

```
sudo apt-get install gcc cmake libjpeg-turbo8-dev libboost-program-options-dev
cmake -DCMAKE_BUILD_TYPE=Release .
make
sudo make install
```
