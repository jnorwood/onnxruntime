﻿#pragma once

#include "core/common/common.h"
#include "core/framework/op_kernel.h"
#include "core/providers/cuda/cuda_common.h"
#include "gsl/gsl_util"
#include "core/providers/cpu/tensor/reshape_helper.h"

namespace Lotus {
namespace Cuda {

template <typename T>
class Reshape final : public CudaKernel {
 public:
  Reshape(const OpKernelInfo& info) : CudaKernel(info) {
  }

  Status Compute(OpKernelContext* context) const override {
    // Copy the second input tensor into the shape vector
    const Tensor* shapeTensor = context->Input<Tensor>(1);
    LOTUS_ENFORCE(shapeTensor->Shape().NumDimensions() == 1,
                  "A shape tensor must be a vector tensor.");
    size_t nDims = static_cast<size_t>(shapeTensor->Shape()[0]);
    const int64_t* data = shapeTensor->Data<int64_t>();
    std::vector<int64_t> shape;
    for (size_t i = 0; i < nDims; ++i)
      shape.push_back(data[i]);

    const Tensor* X = context->Input<Tensor>(0);
    const TensorShape& X_shape = X->Shape();

    ReshapeHelper helper(X_shape, shape);

    Tensor* Y = context->Output(0, TensorShape(shape));
    const T* source = X->Data<T>();
    T* target = Y->MutableData<T>();
    //If source and target pointers are not equal (non-inplace operation), we need to copy the data.
    if (target != source) {
      provider_->CopyTensor(*X, *Y);
    }

    return Status::OK();
  }
};

template <typename T>
class Reshape_1 final : public CudaKernel {
 public:
  Reshape_1(const OpKernelInfo& info) : CudaKernel(info) {
    Status status = info.GetAttrs<int64_t>("shape", shape_);
    LOTUS_ENFORCE(status.IsOK(), "Attribute shape is not set.");
  }

  Status Compute(OpKernelContext* context) const override {
    std::vector<int64_t> shape = shape_;
    const Tensor* X = context->Input<Tensor>(0);
    const TensorShape& X_shape = X->Shape();

    ReshapeHelper helper(X_shape, shape);

    Tensor* Y = context->Output(0, TensorShape(shape));
    const T* source = X->Data<T>();
    T* target = Y->MutableData<T>();
    //If source and target pointers are not equal (non-inplace operation), we need to copy the data.
    if (target != source) {
      provider_->CopyTensor(*X, *Y);
    }

    return Status::OK();
  }

 private:
  std::vector<int64_t> shape_;
};

}  // namespace Cuda
}  //namespace Lotus