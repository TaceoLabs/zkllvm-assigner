
//---------------------------------------------------------------------------//
// Copyright (c) 2022 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2022 Nikita Kaskov <nbering@nil.foundation>
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

#ifndef CRYPTO3_BLUEPRINT_COMPONENT_ONNX_RUNTIME_HPP
#define CRYPTO3_BLUEPRINT_COMPONENT_ONNX_RUNTIME_HPP

#include <set>

namespace nil {
    namespace blueprint {
        namespace onnx {
            template<typename BlueprintFieldType, typename ArithmetizationType>
                class runtime {

                    using var = crypto3::zk::snark::plonk_variable<typename BlueprintFieldType::value_type>;
                    using om_tensor_ptr_type = typename BlueprintFieldType::value_type;
                    public:
                        
                        assignment<ArithmetizationType>* assignmnt;
                        program_memory<var>* memory;
                        size_t* public_input_idx;

                        const std::set<llvm::StringRef> SUPPORTED_FUNCTIONS {"omTensorListGetOmtArray", "omTensorGetDataPtr", "omTensorGetStrides", "omTensorGetShape", "omTensorListGetSize", "omTensorGetDataType", "omTensorGetRank", "omTensorCreateUntyped", "omTensorSetDataPtr", "omTensorSetDataType", "omTensorListCreateWithOwnership"};
                        
                        var handle_om_tensor_list_get_omt_array(om_tensor_ptr_type& om_tensor_list_ptr) {
                            //get the pointer to tensor types
                            ptr_type number = (ptr_type)static_cast<typename BlueprintFieldType::integral_type>(om_tensor_list_ptr.data);
                            return memory->load(number);
                        }

                        var handle_om_tensor_get_data_ptr(om_tensor_ptr_type& om_tensor_ptr) {
                            ptr_type aligned_ptr = (ptr_type)static_cast<typename BlueprintFieldType::integral_type>(om_tensor_ptr.data) + 1;
                            
                            std::cout << "resolving *" << aligned_ptr << " leads to " << var_value(*assignmnt, memory->load(aligned_ptr)).data << std::endl;
                            std::cout << "there we have: " << std::endl << "[";
                            ptr_type test_ptr = (ptr_type)static_cast<typename BlueprintFieldType::integral_type>(var_value(*assignmnt, memory->load(aligned_ptr)).data);
                            for (unsigned i = 0;i<10;++i) {
                                std::cout << var_value(*assignmnt, memory->load(test_ptr++)).data << ", "; 
                            }
                            std::cout << "]" << std::endl;
                            return memory->load(aligned_ptr);
                        }

                        var handle_om_tensor_get_shape(om_tensor_ptr_type& om_tensor_ptr) {
                            ptr_type shape_ptr = (ptr_type)static_cast<typename BlueprintFieldType::integral_type>(om_tensor_ptr.data) + 3;
                            std::cout << "resolving *" << shape_ptr << " leads to " << var_value(*assignmnt, memory->load(shape_ptr)).data << std::endl;
                            std::cout << "there we have: " << std::endl << "[";
                            ptr_type test_ptr = (ptr_type)static_cast<typename BlueprintFieldType::integral_type>(var_value(*assignmnt, memory->load(shape_ptr)).data);
                            for (unsigned i = 0;i<2;++i) {
                                std::cout << var_value(*assignmnt, memory->load(test_ptr++)).data << ", "; 
                            }
                            std::cout << "]" << std::endl;
                            return memory->load(shape_ptr);
                        }

                        var handle_om_tensor_get_strides(om_tensor_ptr_type& om_tensor_ptr) {
                            ptr_type stride_ptr = (ptr_type)static_cast<typename BlueprintFieldType::integral_type>(om_tensor_ptr.data) + 4;
                            return memory->load(stride_ptr);
                        }

                        var handle_om_tensor_list_get_size(om_tensor_ptr_type& om_tensor_list_ptr) {
                            ptr_type size_ptr = (ptr_type)static_cast<typename BlueprintFieldType::integral_type>(om_tensor_list_ptr.data) + 1;
                            return memory->load(size_ptr);
                        }
                        var handle_om_tensor_get_data_type(om_tensor_ptr_type& om_tensor_ptr) {
                            ptr_type data_type_ptr = (ptr_type)static_cast<typename BlueprintFieldType::integral_type>(om_tensor_ptr.data) + 6;
                            return memory->load(data_type_ptr);
                        }

