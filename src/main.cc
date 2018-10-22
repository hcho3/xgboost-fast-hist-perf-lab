#include <dmlc/timer.h>
#include <dmlc/logging.h>
#include <dmlc/omp.h>
#include <perflab/data_structure.h>
#include <perflab/build_hist.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

template <typename T>
std::vector<T> ParseNewlineSeparatedText(const std::string& path) {
  std::ifstream fin(path);
  std::vector<T> vec;
  T val;
  CHECK(!fin.fail()) << "File " << path << " not found!";
  while ( (fin >> val) ) {
    vec.push_back(val);
  }
  return vec;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " [location of extracted record.tar.bz2] [number of threads]" << std::endl;
    return 1;
  }
  const std::string record_path(argv[1]);
  const int nthread = std::stoi(argv[2]); 
  CHECK_GT(nthread, 0) << "There should be positive number of threads";
  CHECK_LE(nthread, omp_get_max_threads()) << "Too many threads";

  LOG(INFO) << "Record location = " << record_path << ", Using " << nthread << " threads";

  double tstart = dmlc::GetTime();

  perflab::GHistIndexMatrix gmat;
  gmat.row_ptr = ParseNewlineSeparatedText<size_t>(record_path + "/gmat_row_ptr.txt");
  gmat.index = ParseNewlineSeparatedText<uint32_t>(record_path + "/gmat_index.txt");
  gmat.cut.row_ptr = ParseNewlineSeparatedText<uint32_t>(record_path + "/gmat_cut_row_ptr.txt");
  gmat.cut.min_val = ParseNewlineSeparatedText<float>(record_path + "/gmat_cut_min_val.txt");
  gmat.cut.cut = ParseNewlineSeparatedText<float>(record_path + "/gmat_cut_cut.txt");

  // size of each gradient histogram
  const uint32_t nbin = gmat.cut.row_ptr.back();

  std::vector<perflab::GradientPair> gpair
    = ParseNewlineSeparatedText<perflab::GradientPair>(record_path + "/gpair-198-0.txt");

  // One histogram is created for each instance set
  const int num_instance_set = 2920;
  std::vector<std::vector<size_t>> instance_set;
  std::vector<std::vector<perflab::GHistEntry>> histogram(
    num_instance_set, std::vector<perflab::GHistEntry>(nbin));

  for (int i = 0; i < num_instance_set; ++i) {
    const std::string path = std::string(record_path + "/rowind-198-") + std::to_string(i) + ".txt";
    instance_set.push_back(ParseNewlineSeparatedText<size_t>(path));
  }

  LOG(INFO) << "Data loaded in " << (dmlc::GetTime() - tstart) << " seconds";

  // Initialize histogram builder
  perflab::GHistBuilder hist_builder(nthread, nbin);

  // Compute histograms
  tstart = dmlc::GetTime();
  for (int i = 0; i < num_instance_set; ++i) {
    hist_builder.BuildHist(gpair, instance_set[i], gmat, &histogram[i]);
  }
  LOG(INFO) << "Gradient histograms computed in " << (dmlc::GetTime() - tstart) << " seconds";

  return 0;
}
