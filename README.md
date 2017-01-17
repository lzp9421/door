# door
智能门禁系统

## 方案设计

0. MCU采用宏晶公司的STC89C52RC，特点：
 - 增强型8051单片机，指令代码完全兼容传统8051
 - 工作电压：5.5～3.3V，工作频率范围：0～40MHz
 - 拥有8KB的片内ROM和512字节的RAM，通用IO口32个
 - 价格便宜，可靠性相对较高。

0. 显示屏的选择LCD NOKIA5110
 - 价格便宜，屏幕显示可内容较多
 - 采用SPI总线，占IO较少，较容易驱动

0. 键盘用3x4矩阵键盘

0. 采用时钟芯片DS1302,特点:
 - 外接备用电源而掉电后仍能正常工作
 - 可对年月日星期时分秒进行计时，拥有闰年自动补偿功能

0. 掉电储存模块采用24C02,特点:
 - 具有256字节的储存容量的EEPROM
 - 采用IIC总线
 - 电压范围宽：1.8~5.5V
 - 高可靠性：擦写寿命100万次，数据保存时间100年

0. 门控制系统
 - 由于采用模型设计，所以选择分立元件的H桥电路驱动电机
 - 采用激光二极管和光敏电阻，通过阻断激光束来检测人的通过，低成本，可靠性高。


