#include <dmlc/omp.h>
#include <perflab/build_hist.h>
#include <omp.h>
#include <vector>

namespace perflab {

void GHistBuilder::BuildHist(const std::vector<GradientPair>& gpair,
                             const std::vector<size_t>& instance_set,
                             const GHistIndexMatrix& gmat,
                             std::vector<GHistEntry>* hist) {
  std::fill(data_.begin(), data_.end(), GHistEntry());

  constexpr int kUnroll = 8;  // loop unrolling factor
  const auto nthread = static_cast<dmlc::omp_uint>(this->nthread_);
  const size_t nrows = instance_set.size();
  const size_t rest = nrows % kUnroll;

  #pragma omp parallel for num_threads(nthread) schedule(guided)
  for (dmlc::omp_uint i = 0; i < nrows - rest; i += kUnroll) {
    const dmlc::omp_uint tid = omp_get_thread_num();
    const size_t off = tid * nbin_;
    size_t rid[kUnroll];
    size_t ibegin[kUnroll];
    size_t iend[kUnroll];
    GradientPair stat[kUnroll];
    for (int k = 0; k < kUnroll; ++k) {
      rid[k] = instance_set[i + k];
    }
    for (int k = 0; k < kUnroll; ++k) {
      ibegin[k] = gmat.row_ptr[rid[k]];
      iend[k] = gmat.row_ptr[rid[k] + 1];
    }
    for (int k = 0; k < kUnroll; ++k) {
      stat[k] = gpair[rid[k]];
    }
    for (int k = 0; k < kUnroll; ++k) {
      for (size_t j = ibegin[k]; j < iend[k]; ++j) {
        const uint32_t bin = gmat.index[j];
        data_[off + bin].Add(stat[k]);
      }
    }
  }
  for (size_t i = nrows - rest; i < nrows; ++i) {
    const size_t rid = instance_set[i];
    const size_t ibegin = gmat.row_ptr[rid];
    const size_t iend = gmat.row_ptr[rid + 1];
    const GradientPair stat = gpair[rid];
    for (size_t j = ibegin; j < iend; ++j) {
      const uint32_t bin = gmat.index[j];
      data_[bin].Add(stat);
    }
  }

  /* reduction */
  const uint32_t nbin = nbin_;
  #pragma omp parallel for num_threads(nthread) schedule(static)
  for (dmlc::omp_uint bin_id = 0; bin_id < dmlc::omp_uint(nbin); ++bin_id) {
    for (dmlc::omp_uint tid = 0; tid < nthread; ++tid) {
      (*hist)[bin_id].Add(data_[tid * nbin_ + bin_id]);
    }
  }
}

}  // namespace perflab
