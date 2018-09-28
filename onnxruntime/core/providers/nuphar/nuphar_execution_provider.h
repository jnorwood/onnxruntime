// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <tvm/build_module.h>

#include "nuphar_allocator.h"
#include "core/framework/allocatormgr.h"
#include "core/framework/execution_provider.h"
#include "core/framework/kernel_registry.h"
#include "core/framework/op_kernel.h"
#include "core/providers/provider_factories.h"

namespace onnxruntime {

// Information needed to construct Nuphar execution providers.
struct NupharExecutionProviderInfo {
  std::string name;
  int device_id{0};
  // By default, construct "stackvm" TVM target, for which the default device_type is kDLCPU.
  std::string target_str{"stackvm"};

  explicit NupharExecutionProviderInfo(const std::string& provider_name,
                                    int dev_id = 0,
                                    const std::string& tgt_str = "stackvm")
      : name(provider_name), device_id(dev_id), target_str(tgt_str) {}
  NupharExecutionProviderInfo() = default;
};

class NupharExecutionProvider : public IExecutionProvider {
 public:
  explicit NupharExecutionProvider(const NupharExecutionProviderInfo& info) {
    tvm_target_ = tvm::Target::create(info.target_str);
    tvm_ctx_.device_type = static_cast<DLDeviceType>(tvm_target_->device_type);
    tvm_ctx_.device_id = info.device_id;

    DeviceAllocatorRegistrationInfo allocator_info(
      {kMemTypeDefault,
       [this](int /*id*/) { return std::make_unique<NupharAllocator>(this->tvm_ctx_); },
       std::numeric_limits<size_t>::max()});
    InsertAllocator(kMemTypeDefault, CreateAllocator(allocator_info, tvm_ctx_.device_id));
  }

  virtual ~NupharExecutionProvider();

  std::string Type() const override {
    return kNupharExecutionProvider;
  }

  Status CopyTensor(const Tensor& src, Tensor& dst) const override;

  virtual const void* GetExecutionHandle() const noexcept override {
    // The Nuphar interface does not return anything interesting.
    return nullptr;
  }

  virtual std::shared_ptr<KernelRegistry> GetKernelRegistry() const override;

 private:
  tvm::Target tvm_target_;

  TVMContext tvm_ctx_;
};

}  // namespace onnxruntime
