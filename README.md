# <p align="center">步兵云台+底盘程序</p>
#### 1.软件功能：
- 控制云台及底盘的运动。
- 利用mpu6050原始数据实现惯性导航。
- 根据视觉数据预测目标运动情况。
- 麦克纳姆轮运动解算。

        
#### 2. 软件效果展示：


#### 3. 依赖环境:
&emsp;&emsp;win10,keil5
#### 4. 编译安装方式:
&emsp;&emsp;利用keil5自带编译工具进行程序编译及下载。

#### 5. 文件目录结构：
.
|-- LICENSE  
|-- README.md             
|-- image       //数据图片  
|-- gimbal      //云台程序  
|   |-- APP     //应用
|   |-- APP-dirver //应用驱动 
|   |-- BSP        //对MX生成的初始化程序再次封装
|   |-- Drivers  
|   |-- Inc  
|   |-- MDK-ARM    
|   |-- Middlewares  
|   |-- TASK        //freeRtos任务
|   |-- Src  
|   |-- Middlewares  
|--

#### 6. 软件与硬件的系统框图及流程图：
![image](https://github.com/shaoPeng24/robot/blob/master/image/2.png)
#### 7. 原理介绍与理论分析：　　
自瞄预判原理:   
&emsp;&emsp;已知摄像头与目标角度偏差：yaw，pitch，与自身的实际角度相加获得目标的绝对角度用x，y代替。  
以x轴为例：  
&emsp;&emsp;设目标相对于车的角速度为v，角加速度为a，预判的角度为Px，初始值都为0。程序运行周期为t。  
利用公式 Px=Px + vt +0.5at² 计算出预判的角度，与实际的x相比较然后更新v和a,下一个程序周期到来时重复上面步骤，最后Px和x的误差将会越来越小。说明v和a的值比较合适，也就达到了预测目标运动的角速度和角加速度的目的。  
&emsp;&emsp;这个方法和卡尔曼滤波的思想是相同的，只是把矩阵运算拆开了，这样写便于理解，同时可以简化代码量，便于调试，提高程序执行速度。
具体v和a的更新方法和pid比较相似，详见工程代码。

#### 8. 软件架构：
<img src="https://github.com/shaoPeng24/robot/blob/master/image/1.png"  width="80%" height="80%" align=center>

#### 9. 未来优化方向：
1. 提高pid的响应速度。
2. 结合视觉的距离信息做预判。

