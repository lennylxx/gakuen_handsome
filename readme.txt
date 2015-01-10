使用方法:

1.首先用WinHex把src.fwd拆分。利用文件头之后offset表找到最后一部分，复制出来。
2.用parse.exe提取出文字。
3.翻译。保证行数相同。
4.处理翻译后的文本。利用查找替换把<ENDFILE> <LASTNAME> <FIRSTNAME> <UNKNOWN9802>都替换为换行符(\n)，然后删除文件中的空行。
5.使用make.exe生成中文bin文件。
6.用generate_ins_offset_table.exe和上一步生成的ins_length_list.txt生成新的指令offset表。
7.把文件合并起来。



ps：
  generate_header_code.exe的作用:
	在src.fwd的 offset=0x0c处开始有四个字节的code。
	此code是验证文件用的，并且arc.fwd和src.fwd相同。
	计算程序利用的是setting.txt中的Game.Title和Game.Author。