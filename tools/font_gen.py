from pathlib import Path
import re,sys,os,freetype,kconfiglib,argparse

ASCII_PRINTABLE=''.join(chr(i) for i in range(0x20, 0x7F))
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
KCONFIG=os.path.join(ROOT, "Kconfig")
CONFIG=os.path.join(ROOT, ".config")
OUTPUT_DIR=os.path.join(ROOT, "generated", "font")
os.environ["srctree"]=ROOT

if sys.stdout.encoding!='utf-8':
    if hasattr(sys.stdout,'reconfigure'):
        sys.stdout.reconfigure(encoding='utf-8')
    else:
        import io
        sys.stdout=io.TextIOWrapper(sys.stdout.buffer,encoding='utf-8')

# -----------------------------------------------------------------------------
# 1. 从 FreeType mono 位图直接提取 1bpp 高位在前数据
# -----------------------------------------------------------------------------
def get_char_bitmap_mono(glyph) -> bytearray:
    w = glyph.bitmap.width
    h = glyph.bitmap.rows
    pitch = glyph.bitmap.pitch
    buf = glyph.bitmap.buffer

    packed = bytearray()
    bytes_per_row = (w + 7) // 8

    for y in range(h):
        row_start = y * pitch
        row_end = row_start + bytes_per_row
        packed.extend(buf[row_start:row_end])

    return packed

# -----------------------------------------------------------------------------
# 2. 统一接口：根据 bpp 选择处理方式
# -----------------------------------------------------------------------------
def get_char_bitmap(glyph, bpp: int) -> bytearray:
    if bpp == 8:
        w = glyph.bitmap.width
        h = glyph.bitmap.rows
        pitch = glyph.bitmap.pitch
        buf = glyph.bitmap.buffer
        raw = bytearray()
        for y in range(h):
            raw.extend(buf[y * pitch : y * pitch + w])
        return raw
    return get_char_bitmap_mono(glyph)

# -----------------------------------------------------------------------------
# 3. 写入C文件
# -----------------------------------------------------------------------------
def write_font_c_file(
    font_size: int,
    bpp: int,
    baseline: int,
    glyphs: list,
    bmp_data: bytearray,
    output_c: str
):
    with open(output_c, "w", encoding="utf-8") as f:
        f.write(f"// {font_size}px {bpp}bpp 高位在前 MSB\n")
        f.write("#include \"font/mgl_font.h\"\n\n")

        f.write(f"const uint8_t mgl_font_{font_size}px_bmp[] = {{\n")
        for i in range(0, len(bmp_data), 16):
            line = ", ".join(f"0x{x:02x}" for x in bmp_data[i:i+16])
            f.write(f"  {line},\n")
        f.write("};\n\n")

        f.write(f"const mgl_glyph_t mgl_font_{font_size}px_glyphs[] = {{\n")
        for cp, w, h, ox, oy, ad, ob in glyphs:
            f.write(f"    {{.code_point=0x{cp:04X},.w={w},.h={h},.ofs_x={ox},.ofs_y={oy},.adv={ad},.ofs_bmp={ob}}},\n")
        f.write("};\n\n")

        f.write(f"const mgl_font_t mgl_font_{font_size}px = {{\n")
        f.write(f"    .font_size={font_size},\n")
        f.write(f"    .bpp={bpp},\n")
        f.write(f"    .baseline={baseline},\n")
        f.write(f"    .glyph_count={len(glyphs)},\n")
        f.write(f"    .glyphs=mgl_font_{font_size}px_glyphs,\n")
        f.write(f"    .bmp=mgl_font_{font_size}px_bmp\n")
        f.write("};\n")

