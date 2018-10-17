// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <vector>
#include "core/session/inference_session.h"
#include "core/session/onnxruntime_c_api.h"

struct ONNXRuntimeSessionOptions {
  onnxruntime::SessionOptions value;
  std::vector<std::string> custom_op_paths;
  std::vector<ONNXRuntimeProviderFactoryPtr*> provider_factories;
  ONNXRuntimeSessionOptions() = default;
  ~ONNXRuntimeSessionOptions();
  ONNXRuntimeSessionOptions(const ONNXRuntimeSessionOptions& other);
  ONNXRuntimeSessionOptions& operator=(const ONNXRuntimeSessionOptions& other);
};