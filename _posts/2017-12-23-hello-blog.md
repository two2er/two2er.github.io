---
layout: post
title: 我的 Blog 今天开通了……
date: 2017-12-23 12:00:10+00:00
categories: 日志
tags: 博客
---

## problems  
### 1、简单题  
  
 - 软件工程的定义  
 - 阅读经典名著“人月神话”等资料，解释 software crisis、COCOMO 模型。  
 - 软件生命周期。  
 - 按照 SWEBok 的 KA 划分，本课程关注哪些 KA 或 知识领域？  
 - 解释 CMMI 的五个级别。例如：Level 1 - Initial：无序，自发生产模式。  
 - 用自己语言简述 SWEBok 或 CMMI （约200字）  
### 2、解释 PSP 各项指标及技能要求：  
  
 - 阅读《现代软件工程》的 PSP: Personal Software Process 章节。 http://www.cnblogs.com/xinz/archive/2011/11/27/2265425.html  
 - 按表格 PSP 2.1， 了解一个软件工程师在接到一个任务之后要做什么，需要哪些技能，解释你打算如何统计每项数据？ （期末考核，每人按开发阶段提交这个表）  
  
## answers  
  
 1. 软件工程的定义    
here are some typical definitions of Software Engineering:    
"Software Engineering is the application of engineering to the development of software in a systematic method." - Wikipedia    
"the systematic application of scientific and technological knowledge, methods, and experience to the design, implementation, testing, and documentation of software" - The Bureau of Labor Statistics—IEEE Systems and software engineering - Vocabulary    
"The application of a systematic, disciplined, quantifiable approach to the development, operation, and maintenance of software"—IEEE Standard Glossary of Software Engineering Terminology    
"an engineering discipline that is concerned with all aspects of software production" - Ian Sommerville    
"the establishment and use of sound engineering principles in order to economically obtain software that is reliable and works efficiently on real machines" - Fritz Bauer    
 2. software crisis  
 Software crisis is a term used in the early days of computer science for the difficulty of writing useful and efficient computer programs in the required time. The software crisis was due to the rapid increases in computer power and the complexity of the problems that could not be tackled. With the increase in the complexity of the software, many software problems arose because existing methods were insufficient.  
In the book *The Mythical Man-Month*, there is an illustration of why the traditional belief *Man-month* does not fit with software industry. With no communication among workers, men and months are interchangealbe commodities:  
![image.png-143.2kB][1]  
but software construction is an exercise in complex interrelationships, from coding to testing and debugging. Under this condition, man-month looks like this:  
![image.png-142.9kB][2]  
Which means managers could no longer solve scheduling problems by merely adding more workers. This is a reason of software crisis.  
 3. COCOMO  
The Constructive Cost Model(COCOMO) is a procedural software cost estimation model developed by Barry W. Boehm. The model parameters are derived from fitting a regression formula using data from historical projects.  
COCOMO consists of a hierarchy of three increasingly detailed and accurate forms. The first level, Basic COCOMO is good for quick, early, rough order of magnitude estimates of software costs, but its accuracy is limited due to its lack of factors to account for difference in project attributes (Cost Drivers). Intermediate COCOMO takes these Cost Drivers into account and Detailed COCOMO additionally accounts for the influence of individual project phases.  
 4. 软件生命周期  
In software engineering, a software development process is the process of dividing software development work into distinct phases to improve design, product management, and project management. It is also known as a **software development life cycle**. The software development life cycle methodologies may include the pre-definition of specific deliverables and artifacts that are created and completed by a project team to develop or maintain an application.  
Most modern development processes can be vaguely described as agile(Kanban, Scrum, ...). Other methodologies include waterfall, spiral development, and so on.    
 5. 按照 SWEBok 的 KA 划分，本课程关注哪些 KA 或 知识领域？  
here is *The 15 SWEBOK KAs* defined by SWEBOK:  
> Software Requirements  
Software Design  
Software Construction  
Software Testing  
Software Maintenance  
Software Configuration Management  
Software Engineering Management  
Software Engineering Process  
Software Engineering Models and Methods  
Software Quality  
Software Engineering Professional Practice  
Software Engineering Economics  
Computing Foundations  
Mathematical Foundations  
Engineering Foundations  
   
 According to PPT 0, this lesson concerns about the following aspects:  
> IEEE 93, 软件工程的定义  
围绕软件质量,软件工程知识体系的三个层次: 过程、方法、工具  
过程模型与软件生命周期  
  
 Therefore, Software Requirements, Software Design, Software Construction. While other KAs are also important in the process of SE, but viewed as auxillary techniques under this lesson.  
 6. 解释 CMMI 的五个级别。例如：Level 1 - Initial：无序，自发生产模式。  
