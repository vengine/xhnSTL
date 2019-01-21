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

template <typename VALUE_TYPE, typename OPERATER>
class neural_node
{
private:
    struct inputted_neural_node
    {
        neural_node* node = nullptr;
        VALUE_TYPE weight = static_cast<VALUE_TYPE>(1.0);
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
    OPERATER m_operator;
public:
    void descend()
    {
        if (m_inputted_nodes.empty())
            return;
        VALUE_TYPE lr = m_operator.learning_rate();
        VALUE_TYPE ETotal_Out = static_cast<VALUE_TYPE>(0.0);
        for (auto& outputted_node : m_outputted_nodes) {
            ETotal_Out +=
            outputted_node.node->m_ETotal_Out *
            outputted_node.node->m_Out_Net *
            outputted_node.node->m_inputted_nodes[outputted_node.index].weight;
        }
        VALUE_TYPE Out_Net = m_operator.eval_derivative(m_outputted_value);
        m_ETotal_Out = ETotal_Out;
        m_Out_Net = Out_Net;
        for (auto& inputted_node : m_inputted_nodes) {
            inputted_node.node->descend();
            VALUE_TYPE ETotal_Wx =
            ETotal_Out * Out_Net * inputted_node.node->m_outputted_value;
            inputted_node.weight = inputted_node.weight - lr * ETotal_Wx;
        }
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
        m_outputted_value = m_operator.activate(sum + m_bias);
        return m_outputted_value;
    }
    void add_inputted_node(neural_node& node) {
        m_inputted_nodes.push_back(inputted_neural_node(&node, static_cast<VALUE_TYPE>(1.0)));
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
    
template <typename VALUE_TYPE, typename OPERATER, unsigned int DIMENSION>
class neural_node_layer : public MemObject
{
private:
    vector< neural_node<VALUE_TYPE, OPERATER> > m_nodes;
    euint m_sizes[DIMENSION] = {0};
    OPERATER m_operator;
public:
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
                    [x * stride + i]
                );
            }
            if (remain > kernel_size) remain -= kernel_size;
        }
    }
    void setup_convolution_layer(neural_node_layer& inputted_layer,
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
                            ]
                        );
                    }
                }
                if (remain_x > kernel_width) remain_x -= kernel_width;
            }
            if (remain_y > kernel_height) remain_y -= kernel_height;
        }
    }
    void setup_direct_connection_layer(neural_node_layer& inputted_layer)
    {
        m_nodes.clear();
        m_nodes.resize(inputted_layer.m_nodes.size());
        for (euint i = 0; i < DIMENSION; i++) {
            m_sizes[i] = inputted_layer.m_sizes[i];
        }
        euint num = m_nodes.size();
        for (euint i = 0; i < num; i++) {
            m_nodes[i].add_inputted_node(inputted_layer.m_nodes[i]);
        }
    }
    void setup_full_connection_layer(neural_node_layer& inputted_layer,
                                     euint size)
    {
        EDebugAssert(DIMENSION == 1, "different dimension!");
        m_sizes[0] = size;
        m_nodes.clear();
        m_nodes.resize(size);
        for (neural_node<VALUE_TYPE, OPERATER>& node : m_nodes) {
            for (neural_node<VALUE_TYPE, OPERATER>& inputted_node : inputted_layer.m_nodes) {
                node.add_inputted_node(inputted_node);
            }
        }
    }
    void setup_full_connection_layer(neural_node_layer& inputted_layer,
                                     euint width,
                                     euint height)
    {
        EDebugAssert(DIMENSION == 2, "different dimension!");
        m_sizes[0] = width;
        m_sizes[1] = height;
        m_nodes.clear();
        m_nodes.resize(width * height);
        for (neural_node<VALUE_TYPE, OPERATER>& node : m_nodes) {
            for (neural_node<VALUE_TYPE, OPERATER>& inputted_node : inputted_layer.m_nodes) {
                node.add_inputted_node(inputted_node);
            }
        }
    }
    void descend(const vector<VALUE_TYPE>& target_values) {
        EDebugAssert(m_nodes.size() == target_values.size(),
                     "the target values not same to number of nodes");
        VALUE_TYPE lr = m_operator.learning_rate();
        euint num = m_nodes.size();
        for (euint i = 0; i < num; i++) {
            // ∂ETotal / ∂Out
            VALUE_TYPE ETotal_Out = - (target_values[i] - m_nodes[i].get_outputted_value());
            // ∂Out / ∂Net
            VALUE_TYPE Out_Net = m_operator.eval_derivative(m_nodes[i].get_outputted_value());
            m_nodes[i].set_ETotal_Out(ETotal_Out);
            m_nodes[i].set_Out_Net(Out_Net);
        }
        for (euint i = 0; i < num; i++) {
            for (auto& inputted_node : m_nodes[i].get_inputted_nodes()) {
                inputted_node.node->descend();
            }
            for (auto& inputted_node : m_nodes[i].get_inputted_nodes()) {
                VALUE_TYPE Net_Wx = inputted_node.node->get_outputted_value();
                VALUE_TYPE ETotal_Wx =
                m_nodes[i].get_ETotal_Out() * m_nodes[i].get_Out_Net() * Net_Wx;
                inputted_node.weight = inputted_node.weight - lr * ETotal_Wx;
            }
        }
    }
    neural_node<VALUE_TYPE, OPERATER>* get_node(euint index) {
        EDebugAssert(DIMENSION == 1, "different dimension!");
        return &m_nodes[index];
    }
    neural_node<VALUE_TYPE, OPERATER>* get_node(euint x, euint y) {
        EDebugAssert(DIMENSION == 2, "different dimension!");
        return &m_nodes[y * m_sizes[0] + x];
    }
};
    
