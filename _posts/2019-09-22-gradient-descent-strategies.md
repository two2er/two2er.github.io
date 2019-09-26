---
layout: post
title: Gradient Descent Strategies
excerpt: "An introduction of several common gradient descent algorithms."
categories: [NLP]
comments: true
---

## Defect of Traditional Gradient Descent

Everyone might know something about the gradient descent (GD), which is one of the basic techniques of machine learning. GD is an iteration-based parameter updating method, at each iteration, it firstly calculates the partial derivative of the loss function $J$ with respect to each parameter $c$, and updates $c$ by:

$$c\leftarrow c-\eta \nabla_c$$

where the $\eta$ is the learning rate, or the step size. Though simple, GD has some disadvantages needed to be improve:

- If the parameters are not normalized before training, it is likely to slow down the speed of convergence. The reason is that the learning rates for each parameter are the same. If a parameter is larger in scale than another parameter, it might get a larger gradient and might diverge after updating. A simple solution is to set a relatively small learning rate for the model, but it would slow down the training process.
- Since the learning rate is invariable, after some iterations, the parameters would reach their minimum. However, if the learning rate is too large, the parameters might "skip" the minimum, and find hard to diverge. This kind of phenomenon is called "overshooting".

To improve these, some parameter optimization algorithms have been proposed. In this article, I will introduce some of them based on the GD, trying to alleviate the disadvantages of GD.

## Momentum

The process of GD, especially stochastic gradient descent (SGD) and mini-batch stochastic gradient descent, usually oscillates, resulting in a "winding" path to the minimum. We denote the updating values for a parameter (omitting the learning rate and the negative sign) at the $i$-th iteration as $m_i$:

$$c\leftarrow c-\eta m_i$$

and the gradient as $\nabla_i$. In the traditional gradient descent:

$$m_i=\nabla_i$$

While with **momentum**, $m$ would be:

$$m_i=\beta m_{i-1}+(1-\beta)\nabla_i$$

Where $\beta$ is a constant value between 0 and 1. It means that, the updating values of a parameters are changing during the iteration. By unfolding each $m$, we can find that:

$$\begin{aligned}m_1&=(1-\beta)\nabla_1\\m_2&=\beta(1-\beta)\nabla_1+(1-\beta)\nabla_2\\m_3&=\beta^2(1-\beta)\nabla_1+\beta(1-\beta)\nabla_2+(1-\beta)\nabla_3\\\cdots\\m_n&=(1-\beta)(\beta^{n-1}\nabla_1+\beta^{n-2}\nabla_2+\cdots+\nabla_n)\end{aligned}$$

$m_n$ is the weighted average of $\nabla_1,\nabla_2,\cdots,\nabla_n$. With momentum, the direction of the gradient at the current iteration is more similar to the direction of that at the previous iteration. This is kind of "smoothing" the gradient path. On the other hand, we can imagine the $m_n$ as the "velocity" of the descendent path. it seems that the $\nabla_n$ for the $m_n$ at the $n$-th iteration is its "accelaration", and $m_{n-1}$ is its current velocity. This simulation to physical motion gives gradient descent the "inertia", and therefore the direction of the gradient is more stable than simply assigning it with $\nabla_n$. This can help avoiding problems like oscillation and overshooting.

By the way, note that $\frac{1}{1-\beta}\sum_{i=1}^n\beta^{n-i}=1-\beta^n$. Therefore, the sum of weights is approximately equal to 1 when $n$ goes larger. Some versions of the momentum do not have the normalized item $\frac{1}{1-\beta}$, because after some iterations the use of this item is getting smaller and smaller, and can be omitted.

## Adagrad

To solve the problem of fixed learning rate, one very intuitive idea might be altering the learning rate during the process of gradient descent. Also intuitively, we want the learning rate decreasing during the process. The larger the gradient, the more the learning rate would decrease. So, we can design the learning rate updating rule as:

$$\begin{aligned}s_n&\leftarrow s_{n-1}+\nabla_n^2\\c\leftarrow &c-\frac{\eta}{\sqrt{s_n+\epsilon}}\nabla_n\end{aligned}$$

Where $\epsilon$ is a small constant value for numerical stability. We can see that the learning rate (now it is $\frac{\eta}{\sqrt{s_n+\epsilon}}$) is keeping decreasing. If the parameter has a constant and large partial derivative, the learning rate would drop faster for this parameter, while if the gradient of the parameter remains small, its learning rate would decline more slowly. This can help us prevent from overshooting while keeping a fast learning speed. However, the method (or, **adagrad**) still has some problems. Since the $s_n$ is an accumulation of $\nabla_1,\cdots,\nabla_n$, at later stages of iteration the learning rate would become too small for updating, and the path just likes getting "stuck" at the point. We have to improve the updating rule of $s$ further.

## RMSProp

We can combine the ideas of momentum and the adaptive learning rate together, to avoid the explosion of $s$:

$$s_n\leftarrow \gamma s_{n-1}+(1-\gamma)\nabla_n^2$$

where $\gamma$ is like the $\beta$, a constant value between 0 and 1. This limits the $s$ from being a very large number, because $s_n$ is like $m_n$, a weighted average of $\nabla_1^2,\cdots, \nabla_n^2$, so it can not grow infinitely. The formula to update the parameters is identical to that of $adagrad$:

$$c\leftarrow c-\frac{\eta}{\sqrt{s_n+\epsilon}}\nabla_n$$

## Adadelta

RMSProp depends on the learning rate constant $\eta$. The **Adadelta** method replaces it with a variable which changes with the iteration. Concretely, the updating value for the parameter becomes:

$$c\leftarrow c-\sqrt{\frac{\delta_{n-1}+\epsilon}{s_n+\epsilon}}\nabla_n$$

where $\delta x$ is an exponentially weighted accumulation of the update value (denote it by $g_n'=\sqrt{\frac{\delta_{n-1}+\epsilon}{s_n+\epsilon}}\nabla_n$):

$$\delta_n\leftarrow \beta\delta{n-1}+(1-\rho)g_n'$$

The hyperparameter $\eta$ is replaced by the variable $\sqrt{\delta}$. The Adadelta method gets rid of the need to tune the learning rate parameter, while keeps the training process robust and even has better performance.

## Adam

As discussed before, $\frac{1}{1-\beta}\sum_{i=1}^n\beta^{n-i}=1-\beta^n$. When $n$ is small, the gradient accumulation from previous steps is small. To eliminate this effect, we can divide the sum by $1-\beta^n$ to correct the bias. By applying this trick called **bias correction** to RMSProp, we get the **Adam**:

$$\begin{aligned}\hat{s}_n\leftarrow \frac{s_n}{1-\gamma^n}\\\hat{m}_n\leftarrow \frac{m_n}{1-\beta^n}\end{aligned}$$



## Reference

- [Dive into Deep Learning](<https://www.d2l.ai/>)
- *Hands-On Machine Learning with Scikit-Learn and TensorFlow*. Aurélien Géron.
- *Adadelta: an adaptive learning rate method*. Matthew D. Zeiler
- [CS224n](https://web.stanford.edu/class/cs224n/ )