![image.png-81.2kB][3]    
CMMI models provide guidance for developing or improving processes that meet the business goals of an organization. A CMMI model may also be used as a framework for appraising the process maturity of the organization.    
At level 1, the Initial, processes of a project is unpredicable and out of control. None of criteria it could meet. It is the least developed state.    
At level 2, the Managed, basic requirements are met, they are:    
> CM - Configuration Management    
MA - Measurement and Analysis    
PPQA - Process and Quality Assurance    
REQM - Requirements Management    
SAM - Supplier Agreement Management    
SD - Service Delivery    
WMC - Work Monitoring and Control    
WP - Work Planning    
   
 It is more controlled and provides more reaction than level 1, but still could not be consided as efficient.    
At level 3, the Defined, followed these criteria:    
> CAM - Capacity and Availability Management    
DAR - Decision Analysis and Resolution    
IRP - Incident Resolution and Prevention    
IWM - Integrated Work Managements    
OPD - Organizational Process Definition    
OPF - Organizational Process Focus...    
OT - Organizational Training    
RSKM - Risk Management    
SCON - Service Continuity    
SSD - Service System Development    
SST - Service System Transition    
STSM - Strategic Service Management    
  
 The organization is proactive to achieve some goals, and does some guarantees on their products. Extra controlment is provided, such as risk management.    
At level 4, the Quanlitatively Managed, followed by these criteria:    
> OPP - Organizational Process Performance    
QWM - Quantitative Work Management    
  
 The organization has quantitative methods to manage work, holding the process in their hands. Problems occur rarely, and everything keeps going correctly.    
At level 5, the optimizing, is based on the following criteria:    
> CAR - Causal Analysis and Resolution    
OPM - Organizational Performance Management    
  
 Leaders have gotten quite familiar with their project, and focus on further development of the products but not annoying management problems.    
 7. 用自己语言简述 SWEBok 或 CMMI （约200字）    
Developed by CMU, CMMI is known as a model which provides guidance for developing or improving processes that meet the business goals of an organization. Note that it does not certify an organization, ranking it by some characteristic criteria or other, but merely appraise it for convenience of management. CMMI classify organizations into 5 levels: Initial, Managed, Defined, Quantitatively Managed and Optimizing. For that CMMI provides only the appraisal for business, in other word, WHAT processes should be implemented, but not HOW they could be implemented, it is suspected by scholars on their benefits for improvement on management performance. Some scholars think CMMI could bring more adaptability and preictability when combined with other models, like Scrum, etc.    
 8. 解释 PSP 各项指标及技能要求  

PSP2.1 | interpretation
------------ | -------------
**Planning** | 
Content in the first column | Content in the second column
Estimate | estimating how much time is needed to finish one task
**Development** | 
Analysis | requirement analysis, including learning imperative new techniques
Design Spec | writing design spec
Design Review | reviewing design spec
Coding Standard | lay down coding standard based on requirement, process, etc.
Design | concrete design
Coding | finishing code implement
Code Review | reviewing code
Test | testing code, refining, submissing revise, etc.
**Reporting** | 
Test Report | reporting results of testing
Size Measurement | measuring size of this task
Postmortem&Process Improvement Plan | postmortem and concluding amendable aspects
 9. 按表格 PSP 2.1， 了解一个软件工程师在接到一个任务之后要做什么，需要哪些技能，解释你打算如何统计每项数据？  
Sheet PSP 2.1 has provided a detailed schedule for software developing. First of all, a programmer should estimate how much time will he spend in a certain task. Based on his estimation, he could do pragmatic planning on analysis and design. He lists all main requirements, deciding whether he needs to learn new techniques. Then, he should write down a design spec in detail. He might utilize modern tools like UML during the process of analysis and design. The grogrammer also needs spend extra time to review his design spec before he could finally put his hand to implement. According to his requirements and cost estimation, he could lay down the coding standard for pratical development. By refining again and again, he finally comes out a concrete design of his expected program, and then, the code implement. The initial code implement must be full of bugs and amendable statements, so the programmer would review his code, and test and debug. He finally submisses a version which he thinks perfect, and could give a report introducing the performance of his program. Before finishing, He might measure the size of this task and summary details in it in order to improve his performance next time. A practical way for postmortem is using collected statistics, such as cost time noted down on the PSP sheet. The programmer might use an electronic clock to measure how much time he spends in a certain process, only in this way could he make relatively precise estimations.  
  
   
   
    
  
  
  [1]: http://static.zybuluo.com/twoer2/2jdadoaadkl3gv2dhwfxb9mb/image.png  
  [2]: http://static.zybuluo.com/twoer2/71uvd14do4srx0854npwnunj/image.png  
  [3]: http://static.zybuluo.com/twoer2/5oqbhnqsis8nkou9qkj1vbhv/image.png  
              



