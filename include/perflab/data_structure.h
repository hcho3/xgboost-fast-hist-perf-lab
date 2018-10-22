#ifndef PERFLAB_DATA_STRUCTURE_H_
#define PERFLAB_DATA_STRUCTURE_H_

#include <iostream>
#include <vector>

namespace perflab {

class GradientPair {
  /*! \brief gradient statistics */
  float grad_;
  /*! \brief second order gradient statistics */
  float hess_;

  void SetGrad(float g) { grad_ = g; }
  void SetHess(float h) { hess_ = h; }

 public:
  GradientPair() : grad_(0), hess_(0) {}
  GradientPair(float grad, float hess) : grad_(grad), hess_(hess) {}
  GradientPair(const GradientPair &g) : grad_(g.grad_), hess_(g.hess_) {}

  float GetGrad() const { return grad_; }
  float GetHess() const { return hess_; }

  GradientPair &operator+=(const GradientPair &rhs) {
    grad_ += rhs.grad_;
    hess_ += rhs.hess_;
    return *this;
  }

  GradientPair operator+(const GradientPair &rhs) const {
    GradientPair g;
    g.grad_ = grad_ + rhs.grad_;
    g.hess_ = hess_ + rhs.hess_;
    return g;
  }

  GradientPair &operator-=(const GradientPair &rhs) {
    grad_ -= rhs.grad_;
    hess_ -= rhs.hess_;
    return *this;
  }

  GradientPair operator-(const GradientPair &rhs) const {
    GradientPair g;
    g.grad_ = grad_ - rhs.grad_;
    g.hess_ = hess_ - rhs.hess_;
    return g;
  }

  friend std::istream& operator>>(std::istream& is, GradientPair& g) {
    is >> g.grad_ >> g.hess_;
    return is;
  }
};

/*! \brief cut configuration for all the features. */
struct HistCutMatrix {
  /*! \brief unit pointer to rows by element position */
  std::vector<uint32_t> row_ptr;
  /*! \brief minimum value of each feature */
  std::vector<float> min_val;
  /*! \brief the cut field */
  std::vector<float> cut;
};

/*!
 * \brief A single row in global histogram index.
 *  Directly represent the global index in the histogram entry.
 */
struct GHistIndexRow {
  /*! \brief The index of the histogram */
  const uint32_t* index;
  /*! \brief The size of the histogram */
  size_t size;
  GHistIndexRow() = default;
  GHistIndexRow(const uint32_t* index, size_t size)
      : index(index), size(size) {}
};

/*!
 * \brief preprocessed global index matrix, in CSR format
 *  Transform floating values to integer index in histogram
 *  This is a global histogram index.
 */
struct GHistIndexMatrix {
  /*! \brief row pointer to rows by element position */
  std::vector<size_t> row_ptr;
  /*! \brief The index data */
  std::vector<uint32_t> index;
  /*! \brief The corresponding cuts */
  HistCutMatrix cut;
  // get i-th row
  inline GHistIndexRow operator[](size_t i) const {
    return {&index[0] + row_ptr[i], row_ptr[i + 1] - row_ptr[i]};
  }
};

/*! \brief sums of gradient statistics corresponding to a histogram bin */
struct GHistEntry {
  /*! \brief sum of first-order gradient statistics */
  double sum_grad{0};
  /*! \brief sum of second-order gradient statistics */
  double sum_hess{0};

  GHistEntry()  = default;

  inline void Clear() {
    sum_grad = sum_hess = 0;
  }

  /*! \brief add a GradientPair to the sum */
  inline void Add(const GradientPair& e) {
    sum_grad += e.GetGrad();
    sum_hess += e.GetHess();
  }

  /*! \brief add a GHistEntry to the sum */
  inline void Add(const GHistEntry& e) {
    sum_grad += e.sum_grad;
    sum_hess += e.sum_hess;
  }

  /*! \brief set sum to be difference of two GHistEntry's */
  inline void SetSubtract(const GHistEntry& a, const GHistEntry& b) {
    sum_grad = a.sum_grad - b.sum_grad;
    sum_hess = a.sum_hess - b.sum_hess;
  }
};

}  // namespace perflab

#endif  // PERFLAB_DATA_STRUCTURE_H_
