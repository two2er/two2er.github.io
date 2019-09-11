---
layout: post
title: Represent Words as Vectors
excerpt: "A brief introduction to word vectors."
categories: [NLP]
comments: true
image:
  feature: NLP_cover.jpg
---

## Reference

 - Neural Network Methods for Natural Language Processing, Yoav Goldberg.
 - [CS224n](https://web.stanford.edu/class/cs224n/ )
 - [Dive into Deep Learning](<https://www.d2l.ai/>)
 - Hope you can give me some advice at the comment area if I make some mistakes.



---

## Why need word vectors?

To train a NLP prediction model from a dataset of natural language, or *corpus*, one very basic step is to extract features from it, mapping the textual data to real valued vectors. These vectors contain properties of the linguistic items, which would be fed to our designed model as the input before we can start training. The quality of input vectors is vital to the performance of the model. If too much information of the corpus gets lost in the feature extraction step, the predictive performance of the model would definitely be greatly reduced. Therefore, people make lots of efforts to improve it. How to construct good embedding vectors has been an important problem.

In earlier NLP researches, people did not tend to  treat single words as atomic symbols, because contexts affect the meaning of a word significantly. Take the word "bank" as an example. In the sentence, "People save money in a bank", the word "bank" means a financial institution, while in the sentence "He sat on the bank of the river" it means the land alongside a river. The true meaning of a word can only be inferred from its nearby words, or from the document it appears in. Therefore, early methods embed the whole document as one vector. The famous *Bag of Words (BOW)*, represents a document as a vector $\mathbf{w}\in\mathbb{R}^{\|V\|\times 1}$. In this notation, $\|V\|$ is the size of the vocabulary. For example, the sentence "The black cat loves the yellow cat." can be expressed as:

$$\begin{bmatrix}2&0&0&\cdots&2&1&\cdots&0\end{bmatrix}^T$$

If the first word in the vocabulary is "the", and the second is "a", etc.. The *i*-th element $\mathbf{w}_i$ of $\mathbf{w}$ denotes the number of times the *i*-th word in the vocabulary appears in the document.

Although simple, BOW has been proven effective in some tasks, like document classification. BOW takes advantages of a feature of natural language, that similar types of documents use similar words. However, BOW is unable to tell some more subtle differences, because it does not consider the order of words, as well as the synonym.

When it comes to the era of deep learning, embedding language with word vectors begins to be widely accepted. This is due to the rapid development of word embedding approaches. Good word vectors have some good properties. For example, vectors of similar words tend to be close to each other on some distance metrics. By representing sentences and documents as the sequences of word vectors, researchers achieve very good results compared to earlier methods. Due to its importance, most NLP beginners get touch in word embeddings in the first several lessons. In this article, I will introduce 4 methods to generate word vector representations. Though most time you do not need to train the word vectors by yourself (it is very convenience to download pre-trained word vectors online), knowing about some details of how to train word vectors may help you to design a better model. All these 4 methods are implemented by myself in my [Github repository](<https://github.com/two2er/ml-toys>).

Generally speaking, there are two kinds of word vector methods, one is *count-based*, and the other is *iteration-based*. Most time we prefer word vectors generated from iteration-based methods, but knowing about count-based methods would help you have a better understanding of some important NLP concepts.

## One-Hot Vector

This is the simplest method, which represents each word as  a $\mathbb{R}^{\|V\|\times 1}$ vector with all 0s and only one 1 at the index of that word in the sorted English language. For example, the first word in the vocabulary, aardvark, would be represented as:

$$\mathbf{w}^{\text{aardvark}}=\begin{bmatrix}1&0&\cdots&0\end{bmatrix}^T$$

Although simple and clear, one-hot vector could not be used to capture similarity of words directly, because any two one-hot vectors are orthogonal, i.e., their inner product is always 0.

In fact, the BOW vector representation of a document, is simply a sum of one-hot vectors of words in it.

$$\text{BOW}(d)=\sum_i \mathbf{w}_i^{one-hot}$$

From this, you will find that BOW is problematic. As well, if the size of the vocabulary, $\|V\|$, grows too large, it would cost a huge part of memory to store the vectors, which is surely an obstruct to training. Not only one-hot vector and BOW have this problem, most count-based methods suffer from it too. The most common way is to apply *singular value decomposition (SVD)* to reduce the vector dimensions. Given a matrix $\mathbf{M}$, SVD decomposes it by:

$$\mathbf{M}=\mathbf{USV}^T$$

where $\mathbf{S}$ is the singular value matrix, $\mathbf{U}$ and $\mathbf{V}$ are two orthogonal matrices. After sorting singular values in $\mathbf{S}$ from big to small, take the first $k$ columns of $\mathbf{U}$ as word vectors, where $k$ is the word vector dimension we desire.

[code here](<https://github.com/two2er/ml-toys/blob/master/NLP/word_vectors/one_hot.py>)

## Co-Occurrence Matrix

To generate more useful word vectors than one-hot vector, in most case we rely on the *distributional hypothesis*, set forth by Firth and Harris: the meaning of a word can be inferred from the contexts in which it is used. Similar words will be used in similar contexts. Therefore, we can extract features of words by discovering the contexts in which they occur. The context of a word could be a number of words co-occur with it, or the document it appears in. With this intuition in mind, many count-based methods came up.

*Co-Occurrence matrix* is one of the count-based methods. It counts the number of times every two words occur together in the corpus. Occurring together means one word appears in the context of the other word. We limit the context of a word to a *window* of fixed size $n$, which contains $n$ words before the word and $n$ words after the word, totally $2n$ words surrounding the target word.  The co-occurrence matrix $\mathbf{M}$, in which $\mathbf{M}_{ji}$ is the number of times word $w_j$ appears inside the window of $w_i$. The *i*-th column of the matrix is taken as the word vector of the *i*-th word $w_i$.  There is an example below, in which the corpus contains three sentences and the window size is 1:

1. I enjoy flying.
2. I like NLP.
3. I like deep learning

The co-occurrence matrix is as following:

![co-occurrence matrix](https://two2er.github.io/img/co_occurrence_matrix.jpg)

As we can see, the shape of the co-occurrence matrix is $\|V\|\times\|V\|$, each word's vector is $\|V\|\times 1$, same as the one-hot vector. This kind of vectors can capture some extend of word similarity. However, it is still a little naïve , and the dimension needs to be further reduced.

[code here](<https://github.com/two2er/ml-toys/blob/master/NLP/word_vectors/co_occurrence.py>)

## Word-Context Matrix

*Word-context matrix* is another count-based method. It builds a word-context matrix $X$, in which $X{ij}$ is the number of times that word $w_i$ appears in the *j*-th linguistic context (for example, a document. do not confuse it with the previous context). Let us denote the number of linguistic contexts with $\|D\|$, the shape of the word-context matrix would be $\|V\|\times\|D\|$. Since similar words tend to appear in similar contexts, the word-context matrix can also capture similarity between words. The *i*-th word $w_i$ is represented by the *i*-th row of the matrix. However, the word-context correlation metric needs to be modified to get rid of some problems. For example, frequent contexts like "the cat", "an apple", receive higher scores than less frequent but more valuable contexts, like "cute cat", "green apple". Therefore, some would use the metric *pointwise mutual information (PMI)* to calculate $X{ij}$:

$$\text{PMI}(w, c)=\log\frac{\#(w,c)\cdot\|D\|}{\#(w)\cdot\#(c)}$$

PMI rescales values by words' marginal probabilities. Furthermore, the PMI value of word-context pair $(w,c)$ that were never observed in the corpus is $\log(0)=-\infty$. To avoid it, we modify PMI to be *positive PMI (PPMI)*:

$$\text{PPMI}(w,c)=\max(\text{PMI}(w,c),0)$$

Although carefully defined, many count-based methods including co-occurrence matrix and word-context matrix have to face the problem that the counting matrix is sparse and high-dimensional. As well, when new words come into the corpus or the corpus is changed, it is hard to update the matrix, which would even change the shape of the matrix. That's why we need iteration-based methods, which could overcome these difficulties in elegant way.

[code here](<https://github.com/two2er/ml-toys/blob/master/NLP/word_vectors/word_context.py>)

## Word2vec

Iteration-based methods try to create a model with word vectors as parameters and an objective function, just like a supervised machine learning model. At each iteration, the model updates parameters to optimize the objective. The training process may be gradient descent. Because word vectors are the parameters of the model, we can arbitrarily define the dimension of the vectors. Well-trained word vectors are dense, and have good linguistic properties.

*Word2vec* is an epitome of iteration-based methods.



