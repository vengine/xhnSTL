/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_NEURAL_NETWORK_HPP
#define XHN_NEURAL_NETWORK_HPP

#ifdef __cplusplus
#include "common.h"
#include "etypes.h"
#include "xhn_vector.hpp"

namespace xhn {
    
template <typename VALUE_TYPE, typename OPERATER0, typename OPERATER1>
class operator_base
{
public:
    euint m_op_code;
    OPERATER0 m_operator0;
    OPERATER1 m_operator1;
    operator_base(euint op_code)
    : m_op_code(op_code)
    {}
    VALUE_TYPE get_learning_rate()
    {
        if (0 == m_op_code) {
            return m_operator0.learning_rate();
        } else {
            return m_operator1.learning_rate();
        }
    }
    VALUE_TYPE get_biasing_rate()
    {
        if (0 == m_op_code) {
            return m_operator0.biasing_rate();
        } else {
            return m_operator1.biasing_rate();
        }
    }
    VALUE_TYPE activate(VALUE_TYPE x) {
        if (0 == m_op_code) {
            return m_operator0.activate(x);
        } else {
            return m_operator1.activate(x);
        }
    }
    VALUE_TYPE eval_derivative(VALUE_TYPE x) {
        if (0 == m_op_code) {
            return m_operator0.eval_derivative(x);
        } else {
            return m_operator1.eval_derivative(x);
        }
    }
};
    
enum connection_method
{
    InitialConnection,
    ConvolutionConnection,
    FullConnection,
    DirectConnection,
};

enum init_method
{
    InitAsZero,
    InitAsOne,
    InitAsRandomHalfToOne,
    InitAsRandomZeroToHalf,
};

template <typename VALUE_TYPE, typename OPERATER0, typename OPERATER1>
class neural_node : public operator_base<VALUE_TYPE, OPERATER0, OPERATER1>
{
private:
    struct inputted_neural_node
    {
        neural_node* node = nullptr;
        VALUE_TYPE weight = static_cast<VALUE_TYPE>(0.0);
        inputted_neural_node(neural_node* n, VALUE_TYPE w)
        : node(n)
        , weight(w)
        {}
    };
    struct outputted_neural_node
    {
        neural_node* node = nullptr;
        euint index;
        outputted_neural_node(neural_node* n, euint i)
        : node(n)
        , index(i)
        {}
    };
private:
    VALUE_TYPE m_ETotal_Out = static_cast<VALUE_TYPE>(0.0);
    VALUE_TYPE m_Out_Net = static_cast<VALUE_TYPE>(0.0);
private:
    vector<inputted_neural_node> m_inputted_nodes;
    vector<outputted_neural_node> m_outputted_nodes;
    VALUE_TYPE m_bias = static_cast<VALUE_TYPE>(0.0);
    VALUE_TYPE m_total_inputted_values = static_cast<VALUE_TYPE>(0.0);
    VALUE_TYPE m_outputted_value = static_cast<VALUE_TYPE>(0.0);
public:
    neural_node()
    : operator_base<VALUE_TYPE, OPERATER0, OPERATER1>(0)
    {}
    neural_node(euint op_code)
    : operator_base<VALUE_TYPE, OPERATER0, OPERATER1>(op_code)
    {}
    void descend()
    {
        if (m_inputted_nodes.empty())
            return;
        VALUE_TYPE lr = operator_base<VALUE_TYPE, OPERATER0, OPERATER1>::get_learning_rate();
        VALUE_TYPE br = operator_base<VALUE_TYPE, OPERATER0, OPERATER1>::get_biasing_rate();
        VALUE_TYPE ETotal_Out = static_cast<VALUE_TYPE>(0.0);
        for (auto& outputted_node : m_outputted_nodes) {
            ETotal_Out +=
            outputted_node.node->m_ETotal_Out *
            outputted_node.node->m_Out_Net *
            outputted_node.node->m_inputted_nodes[outputted_node.index].weight;
        }
        VALUE_TYPE Out_Net = operator_base<VALUE_TYPE, OPERATER0, OPERATER1>::eval_derivative(m_outputted_value);
        m_ETotal_Out = ETotal_Out;
        m_Out_Net = Out_Net;
        for (auto& inputted_node : m_inputted_nodes) {
            inputted_node.node->descend();
        }
        for (auto& inputted_node : m_inputted_nodes) {
            VALUE_TYPE ETotal_Wx =
            ETotal_Out * Out_Net * inputted_node.node->m_outputted_value;
            inputted_node.weight = inputted_node.weight - lr * ETotal_Wx;
        }
        VALUE_TYPE ETotal_Bx =
        ETotal_Out * Out_Net;
        m_bias = m_bias - br * ETotal_Bx;
    }
    float forward_propagate()
    {
        if (m_inputted_nodes.empty())
            return m_outputted_value;
        VALUE_TYPE sum = static_cast<VALUE_TYPE>(0.0);
        for (inputted_neural_node& node : m_inputted_nodes) {
            sum += node.node->forward_propagate() * node.weight;
        }
        m_total_inputted_values = sum;
        m_outputted_value = operator_base<VALUE_TYPE, OPERATER0, OPERATER1>::activate(sum + m_bias);
        return m_outputted_value;
    }
    void add_inputted_node(neural_node& node, init_method weight_init_method) {
        VALUE_TYPE weight = static_cast<VALUE_TYPE>(0.0);
        switch (weight_init_method)
        {
            case InitAsZero:
                weight = static_cast<VALUE_TYPE>(0.0);
                break;
            case InitAsOne:
                weight = static_cast<VALUE_TYPE>(1.0);
                break;
            case InitAsRandomHalfToOne:
                weight = static_cast<VALUE_TYPE>(rand()) /
                         static_cast<VALUE_TYPE>(RAND_MAX) *
                         static_cast<VALUE_TYPE>(0.5) +
                         static_cast<VALUE_TYPE>(0.5);
                break;
            case InitAsRandomZeroToHalf:
                weight = static_cast<VALUE_TYPE>(rand()) /
                static_cast<VALUE_TYPE>(RAND_MAX) *
                static_cast<VALUE_TYPE>(0.5);
                break;
        }
        m_inputted_nodes.push_back(inputted_neural_node(&node, weight_init_method));
        node.m_outputted_nodes.push_back(outputted_neural_node(this, m_inputted_nodes.size() - 1));
    }
    vector<inputted_neural_node>& get_inputted_nodes() {
        return m_inputted_nodes;
    }
    void set_outputted_value(VALUE_TYPE value) {
        m_outputted_value = value;
    }
    VALUE_TYPE get_outputted_value() const {
        return m_outputted_value;
    }
    void set_bias(VALUE_TYPE bias) {
        m_bias = bias;
    }
    VALUE_TYPE get_bias() const {
        return m_bias;
    }
    void set_ETotal_Out(VALUE_TYPE ETotal_Out) {
        m_ETotal_Out = ETotal_Out;
    }
    VALUE_TYPE get_ETotal_Out() const {
        return m_ETotal_Out;
    }
    void set_Out_Net(VALUE_TYPE Out_Net) {
        m_Out_Net = Out_Net;
    }
    VALUE_TYPE get_Out_Net() const {
        return m_Out_Net;
    }
};
    
template <typename VALUE_TYPE, unsigned int DIMENSION, typename OPERATER0, typename OPERATER1>
class neural_node_layer : public MemObject, public operator_base<VALUE_TYPE, OPERATER0, OPERATER1>
{
private:
    vector< neural_node<VALUE_TYPE, OPERATER0, OPERATER1> > m_nodes;
    euint m_sizes[DIMENSION] = {0};
public:
    neural_node_layer(euint op_code)
    : operator_base<VALUE_TYPE, OPERATER0, OPERATER1>(op_code)
    {}
    void initialize_layer(euint size)
    {
        EDebugAssert(DIMENSION == 1, "different dimension!");
        m_sizes[0] = size;
        m_nodes.clear();
        m_nodes.resize(size);
    }
    void initialize_layer(euint width,
                          euint height)
    {
        EDebugAssert(DIMENSION == 2, "different dimension!");
        m_sizes[0] = width;
        m_sizes[1] = height;
        m_nodes.clear();
        m_nodes.resize(width * height);
    }
    void setup_convolution_layer(neural_node_layer& inputted_layer,
                                 init_method weight_init_method,
                                 euint kernel_size,
                                 euint stride)
    {
        EDebugAssert(DIMENSION == 1, "different dimension!");
        euint num = 1;
        if (kernel_size < inputted_layer.m_sizes[0]) {
            num = (inputted_layer.m_sizes[0] - kernel_size) / stride + 1;
            if ((inputted_layer.m_sizes[0] - kernel_size) % stride)
                num++;
        }
        
        kernel_size = kernel_size < inputted_layer.m_sizes[0] ?
        kernel_size : inputted_layer.m_sizes[0];
        m_sizes[0] = num;
        m_nodes.clear();
        m_nodes.resize(num);
        
        euint remain = inputted_layer.m_sizes[0];
        for (euint x = 0; x < num; x++) {
            for (euint i = 0; i < (kernel_size < remain ? kernel_size : remain); i++) {
                m_nodes[x].add_inputted_node
                (
                    inputted_layer.m_nodes
                    [x * stride + i],
                    weight_init_method
                );
            }
            if (remain > kernel_size) remain -= kernel_size;
        }
    }
    void setup_convolution_layer(neural_node_layer& inputted_layer,
                                 init_method weight_init_method,
                                 euint kernel_width,
                                 euint kernel_height,
                                 euint stride_x,
                                 euint stride_y)
    {
        EDebugAssert(DIMENSION == 2, "different dimension!");
        euint num_x = 1;
        euint num_y = 1;
        
        if (kernel_width < inputted_layer.m_sizes[0]) {
            num_x = (inputted_layer.m_sizes[0] - kernel_width) / stride_x + 1;
            if ((inputted_layer.m_sizes[0] - kernel_width) % stride_x) {
                num_x++;
            }
        }
        if (kernel_height < inputted_layer.m_sizes[1]) {
            num_y = (inputted_layer.m_sizes[1] - kernel_height) / stride_y + 1;
            if ((inputted_layer.m_sizes[1] - kernel_height) % stride_y) {
                num_y++;
            }
        }
        
        kernel_width = kernel_width < inputted_layer.m_sizes[0] ?
        kernel_width : inputted_layer.m_sizes[0];
        kernel_height = kernel_height < inputted_layer.m_sizes[1] ?
        kernel_height : inputted_layer.m_sizes[1];
        m_sizes[0] = num_x;
        m_sizes[1] = num_y;
        m_nodes.clear();
        m_nodes.resize(num_x * num_y);
        
        euint remain_y = inputted_layer.m_sizes[1];
        for (euint y = 0; y < num_y; y++) {
            euint remain_x = inputted_layer.m_sizes[0];
            for (euint x = 0; x < num_x; x++) {
                for (euint i = 0; i < (kernel_height < remain_y ? kernel_height : remain_y); i++) {
                    for (euint j = 0; j < (kernel_width < remain_x ? kernel_width : remain_x); j++) {
                        m_nodes[y * num_x + x].add_inputted_node
                        (
                            inputted_layer.m_nodes
                            [
                                (y * stride_y + i) * inputted_layer.m_sizes[0] +
                                (x * stride_x + j)
                            ],
                            weight_init_method
                        );
                    }
                }
                if (remain_x > kernel_width) remain_x -= kernel_width;
            }
            if (remain_y > kernel_height) remain_y -= kernel_height;
        }
    }
    void setup_direct_connection_layer(neural_node_layer& inputted_layer,
                                       init_method weight_init_method)
    {
        m_nodes.clear();
        m_nodes.resize(inputted_layer.m_nodes.size());
        for (euint i = 0; i < DIMENSION; i++) {
            m_sizes[i] = inputted_layer.m_sizes[i];
        }
        euint num = m_nodes.size();
        for (euint i = 0; i < num; i++) {
            m_nodes[i].add_inputted_node(inputted_layer.m_nodes[i],
                                         weight_init_method);
        }
    }
    void setup_full_connection_layer(neural_node_layer& inputted_layer,
                                     init_method weight_init_method,
                                     euint size)
    {
        EDebugAssert(DIMENSION == 1, "different dimension!");
        m_sizes[0] = size;
        m_nodes.clear();
        m_nodes.resize(size);
        for (neural_node<VALUE_TYPE, OPERATER0, OPERATER1>& node : m_nodes) {
            for (neural_node<VALUE_TYPE, OPERATER0, OPERATER1>& inputted_node : inputted_layer.m_nodes) {
                node.add_inputted_node(inputted_node, weight_init_method);
            }
        }
    }
    void setup_full_connection_layer(neural_node_layer& inputted_layer,
                                     init_method weight_init_method,
                                     euint width,
                                     euint height)
    {
        EDebugAssert(DIMENSION == 2, "different dimension!");
        m_sizes[0] = width;
        m_sizes[1] = height;
        m_nodes.clear();
        m_nodes.resize(width * height);
        for (neural_node<VALUE_TYPE, OPERATER0, OPERATER1>& node : m_nodes) {
            for (neural_node<VALUE_TYPE, OPERATER0, OPERATER1>& inputted_node : inputted_layer.m_nodes) {
                node.add_inputted_node(inputted_node, weight_init_method);
            }
        }
    }
    void descend(const vector<VALUE_TYPE>& target_values) {
        EDebugAssert(m_nodes.size() == target_values.size(),
                     "%d != %d,the target values not same to number of nodes",
                     static_cast<int>(m_nodes.size()),
                     static_cast<int>(target_values.size()));
        VALUE_TYPE lr = operator_base<VALUE_TYPE, OPERATER0, OPERATER1>::get_learning_rate();
        VALUE_TYPE br = operator_base<VALUE_TYPE, OPERATER0, OPERATER1>::get_biasing_rate();
        euint num = m_nodes.size();
        for (euint i = 0; i < num; i++) {
            // ∂ETotal / ∂Out
            VALUE_TYPE ETotal_Out = - (target_values[i] - m_nodes[i].get_outputted_value());
            // ∂Out / ∂Net
            VALUE_TYPE Out_Net = operator_base<VALUE_TYPE, OPERATER0, OPERATER1>::eval_derivative(m_nodes[i].get_outputted_value());
            m_nodes[i].set_ETotal_Out(ETotal_Out);
            m_nodes[i].set_Out_Net(Out_Net);
        }
        for (euint i = 0; i < num; i++) {
            for (auto& inputted_node : m_nodes[i].get_inputted_nodes()) {
                inputted_node.node->descend();
            }
        }
        for (euint i = 0; i < num; i++) {
            VALUE_TYPE ETotal_Out = m_nodes[i].get_ETotal_Out();
            VALUE_TYPE Out_Net = m_nodes[i].get_Out_Net();
            for (auto& inputted_node : m_nodes[i].get_inputted_nodes()) {
                VALUE_TYPE Net_Wx = inputted_node.node->get_outputted_value();
                VALUE_TYPE ETotal_Wx =
                ETotal_Out * Out_Net * Net_Wx;
                inputted_node.weight = inputted_node.weight - lr * ETotal_Wx;
            }
            VALUE_TYPE ETotal_Bx =
            ETotal_Out * Out_Net;
            m_nodes[i].set_bias(m_nodes[i].get_bias() - br * ETotal_Bx);
        }
    }
    neural_node<VALUE_TYPE, OPERATER0, OPERATER1>* get_node(euint index) {
        EDebugAssert(DIMENSION == 1, "different dimension!");
        return &m_nodes[index];
    }
    neural_node<VALUE_TYPE, OPERATER0, OPERATER1>* get_node(euint x, euint y) {
        EDebugAssert(DIMENSION == 2, "different dimension!");
        return &m_nodes[y * m_sizes[0] + x];
    }
};
    
struct layer_config
{
    connection_method conn_method;
    init_method weight_init_method;
    euint kernel_width;
    euint kernel_height;
    euint stride_x;
    euint stride_y;
    euint op_code;
};

template <typename VALUE_TYPE, unsigned int DIMENSION, typename OPERATER0, typename OPERATER1>
class neural_node_network
{
private:
    vector<layer_config> m_layer_configs;
    vector<neural_node_layer<VALUE_TYPE, DIMENSION, OPERATER0, OPERATER1>*> m_layers;
public:
    ~neural_node_network() {
        clear();
    }
    void clear() {
        for (auto* layer : m_layers) {
            VDELETE layer;
        }
        m_layers.clear();
        m_layer_configs.clear();
    }
    void setup_layers(const vector<layer_config>& configs) {
        clear();
        for (const layer_config& config : configs) {
            switch (config.conn_method)
            {
                case InitialConnection:
                {
                    neural_node_layer<VALUE_TYPE, DIMENSION, OPERATER0, OPERATER1>* layer =
                    VNEW neural_node_layer<VALUE_TYPE, DIMENSION, OPERATER0, OPERATER1>(config.op_code);
                    if (DIMENSION == 1) {
                        layer->initialize_layer(config.kernel_width);
                    } else if (DIMENSION == 2) {
                        layer->initialize_layer(config.kernel_width,
                                                config.kernel_height);
                    } else {
                        EDebugAssert(0, "different dimension!");
                    }
                    m_layers.push_back(layer);
                }
                    break;
                case ConvolutionConnection:
                {
                    neural_node_layer<VALUE_TYPE, DIMENSION, OPERATER0, OPERATER1>* layer =
                    VNEW neural_node_layer<VALUE_TYPE, DIMENSION, OPERATER0, OPERATER1>(config.op_code);
                    if (DIMENSION == 1) {
                        layer->setup_convolution_layer(*m_layers.back(),
                                                       config.weight_init_method,
                                                       config.kernel_width,
                                                       config.stride_x);
                    } else if (DIMENSION == 2) {
                        layer->setup_convolution_layer(*m_layers.back(),
                                                       config.weight_init_method,
                                                       config.kernel_width,
                                                       config.kernel_height,
                                                       config.stride_x,
                                                       config.stride_y);
                    } else {
                        EDebugAssert(0, "different dimension!");
                    }
                    m_layers.push_back(layer);
                }
                    break;
                case FullConnection:
                {
                    neural_node_layer<VALUE_TYPE, DIMENSION, OPERATER0, OPERATER1>* layer =
                    VNEW neural_node_layer<VALUE_TYPE, DIMENSION, OPERATER0, OPERATER1>(config.op_code);
                    if (DIMENSION == 1) {
                        layer->setup_full_connection_layer(*m_layers.back(),
                                                           config.weight_init_method,
                                                           config.kernel_width);
                    } else if (DIMENSION == 2) {
                        layer->setup_full_connection_layer(*m_layers.back(),
                                                           config.weight_init_method,
                                                           config.kernel_width,
                                                           config.kernel_height);
                    } else {
                        EDebugAssert(0, "different dimension!");
                    }
                    m_layers.push_back(layer);
                }
                    break;
                case DirectConnection:
                {
                    neural_node_layer<VALUE_TYPE, DIMENSION, OPERATER0, OPERATER1>* layer =
                    VNEW neural_node_layer<VALUE_TYPE, DIMENSION, OPERATER0, OPERATER1>(config.op_code);
                    layer->setup_direct_connection_layer(*m_layers.back(),
                                                         config.weight_init_method);
                    m_layers.push_back(layer);
                }
                    break;
            }
        }
    }
    neural_node_layer<VALUE_TYPE, DIMENSION, OPERATER0, OPERATER1>* get_inputted_layer() {
        return m_layers.front();
    }
    neural_node_layer<VALUE_TYPE, DIMENSION, OPERATER0, OPERATER1>* get_outputted_layer() {
        return m_layers.back();
    }
};
    
}

#endif

#endif

/**
 * Copyright (c) 2011-2013 Xu Haining
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
