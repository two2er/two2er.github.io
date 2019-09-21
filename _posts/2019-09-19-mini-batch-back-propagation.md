---
layout: post
title: Mini-batch Back-Propagation
excerpt: "Stacking vectors to be matrices."
categories: [NLP]
comments: true
---

## Introduction

Months ago, I learned Back Propagation from this [post](<http://neuralnetworksanddeeplearning.com/chap2.html>), which I thought an outstanding tutorial of neural network. Currently, I am studying [CS224N](<https://web.stanford.edu/class/cs224n/>), which reviews me some details of BP process. In this article, I will summarize the ideas in [post](<http://neuralnetworksanddeeplearning.com/chap2.html>), but stack single input sample vectors to be an input matrix in mini-batch style.

## Notation

Though column vectors are more common, in this article, I will use row vectors, because conventionally the first dimension of input tensors is `batch-size`. For example, if we want to input 5 images, whose height and width are 10, and channel is 3, we would construct an input tensor of shape `(5, 10, 10, 3)` or `(5, 3, 10, 10)` to feed our model. It would be more convenient to implement the algorithm with code if using row vectors. Therefore, I denote the input matrix as $X\in\mathbf{R}^{m\times d}$, where $m$ is the size of mini-batch, and the $d$ is the dimension of sample vectors.

Given a neural network, suppose there are $L$ layers in it. In  the forward propagation process of the $l$-th layer, neurons receive activated outputs $A^{l-1}$ from the $(l-1)$-th layer, and do linear transformation:

$$\begin{equation}Z^l=A^{l-1}W^l+b^l\end{equation}$$

where the $W^l$ is the weight matrix of edges connecting the $(l-1)$-th layer and the $l$-th layer, and the $b^l$ is its bias. The shape of $A^{l-1}$ is $(m,n^{l-1})$, where $n^{l-1}$ is the number of neurons in the $(l-1)$-th layer. The shape of $W^l$ is $(n^{l-1},n^l)$, and the shape of $b^l$ is $(1,n^l)$. Note that the addition between $A^{l-1}W^l$ and $b^l$ is similar to "broadcasting" in Numpy. $b^l$ would be added to each row of $A^{l-1}W^l$.

After the linear transformation, we apply the activate function to $Z^l$:

$$\begin{equation}A^l=\sigma(Z^l)\end{equation}$$

$\sigma$ is the activate function, which is element-wise.

After propagating the signals to the last layer, we get the output of the network: $A^L\in\mathbf{R}^{m\times n^L}$. Then we can compute the loss of this batch: $C$ (for the sake of simplicity, the network only predicts one target, so the $C$ is a scalar). The partial derivative of $C$ with respect to $A^L$ is easy to compute, since $C$ is a direct operation result of $C$ and labels. With the help of chain rule, the derivative with respect to $Z^L$ is:

$$\begin{equation}\frac{\partial C}{\partial Z^L}=\frac{\partial C}{\partial A^L}\frac{\partial C}{\partial A^L}=\frac{\partial C}{\partial A^L}\odot\sigma'(Z^L)\end{equation}$$

(note that the shape of $\frac{\partial C}{\partial A^L}$ is $(m,n^L)$, which follows the [denominator layout](<https://en.wikipedia.org/wiki/Matrix_calculus#Layout_conventions>). just for implementation convenience)

If you can not figure out why timing $\frac{\partial C}{\partial A^L}$ is equivalent to multiplying $\sigma'(Z^L)$ element-wisely, try to compute the partial derivative of every element in $Z^L$ and combine them to be a entire matrix. We denote this derivative as $\delta^L$, or call it the "error" of the $L$-th layer. The $\delta$ notation significantly simplify the formulas of Back Propagation, though it has no special meaning in itself.

Since we are doing mini-batch training, the $i$-th row of $Z^{l+1}$ is only dependent to the $i$-th row of $Z^l$, so we get

$$\begin{equation}\delta_{ij}^l=\frac{\partial C}{\partial Z_{ij}^l}=\sum_k\frac{\partial C}{\partial Z_{ik}^{l+1}}\frac{\partial Z_{ik}^{l+1}}{\partial Z_{ij}^l}=\sum_k\delta_{ik}^{l+1}\frac{\partial Z_{ik}^{l+1}}{\partial Z_{ij}^l}\end{equation}$$

(hope you would not feel confused about the dependency between variables. if you have trouble and can not understand the chain rule, you can draw a computational graph as suggested in [CS224N notes03](https://web.stanford.edu/class/cs224n/readings/cs224n-2019-notes03-neuralnets.pdf) to clear it)

from

$$\begin{equation}Z_{ik}^{l+1}=\sum_jA_{ij}^lW_{jk}^{l+1}+b_k^{l+1}=\sum_j\sigma(Z_{ij}^l)W_{jk}^{l+1}+b_k^{l+1}\end{equation}$$_

we can know that

$$\begin{equation}\frac{\partial Z_{ik}^{l+1}}{\partial Z_{ij}^l}=\sigma'(Z_{ij}^l)W_{jk}^{l+1}\end{equation}$$

substitute it into $(4)$, we get

$$\begin{equation}\delta^l_{ij}=\sigma'(Z_{ij}^l)\sum_k\delta_{ik}^{l+1}W_{jk}^{l+1}\end{equation}$$

so

$$\begin{equation}\delta^l=\delta^{l+1}(W^{l+1})^T\odot \sigma'(Z^l)\end{equation}$$

With this formula, and that $\delta_L=\frac{\partial C}{\partial Z^L}=\frac{\partial C}{\partial A^L}\odot\sigma'(Z^L)$, we can compute $\delta^{L-1}, \delta^{L-2}, \cdots, \delta^{1}$ reccurently. With these errors of each layer, we can easily calculate the partial derivatives of trainable parameters (i.e., $W$ and $b$ for each layer)

From

$$\begin{equation}\frac{\partial C}{\partial W_{ij}^l}=\sum_k\frac{\partial C}{\partial Z_{kj}^l}\frac{\partial Z_{kj}^l}{\partial W_{ij}^l}=\sum_k\delta_{kj}^lA_{ki}^{l-1}\end{equation}$$

($W_{ij}^l is the weight of the edge connecting the $i$-th neuron of the $l$-th layer and the $j$-th neuron of the $(l+1)$-th layer)

we can get

$$\begin{equation}\frac{\partial C}{\partial W^l}=(A^{l-1})^T\delta^l\end{equation}$$

Similarly, from

$$\begin{equation}\frac{\partial C}{\partial b_j^l}=\sum_k\delta_{kj}^l\frac{\partial Z_{kj}^l}{\partial b_j^l}=\sum_k\delta_{kj}^l\end{equation}$$
we can also get
$$\begin{equation}\frac{\partial C}{\partial b^l}=\sum_k\delta_k^l\end{equation}$$

Those are the partial derivatives of model parameters.

In summary, the process of Back Propagation is as following:

 - firstly, do forward propagation to calculate necessary values：
$$\begin{array}}Z^l=A^{l-1}W^l+b^l\\A^l=\sigma(Z^l)\end{array}$$
 - secondly, compute the error of the output layer：
$$\delta^L=\frac{\partial C}{\partial Z^L}=\frac{\partial C}{\partial A^L}\odot\sigma'(Z^L)$$
 - then, from the expression of the error of the output layer, and the recurrence formula, we can calculate errors of each layer in the network：
$$\delta^l=\delta^{l+1}(W^{l+1})^T\odot \sigma'(Z^l)$$
 - finally, we use these errors to compute the derivatives of each parameter, and update them：
$$\begin{array}}\frac{\partial C}{\partial W^l}=(A^{l-1})^T\delta^l\\
\frac{\partial C}{\partial b^l}=\sum_k\delta_k^l\end{array}}$$

## Back Propagation Through Time (BPTT)

to be continued...