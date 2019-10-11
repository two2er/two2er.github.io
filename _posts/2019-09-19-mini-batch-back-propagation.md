---
layout: post
title: Mini-batch Back-Propagation
excerpt: "Stacking vectors to be matrices."
categories: [NLP]
comments: true
---

## Introduction

Months ago, I learned Back Propagation from this an outstanding [tutorial](http://neuralnetworksanddeeplearning.com/chap2.html) of neural network. Currently, I am studying [CS224N](<https://web.stanford.edu/class/cs224n/>), which reviews some details of the BP algorithm. In this article, the [tutorial](<http://neuralnetworksanddeeplearning.com/chap2.html>) would be summarized, but in mini-batch style by stacking single input sample vectors to be an input matrix. The Back Propagation Through Time (BPTT) would also be introduced (additional part).

## Mini-Batch BP

Though column vectors are more common, in this article, row vectors are preferred because conventionally the first dimension of input tensors denotes `batch-size`. For example, if we want to input 5 images, whose height and width are 10, and the number of channels is 3, we would construct an input tensor of shape `(5, 10, 10, 3)` or `(5, 3, 10, 10)` to feed our model. It would be more convenient to implement the algorithm if using row vectors. Therefore, the input matrix could be denoted as $X\in\mathbf{R}^{m\times d}$, where $m$ is the batch size, and the $d$ is the dimension of sample vectors.

Given a neural network, suppose it has $L$ layers. In  the forward propagation process of the $l^th$ layer, neurons receive activated outputs $A^{l-1}$ from the $(l-1)^th$ layer, and do linear transformation:

$$
Z^l=A^{l-1}W^l+b^l\tag{1}
$$

where the $W^l$ is the weight matrix of edges connecting neurons on the $(l-1)^th$ layer and the $l^th$ layer, and the $b^l$ is their bias vector. The shape of $A^{l-1}$ is $(m,n^{l-1})$, where $n^{l-1}$ is the number of neurons in the $(l-1)^th$ layer. The shape of $W^l$ is $(n^{l-1},n^l)$, where $W_{ij}^l$ is the weight of the edge connecting the $i^th$ neuron of the $(l-1)^th$ layer and the $j^th$ neuron of the $l^th$ layer. The shape of $b^l$ is $(1,n^l)$, and the addition between $A^{l-1}W^l$ and $b^l$ is similar to "broadcasting" in Numpy. $b^l$ would be added to each row of $A^{l-1}W^l$.

After the linear transformation, we apply the activation function to $Z^l$, which is denoted as $\sigma$:

$$
A^l=\sigma(Z^l)\tag{2}
$$

The activation function is element-wise.

After propagating the signals to the last layer, we get the output of the network: $A^L\in\mathbf{R}^{m\times n^L}$. Then we can compute the loss of this batch: $C$ (for the sake of simplicity, the network only predicts one target, so the $C$ is a scalar). The partial derivative of $C$ with respect to $A^L$ is easy to compute since $C$ is directly computed from $A^L$ and labels. According to the chain rule, the derivative with respect to $Z^L$ is:

$$
\frac{\partial C}{\partial Z^L}=\frac{\partial C}{\partial A^L}\frac{\partial A^L}{\partial Z^L}=\frac{\partial C}{\partial A^L}\odot\sigma'(Z^L)\tag{3}
$$

(note that the shapes of $\frac{\partial C}{\partial Z^L}$ and $\frac{\partial C}{\partial A^L}$ are all $(m,n^L)$, which follow the [denominator layout](<https://en.wikipedia.org/wiki/Matrix_calculus#Layout_conventions>). just for implementation convenience)

If you can not figure out why timing $\frac{\partial A^L}{\partial Z^L}$ is equivalent to multiplying $\sigma'(Z^L)$ element-wisely, try to compute the partial derivatives of every element in $Z^L$ and compare them with $(3)$. We denote the $\frac{\partial C}{\partial Z^L}$ as $\delta^L$ or call it the "error" of the $L^th$ layer. The notation $\delta$ significantly simplifies the formulas of Back Propagation, though it has no special meaning in itself.

Since we are doing mini-batch training, the $i^th$ row of $Z^{l+1}$ is only dependent to the $i^th$ row of $Z^l$. By the chain rule, elements in $\delta^l$ are equal to:

$$
\delta_{ij}^l=\frac{\partial C}{\partial Z_{ij}^l}=\sum_k\frac{\partial C}{\partial Z_{ik}^{l+1}}\frac{\partial Z_{ik}^{l+1}}{\partial Z_{ij}^l}=\sum_k\delta_{ik}^{l+1}\frac{\partial Z_{ik}^{l+1}}{\partial Z_{ij}^l}\tag{4}
$$

(hope you would not feel confused about the dependencies between variables. If you have trouble and can not understand the chain rule, you can draw a computational graph as suggested in [CS224N notes03](https://web.stanford.edu/class/cs224n/readings/cs224n-2019-notes03-neuralnets.pdf) to clear it)

From the forward propagation formulas $(1)$ and $(2)$:

$$
Z_{ik}^{l+1}=\sum_jA_{ij}^lW_{jk}^{l+1}+b_k^{l+1}=\sum_j\sigma(Z_{ij}^l)W_{jk}^{l+1}+b_k^{l+1}\tag{5}
$$

we can derive that

$$
\frac{\partial Z_{ik}^{l+1}}{\partial Z_{ij}^l}=\sigma'(Z_{ij}^l)W_{jk}^{l+1}\tag{6}
$$

and then, substitute $(6)$ into $(4)$, we get

$$
\delta^l_{ij}=\sigma'(Z_{ij}^l)\sum_k\delta_{ik}^{l+1}W_{jk}^{l+1}\tag{7}
$$

write $(7)$ in the form of matrix:

$$
\delta^l=\delta^{l+1}(W^{l+1})^T\odot \sigma'(Z^l)\tag{8}
$$

According to this formula, and $\delta_L$, we can compute $\delta^{L-1}. And then, we can compute \delta^{L-2}, \cdots, \delta^{1}$ recurrently. With these errors of each layer, we can easily calculate the partial derivatives of all trainable parameters (i.e., the $W$ and $b$ for each layer)

Again, we apply the chain rule to the partial derivative with respect to $W$:

$$
\frac{\partial C}{\partial W_{ij}^l}=\sum_k\frac{\partial C}{\partial Z_{kj}^l}\frac{\partial Z_{kj}^l}{\partial W_{ij}^l}=\sum_k\delta_{kj}^lA_{ki}^{l-1}\tag{9}
$$

and convert it to the matrix form:

$$
\frac{\partial C}{\partial W^l}=(A^{l-1})^T\delta^l\tag{10}
$$

It shows that the partial derivative of $W$ could be derived by only performing one matrix multiplication on the $\delta$. Similarly, we apply the chain rule to the $\frac{\partial C}{\partial b^l}$:

$$
\frac{\partial C}{\partial b_j^l}=\sum_k\delta_{kj}^l\frac{\partial Z_{kj}^l}{\partial b_j^l}=\sum_k\delta_{kj}^l\tag{11}
$$

and then write in the form of matrix:

$$
\frac{\partial C}{\partial b^l}=\sum_k\delta_{k,\cdot}^l\tag{12}
$$

In this way, we finally get derivatives of parameters of each layer and update them by gradient descent.

In summary, the process of Back Propagation is as following:

 - firstly, run forward propagation to calculate necessary values:

$$
\begin{aligned}Z^l&=A^{l-1}W^l+b^l\\A^l&=\sigma(Z^l)\end{aligned}
$$

 - secondly, compute the error of the output layer:

$$
\delta^L=\frac{\partial C}{\partial Z^L}=\frac{\partial C}{\partial A^L}\odot\sigma'(Z^L)
$$

 - then, apply the backward $\delta$ formula on the $\delta^L$ recurrently, to compute the errors of each layer:

$$
\delta^l=\delta^{l+1}(W^{l+1})^T\odot \sigma'(Z^l)
$$

 - finally, use these errors to compute the derivatives for every parameter, and update them by gradient descent:

$$
\begin{aligned}\frac{\partial C}{\partial W^l}&=(A^{l-1})^T\delta^l\\
  \frac{\partial C}{\partial b^l}&=\sum_k\delta_k^l\end{aligned}
$$

A simple neural network with mini-batch Back Propagation was implemented and shared in my [Github repository](<https://github.com/two2er/ml-toys/blob/master/supervised/neural_network.py>), which might be a help to understand these formulas.

## Back Propagation Through Time (BPTT)

The Back Propagation Through Time is an application of Back Propagation to sequence models with hidden states, like RNN. A RNN receives the input $X_t\in\mathbb{R}^{n\times d_x}$ at the $t^th$ time step, linearly combining it with the hidden state $h_{t-1}$ and applying an activation function:

$$
h_t = \sigma(h_{t-1}W_{hh}+X_tW_{hx})\tag{13}
$$

where the hidden state $h$ is a matrix of shape $(n,d_h)$ ($d_h$ is the dimension of the hidden state), and $W_{hh}\in\mathbb{R}^{d_h\times d_h}$, $W_{xh}\in\mathbb{R}^{d_x\times d_h}$ are weight matrices. Then, the network outputs according to the hidden state:

$$
\hat{y}_t=softmax(h_tW_{hs})\tag{14}
$$

The shape of the weight matrix $W_{hs}$ is $(d_h,\|V\|)$, where $\|V\|$ is the number of classes (in a language model, it is the size of the vocabulary).

Since the weight matrices are reused at each time step, the updating method is different from that of a simple multi-layer neural network. Suppose the RNN is for word predicting tasks. Then, the loss function would be the sum of the discrepancies between the prediction $\hat{y}$ and the label $y$ at every time step:

$$
L = \sum_{t=1}^Tl(y_t,\hat{y}_t) = \sum_{t=1}^Tl_t\tag{15}
$$

The partial derivative of $l_t$ with respect to $W_{hh}$ or $W_{hx}$ could be calculated by the chain rule through the derivative with respect to $h$:

$$
\frac{\partial l_t}{\partial W}=\frac{\partial l_t}{\partial \hat{y}_t}\frac{\partial \hat{y}_t}{\partial h_t}\frac{\partial h_t}{\partial W}=\frac{\partial l_t}{\partial \hat{y}_t}\frac{\partial \hat{y}_t}{\partial h_t}\sum_{k=1}^t\frac{\partial h_t}{\partial h_k}\frac{\partial h_k}{\partial W}\tag{16}
$$

The $\frac{\partial h_t}{\partial h_k}$ is:

$$
\frac{\partial h_t}{\partial h_k}=\prod_{j=k+1}^t\frac{\partial h_j}{\partial h_{j-1}}\tag{17}
$$

from $(13)$, the partial derivative $\frac{\partial h_j}{\partial h_{j-1}}$ is:

$$
\frac{\partial h_j}{\partial h_{j-1}}=W_{hh}\odot\sigma'(h_{j-1}W_{hh}+X_jW_{hx})\tag{18}
$$

(since both $h_j$ and $h_{j-1}$ are matrices, the $n$ is set to $1$ for clarity)

Substitute $(18)$ into $(17)$:

$$
\frac{\partial h_t}{\partial h_k}=W_{hh}^{t-k}\odot_{j=k+1}^t\sigma'(h_{j-1}W_{hh}+X_jW_{hx})\tag{19}
$$

From $\frac{\partial h_1}{\partial W}$ and $\frac{\partial h_t}{\partial W}=\sum_{k=1}^t\frac{\partial h_t}{\partial h_k}\frac{\partial h_k}{\partial W}$, we can calculate the $\frac{\partial h}{\partial W}$ for each time step. Then the results are substituted into $(16)$ , where we derive $\frac{\partial l_t}{\partial W}$. Finally, accumulate $l$ through the sequence:

$$
\frac{\partial L}{\partial W}=\sum_{t=1}^T\frac{\partial l_t}{\partial \hat{y}_t}\frac{\partial \hat{y}_t}{\partial h_t}\sum_{k=1}^t\left(\prod_{j=k+1}^t\frac{\partial h_j}{\partial h_{j-1}}\right)\frac{\partial h_k}{\partial W}\tag{20}
$$

This is the partial derivative of a weight matrix of the RNN.

By the way, there is a term $W_{hh}^{t-k}$ in $\frac{\partial h_t}{\partial h_k}$, which is an exponent of the weight matrix. Pascanu et al showed in his paper that if the largest eigenvalue of $W_{hh}$ is less than 1, and $(t-k)$ is sufficiently large, the norm of the gradient will shrink exponentially (suppose the activation function is the sigmoid function). Conversely, if it is larger than 1, the norm will grow extremely large. Therefore, if the length of the sequence is too long, updating parameters of early time steps would have troubles, since the gradient is either too large or too small. These are called the *Gradient Vanishing* and the *Gradient Explosion* problems. Due to these facts, training RNNs are thought to be difficult. Many techniques are proposed to alleviate vanishing and explosion problems, like the Gradient Clipping. Normally, we can use activation function rather than sigmoid, like ReLU (Rectified Linear Units), or alter the network architecture to LSTM or GRU, to improve training efficiency. However, the gradient vanishing and gradient explosion problems could not be eliminated. Carefully selected hyperparameters and appropriate initialization are still vital.



## Reference

- [How the Backpropagation Algorithm Works](<http://neuralnetworksanddeeplearning.com/chap2.html>)
- [CS224n](https://web.stanford.edu/class/cs224n/ )
- *On the difficulty of training Recurrent Neural Networks*. Razvan Pascanu, Tomas Mikolov, Yoshua Bengio
- [Dive into Deep Learning](<https://www.d2l.ai/>)

