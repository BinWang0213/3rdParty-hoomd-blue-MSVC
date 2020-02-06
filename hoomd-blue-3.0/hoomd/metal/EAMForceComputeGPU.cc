// Copyright (c) 2009-2019 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.

// Maintainer: Lin Yang, Alex Travesset
// Previous Maintainer: Morozov

/*! \file EAMForceComputeGPU.cc
 \brief Defines the EAMForceComputeGPU class
 */

#include "EAMForceComputeGPU.h"
#include <hip/hip_runtime.h>

#include <stdexcept>

#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace std;

/*! \param sysdef System to compute forces on
 \param filename Name of EAM potential file to load
 \param type_of_file EAM/Alloy=0, EAM/FS=1
 */
EAMForceComputeGPU::EAMForceComputeGPU(std::shared_ptr<SystemDefinition> sysdef, char *filename, int type_of_file) :
        EAMForceCompute(sysdef, filename, type_of_file)
    {

    // can't run on the GPU if there aren't any GPUs in the execution configuration
    if (!m_exec_conf->isCUDAEnabled())
        {
        m_exec_conf->msg->error() << "Creating a EAMForceComputeGPU with no GPU in the execution configuration" << endl;
        throw std::runtime_error("Error initializing EAMForceComputeGPU");
        }

    unsigned int warp_size = m_exec_conf->dev_prop.warpSize;
    unsigned int max_threads = m_exec_conf->dev_prop.maxThreadsPerBlock;
    m_tuner.reset(new Autotuner(warp_size, max_threads, warp_size, 5, 100000, "pair_eam", this->m_exec_conf));

    // allocate the coefficients data on the GPU
    loadFile(filename, type_of_file);
    GlobalArray<EAMTexInterData> eam_data(1, m_exec_conf);
    std::swap(eam_data, m_eam_data);

    ArrayHandle<EAMTexInterData> h_eam_data(m_eam_data, access_location::host, access_mode::overwrite);
    h_eam_data.data->nr = nr; //!< number of tabulated values of interpolated rho(r), r*phi(r)
    h_eam_data.data->nrho = nrho; //!< number of tabulated values of interpolated F(rho)
    h_eam_data.data->dr = dr;                   //!< interval of r in interpolated table
    h_eam_data.data->rdr = 1.0 / dr;              //!< 1.0 / dr
    h_eam_data.data->drho = drho;             //!< interval of rho in interpolated table
    h_eam_data.data->rdrho = 1.0 / drho;          //!< 1.0 / drho
    h_eam_data.data->r_cut = m_r_cut;             //!< cut-off radius
    h_eam_data.data->r_cutsq = m_r_cut * m_r_cut; //!< r_cut^2
    h_eam_data.data->ntypes = m_ntypes;           //!< number of potential element types
    }

EAMForceComputeGPU::~EAMForceComputeGPU()
    {
    }

void EAMForceComputeGPU::computeForces(unsigned int timestep)
    {
    // start by updating the neighborlist
    m_nlist->compute(timestep);

    // start the profile
    if (m_prof)
        m_prof->push(m_exec_conf, "EAM pair");

    // The GPU implementation CANNOT handle a half neighborlist, error out now
    bool third_law = m_nlist->getStorageMode() == NeighborList::half;
    if (third_law)
        {
        m_exec_conf->msg->error() << "EAMForceComputeGPU cannot handle a half neighborlist" << endl;
        throw runtime_error("Error computing forces in EAMForceComputeGPU");
        }

    // access the neighbor list, which just selects the neighborlist into the device's memory, copying
    // it there if needed
    ArrayHandle<unsigned int> d_n_neigh(this->m_nlist->getNNeighArray(), access_location::device, access_mode::read);
    ArrayHandle<unsigned int> d_nlist(this->m_nlist->getNListArray(), access_location::device, access_mode::read);
    ArrayHandle<unsigned int> d_head_list(this->m_nlist->getHeadList(), access_location::device, access_mode::read);

    // access the particle data
    ArrayHandle<Scalar4> d_pos(m_pdata->getPositions(), access_location::device, access_mode::read);
    BoxDim box = m_pdata->getBox();

    ArrayHandle<Scalar4> d_force(m_force, access_location::device, access_mode::overwrite);
    ArrayHandle<Scalar> d_virial(m_virial, access_location::device, access_mode::overwrite);

    // access the potential data
    ArrayHandle<Scalar4> d_F(m_F, access_location::device, access_mode::read);
    ArrayHandle<Scalar4> d_dF(m_dF, access_location::device, access_mode::read);
    ArrayHandle<Scalar4> d_rho(m_rho, access_location::device, access_mode::read);
    ArrayHandle<Scalar4> d_drho(m_drho, access_location::device, access_mode::read);
    ArrayHandle<Scalar4> d_rphi(m_rphi, access_location::device, access_mode::read);
    ArrayHandle<Scalar4> d_drphi(m_drphi, access_location::device, access_mode::read);
    ArrayHandle<EAMTexInterData> d_eam_data(m_eam_data, access_location::device, access_mode::read);

    // Derivative Embedding Function for each atom
    GPUArray<Scalar> t_dFdP(m_pdata->getN(), m_exec_conf);
    m_dFdP.swap(t_dFdP);
    ArrayHandle<Scalar> d_dFdP(m_dFdP, access_location::device, access_mode::overwrite);

    // Compute energy and forces in GPU
    m_tuner->begin();
    gpu_compute_eam_tex_inter_forces(d_force.data, d_virial.data, m_virial.getPitch(), m_pdata->getN(), d_pos.data, box,
            d_n_neigh.data, d_nlist.data, d_head_list.data, this->m_nlist->getNListArray().getPitch(), d_eam_data.data,
            d_dFdP.data, d_F.data, d_rho.data, d_rphi.data, d_dF.data, d_drho.data, d_drphi.data,
            m_tuner->getParam());

    if (m_exec_conf->isCUDAErrorCheckingEnabled())
        CHECK_CUDA_ERROR();
    m_tuner->end();

    if (m_prof)
        m_prof->pop(m_exec_conf);
    }

void export_EAMForceComputeGPU(py::module &m)
    {
    py::class_<EAMForceComputeGPU, EAMForceCompute, std::shared_ptr<EAMForceComputeGPU>>(m, "EAMForceComputeGPU")
        .def(py::init<std::shared_ptr<SystemDefinition>, char *, int>());
    }