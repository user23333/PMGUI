# PMGUI

## 前言

​	自己从D3D11底层开始写的一个GUI,只需要提供获取鼠标坐标函数(GetCursorPos)和获取鼠标按键状态函数(GetAsyncKeyState)即可使用,支持双进程异步绘制(AsyncRenderTask)

​	几乎把我用到的所有R3的技术都放上去了,整个GUI Runtime期间无堆内存申请,并且全程使用syscall调用系统API,兼容性极好

## 使用方法

​	本项目使用VS2017+DirectX SDK开发,建议使用VS2017编译

- demo: PMGUI演示Demo , 使用项目配置的Release X64方案编译
- GlyphFactor: PMGUI所需字体文件的生成器 , 使用项目配置的Release X86方案编译 

## 效果

考虑到项目大小的问题，demo自带的字体文件分辨率不高，所以大字会有点模糊。想要又大又清晰的文字的话需要将GlyphFactor的-S参数调大一些

![shot](./shot.png)