# -----------------------------------------------------------------------------
# 4. 生成单字号字体（增加异常捕获+过滤危险字符）
# -----------------------------------------------------------------------------
def generate_mgl_font(
    font_path: str,
    font_size: int,
    bpp: int,
    char_list: str,
    output_c: str = "mgl_font.c"
):
    if not os.path.exists(font_path):
        print(f"❌ 字体文件不存在：{font_path}")
        return

    ft = freetype.Face(font_path)
    ft.set_pixel_sizes(font_size, font_size)

    # 去重排序 + 过滤危险字符（防止光栅溢出）
    char_set = set(char_list)
    safe_chars = []
    for c in char_set:
        cp = ord(c)
        # 过滤控制字符、代理区、非法码点
        if cp < 0x20 or (0xD800 <= cp <= 0xDFFF) or cp > 0x10FFFF:
            continue
        safe_chars.append(c)
    char_list = sorted(safe_chars, key=lambda c: ord(c))

    glyphs = []
    bmp_data = bytearray()
    bmp_offset = 0

    for char in char_list:
        code = ord(char)
        load_flags = freetype.FT_LOAD_RENDER
        if bpp == 1:
            load_flags |= freetype.FT_LOAD_MONOCHROME

        # 捕获FreeType渲染崩溃，跳过坏字符
        try:
            ft.load_char(code, load_flags)
        except freetype.ft_errors.FT_Exception as e:
            print(f"⚠️ 跳过异常字符 '{char}' (0x{code:04X}) 错误: {e}")
            continue

        glyph = ft.glyph
        w = glyph.bitmap.width
        h = glyph.bitmap.rows
        ofs_x = glyph.bitmap_left
        ofs_y = -glyph.bitmap_top
        adv = glyph.advance.x // 64
        ofs_bmp = bmp_offset

        char_bmp = get_char_bitmap(glyph, bpp)
        bmp_data += char_bmp
        bmp_offset += len(char_bmp)
        glyphs.append((code, w, h, ofs_x, ofs_y, adv, ofs_bmp))

    if not glyphs:
        print(f"⚠️ {font_size}px 无有效字符，跳过生成")
        return

    baseline = max(-ofs_y for _, _, _, _, ofs_y, _, _ in glyphs)
    write_font_c_file(font_size, bpp, baseline, glyphs, bmp_data, output_c)
    print(f"✅ {font_size}px 生成完成：{len(glyphs)} 字符，位图 {len(bmp_data)} 字节")

# -----------------------------------------------------------------------------
# 扫描源码提取MGL_STR
# -----------------------------------------------------------------------------
def scan_sources(dirs, default_size: int):
    charsets = {}

    # 1. MGL_STR("文本") 无字号，默认大小
    pat_str = re.compile(r'MGL_STR\(\s*"([^"]*)"\s*\)', re.DOTALL)
    # 2. MGL_STR_SIZE("文本", 数字) 指定字号
    pat_str_size = re.compile(r'MGL_STR_SIZE\(\s*"([^"]*)"\s*,\s*(\d+)\s*\)', re.DOTALL)
    # 3. MGL_FMT("格式化串") 默认字号，自带数字
    pat_fmt = re.compile(r'MGL_FMT\(\s*"([^"]*)"\s*\)', re.DOTALL)
    # 4. MGL_FMT_SIZE("格式化串", 数字) 指定字号，自带数字
    pat_fmt_size = re.compile(r'MGL_FMT_SIZE\(\s*"([^"]*)"\s*,\s*(\d+)\s*\)', re.DOTALL)

    for d in dirs:
        base_dir = Path(d)
        if not base_dir.exists():
            print(f"⚠️ 目录不存在：{d}，跳过")
            continue
        for root, _, files in os.walk(base_dir):
            for f in files:
                if not f.endswith(('.c', '.h', '.cpp')):
                    continue
                file_path = Path(root) / f
                # 读取文件
                try:
                    with open(file_path, 'r', encoding='utf-8') as fh:
                        content = fh.read()
                except UnicodeDecodeError:
                    with open(file_path, 'r', encoding='gbk') as fh:
                        content = fh.read()

                hit_count = 0

                # 处理 MGL_STR
                for m in pat_str.finditer(content):
                    hit_count += 1
                    text = m.group(1)
                    size = default_size
                    if size not in charsets:
                        charsets[size] = set()
                    charsets[size].update(text)

                # 处理 MGL_STR_SIZE
                for m in pat_str_size.finditer(content):
                    hit_count += 1
                    text = m.group(1)
                    size = int(m.group(2))
                    if size not in charsets:
                        charsets[size] = set()
                    charsets[size].update(text)

                # 处理 MGL_FMT
                for m in pat_fmt.finditer(content):
                    hit_count += 1
                    text = m.group(1)
                    size = default_size
                    if size not in charsets:
                        charsets[size] = set()
                    charsets[size].update(text)
                    # 格式化字符串追加数字
                    charsets[size].update("0123456789")

                # 处理 MGL_FMT_SIZE
                for m in pat_fmt_size.finditer(content):
                    hit_count += 1
                    text = m.group(1)
                    size = int(m.group(2))
                    if size not in charsets:
                        charsets[size] = set()
                    charsets[size].update(text)
                    charsets[size].update("0123456789")

                if hit_count > 0:
                    print(f"✅ 文件命中 {hit_count} 处字符串宏：{file_path}")

    return charsets

