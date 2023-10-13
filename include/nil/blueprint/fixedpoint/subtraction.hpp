
#ifndef CRYPTO3_ASSIGNER_FIXEDPOINT_SUBTRACTION_HPP
#define CRYPTO3_ASSIGNER_FIXEDPOINT_SUBTRACTION_HPP

#include "llvm/IR/Type.h"
#include "llvm/IR/TypeFinder.h"
#include "llvm/IR/TypedPointerType.h"

#include <nil/crypto3/zk/snark/arithmetization/plonk/constraint_system.hpp>

#include <nil/blueprint/basic_non_native_policy.hpp>
#include <nil/blueprint/fields/subtraction.hpp>

#include <nil/blueprint/asserts.hpp>
#include <nil/blueprint/stack.hpp>

namespace nil {
    namespace blueprint {

        template<typename BlueprintFieldType, typename ArithmetizationParams>
        void handle_fixedpoint_subtraction_component(
            const llvm::Instruction *inst,
            stack_frame<crypto3::zk::snark::plonk_variable<typename BlueprintFieldType::value_type>> &frame,
            circuit<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>> &bp,
            assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>>
                &assignment,
            std::uint32_t start_row) {

            using non_native_policy_type = basic_non_native_policy<BlueprintFieldType>;

            llvm::Value *operand0 = inst->getOperand(0);
            llvm::Value *operand1 = inst->getOperand(1);

            // TACEO_TODO make an assert that both have the same scale?
            // TACEO_TODO we probably have to extract the field element from the type here

            frame.scalars[inst] =
                detail::handle_native_field_subtraction_component<BlueprintFieldType, ArithmetizationParams>(
                    operand0, operand1, frame.scalars, bp, assignment, start_row)
                    .output;
        }
    }    // namespace blueprint
}    // namespace nil

#endif    // CRYPTO3_ASSIGNER_FIXEDPOINT_SUBTRACTION_HPP
