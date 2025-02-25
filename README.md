# ESP32-S3 欧卡2组合开关
使用PlatformIO，包含了源代码和模型文件，可以自己DIY组合开关

## 1. 购买清单
**后面我会给出我的购买截图，请注意型号**
|              名称              | 数量  |
| :----------------------------: | :---: |
|         ESP32-S3开发板         |   1   |
|            104电容             |  16   |
|     加厚角码直角60x120x40      |   2   |
|      五菱荣光雨刷拨杆 A款      |   1   |
|      五菱荣光灯光拨杆 A款      |   1   |
|          4.8插簧线/条          |  13   |
|          M3*5杯头螺丝          |   8   |
|        M3x4x4.2热熔螺母        |   8   |
|           M5法兰螺母           |   4   |
|         M5*16杯头螺丝          |   4   |
|         M5*12*1平垫圈          |   4   |
|     10P航插(插头+插座一套)     |   1   |
|     8P航插(插头+插座一套)      |   1   |
|        5x0.15平方硅胶线        |   1   |
|        8x0.15平方硅胶线        |   1   |
| 醋酸胶布(非必须，用于固定线束) |   1   |

### 注意事项：
1.角码需要在远端打个5mm的孔以适配V99基座，千万不要买大一号的\
<img src="https://raw.githubusercontent.com/BoMei233/ESP32-S3-ETS2-Combination-switch/refs/heads/main/%E5%9B%BE%E7%89%87/%E8%A7%92%E7%A0%81%E6%89%93%E5%AD%94.jpg" alt="角码打孔孔位" width=400 height=300>

## 2. 组装步骤

### 2.1 转向灯开关
#### 2.1.1 转向灯开关插簧连接
    按照下图所示，将插簧线插入转向灯开关，插簧插入方向任意
<img src="https://raw.githubusercontent.com/BoMei233/ESP32-S3-ETS2-Combination-switch/refs/heads/main/%E5%9B%BE%E7%89%87/%E7%81%AF%E5%85%89%E6%8B%A8%E6%9D%86%E7%BA%BF%E5%BA%8F2.png" alt="灯光拨杆线序2" width=400 height=300>
<img src="https://raw.githubusercontent.com/BoMei233/ESP32-S3-ETS2-Combination-switch/refs/heads/main/%E5%9B%BE%E7%89%87/%E8%BD%AC%E5%90%91%E7%81%AF%E7%BA%BF%E6%9D%9F%E6%8E%A5%E6%B3%95.jpg" alt="转向灯线束接法" style="width: 300px; height: 300px; object-fit: cover;">\

#### 2.1.2 转向灯开关手柄线束连接
    下图给出了手柄五根线具体的作用，将两根GND与插簧线的GND并联在一起：
<img src="https://raw.githubusercontent.com/BoMei233/ESP32-S3-ETS2-Combination-switch/refs/heads/main/%E5%9B%BE%E7%89%87/%E7%81%AF%E5%85%89%E6%8B%A8%E6%9D%86%E7%BA%BF%E5%BA%8F1.jpg" alt="灯光拨杆线序1" style="width: 300px">\

至此，转向灯组合开关已经引出了8根线，将这8根线用“8x0.15平方硅胶线”延长，最后接到航空插头上。(请记住线序，航插上面会印有数字编号，这里买10P的用处是方便焊接，因为中间两根插针非常难焊接，因此使用四周的8根)

### 2.2 雨刮器开关
雨刮器相对简单，因为手柄没有线束，只需要插簧线。\
<img src="https://raw.githubusercontent.com/BoMei233/ESP32-S3-ETS2-Combination-switch/refs/heads/main/%E5%9B%BE%E7%89%87/%E9%9B%A8%E5%88%AE%E5%99%A8%E6%8B%A8%E6%9D%86%E7%BA%BF%E5%BA%8F.png" alt="雨刮器拨杆线序" style="width: 300px">\
现在将引出的5根线用“5x0.15平方硅胶线”延长，最后接到航空插头上。(请记住线序，航插上面会印有数字编号)

### 2.3 控制板组装
#### 2.3.1 控制板与洞洞板连接
这一步相对简单，只需要将航空插座上的引脚与开发板上的引脚对应连接起来就好，<span style="color: red;">记得在每个引脚与地之间并联一个104电容，这步非常重要！！！</span>如下图所示：
<img src="https://raw.githubusercontent.com/BoMei233/ESP32-S3-ETS2-Combination-switch/refs/heads/main/%E5%9B%BE%E7%89%87/%E6%8E%A7%E5%88%B6%E7%9B%92%E5%86%85%E9%83%A8.jpg" alt="控制盒内部" style="width: 300px">\
使用洞洞板固定，将开发板焊接至洞洞板上即可，注意，需要将洞洞板短边剪去一排孔位，将USB接口与洞洞板边缘对齐，否则在装壳后USB线将无法插入。\
洞洞板尺寸为50x65mm，不一致的可以使用剪刀和砂纸进行修正。\
<img src="https://raw.githubusercontent.com/BoMei233/ESP32-S3-ETS2-Combination-switch/refs/heads/main/%E5%9B%BE%E7%89%87/%E6%B3%A8%E6%84%8F%E5%B9%B3%E9%BD%90.jpg" alt="注意平齐" style="width: 300px">\

#### 2.3.2 控制板与航空插座连接
这步就是单纯连线，没什么特别要注意的，按照下面的表格进行连接，线序我想你应该在焊接插头的时候记住了，没记住的拿万用表量一下。

| 开发板引脚 | 开关功能  |
| :--------: | :-------: |
|     21     |   左转    |
|     47     |   右转    |
|     48     |   雾灯    |
|     45     |  玻璃水   |
|     35     |  远光灯   |
|     14     |  示廓灯   |
|     13     |   大灯    |
|     11     | 雨刮器HI  |
|     10     | 雨刮器LO  |
|     9      | 雨刮器INT |
|     3      |  超车灯   |

OK，直到现在，就差外壳没安装了，现在可以将程序烧写进开发板，然后进游戏里测试一下。

### 2.4 外壳安装
我将我的模型上传到项目里了，格式是STP，可以下载来直接打印，也能方便修改，模型内部有个箭头，表示了组合开关的安装方向，模型与角码使用两个M5螺丝与螺母进行连接，安装好后效果如图所示：\
<img src="https://raw.githubusercontent.com/BoMei233/ESP32-S3-ETS2-Combination-switch/refs/heads/main/%E5%9B%BE%E7%89%87/%E5%AE%89%E8%A3%85%E6%95%88%E6%9E%9C.jpg" alt="安装效果" style="width: 300px">\
