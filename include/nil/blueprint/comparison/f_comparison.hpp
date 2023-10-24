
//---------------------------------------------------------------------------//
// Copyright (c) 2023 Nikita Kaskov <nbering@nil.foundation>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_ASSIGNER_F_COMPARISON_HPP
#define CRYPTO3_ASSIGNER_F_COMPARISON_HPP

#include "llvm/IR/Type.h"
#include "llvm/IR/TypeFinder.h"
#include "llvm/IR/TypedPointerType.h"

#include <nil/crypto3/zk/snark/arithmetization/plonk/constraint_system.hpp>

#include <nil/blueprint/components/algebra/fixedpoint/plonk/cmp.hpp>

#include <nil/blueprint/component.hpp>
#include <nil/blueprint/basic_non_native_policy.hpp>
#include <nil/blueprint/policy/policy_manager.hpp>

#include <nil/blueprint/asserts.hpp>
#include <nil/blueprint/stack.hpp>

namespace nil {
    namespace blueprint {
        using namespace detail;

        template<typename BlueprintFieldType, typename ArithmetizationParams>
        typename crypto3::zk::snark::plonk_variable<typename BlueprintFieldType::value_type>
        handle_f_comparison_component(
            llvm::FCmpInst::Predicate p,
            const typename crypto3::zk::snark::plonk_variable<typename BlueprintFieldType::value_type> &x,
            const typename crypto3::zk::snark::plonk_variable<typename BlueprintFieldType::value_type> &y,
            std::size_t Bitness,
            circuit<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>> &bp,
            assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>>
            &assignment,
            std::uint32_t start_row,
            std::size_t &public_input_idx) {
                using non_native_policy_type = basic_non_native_policy<BlueprintFieldType>;
                using var = crypto3::zk::snark::plonk_variable<typename BlueprintFieldType::value_type>;
                using component_type = components::fix_cmp<
                    crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>,
                    BlueprintFieldType, basic_non_native_policy<BlueprintFieldType>>;
                const auto params = PolicyManager::get_parameters(ManifestReader<component_type, ArithmetizationParams,1,1>::get_witness(0));
                component_type component_instance(params.witness, ManifestReader<component_type, ArithmetizationParams,1,1>::get_constants(), ManifestReader<component_type, ArithmetizationParams,1,1>::get_public_inputs(), 1, 1);

                 components::generate_circuit(component_instance, bp, assignment, {x, y}, start_row);
                 auto cmp_result = components::generate_assignments(component_instance, assignment, {x, y}, start_row);

                switch (p) {
                    case llvm::FCmpInst::FCMP_UGT:	
                    case llvm::FCmpInst::FCMP_OGT: {
                        return cmp_result.gt;
                    }
                    case llvm::FCmpInst::FCMP_OLT:	
                    case llvm::FCmpInst::FCMP_ULT: {
                        return cmp_result.lt;
                    }
                    case llvm::FCmpInst::FCMP_OGE:	
                    case llvm::FCmpInst::FCMP_UGE:	{
                        //TACEO_TODO what is correct way to handle that?
                        if (var_value(assignment, cmp_result.gt).data == 1) {
                            return cmp_result.gt;
                        } else {
                            return cmp_result.eq;
                        }
                    }
                    case llvm::FCmpInst::FCMP_OLE:	
                    case llvm::FCmpInst::FCMP_ULE: {
                        UNREACHABLE("TACEO_TODO implement fcmp <");
                        break;
                    }
                    case llvm::FCmpInst::FCMP_ONE:	
                    case llvm::FCmpInst::FCMP_UNE: {
                        UNREACHABLE("TACEO_TODO implement fcmp !=");
                        break;
                    }
                    case llvm::FCmpInst::FCMP_OEQ:	
                    case llvm::FCmpInst::FCMP_UEQ: {
                        return cmp_result.eq;
                    }
                    case llvm::FCmpInst::FCMP_UNO:	
                    case llvm::FCmpInst::FCMP_ORD:	
                    case llvm::FCmpInst::FCMP_FALSE:
                    case llvm::FCmpInst::FCMP_TRUE: {
                        UNREACHABLE("TACEO_TODO implement fcmp");
                        break;
                    }
                    default:
                        UNREACHABLE("Unsupported fcmp predicate");
                        break;
                }
        }
    }    // namespace blueprint
}    // namespace nil
#endif // CRYPTO3_ASSIGNER_F_COMPARISON_HPP
