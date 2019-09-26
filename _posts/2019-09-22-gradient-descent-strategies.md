---
layout: post
title: Gradient Descent Strategies
excerpt: "An introduction of several common gradient descent algorithms."
categories: [NLP]
comments: true
---

## Defect of Traditional Gradient Descent

Everyone might know something about the gradient descent (GD), which is one of the basic techniques of machine learning. GD is an iteration-based parameter optimizing method. At each iteration, it firstly calculates the partial derivative of the loss function $J$ with respect to each parameter $c$, and then updates $c$ by:

$$c\leftarrow c-\eta \nabla_c$$

where the $\eta$ is the learning rate, or the step size. Though simple, GD has some disadvantages to be improve:

- If the parameters are not normalized before training, it is likely to slow down the speed of convergence. The reason is that the learning rates for each parameter are the same. If a parameter is much greater than another parameter, it might get a larger gradient and overshoot. As well, the path to the minimum would oscillate:

  ![1569527252988](C:\Users\dtcf\AppData\Roaming\Typora\typora-user-images\1569527252988.png)

  All these will increase the difficulty of training. A simple solution is to set a relatively small learning rate for the model, but it would slow down the training process, which is also not what we expect.

- Since the learning rate is invariable, after some iterations, the parameters would reach their minimum. However, if the learning rate is too large, the parameters might "skip" the minimum, and fail to converge.

  ![1569522726545](C:\Users\dtcf\AppData\Roaming\Typora\typora-user-images\1569522726545.png)

To improve these, some parameter optimization algorithms have been proposed. In this article, 5 ideas to improve GD would be introduced. They are: Momentum, Adagrad, RMSProp, Adadelta, and Adam. They are all based on GD, and are still iteration-based, but with some modifications on hyperparameters.

**Gradient Descent**:
$$
\begin{aligned}
c\leftarrow c-\eta \nabla_c
\end{aligned}
$$


## Momentum

The process of GD, especially stochastic gradient descent (SGD) and mini-batch stochastic gradient descent, usually oscillates, resulting in a "winding" path to the minimum.

We denote the updating values for a parameter (omitting the learning rate and the negative sign) at the $n$-th iteration as $v_n$:

$$c\leftarrow c-\eta v_n$$

and the gradient as $\nabla_n$. In the traditional gradient descent, the $v_n$ is equal to $\nabla_n$. If we mix into the concept of **Momentum**, the $v_n$ would be:

$$v_n=\beta v_{n-1}+(1-\beta)\nabla_n$$

Where $\beta$ is a constant value between 0 and 1. It means that, the updating value of a parameter is changing during the iteration. By unfolding each $v$, we can find that:

$$\begin{aligned}v_1&=(1-\beta)\nabla_1\\v_2&=\beta(1-\beta)\nabla_1+(1-\beta)\nabla_2\\v_3&=\beta^2(1-\beta)\nabla_1+\beta(1-\beta)\nabla_2+(1-\beta)\nabla_3\\\cdots\\v_n&=(1-\beta)(\beta^{n-1}\nabla_1+\beta^{n-2}\nabla_2+\cdots+\nabla_n)\end{aligned}$$

According to the last equation, $v_n$ is the weighted average of $\nabla_1,\nabla_2,\cdots,\nabla_n$. With Momentum, the direction of the gradient at the current iteration is more similar to the direction of that at the previous iteration. The gradient path seems to be "smoothed", and oscillates less. 

![1569527228723](C:\Users\dtcf\AppData\Roaming\Typora\typora-user-images\1569527228723.png)

On the other hand, we can imagine the $v$ as the "velocity" of the descendent path. It seems that the $\nabla_n$ for the $v_n$ at the $n$-th iteration is its "accelaration", and $v_{n-1}$ is its current velocity. This simulation to physical motion gives gradient descent the "inertia", and therefore the direction of the gradient varies less. This can help to avoid problems like oscillation and overshooting.

By the way, note that $\frac{1}{1-\beta}\sum_{i=1}^n\beta^{n-i}=1-\beta^n$. Therefore, the sum of weights is approximately equal to 1 when $n$ is big enough. Some versions of Momentum implementation do not have the normalized item $\frac{1}{1-\beta}$, because after some iterations, its effect to magnify gradients would be smaller and smaller, so it can be omitted reasonably.

**Momentum**:
$$
\begin{aligned}
v_n\leftarrow &\beta v_{n-1}+(1-\beta)\nabla_n\\
&c\leftarrow c-\eta v_n
\end{aligned}
$$


## Adagrad

To solve the problem caused by fixed learning rate, one very intuitive solution might be altering the learning rate during the process of gradient descent. Also intuitively, the learning rate is expected to decrease during the process. If the gradient is too large, the learning rate has to be dropped significantly to ensure the updating value is under control. So, one way to gradually reduce the learning rate is by dividing a variable:

$$\begin{aligned}s_n&\leftarrow s_{n-1}+\nabla_n^2\\c\leftarrow &c-\frac{\eta}{\sqrt{s_n+\epsilon}}\nabla_n\end{aligned}$$

where the $\epsilon$ is a small constant value for numerical stability. At each iteration, the $s_n$ is added to the square of the gradient, so the denominator of the learning rate increases, keeping reducing the learning rate. If the parameter has a constant and large partial derivative, the learning rate would drop faster for this parameter, while if the gradient of the parameter remains small, its learning rate would decline more slowly. This prevents the model from overshooting while keeping a fast learning speed. The trick is called **adaptive learning rate**, which is the core idea of **Adagrad**. However, the Adagrad still has some problems. Since the $s_n$ is an accumulation of $\nabla_1$ to $\nabla_n$, at later stages of iteration the learning rate would become too small for updating, and the descendent path just likes getting "stuck" at the point.