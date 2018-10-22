#ifndef PERFLAB_BUILD_HIST_H_
#define PERFLAB_BUILD_HIST_H_

#include <perflab/data_structure.h>
#include <vector>

namespace perflab {

/*!
 * \brief builder for histograms of gradient statistics
 */
class GHistBuilder {
 public:
  // initialize builder
  GHistBuilder(size_t nthread, uint32_t nbin)
    : nthread_(nthread), nbin_(nbin), data_(nbin * nthread) {}

  // construct a histogram via histogram aggregation
  void BuildHist(const std::vector<GradientPair>& gpair,
                 const std::vector<size_t>& instance_set,
                 const GHistIndexMatrix& gmat,
                 std::vector<GHistEntry>* hist);

 private:
  /*! \brief number of threads for parallel computation */
  size_t nthread_;
  /*! \brief number of all bins over all features */
  uint32_t nbin_;
  /*! \brief workspace */
  std::vector<GHistEntry> data_;
};


}  // namespace perflab

#endif  // PERFLAB_BUILD_HIST_H_
