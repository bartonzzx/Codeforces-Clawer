# Codeforces-Clawer 2023

2023年湘潭大学C语言课程作业

## 使用方法

1.将CodeforcesRatingClawer2023.exe、libcurl.dll、zlib1.dll、cjson.dll放在同一目录下

2.运行CodeforcesRatingClawer2023.exe，输入用户名，会自动获取用户数据，然后自动关闭(由于请求的是codeforce，可能需要科学上网)，在文件根目录下生成response.json和response_parsed.json

3.打开CodeforcesRatingClawer2023.html，在文件选择控件处,选中response_parsed.json，即可查看数据

## 开发方法

1.在visual studio中新建空项目

2.根据视频教程，安装包管理器vcpkg，通过vcpkg安装curl库、cjson库。方便在visual studio中配置c/c++配置第三方库

视频链接：(【五分钟掌握包管理！还在用难用的C语言/C++包管理？用VCPKG帮你便利第三方库的安装！以Curl第三方库为例演示VCPKG的安装和基本操作（文字简略版见简介）】(https://www.bilibili.com/video/BV1dM411g7a4?vd_source=0ebdc0a5050d0b187d206015b590c3e8)

3.在“源文件”中导入CodeforcesRatingClawer2023.cpp

4.打开cpp文件进行编辑

其他详细内容请见word文档

## 打包方法

推荐博客https://blog.csdn.net/m0_63509102/article/details/132416921

## 说明

* 关于数据准确性的说明:
  * 统计数据全部来源于Codeforces用户Submission记录,因此过题数存在部分重复现象;
  * 热力图数据全部来源于Codeforces用户Submission记录中的Accepted记录,且官方热力图数据统计规则较特殊,存在官方热力图不显示部分Submission记录,因此本热力图与官方热力图存在差别;

* 关于难度分布图内的分类说明：
  * problem rating <= 1000 : Easy
  * 1000 < problem rating <=1500 : Intermediate
  * 1500 < problem rating <=2000 : Moderate
  * 2000 < problem rating <=2500 : Challenging
  * 2500 < problem rating <=3000 : Difficult
  * 3000 < problem rating : Expert

## 当前问题

1."假后端"：由于是初学C语言时编写的，对前后端数据交互尚不熟悉，通过单独运行exe文件和在html中选择exe文件处理结果的形式实现了“假后端”，日后改进

2.未对重复做题的情况去重：由于有题型标签统计功能，如果同一道正确的题重复提交，该题对应的题型标签计数也会重复增加，而正确的题型统计只应该计数一次，需要通过哈希表或其他方法进行去重

## 运行截图

![image-20240520200408605](https://hila-1300222503.cos.ap-shanghai.myqcloud.com/md_image/image-20240520200408605.png)

![image-20240520200415951](https://hila-1300222503.cos.ap-shanghai.myqcloud.com/md_image/image-20240520200415951.png)

![image-20240520200424378](https://hila-1300222503.cos.ap-shanghai.myqcloud.com/md_image/image-20240520200424378.png)

![image-20240520200458656](https://hila-1300222503.cos.ap-shanghai.myqcloud.com/md_image/image-20240520200458656.png)
