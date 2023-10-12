#ifndef CRYPTO3_ASSIGNER_FIXEDPOINT_MULTIPLICATION_RESCALE_HPP
#define CRYPTO3_ASSIGNER_FIXEDPOINT_MULTIPLICATION_RESCALE_HPP

#include "llvm/IR/Type.h"
#include "llvm/IR/TypeFinder.h"
#include "llvm/IR/TypedPointerType.h"

#include <nil/crypto3/zk/snark/arithmetization/plonk/constraint_system.hpp>

#include <nil/blueprint/component.hpp>
#include <nil/blueprint/basic_non_native_policy.hpp>
#include <nil/blueprint/components/algebra/fixedpoint/plonk/mul_rescale.hpp>

#include <nil/blueprint/asserts.hpp>
#include <nil/blueprint/stack.hpp>
#include <nil/blueprint/policy/policy_manager.hpp>

namespace nil {
    namespace blueprint {
        namespace detail {

            template<typename BlueprintFieldType, typename ArithmetizationParams>
            typename components::mul_rescale<
                crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>,
                BlueprintFieldType, basic_non_native_policy<BlueprintFieldType>>::result_type
                handle_fixedpoint_mul_rescale_component(
                    llvm::Value *operand0, llvm::Value *operand1,
                    std::map<const llvm::Value *,
                             crypto3::zk::snark::plonk_variable<typename BlueprintFieldType::value_type>> &variables,
                    circuit<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>> &bp,
                    assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>>
                        &assignment,
                    std::uint32_t start_row) {

                using var = crypto3::zk::snark::plonk_variable<typename BlueprintFieldType::value_type>;

                using component_type = components::mul_rescale<
                    crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>,
                    BlueprintFieldType, basic_non_native_policy<BlueprintFieldType>>;
                const auto p = PolicyManager::get_parameters(
                    ManifestReader<component_type, ArithmetizationParams>::get_witness(0));
                component_type component_instance(
                    p.witness, ManifestReader<component_type, ArithmetizationParams>::get_constants(),
                    ManifestReader<component_type, ArithmetizationParams>::get_public_inputs(), 1);

                // TACEO_TODO in the previous line I hardcoded 1 for now!!! CHANGE THAT
                // TACEO_TODO make an assert that both have the same scale?
                // TACEO_TODO we probably have to extract the field element from the type here

                var x = variables[operand0];
                var y = variables[operand1];

                components::generate_circuit(component_instance, bp, assignment, {x, y}, start_row);
                return components::generate_assignments(component_instance, assignment, {x, y}, start_row);
            }
        }    // namespace detail
    }        // namespace blueprint
}    // namespace nil

#endif    // CRYPTO3_ASSIGNER_FIXEDPOINT_MULTIPLICATION_RESCALE_HPP
