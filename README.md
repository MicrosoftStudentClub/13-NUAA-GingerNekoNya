# Cattleya-the-Converter

---

## AMR2PCM for Microsoft Research RC

## Codename from "Violet Evergarden".

---

## 第一阶段开发完成:

实现了在win/linux下进行amr_nb/silk v3 to wav的操作

---

## Old Question:

一个问题：现在有两种Classic AMR模式：一种是AMR-NB，一种是AMR-WB；而且微信中所使用的AMR更像是SLIK-V3，那么应该怎么写好呢？

或许需要先完成Classic AMR，再完成SLIK-V3？

---

## 简要代码开发规范：


    // 变量命名
    e.g. frame_buffer

    // 函数/方法命名
    e.g.
    /**
    * 函数/方法的用途
    * @param   参数
    * @return  返回值
    */
    void readTextBuffer() {
        // 大括号空格不换行
    }