//===- LinalgTransforms.cpp - Linalg transformations as patterns ----------===//
//
// Copyright 2019 The MLIR Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =============================================================================
//
// This file implements logic for transforming Linalg operations.
//
//===----------------------------------------------------------------------===//

#include "mlir/Dialect/Linalg/IR/LinalgOps.h"
#include "mlir/Dialect/Linalg/Passes.h"
#include "mlir/Dialect/Linalg/Utils/Utils.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"

using namespace mlir;
using mlir::linalg::LinalgOp;

// Marker used as attribute name in generated Linalg rewriting transformations.
static constexpr auto kLinalgTransformMarker = "__internal_linalg_transform__";

namespace mlir {
namespace linalg {
namespace {
#include "mlir/Dialect/Linalg/Transforms/LinalgTransformPatterns.h.inc"
} // end namespace
} // end namespace linalg
} // end namespace mlir

namespace {
struct LinalgTransforms : public FunctionPass<LinalgTransforms> {
  void runOnFunction() override;
};
} // end anonymous namespace

/// Apply transformations specified as patterns.
void LinalgTransforms::runOnFunction() {
  OwningRewritePatternList patterns;
  auto funcOp = getFunction();

  // Add the generated patterns to the list.
  linalg::populateWithGenerated(&getContext(), &patterns);
  applyPatternsGreedily(funcOp, patterns);

  // Drop the marker.
  funcOp.walk([](LinalgOp op) { op.removeAttr(kLinalgTransformMarker); });
}

std::unique_ptr<OpPassBase<FuncOp>> mlir::linalg::createLinalgTransformsPass() {
  return std::make_unique<LinalgTransforms>();
}

static PassRegistration<LinalgTransforms>
    pass("test-linalg-transform-patterns",
         "Test Linalg transformation patterns by applying them greedily.");
