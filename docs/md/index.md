---
layout: home

hero:
  name: "MINGL"
  text: "声明即界面，编译即确定"
  tagline: "面向MCU与低性能Linux的嵌入式UI框架"
  actions:
    - theme: brand
      text: 进入
      link: /hal

features:
  - title: 声明式描述页面
    details: 纯 C 宏 DSL 描述界面，层次关系一目了然。无需手动管理控件生命周期，结构即结果。IDE 友好的自动补全。
  - title: 无动态内存
    details: 栈式静态内存池，零 malloc / free。所有配置在编译期完成，无碎片，运行行为完全可预测。
  - title: 精准局部刷新
    details: 多脏区域独立绘制 + 帧级跳过。空闲帧零 CPU 开销，变化帧仅重绘受影响的最小区域。
---

