/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd. All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LUCI_INTERPRETER_CORE_RUNTIMEGRAPH_H
#define LUCI_INTERPRETER_CORE_RUNTIMEGRAPH_H

#include "luci_interpreter/core/Tensor.h"
#include "luci_interpreter/MemoryManager.h"
#include "core/Kernel.h"

#include <memory>
#include <vector>

namespace luci_interpreter
{

class RuntimeModule;

class RuntimeGraph
{
public:
  explicit RuntimeGraph(RuntimeModule *owning_module, IMemoryManager *memory_manager);
  ~RuntimeGraph();

  Tensor *addTensor(std::unique_ptr<Tensor> &&tensor);
  AffineQuantization *addAffineQuantization(std::unique_ptr<AffineQuantization> &&quantization);

  void addInputTensor(Tensor *input_tensor);
  void addOutputTensor(Tensor *output_tensor);

  void setInputTensors(const std::vector<Tensor *> &input_tensors);
  void setOutputTensors(const std::vector<Tensor *> &output_tensors);

  void configureAllocations(Tensor *tensor);

  const std::vector<Tensor *> &getInputTensors() const { return _input_tensors; }
  const std::vector<Tensor *> &getOutputTensors() const { return _output_tensors; }

  void addKernel(std::unique_ptr<Kernel> &&kernel);

  void execute();
  void configure();

  void invalidate() { _is_valid = false; }
  bool isValid() const { return _is_valid; }

private:
  void buildAllocDeallocPlan();
  void allocate(size_t kernel_index) const;
  void deallocate(size_t kernel_index) const;

private:
  IMemoryManager *_memory_manager;
  RuntimeModule *_owning_module;
  std::vector<std::unique_ptr<Tensor>> _tensors;
  std::vector<std::unique_ptr<AffineQuantization>> _affine_quantizations;
  std::vector<Tensor *> _input_tensors;
  std::vector<Tensor *> _output_tensors;

  bool _is_valid = false;

  // Kernels in execution order.
  std::vector<std::unique_ptr<Kernel>> _kernels;
  // Tensors that are not used anymore after given op
  std::vector<std::vector<Tensor *>> _alloc_plan;
  std::vector<std::vector<Tensor *>> _dealloc_plan;
};

} // namespace luci_interpreter

#endif // LUCI_INTERPRETER_CORE_RUNTIMEGRAPH_H
