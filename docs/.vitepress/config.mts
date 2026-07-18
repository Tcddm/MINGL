import { defineConfig } from 'vitepress'

// https://vitepress.dev/reference/site-config
export default defineConfig({
  srcDir: "md",
  base: '/MINGL/',
  title: "MINGL",
  description: "声明即界面，编译即确定",
  themeConfig: {
    // https://vitepress.dev/reference/default-theme-config
    nav: [
      { text: '首页', link: '/' },
      { text: 'Releases', link: 'https://github.com/Tcddm/MINGL/releases' }
    ],

    sidebar: {
      "/": {
        base: "/",
        items: [
		{
            text: 'MINGL',
            collapsed: false,
            items: [
              { text: '架构总览', link: '/core/architecture' },
              { text: '核心工具 API', link: '/core/api' },
              { text: '类型表', link: '/core/types' }
            ]
          },
          {
            text: '硬件抽象层（HAL）',
            collapsed: false,
            items: [
              { text: '硬件抽象层（HAL）移植指南', link: '/hal' }
            ]
          },
          {
            text: '裁剪系统（CTX）',
            collapsed: false,
            items: [
              { text: '裁剪系统概述', link: '/ctx' },
              { text: '基础绘制', link: '/ctx/base' },
              { text: '填充多边形', link: '/ctx/fill_polygon' },
              { text: '绘制圆弧', link: '/ctx/arc' },
              { text: '绘制圆', link: '/ctx/circle' },
              { text: '绘制圆角矩形', link: '/ctx/round_rect' },
              { text: '绘制位图', link: '/ctx/bitmap' },
              { text: '绘制文本', link: '/ctx/text' }
            ]
          },
          {
            text: '子系统',
            collapsed: false,
            items: [
              { text: '渲染器', link: '/subsystem/render' },
			  { text: '页面内存池', link: '/subsystem/pool' },
			  { text: '事件系统', link: '/subsystem/event' },
			  { text: '动作系统', link: '/subsystem/action' },
			  { text: '日志系统', link: '/subsystem/logger' },
			  { text: '画笔系统', link: '/subsystem/painter' },
			  { text: '主题系统', link: '/subsystem/theme' },
			  { text: '字体系统', link: '/subsystem/font' }
            ]
          },
          {
            text: '控件与声明式DSL',
            collapsed: false,
            items: [
              { text: '概述', link: '/widget' },
			  { text: '页面定义指南', link: '/widget/define_page' },
              {
                text: '内置控件',
                collapsed: false,
                items: [
                  { text: 'MGL_LINEAR_LAYOUT', link: '/widget/layout/linear_layout' },
				  { text: 'MGL_LABEL', link: '/widget/label/label' },
				  { text: 'MGL_BUTTON', link: '/widget/button/button' },
				  { text: 'MGL_SLIDER', link: '/widget/slider/slider' },
				  { text: 'MGL_SCROLLBAR', link: '/widget/scrollbar/scrollbar' },
				  { text: 'MGL_LIST', link: '/widget/list/list' }
                ]
              }
            ]
          },
		  {
            text: '工具',
            collapsed: false,
            items: [
			  { text: '图形化配置', link: '/tools/kconfig' },
              { text: '字体生成', link: '/tools/font_gen' }
			  
            ]
          }
        ]
      }
    },

    socialLinks: [
      { icon: 'github', link: 'https://github.com/Tcddm/MINGL' }
    ],
    outline: [2, 3]
  },
  markdown: {
    container: {
      tipLabel: '提示',
      warningLabel: '警告',
      dangerLabel: '危险',
      infoLabel: '信息',
      detailsLabel: '详细信息'
    }
  },
})