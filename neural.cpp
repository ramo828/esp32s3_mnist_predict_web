#include "neural.h"  // NeuralNet sinifinin header faylı

// TensorFlow Lite Micro kitabxanalarının başlıqları
// Bu kitabxanalar mikrokontrollerlərdə (ESP32 kimi) model işlətmək üçün istifadə olunur
#include <tensorflow/lite/micro/micro_interpreter.h>
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/system_setup.h"

// Model məlumatları (model.h faylı içində binary formatda saxlanılır)
// Bu fayl Python-da train edilən modelin C array formatına çevrilmiş versiyasıdır
#include "model.h"

// TensorFlow Lite modelini yaddaşdan oxuyuruq
// g_model dəyişəni model.h faylında saxlanılır
const tflite::Model *model = tflite::GetModel(g_model);

// Modeldə istifadə olunacaq operatorları qeyd edirik
// Bizim modeldə sadəcə 3 əməliyyat var:
// 1) Reshape
// 2) FullyConnected
// 3) Softmax
static tflite::MicroMutableOpResolver<3> resolver;

// Tensor arena
// Bu massiv TensorFlow Lite Micro üçün RAM yaddaşı rolunu oynayır
// Modelin input, output və ara hesablamaları burada saxlanılır
static uint8_t tensor_arena[TENSOR_ARENA_SIZE];

// Interpreter obyektini yaradırıq
// Bu obyekt modelin işləməsinə cavabdeh əsas komponentdir
static tflite::MicroInterpreter interpreter(
  model,         // model pointeri
  resolver,      // istifadə olunan operatorlar
  tensor_arena,  // yaddaş sahəsi
  TENSOR_ARENA_SIZE);

// NeuralNet sinifinin konstruktoru
// Bu funksiya obyekt yaradılarkən avtomatik işləyir
NeuralNet::NeuralNet() {

  // Model versiyasını yoxlayırıq
  // Əgər TensorFlow Lite schema versiyası uyğun gəlmirsə model işləməyəcək
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    MicroPrintf("Model versiyasi uygun deyil: %d", model->version());
    return;
  }

  // Modeldə istifadə olunan operatorları interpreterə əlavə edirik

  // Reshape operatoru əlavə edilir
  if (resolver.AddReshape() != kTfLiteOk) {
    MicroPrintf("Reshape operatoru elave edile bilmedi!");
    return;
  }

  // FullyConnected layer (dense layer)
  if (resolver.AddFullyConnected() != kTfLiteOk) {
    MicroPrintf("FullyConnected operatoru elave edile bilmedi!");
    return;
  }

  // Softmax layer (çıxış ehtimallarını hesablayır)
  if (resolver.AddSoftmax() != kTfLiteOk) {
    MicroPrintf("Softmax operatoru elave edile bilmedi!");
    return;
  }

  // TensorFlow Lite modelinin istifadə edəcəyi bütün tensörləri RAM-da ayırırıq
  if (interpreter.AllocateTensors() != kTfLiteOk) {
    MicroPrintf("Tensorlerin ayrilmasi ugursuz oldu!");
    return;
  }
}

// Bu funksiya modelə input verib nəticəni qaytarır
// data[] -> 28x28 = 784 float dəyər
int NeuralNet::getData(float data[]) {

  // Modelin input və output tensorlarını alırıq
  TfLiteTensor *input = interpreter.input(0);
  TfLiteTensor *output = interpreter.output(0);

  // Input tensorun ölçüsünü hesablayırıq
  // Batch ölçüsünü nəzərə almırıq
  int input_size = 1;

  for (int i = 1; i < input->dims->size; i++) {
    input_size *= input->dims->data[i];
  }

  // Model float istifadə etdiyi üçün quantization yoxdur
  // Gələn məlumatları birbaşa input tensoruna yazırıq
  for (int i = 0; i < input_size; i++) {
    input->data.f[i] = data[i];
  }

  // Modeli işə salırıq (inference)
  if (interpreter.Invoke() != kTfLiteOk) {
    MicroPrintf("Model isledile bilmedi!");
    return -1;
  }

  // Output tensorun ölçüsünü hesablayırıq
  int output_size = 1;

  for (int i = 1; i < output->dims->size; i++) {
    output_size *= output->dims->data[i];
  }

  // Modelin çıxışında 10 ehtimal olacaq (0-9 rəqəmləri)
  // Ən böyük ehtimalı tapırıq

  int best_index = 0;
  float max_score = output->data.f[0];

  for (int i = 1; i < output_size; i++) {

    if (output->data.f[i] > max_score) {

      max_score = output->data.f[i];
      best_index = i;
    }
  }

  // Serial monitorda ehtimalı göstəririk
  printf("Təxmin ehtimal;: %f\n", max_score);

  // Ən böyük ehtimala sahib rəqəmi qaytarırıq
  return best_index;
}