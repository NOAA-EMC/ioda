/*
 * (C) Copyright 2017-2019 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef FILEIO_NETCDFIO_H_
#define FILEIO_NETCDFIO_H_

#include <map>
#include <string>
#include <vector>

#include "fileio/IodaIO.h"
#include "oops/util/DateTime.h"
#include "oops/util/ObjectCounter.h"

#include "database/ObsSpaceContainer.h"


////////////////////////////////////////////////////////////////////////
// Implementation of IodaIO for netcdf.
////////////////////////////////////////////////////////////////////////

// Forward declarations
namespace eckit {
  class Configuration;
}

namespace ioda {

/*! \brief Implementation of IodaIO for netcdf.
 *
 * \details The NetcdfIO class defines the constructor and methods for netcdf
 *          file access. These fill in the abstract base class IodaIO methods.
 *
 * \author Stephen Herbener (JCSDA)
 */
class NetcdfIO : public IodaIO,
                 private util::ObjectCounter<NetcdfIO> {
 public:
  /*!
   * \brief classname method for object counter
   *
   * \details This method is supplied for the ObjectCounter base class.
   *          It defines a name to identify an object of this class
   *          for reporting by OOPS.
   */
  static const std::string classname() {return "ioda::NetcdfIO";}

  NetcdfIO(const std::string & FileName, const std::string & FileMode,
           const std::size_t MaxFrameSize);
  ~NetcdfIO();

  IodaIO::FrameIter frame_begin();
  void frame_next(IodaIO::FrameIter &);
  IodaIO::FrameIter frame_end();

  std::size_t frame_start(IodaIO::FrameIter);
  std::size_t frame_size(IodaIO::FrameIter);

 private:
  // For the oops::Printable base class
  void print(std::ostream & os) const;

  void NcReadVar(const int FileId, const int VarId,
                 const std::vector<std::size_t> & Starts,
                 const std::vector<std::size_t> & Counts,
                 const std::vector<std::int64_t> & Strides,
                 int & FillValue, std::vector<int> & VarData);
  void NcReadVar(const int FileId, const int VarId,
                 const std::vector<std::size_t> & Starts,
                 const std::vector<std::size_t> & Counts,
                 const std::vector<std::int64_t> & Strides,
                 float & FillValue, std::vector<float> & VarData);
  void NcReadVar(const int FileId, const int VarId,
                 const std::vector<std::size_t> & Starts,
                 const std::vector<std::size_t> & Counts,
                 const std::vector<std::int64_t> & Strides,
                 double & FillValue, std::vector<double> & VarData);
  void NcReadVar(const int FileId, const int VarId,
                 const std::vector<std::size_t> & Starts,
                 const std::vector<std::size_t> & Counts,
                 const std::vector<std::int64_t> & Strides,
                 char & FillValue, std::vector<std::string> & VarData);

  void NcWriteVar(const int FileId, const int VarId,
                 const std::vector<std::size_t> & Starts,
                 const std::vector<std::size_t> & Counts,
                 const std::vector<std::int64_t> & Strides,
                 const std::vector<int> & VarData);
  void NcWriteVar(const int FileId, const int VarId,
                 const std::vector<std::size_t> & Starts,
                 const std::vector<std::size_t> & Counts,
                 const std::vector<std::int64_t> & Strides,
                 const std::vector<float> & VarData);
  void NcWriteVar(const int FileId, const int VarId,
                 const std::vector<std::size_t> & Starts,
                 const std::vector<std::size_t> & Counts,
                 const std::vector<std::int64_t> & Strides,
                 const std::vector<std::string> & VarData);

  void CheckNcCall(int RetCode, std::string & ErrorMsg);

  bool NcAttrExists(const int & AttrOwnerId, const std::string & AttrName);

  std::string FormNcVarName(const std::string & GroupName, const std::string & VarName);

  void CreateNcDim(const std::string DimName, const std::size_t DimSize);

  int GetStringDimBySize(const std::size_t DimSize);

  void ReadConvertDateTime(std::string GroupName, std::string VarName,
                           std::vector<std::string> & VarData);

  template <typename DataType>
  void ReplaceFillWithMissing(std::vector<DataType> & VarData, DataType NcFillValue);

  std::size_t GetMaxStringSize(const std::vector<std::string> & Strings);
  std::vector<int> GetNcDimIds(const std::string & GroupName,
                               const std::vector<std::size_t> & VarShape);

  void GetFrame(IodaIO::FrameIter & iframe);

  // Data members
  /*!
   * \brief netcdf file id
   *
   * \details This data member holds the file id of the open netcdf file.
   *          It gives access to the dimensions, attributes and variables in
   *          the netcdf file.
   */
  int ncid_;

  /*!
   * \brief offset time flag
   *
   * \details This data member is a flag indicating the existence of the
   *          offset time variable in the netcdf file.
   */
  bool have_offset_time_;

  /*!
   * \brief date time flag
   *
   * \details This data member is a flag indicating the existence of the
   *          date_time variable in the netcdf file.
   */
  bool have_date_time_;

  /*! \brief Containers for file frame */
  std::unique_ptr<ioda::ObsSpaceContainer<int>> int_frame_;
  std::unique_ptr<ioda::ObsSpaceContainer<float>> float_frame_;
  std::unique_ptr<ioda::ObsSpaceContainer<std::string>> string_frame_;
};

}  // namespace ioda

#endif  // FILEIO_NETCDFIO_H_
