/*
 * (C) Copyright 2017-2019 UCAR
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 */

#include <string>

#include "oops/parallel/mpi/mpi.h"
#include "oops/util/abor1_cpp.h"
#include "oops/util/Logger.h"

#include "fileio/IodaIO.h"

namespace ioda {

// -----------------------------------------------------------------------------

IodaIO::~IodaIO() { }

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the path to the file.
 */

std::string IodaIO::fname() const {
  return fname_;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the mode (read, write, etc) for access to the file.
 */

std::string IodaIO::fmode() const {
  return fmode_;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the number of unique locations in the obs data.
 */

std::size_t IodaIO::nlocs() const {
  return nlocs_;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the number of unique recoreds in the obs data.
 *          A record is an atomic unit that will remain intact during distribution
 *          across multiple process elements. An example is a single sounding in
 *          radiosonde obs data.
 */

std::size_t IodaIO::nrecs() const {
  return nrecs_;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the number of unique variables in the obs data.
 */

std::size_t IodaIO::nvars() const {
  return nvars_;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the begin iterator for the groups contained
 *          in the group, variable information map.
 */

IodaIO::GroupIter IodaIO::group_begin() {
  return grp_var_info_.begin();
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the end iterator for the groups contained
 *          in the group, variable information map.
 */

IodaIO::GroupIter IodaIO::group_end() {
  return grp_var_info_.end();
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the group name for the current iteration
 *          in the group, variable information map.
 *
 * \param[in] igrp Group iterator for GrpVarInfoMap
 */

std::string IodaIO::group_name(IodaIO::GroupIter igrp) {
  return igrp->first;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the begin iterator for the variables, of a
 *          particular group, contained in the group, variable information map.
 *
 * \param[in] igrp Group iterator for GrpVarInfoMap
 */

IodaIO::VarIter IodaIO::var_begin(GroupIter igrp) {
  return igrp->second.begin();
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the end iterator for the variables, of a
 *          particular group, contained in the group, variable information map.
 *
 * \param[in] igrp Group iterator for GrpVarInfoMap
 */

IodaIO::VarIter IodaIO::var_end(GroupIter igrp) {
  return igrp->second.end();
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the variable name for the current iteration
 *          in the group, variable information map.
 *
 * \param[in] ivar Variable iterator for GrpVarInfoMap
 */

std::string IodaIO::var_name(IodaIO::VarIter ivar) {
  return ivar->first;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the variable data type for the current iteration
 *          in the group, variable information map.
 *
 * \param[in] ivar Variable iterator for GrpVarInfoMap
 */

std::string IodaIO::var_dtype(IodaIO::VarIter ivar) {
  return ivar->second.dtype;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the variable data type for the current iteration
 *          in the group, variable information map.
 *
 * \param[in] GroupName Group key for GrpVarInfoMap
 * \param[in] VarName Variable key for GrpVarInfoMap
 */

bool IodaIO::grp_var_exists(const std::string & GroupName, const std::string & VarName) {
  bool GroupExists = false;
  bool VarExists = false;

  // Check for group, and if group exists check for the variable
  GroupIter igrp = grp_var_info_.find(GroupName);
  GroupExists = !(igrp == grp_var_info_.end());
  if (!GroupExists) {
    std::string ErrorMsg = "Group name is not available: " + GroupName;
    oops::Log::error() << ErrorMsg << std::endl;
  }

  if (GroupExists) {
    VarIter ivar = igrp->second.find(VarName);
    VarExists = !(ivar == igrp->second.end());
    if (!VarExists) {
      std::string ErrorMsg = "Group name, variable name combination is not available: " +
                   GroupName + ", " + VarName;
      oops::Log::error() << ErrorMsg << std::endl;
    }
  }

  return GroupExists & VarExists;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the variable data type for the group name, variable
 *          name combination in the group, variable information map.
 *
 * \param[in] GroupName Group key for GrpVarInfoMap
 * \param[in] VarName Variable key for GrpVarInfoMap
 */

std::string IodaIO::var_dtype(const std::string & GroupName, const std::string & VarName) {
  if (!grp_var_exists(GroupName, VarName)) {
    std::string ErrorMsg = "Group name, variable name combination is not available: " +
                            GroupName + ", " + VarName;
    ABORT(ErrorMsg);
  }

  GroupIter igrp = grp_var_info_.find(GroupName);
  VarIter ivar = igrp->second.find(VarName);
  return ivar->second.dtype;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the variable shape for the current iteration
 *          in the group, variable information map.
 *
 * \param[in] ivar Variable iterator for GrpVarInfoMap
 */

std::vector<std::size_t> IodaIO::var_shape(IodaIO::VarIter ivar) {
  return ivar->second.shape;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the variable shape for the group name, variable name
 *          combination in the group, variable information map.
 *
 * \param[in] GroupName Group key for GrpVarInfoMap
 * \param[in] VarName Variable key for GrpVarInfoMap
 */

std::vector<std::size_t> IodaIO::var_shape(const std::string & GroupName,
                                           const std::string & VarName) {
  if (!grp_var_exists(GroupName, VarName)) {
    std::string ErrorMsg = "Group name, variable name combination is not available: " +
                            GroupName + ", " + VarName;
    ABORT(ErrorMsg);
  }

  GroupIter igrp = grp_var_info_.find(GroupName);
  VarIter ivar = igrp->second.find(VarName);
  return ivar->second.shape;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the variable id for the current iteration
 *          in the group, variable information map.
 *
 * \param[in] ivar Variable iterator for GrpVarInfoMap
 */

std::size_t IodaIO::var_id(IodaIO::VarIter ivar) {
  return ivar->second.var_id;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the variable id for the group name, variable
 *          name combination in the group, variable information map.
 *
 * \param[in] GroupName Group key for GrpVarInfoMap
 * \param[in] VarName Variable key for GrpVarInfoMap
 */

std::size_t IodaIO::var_id(const std::string & GroupName, const std::string & VarName) {
  if (!grp_var_exists(GroupName, VarName)) {
    std::string ErrorMsg = "Group name, variable name combination is not available: " +
                            GroupName + ", " + VarName;
    ABORT(ErrorMsg);
  }

  GroupIter igrp = grp_var_info_.find(GroupName);
  VarIter ivar = igrp->second.find(VarName);
  return ivar->second.var_id;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns a flag indicating the existence of the given
 *          dimension name. True indicates the dimension exists, false
 *          indicates the dimension does not exist.
 *
 * \param[in] name Dimension name
 */

bool IodaIO::dim_exists(const std::string & name) {
  return (dim_info_.find(name) != dim_info_.end());
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the dimension name given a dimension iterator.
 *
 * \param[in] idim Dimension iterator
 */

std::string IodaIO::dim_name(IodaIO::DimIter idim) {
  return idim->first;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the dimension id given a dimension iterator.
 *
 * \param[in] idim Dimension iterator
 */

int IodaIO::dim_id(IodaIO::DimIter idim) {
  return idim->second.id;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the dimension size given a dimension iterator.
 *
 * \param[in] idim Dimension iterator
 */

std::size_t IodaIO::dim_size(IodaIO::DimIter idim) {
  return idim->second.size;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the dimension size given a dimension id.
 *
 * \param[in] id Dimension id
 */

std::size_t IodaIO::dim_id_size(const int & id) {
  DimIter idim;
  for (idim = dim_info_.begin(); idim != dim_info_.end(); idim++) {
    if (id == idim->second.id) {
      break;
    }
  }

  if (idim == dim_info_.end()) {
    std::string ErrorMsg = "IodaIO::dim_id_size: Dimension id does not exist: " + id;
    ABORT(ErrorMsg);
  }

  return idim->second.size;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the dimension name given a dimension id.
 *
 * \param[in] id Dimension id
 */

std::string IodaIO::dim_id_name(const int & id) {
  DimIter idim;
  for (idim = dim_info_.begin(); idim != dim_info_.end(); idim++) {
    if (id == idim->second.id) {
      break;
    }
  }

  if (idim == dim_info_.end()) {
    std::string ErrorMsg = "IodaIO::dim_id_name: Dimension id does not exist: " + id;
    ABORT(ErrorMsg);
  }

  return idim->first;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the dimension size given a dimension name.
 *
 * \param[in] name Dimension name
 */

std::size_t IodaIO::dim_name_size(const std::string & name) {
  if (!dim_exists(name)) {
    std::string ErrorMsg = "IodaIO::dim_name_size: Dimension name does not exist: " + name;
    ABORT(ErrorMsg);
  }

  return dim_info_.find(name)->second.size;
}

// -----------------------------------------------------------------------------
/*!
 * \details This method returns the dimension id given a dimension name.
 *
 * \param[in] name Dimension name
 */

int IodaIO::dim_name_id(const std::string & name) {
  if (!dim_exists(name)) {
    std::string ErrorMsg = "IodaIO::dim_name_id: Dimension name does not exist: " + name;
    ABORT(ErrorMsg);
  }

  return dim_info_.find(name)->second.id;
}

}  // namespace ioda
