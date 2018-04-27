---  
layout: post  
title: 系统分析与设计 homework5  
date: 2018-04-10 12:00:10+00:00  
categories: 日志  
tags: 博客  
---  
  
# 工具使用报告：MySQL+pymysql  
我们的项目使用MySQL+Python3作为后端开发环境。为了通过Python管理数据库，我学习了pymysql库的使用方法。下面是一些常用方法的介绍。  
首先，我们需要在MySQL上建立一个叫`zyf_db`的数据库，在这个数据库中创建一个叫`zyf_table`的表。  
```  
CREATE TABLE IF NOT EXISTS `zyf_table` (  
`id` int unsigned not null auto_increment,  
`content` BIGINT(20) unsigned not null,  
PRIMARY KEY(`id`)  
) ENGINE = InnoDB DEFAULT CHARSET=utf8;  
```  
首先，安装PyMySQL库：  
```  
$ pip install PyMySQL  
```  
在操作数据库前，要进行连接数据库操作：  
```  
import pymysql  
  
db = pymysql.connect(host = 'localhost',  
user = 'zyf',  
passwd = 'xxx',  
db = 'zyf_db')  
```  
连接成功后，创建一个游标对象cursor：  
```  
cursor = db.cursor()  
```  
使用该游标对数据库进行操作。只需要把SQL语句传给`cursor.execute()`方法参数即可：  
```  
sql = 'SELECT * FROM zyf_table;'  
cursor.execute(sql)  
```  
如果SQL语句有一些自定义数据，可以使用`cursor.execute()`提供的格式化方法：  
```  
sql = 'SELECT * FROM zyf_table WHERE id = %s;'  
cursor.execute(sql, 1)  
```  
SQL可以分成两类：查询和删改。如果是前者，还需要用`cursor.fetchall()`方法获取返回结果：  
```  
results = cursor.fetchall()  
```  
这样results就包含了SQL查询语句的查询结果。  
对于删改操作，只需在`cursor.execute()`后，进行提交：  
```  
db.commit()  
```  
使用Python操作数据库完毕，记得关闭连接：  
```  
db.close()  
```  
下面是一个比较完整的操作例子：  
```  
import pymysql  
  
// 连接数据库  
db = pymysql.connect(host = 'localhost',  
user = 'zyf',  
passwd = 'xxx',  
db = 'zyf_db')  
  
// 获取游标  
cursor = db.cursor()  
  
// 创建一个表  
sql = '''CREATE TABLE IF NOT EXISTS `zyf_table` (  
`id` int unsigned not null auto_increment,  
`content` BIGINT(20) unsigned not null,  
PRIMARY KEY(`id`)  
) ENGINE = InnoDB DEFAULT CHARSET=utf8;'''  
cursor.execute(sql)  
  
// 插入一条数据，content值为6  
sql = INSERT INTO zyf_table(content) VALUES (%s);  
try:  
cursor.execute(sql, 6)  
db.commit()  
except:  
// 如果发生错误则回滚  
db.rollback()  
  
// 查询所有数据  
sql = 'SELECT * FROM zyf_table;'  
cursor.execute(sql)  
results = cursor.fetchall()  
// results为((1, 6),)  
  
// 关闭连接  
db.close()  
```  