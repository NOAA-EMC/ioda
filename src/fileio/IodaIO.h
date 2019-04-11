/*
 * (C) Copyright 2017-2019 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef FILEIO_IODAIO_H_
#define FILEIO_IODAIO_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "eckit/mpi/Comm.h"

#include "distribution/Distribution.h"
#include "oops/util/Printable.h"

// Forward declarations
namespace eckit {
  class Configuration;
}

namespace ioda {

/*!
 * \brief File access class for IODA
 *
 * \details The IodaIO class provides the interface for file access. Note that IodaIO is an
 *          abstract base class.
 *
 * Eventually, we want to get to the same file format for every obs type.
 * Currently we are defining this as follows. A file can contain any
 * number of variables. Each variable is a 1D vector that is nlocs long.
 * Variables can contain missing values.
 *
 * There are three dimensions defined in the file:
 *
 *   nlocs: number of locations
 *   nvars: number of variables
 *   nrecs: number of records
 *
 * A record is an atomic unit that is to stay intact when distributing
 * observations across multiple processes.
 *
 * The constructor that you fill in with a subclass is responsible for:
 *    1. Open the file
 *         The file name and mode (read, write) is passed in to the subclass
 *         constructor via a call to the factory method Create in the class IodaIOfactory.
 *    2. The following data members are set according to the file mode
 *         * nlocs_
 *         * nrecs_
 *         * nvars_
 *         * grp_var_info_
 *
 *       If in read mode, metadata from the input file are used to set the data members
 *       If in write mode, the data members are set from the constructor arguments 
 *       (grp_var_info_ is not used in the write mode case).
 *
 * \author Stephen Herbener (JCSDA)
 */

class IodaIO : public util::Printable {
 protected:
    // typedefs - these need to defined here before the public typedefs that use them

    // Container for information about a variable. Use a nested map structure with the group
    // name for the key in the outer nest, and the variable name for the key in the inner
    // nest. Information that is recorded is the variable data type and shape.
    typedef struct {
      std::string dtype;
      std::size_t var_id;
      std::vector<std::size_t> shape;
      std::vector<std::string> dim_names;
    } VarInfoRec;

    /*!
     * \brief variable information map
     *
     * \details This typedef is part of the group-variable map which is a nested map
     *          containing information about the variables in the input file (see
     *          GroupVarInfoMap typedef for details).
     */
    typedef std::map<std::string, VarInfoRec> VarInfoMap;

    /*!
     * \brief group-variable information map
     *
     * \details This typedef is defining the group-variable map which is a nested map
     *          containing information about the variables in the input file.
     *          This map is keyed first by group name, then by variable name and is
     *          used to pass information to the caller so that the caller can iterate
     *          through the contents of the input file.
     */
    typedef std::map<std::string, VarInfoMap> GroupVarInfoMap;

    // Container for information about a dimension. Holds dimension id and size.
    typedef struct {
      std::size_t size;
      int         id;
    } DimInfoRec;
 
    /*!
     * \brief dimension information map
     *
     * \details This typedef is dimension information map which containes
     *          information about the dimensions of the variables.
     */
    typedef std::map<std::string, DimInfoRec> DimInfoMap;

 public:
    virtual ~IodaIO() = 0;

    // Methods provided by subclasses
    virtual void ReadVar(const std::string & GroupName, const std::string & VarName,
                         const std::vector<std::size_t> & VarShape, int * VarData) = 0;
    virtual void ReadVar(const std::string & GroupName, const std::string & VarName,
                         const std::vector<std::size_t> & VarShape, float * VarData) = 0;
    virtual void ReadVar(const std::string & GroupName, const std::string & VarName,
                         const std::vector<std::size_t> & VarShape, double * VarData) = 0;
    virtual void ReadVar(const std::string & GroupName, const std::string & VarName,
                         const std::vector<std::size_t> & VarShape, char * VarData) = 0;

    virtual void WriteVar(const std::string & GroupName, const std::string & VarName,
                          const std::vector<std::size_t> & VarShape, int * VarData) = 0;
    virtual void WriteVar(const std::string & GroupName, const std::string & VarName,
                          const std::vector<std::size_t> & VarShape, float * VarData) = 0;
    virtual void WriteVar(const std::string & GroupName, const std::string & VarName,
                          const std::vector<std::size_t> & VarShape, char * VarData) = 0;

    // Methods inherited from base class
    std::string fname() const;
    std::string fmode() const;

    std::size_t nlocs() const;
    std::size_t nrecs() const;
    std::size_t nvars() const;

    // Group level iterator
    /*!
     * \brief group-variable map, group iterator
     *
     * \details This typedef is defining the iterator for the group key in the
     *          group-variable map. See the GrpVarInfoMap typedef for details.
     */
    typedef GroupVarInfoMap::const_iterator GroupIter;
    GroupIter group_begin();
    GroupIter group_end();
    std::string group_name(GroupIter);

    // Variable level iterator
    /*!
     * \brief group-variable map, variable iterator
     *
     * \details This typedef is defining the iterator for the variable key in the
     *          group-variable map. See the GrpVarInfoMap typedef for details.
     */
    typedef VarInfoMap::const_iterator VarIter;
    VarIter var_begin(GroupIter);
    VarIter var_end(GroupIter);
    std::string var_name(VarIter);

    // Access to variable information
    bool grp_var_exist(const std::string &, const std::string &);
    std::string var_dtype(VarIter);
    std::string var_dtype(const std::string &, const std::string &);
    std::vector<std::size_t> var_shape(VarIter);
    std::vector<std::size_t> var_shape(const std::string &, const std::string &);
    std::size_t var_id(VarIter);
    std::size_t var_id(const std::string &, const std::string &);

    // Access to dimension information
    typedef DimInfoMap::const_iterator DimIter;

    std::string dim_name(DimIter);
    int         dim_id(DimIter);
    std::size_t dim_size(DimIter);

    std::size_t dim_id_size(const int &);
    std::string dim_id_name(const int &);

    std::size_t dim_name_size(const std::string &);
    int         dim_name_id(const std::string &);

 protected:
    // Methods provided by subclasses

    // Methods inherited from base class

    // Data members

    /*!
     * \brief file name
     */
    std::string fname_;

    /*!
     * \brief file mode
     *
     * \details File modes that are accepted are: "r" -> read, "w" -> overwrite,
                and "W" -> create and write.
     */
    std::string fmode_;

    /*!
     * \brief number of unique locations
     */
    std::size_t nlocs_;

    /*!
     * \brief number of unique records
     */
    std::size_t nrecs_;

    /*!
     * \brief number of unique variables
     */
    std::size_t nvars_;

    /*!
     * \brief group-variable information map
     *
     * \details See the GrpVarInfoMap typedef for details.
     */
    GroupVarInfoMap grp_var_info_;

    /*!
     * \brief dimension information map
     */
    DimInfoMap dim_info_;
};

}  // namespace ioda

#endif  // FILEIO_IODAIO_H_