enum connection_method
{
    InitialConnection,
    ConvolutionConnection,
    FullConnection,
    DirectConnection,
};
    
struct layer_config
{
    connection_method method;
    euint kernel_width;
    euint kernel_height;
    euint stride_x;
    euint stride_y;
};

template <typename VALUE_TYPE, typename OPERATER, unsigned int DIMENSION>
class neural_node_network
{
private:
    vector<layer_config> m_layer_configs;
    vector<neural_node_layer<VALUE_TYPE, OPERATER, DIMENSION>*> m_layers;
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
            switch (config.method)
            {
                case InitialConnection:
                {
                    neural_node_layer<VALUE_TYPE, OPERATER, DIMENSION>* layer =
                    VNEW neural_node_layer<VALUE_TYPE, OPERATER, DIMENSION>();
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
                    neural_node_layer<VALUE_TYPE, OPERATER, DIMENSION>* layer =
                    VNEW neural_node_layer<VALUE_TYPE, OPERATER, DIMENSION>();
                    if (DIMENSION == 1) {
                        layer->setup_convolution_layer(*m_layers.back(),
                                                       config.kernel_width,
                                                       config.stride_x);
                    } else if (DIMENSION == 2) {
                        layer->setup_convolution_layer(*m_layers.back(),
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
                    neural_node_layer<VALUE_TYPE, OPERATER, DIMENSION>* layer =
                    VNEW neural_node_layer<VALUE_TYPE, OPERATER, DIMENSION>();
                    if (DIMENSION == 1) {
                        layer->setup_full_connection_layer(*m_layers.back(),
                                                           config.kernel_width);
                    } else if (DIMENSION == 2) {
                        layer->setup_full_connection_layer(*m_layers.back(),
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
                    neural_node_layer<VALUE_TYPE, OPERATER, DIMENSION>* layer =
                    VNEW neural_node_layer<VALUE_TYPE, OPERATER, DIMENSION>();
                    layer->setup_direct_connection_layer(*m_layers.back());
                    m_layers.push_back(layer);
                }
                    break;
            }
        }
    }
    neural_node_layer<VALUE_TYPE, OPERATER, DIMENSION>* get_inputted_layer() {
        return m_layers.front();
    }
    neural_node_layer<VALUE_TYPE, OPERATER, DIMENSION>* get_outputted_layer() {
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
