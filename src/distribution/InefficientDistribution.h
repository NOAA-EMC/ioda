/*
 * (C) Copyright 2019 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef DISTRIBUTION_INEFFICIENTDISTRIBUTION_H_
#define DISTRIBUTION_INEFFICIENTDISTRIBUTION_H_

#include <vector>

#include "eckit/mpi/Comm.h"

#include "oops/util/Logger.h"

#include "ioda/distribution/Distribution.h"

namespace ioda {

// ---------------------------------------------------------------------
/*!
 * \brief Inefficient distribution
 *
 * \details This class implements distribution that has copies of all
 *          observations on each processor (to be used for testing)
 *
 */
class InefficientDistribution: public Distribution {
 public:
     explicit InefficientDistribution(const eckit::mpi::Comm & Comm);
     ~InefficientDistribution();

     bool isMyRecord(std::size_t RecNum) const override;

     // The sum/min/max functions do nothing for the inefficient
     // distribution. Each processor has each observation so the local 
     // sum/min/max is equal to the global sum/min/max 
     void sum(double &x) override {};
     void sum(int &x) override {};
     void sum(size_t &x) override {};
     void sum(std::vector<double> &x) override {};
     void sum(std::vector<size_t> &x) override {};

     void min(double &x) override {};
     void min(float &x) override {};
     void min(int &x) override {};

     void max(double &x) override {};
     void max(float &x) override {};
     void max(int &x) override {};
};

}  // namespace ioda

#endif  // DISTRIBUTION_INEFFICIENTDISTRIBUTION_H_