# -----------------------------------------------------------------------------
# 批量生成所有字号
# -----------------------------------------------------------------------------
def generate_all(charsets,bpp:int,font_path:str):
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    for size, chars in sorted(charsets.items()):
        if not chars: continue
        output = os.path.join(OUTPUT_DIR, f"mgl_default_font_{size}px.c")
        generate_mgl_font(
            font_path = font_path,
            font_size = size,
            bpp       = bpp,
            char_list = ''.join(sorted(chars)),
            output_c  = output
        )
    return list(charsets.keys())

# -----------------------------------------------------------------------------
# 更新头文件extern声明
# -----------------------------------------------------------------------------
def update_header(sizes, default_size):
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    h_file=os.path.join(OUTPUT_DIR, f"mgl_font_gen.h")
    lines = [f"// Auto-generated by MINGL Font Generator",
            f"#ifndef MGL_FONT_GEN_H",
            f"#define MGL_FONT_GEN_H", 
            f'#include "font/mgl_font.h"',
            f"",
            f"#define MGL_STR(s) (s), .font = &mgl_font_{default_size}px",
            ]
    for sz in sorted(sizes): 
         lines+=[f"extern const mgl_font_t mgl_font_{sz}px;"]
    lines+=[f"#endif"]     
        
    with open(h_file, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')
        
        print(f"✅ 已生成 {h_file}")

# -----------------------------------------------------------------------------
# 程序入口
# -----------------------------------------------------------------------------
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--scan-path',default=None,help='强制指定源码扫描目录')
    args=parser.parse_args()

    kconf = kconfiglib.Kconfig(KCONFIG)
    if os.path.exists(CONFIG):
        kconf.load_config(CONFIG, replace=False)

    if args.scan_path:
        scan_path=args.scan_path
        if not os.path.isabs(scan_path):
            scan_path=os.path.join(ROOT,scan_path)
    else:
        scan_path=os.path.join(ROOT, kconf.syms["MGL_FONT_GEN_SCAN_PATH"].str_value)
    font_dir=os.path.join(ROOT, kconf.syms["MGL_FONT_GEN_FONT_PATH"].str_value)
    default_font=kconf.syms["MGL_FONT_GEN_DEFAULT_FONT"].str_value
    default_size=int(kconf.syms["MGL_FONT_GEN_DEFAULT_SIZE"].str_value)

    print("扫描目录:", scan_path)
    charsets = scan_sources([scan_path], default_size)
    if not charsets:
        print("未扫描到 MGL_STR/MGL_FMT")
        sys.exit(0)

    print(f"发现字号: {sorted(charsets.keys())}")
    for sz in sorted(charsets):
        print(f"  {sz}px: {len(charsets[sz])} 个字符")

    print("生成字体...")
    sizes = generate_all(charsets, 1, os.path.join(font_dir, default_font))

    print("更新头文件...")
    update_header(sizes, default_size)
    print("完成")