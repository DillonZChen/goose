#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <xgboost/c_api.h>
// #include <xgboost/data.h>
// #include <xgboost/learner.h>

#define safe_xgboost(call)                                                     \
  {                                                                            \
    int err = (call);                                                          \
    if (err != 0) {                                                            \
      std::cout << std::string(__FILE__) + ":" + std::to_string(__LINE__) +    \
                       ": error in " + #call + ":" + XGBGetLastError()         \
                << std::endl;                                                  \
      exit(-1);                                                                \
    }                                                                          \
  }

std::vector<std::vector<float>> mat_2d_from_file(const std::string &path) {
  std::vector<std::vector<float>> matrix;
  std::ifstream file(path);

  if (!file.good()) {
    std::cerr << "File does not exist: " << path << std::endl;
    exit(-1);
  }

  if (!file.is_open()) {
    std::cerr << "Error opening file: " << path << std::endl;
    exit(-1);
  }

  std::string line;
  while (std::getline(file, line)) {
    std::vector<float> row;
    std::stringstream ss(line);
    std::string s;
    float value;

    while (std::getline(ss, s, ' ')) {
      row.push_back(stof(s));
    }

    matrix.push_back(row);
  }

  return matrix;
}

void print_shape(const std::vector<std::vector<float>> &matrix) {
  size_t numRows = matrix.size();
  size_t numCols = (numRows > 0) ? matrix[0].size() : 0;

  std::cout << "(" << numRows << ", " << numCols << ")" << std::endl;
}

const float *convertMatToArr(const std::vector<std::vector<float>> &myVector) {
  // Get the dimensions of the vector
  size_t rows = myVector.size();
  size_t cols = (rows > 0) ? myVector[0].size() : 0;

  // Dynamically allocate a contiguous 2D array
  float *myArray = new float[rows * cols];

  // Copy elements from vector to array
  size_t index = 0;
  for (size_t i = 0; i < rows; ++i) {
    for (size_t j = 0; j < cols; ++j) {
      myArray[index++] = myVector[i][j];
    }
  }

  return myArray;
}

void print2DArray(const float *myArray, size_t rows, size_t cols) {
  // Print the 2D array
  for (size_t i = 0; i < rows; ++i) {
    for (size_t j = 0; j < cols; ++j) {
      std::cout << myArray[i * cols + j] << ' ';
    }
    std::cout << '\n';
  }
}

void free2DArray(const float *myArray) {
  // Free the allocated memory
  delete[] myArray;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <data_prefix> <save_path>" << std::endl;
    return 1;
  }

  std::string file_prefix = argv[1];
  std::string save_path = argv[2];

  std::string x_train_path = file_prefix + "_X_train.csv";
  std::string y_train_path = file_prefix + "_y_train.csv";
  std::string x_val_path = file_prefix + "_X_val.csv";
  std::string y_val_path = file_prefix + "_y_val.csv";

  std::vector<std::vector<float>> x_train = mat_2d_from_file(x_train_path);
  std::vector<std::vector<float>> y_train = mat_2d_from_file(y_train_path);
  std::vector<std::vector<float>> x_val = mat_2d_from_file(x_val_path);
  std::vector<std::vector<float>> y_val = mat_2d_from_file(y_val_path);

  std::cout << "x_train size: ";
  print_shape(x_train);
  std::cout << "y_train size: ";
  print_shape(y_train);
  std::cout << "x_val size: ";
  print_shape(x_val);
  std::cout << "y_val size: ";
  print_shape(y_val);

  const float *x_train_arr = convertMatToArr(x_train);
  const float *y_train_arr = convertMatToArr(y_train);
  const float *x_val_arr = convertMatToArr(x_val);
  const float *y_val_arr = convertMatToArr(y_val);
  size_t train_rows = x_train.size();
  size_t val_rows = x_val.size();
  size_t cols = x_train[0].size();

  DMatrixHandle train_dmat, val_dmat;
  safe_xgboost(
      XGDMatrixCreateFromMat(x_train_arr, train_rows, cols, -1, &train_dmat));
  safe_xgboost(
      XGDMatrixSetFloatInfo(train_dmat, "label", y_train_arr, train_rows));
  safe_xgboost(
      XGDMatrixCreateFromMat(x_val_arr, val_rows, cols, -1, &val_dmat));
  safe_xgboost(XGDMatrixSetFloatInfo(val_dmat, "label", y_val_arr, val_rows));

  const int eval_dmats_size = 2;
  DMatrixHandle eval_dmats[eval_dmats_size] = {train_dmat, val_dmat};

  BoosterHandle booster;
  safe_xgboost(XGBoosterCreate(eval_dmats, eval_dmats_size, &booster));
  XGBoosterSetParam(booster, "booster", "gbtree");
  XGBoosterSetParam(booster, "objective", "reg:squarederror");
  XGBoosterSetParam(booster, "max_depth", "6");
  XGBoosterSetParam(booster, "lambda", "0");  // L2 reg
  XGBoosterSetParam(booster, "alpha", "0");  // L1 reg

  int num_of_iterations = 500;
  const char *eval_names[eval_dmats_size] = {"train", "val"};
  const char *eval_result = NULL;

  for (int i = 0; i < num_of_iterations; ++i) {
    // Update the model performance for each iteration
    safe_xgboost(XGBoosterUpdateOneIter(booster, i, train_dmat));
    // Give the statistics for the learner for training & testing dataset in
    // terms of error after each iteration
    safe_xgboost(XGBoosterEvalOneIter(booster, i, eval_dmats, eval_names,
                                      eval_dmats_size, &eval_result));
    printf("%s\n", eval_result);
  }

  std::cout << "Saving XGBoost json model..." << std::endl;
  safe_xgboost(XGBoosterSaveModel(booster, save_path.c_str()));
  std::cout << "XGBoost json model saved!" << std::endl;
  std::cout << "XGBoost json file: " << save_path << std::endl;

  free2DArray(x_train_arr);
  free2DArray(y_train_arr);
  free2DArray(x_val_arr);
  free2DArray(y_val_arr);

  return 0;
}
