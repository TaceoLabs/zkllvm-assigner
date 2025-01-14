//---------------------------------------------------------------------------//
// Copyright (c) 2023 Alexey Kokoshnikov <alexeikokoshnikov@nil.foundation>
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

#ifndef CRYPTO3_ASSIGNER_COMPONENT_MANIFEST_UTILITIES_HPP
#define CRYPTO3_ASSIGNER_COMPONENT_MANIFEST_UTILITIES_HPP

#include <vector>
#include <array>
#include <limits>

#include <nil/blueprint/manifest.hpp>

namespace nil {
    namespace blueprint {
        namespace detail {

            struct FlexibleParameters {
                std::vector <std::uint32_t> witness;

                FlexibleParameters(std::uint32_t witness_amount) {
                    witness.resize(witness_amount);
                    std::iota(witness.begin(), witness.end(), 0); // fill 0, 1, ...
                }
            };

            template<typename ArithmetizationParams>
            struct CompilerRestrictions {
                inline static compiler_manifest common_restriction_manifest = compiler_manifest(ArithmetizationParams::witness_columns,
                                                                                                std::numeric_limits<std::int32_t>::max() - 1,
                                                                                                std::numeric_limits<std::int32_t>::max(), true);
            };

            template<typename ComponentType, typename ArithmetizationParams>
            struct ManifestReader {
                inline static typename ComponentType::manifest_type manifest =
                        CompilerRestrictions<ArithmetizationParams>::common_restriction_manifest.intersect(ComponentType::get_manifest());

                template<typename... Args>
                static std::vector <std::pair<std::uint32_t, std::uint32_t>>
                get_witness(Args... args) {
                    ASSERT(manifest.is_satisfiable());
                    auto witness_amount_ptr = manifest.witness_amount;
                    std::vector <std::pair<std::uint32_t, std::uint32_t>> values;
                    for (auto it = witness_amount_ptr->begin();
                         it != witness_amount_ptr->end(); it++) {
                        const auto witness_amount = *it;
                        const auto rows_amount = ComponentType::get_rows_amount(witness_amount,
                                                                                args...);
                        const auto total_amount_rows_power_two = std::pow(2, std::ceil(std::log2(rows_amount)));
                        const auto total_amount_of_gates = ComponentType::get_gate_manifest(witness_amount, args...).get_gates_amount();
                        values.emplace_back(witness_amount,
                                            total_amount_rows_power_two + total_amount_of_gates);
                    }
                    ASSERT(values.size() > 0);
                    return values;
                }

                static typename ComponentType::component_type::constant_container_type
                get_constants() {
                    typename ComponentType::component_type::constant_container_type constants;
                    std::iota(constants.begin(), constants.end(), 0); // fill 0, 1, ...
                    return constants;
                }

                static typename ComponentType::component_type::public_input_container_type
                get_public_inputs() {
                    typename ComponentType::component_type::public_input_container_type public_inputs;
                    std::iota(public_inputs.begin(), public_inputs.end(), 0); // fill 0, 1, ...
                    return public_inputs;
                }
            };
        }    // namespace detail
    }    // namespace blueprint
}    // namespace nil

#endif    // CRYPTO3_ASSIGNER_COMPONENT_MANIFEST_UTILITIES_HPP