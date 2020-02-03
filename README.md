# HOOMD-blue-MSVC

[![Citing HOOMD](https://img.shields.io/badge/cite-hoomd-blue.svg)](https://glotzerlab.engin.umich.edu/hoomd-blue/citing.html)
[![conda-forge](https://img.shields.io/conda/vn/conda-forge/hoomd.svg?style=flat)](https://anaconda.org/conda-forge/hoomd)
[![conda-forge Downloads](https://img.shields.io/conda/dn/conda-forge/hoomd.svg?style=flat)](https://anaconda.org/conda-forge/hoomd)
[![Azure Pipelines](https://dev.azure.com/glotzerlab/hoomd-blue/_apis/build/status/test?branchName=master)](https://dev.azure.com/glotzerlab/hoomd-blue/_build)
[![Read the Docs](https://img.shields.io/readthedocs/hoomd-blue/stable.svg)](https://hoomd-blue.readthedocs.io/en/stable/?badge=stable)
[![Contributors](https://img.shields.io/github/contributors-anon/glotzerlab/hoomd-blue.svg?style=flat)](https://hoomd-blue.readthedocs.io/en/stable/credits.html)
[![License](https://img.shields.io/badge/license-BSD--3--Clause-green.svg)](LICENSE)

[**HOOMD-blue**](https://github.com/glotzerlab/hoomd-blue) is a general purpose particle simulation toolkit. It performs
hard particle Monte Carlo simulations of a variety of shape classes, and
molecular dynamics simulations of particles with a range of pair, bond, angle,
and other potentials. **HOOMD-blue** runs fast on NVIDIA GPUs, and can scale
across thousands of nodes. For more information, see the [**HOOMD-blue**
website](https://glotzerlab.engin.umich.edu/hoomd-blue/).

## HOOMD-blue-MSVC

This project is a windows version of [**HOOMD-blue**](https://github.com/glotzerlab/hoomd-blue).
The MPI is 

## Dependencies

- Visual Studio 2017
- Anaconda 2019.10 (Python 3.7) with default install path
- CUDA 10.1
- CUB 1.8 @ https://github.com/NVlabs/cub
- Moderngpu 1.1 @ https://github.com/moderngpu/moderngpu
- Pybind11 2.4.3 @ https://github.com/pybind/pybind11
- upp11 is replaced by Catch 2.11.1 @ https://github.com/catchorg/Catch2
- nano-singal-slot 2.0.1 @ https://github.com/NoAvailableAlias/nano-signal-slot

## Progress
Classes and module are modified and tested based on original unit test.

- [x] Core modules
  - test_gpu_array
  - test_global_array
  - test_index1d, test_shared_signal, test_messenger, test_profiler
  - test_vec2, test_vec3, test_rotmat2, test_rotmat3, test_quat
  - test_pdata
  - test_particle_group
- [ ] md module
- [ ] dem module
- [ ] Python
- [ ] MPI



## Change log

See [CHANGELOG.rst](CHANGELOG.rst).