                        var handle_om_tensor_get_rank(om_tensor_ptr_type& om_tensor_ptr) {
                            ptr_type rank_ptr = (ptr_type)static_cast<typename BlueprintFieldType::integral_type>(om_tensor_ptr.data) + 5;
                            return memory->load(rank_ptr);
                        }

                        var handle_om_tensor_create_untyped(const llvm::CallInst* inst, stack_frame<var>& frame, om_tensor_ptr_type& om_tensor_ptr) {
                            var val_ptr;
                            ptr_type ptr = create_empty_om_tensor(val_ptr); 
                            memory->store(ptr + 5, frame.scalars[inst->getOperand(0)]);
                            return val_ptr;
                        }

                        void handle_om_tensor_set_data_ptr(const llvm::CallInst* inst, stack_frame<var>& frame, om_tensor_ptr_type& om_tensor_ptr) {
                            ptr_type ptr = (ptr_type)static_cast<typename BlueprintFieldType::integral_type>(om_tensor_ptr.data);
                            //get owning 
                            //get allocated_ptr
                            //get aligned_ptr
                            var owning = frame.scalars[inst->getOperand(1)];
                            var allocated_ptr = frame.scalars[inst->getOperand(2)];
                            var aligned_ptr = frame.scalars[inst->getOperand(3)];
                            if (!var_value(*assignmnt, aligned_ptr).data) {
                                aligned_ptr = allocated_ptr;
                            }
                            memory->store(ptr + 7, owning);
                            memory->store(ptr, allocated_ptr);
                            memory->store(ptr, aligned_ptr);
                        }

                        void handle_om_tensor_set_data_type(const llvm::CallInst* inst, stack_frame<var>& frame, om_tensor_ptr_type& om_tensor_ptr) {
                            ptr_type data_type_ptr = (ptr_type)static_cast<typename BlueprintFieldType::integral_type>(om_tensor_ptr.data) + 6;
                            //get data type 
                            memory->store(data_type_ptr, frame.scalars[inst->getOperand(1)]);
                            //FIXME for the time being that we find this later when we add
                            //ints and bool inputs
                            ASSERT(var_value(*assignmnt, frame.scalars[inst->getOperand(1)]).data == 1);
                        }

                        var handle_om_tensor_list_create_with_ownership(const llvm::CallInst* inst, stack_frame<var>& frame, om_tensor_ptr_type& om_tensor_ptr) {
                            var val_ptr;
                            ptr_type ptr = create_empty_om_tensor_list(val_ptr); 
                            //get _omts
                            //get size
                            //get owning
                            var _omts = frame.scalars[inst->getOperand(0)];
                            var size = frame.scalars[inst->getOperand(1)];
                            var owning = frame.scalars[inst->getOperand(2)];
                            std::cout << "I return " << ptr << std::endl;
                            std::cout << "I store " << var_value(*assignmnt, _omts).data << std::endl;
                            std::cout << "I store " << var_value(*assignmnt, size).data << std::endl;
                            std::cout << "I store " << var_value(*assignmnt, owning).data << std::endl;
                            memory->store(ptr++, _omts);
                            memory->store(ptr++, size);
                            memory->store(ptr, owning);
                            return val_ptr;
                        }

                        inline ptr_type create_empty_om_tensor(var& var_ptr) {
                            ptr_type ptr = memory->add_cells(std::vector<unsigned>(8, 1));
                            assignmnt->public_input(0,*public_input_idx) = ptr;
                            var_ptr = var(0, (*public_input_idx)++, false, var::column_type::public_input);
                            return ptr;
                        }

                        inline ptr_type create_empty_om_tensor_list(var& var_ptr) {
                            ptr_type ptr = memory->add_cells(std::vector<unsigned>(3, 1));
                            assignmnt->public_input(0,*public_input_idx) = ptr;
                            var_ptr = var(0, (*public_input_idx)++, false, var::column_type::public_input);
                            return ptr;
                        }

