#ifndef __NEURAL_H__
#define __NEURAL_H__
#ifdef __cplusplus
extern "C" {
#include <stdint.h>
  class NeuralNet {

#define TENSOR_ARENA_SIZE (10 * 1024)
    static constexpr char *TAG = "TFLM_EXAMPLE";

  public:
    NeuralNet();
    int getData(float data[]);

  private:
  };
}
#endif
#endif