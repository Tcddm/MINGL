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

    sidebar:{ 
	"/":{
		base: "/",
        items:[
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
            text: '渲染器',
            collapsed: false,
            items: [
              { text: '渲染器', link: '/render' }
            ]
          }
		]
			
    }},
	
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
  }
})
