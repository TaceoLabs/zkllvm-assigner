#ifndef CRYPTO3_ASSIGNER_FIXEDPOINT_DIVISION_HPP
#define CRYPTO3_ASSIGNER_FIXEDPOINT_DIVISION_HPP

#include "llvm/IR/Type.h"
#include "llvm/IR/TypeFinder.h"
#include "llvm/IR/TypedPointerType.h"

#include <nil/crypto3/zk/snark/arithmetization/plonk/constraint_system.hpp>

#include <nil/blueprint/component.hpp>
#include <nil/blueprint/basic_non_native_policy.hpp>
#include <nil/blueprint/components/algebra/fixedpoint/plonk/div.hpp>

#include <nil/blueprint/asserts.hpp>
#include <nil/blueprint/stack.hpp>
#include <nil/blueprint/policy/policy_manager.hpp>

namespace nil {
    namespace blueprint {
        namespace detail {

            template<typename BlueprintFieldType, typename ArithmetizationParams>
            typename components::fix_div<
                crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>,
                BlueprintFieldType, basic_non_native_policy<BlueprintFieldType>>::result_type
                handle_fixedpoint_division_component(
                    llvm::Value *operand0, llvm::Value *operand1,
                    std::map<const llvm::Value *,
                             crypto3::zk::snark::plonk_variable<typename BlueprintFieldType::value_type>> &variables,
                    circuit<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>> &bp,
                    assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>>
                        &assignment,
                    std::uint32_t start_row) {

                using var = crypto3::zk::snark::plonk_variable<typename BlueprintFieldType::value_type>;

                using component_type = components::fix_div<
                    crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>,
                    BlueprintFieldType, basic_non_native_policy<BlueprintFieldType>>;
                const auto p = PolicyManager::get_parameters(
                    ManifestReader<component_type, ArithmetizationParams,1,1>::get_witness(0,1,1));
                component_type component_instance(
                    p.witness, ManifestReader<component_type, ArithmetizationParams,1,1>::get_constants(),
                    ManifestReader<component_type, ArithmetizationParams,1,1>::get_public_inputs(), 1, 1);

                // TACEO_TODO in the previous line I hardcoded 1 for now!!! CHANGE THAT
                // TACEO_TODO make an assert that both have the same scale?
                // TACEO_TODO we probably have to extract the field element from the type here

                var x = variables[operand0];
                var y = variables[operand1];

                components::generate_circuit(component_instance, bp, assignment, {x, y}, start_row);
                return components::generate_assignments(component_instance, assignment, {x, y}, start_row);
            }

        }    // namespace detail
        template<typename BlueprintFieldType, typename ArithmetizationParams>
        void handle_fixedpoint_division_component(
            const llvm::Instruction *inst,
            stack_frame<crypto3::zk::snark::plonk_variable<typename BlueprintFieldType::value_type>> &frame,
            circuit<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>> &bp,
            assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>>
                &assignment,
            std::uint32_t start_row) {
                llvm::Value *operand0 = inst->getOperand(0);
                llvm::Value *operand1 = inst->getOperand(1);

                llvm::Type *op0_type = operand0->getType();
                llvm::Type *op1_type = operand1->getType();
                ASSERT(llvm::isa<llvm::ZkFixedPointType>(op0_type) &&
                       llvm::isa<llvm::ZkFixedPointType>(op1_type));
                frame.scalars[inst] = detail::handle_fixedpoint_division_component<BlueprintFieldType, ArithmetizationParams>(operand0, operand1, frame.scalars, bp, assignment, start_row).output;

                //TACEO_TODO check Scale size here in LLVM???
               //ASSERT(llvm::cast<llvm::GaloisFieldType>(op0_type)->getFieldKind() ==
               //       llvm::cast<llvm::GaloisFieldType>(op1_type)->getFieldKind());
        }
    }        // namespace blueprint
}    // namespace nil

#endif    // CRYPTO3_ASSIGNER_FIXEDPOINT_DIVISION_HPP