                    public:
                        //lets see if we need anything later
                        runtime(assignment<ArithmetizationType>* assignmnt, program_memory<var>* memory, size_t* public_input_idx) {
                            this->assignmnt = assignmnt;
                            this->memory = memory;
                            this->public_input_idx = public_input_idx;
                        };
                        ~runtime() = default;
                        bool is_supported(llvm::Function* fun) {
                            //check if we support the function AND
                            //the argument list matches the function call
                            llvm::StringRef fun_name = fun->getName();
                            if (SUPPORTED_FUNCTIONS.count(fun_name)) {
                                if (fun_name == "omTensorCreateUntyped") {
                                    return fun->arg_size() == 1
                                    && fun->getArg(0)->getType()->isIntegerTy();
                                } else if (fun_name == "omTensorSetDataPtr") {
                                    return  fun->arg_size() == 4
                                        && fun->getArg(0)->getType()->isPointerTy()
                                        && fun->getArg(1)->getType()->isIntegerTy()
                                        && fun->getArg(2)->getType()->isPointerTy()
                                        && fun->getArg(3)->getType()->isPointerTy();
                                } else if (fun_name == "omTensorSetDataType") {
                                    return  fun->arg_size() == 2
                                        && fun->getArg(0)->getType()->isPointerTy()
                                        && fun->getArg(1)->getType()->isIntegerTy();
                                } else if(fun_name == "omTensorListCreateWithOwnership") {
                                    return  fun->arg_size() == 3
                                        && fun->getArg(0)->getType()->isPointerTy()
                                        && fun->getArg(1)->getType()->isIntegerTy()
                                        && fun->getArg(2)->getType()->isIntegerTy();
                                } else {
                                    return  fun->arg_size() == 1
                                        && fun->getArg(0)->getType()->isPointerTy();
                                }
                            }
                            return false;
                        }

                        bool handle_func_call(const llvm::CallInst* inst, llvm::Function* fun, stack_frame<var>& frame, var& ret_val) {
                           llvm::StringRef fun_name = fun->getName();
                           auto ptr = var_value(*assignmnt, frame.scalars[inst->getOperand(0)]);
                           if (fun_name == "omTensorListGetOmtArray") {
                               ret_val = handle_om_tensor_list_get_omt_array(ptr);
                               return true;
                           } else if (fun_name == "omTensorGetDataPtr") {
                               ret_val = handle_om_tensor_get_data_ptr(ptr);
                               return true;
                           } else if (fun_name == "omTensorGetShape") {
                               ret_val = handle_om_tensor_get_shape(ptr);
                               return true;
                           } else if (fun_name == "omTensorGetStrides") {
                               ret_val = handle_om_tensor_get_strides(ptr);
                               return true;
                           } else if (fun_name == "omTensorListGetSize") {
                               ret_val = handle_om_tensor_list_get_size(ptr);
                               return true;
                           } else if (fun_name == "omTensorGetDataType") {
                               ret_val = handle_om_tensor_get_data_type(ptr);
                               return true;
                           } else if (fun_name == "omTensorGetRank") {
                               ret_val = handle_om_tensor_get_rank(ptr);
                               return true;
                           } else if (fun_name == "omTensorCreateUntyped") {
                               ret_val = handle_om_tensor_create_untyped(inst, frame, ptr);
                               return true;
                           } else if (fun_name == "omTensorSetDataPtr") {
                               handle_om_tensor_set_data_ptr(inst, frame, ptr);
                               return false;
                           } else if (fun_name == "omTensorSetDataType") {
                               handle_om_tensor_set_data_type(inst, frame, ptr);
                               return false;
                           } else if (fun_name == "omTensorListCreateWithOwnership") {
                               ret_val = handle_om_tensor_list_create_with_ownership(inst, frame, ptr);
                               return true;
                           }


                           llvm::errs() << "unsupported onnx runtime function: \"" << fun->getName() << "\"\n";
                           UNREACHABLE("");

                        }
                };


        }       // namespace onnx
    }     // namespace blueprint
}    // namespace nil

#endif // CRYPTO3_BLUEPRINT_COMPONENT_ONNX_RUNTIME_HPP
