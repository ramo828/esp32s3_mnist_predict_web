#include "neural.h"
#include "predict.h"

#ifdef __cplusplus
extern "C" {
  int predict(float data[]) {
    NeuralNet net = NeuralNet();
    int result = net.getData(data);
    return result;
  }
}
#endif