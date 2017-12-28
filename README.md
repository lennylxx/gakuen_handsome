Codes and files used for the Chinese translation of Gakuen Handsome (学園ハンサム), which uses Famous Writer as its game engine.

Info: Famous Writer is a free gal game engine.


### Usage
1. Use WinHex to split `src.fwd` the binary archive into pieces, get header and offset table.
The last offset in the offset table indicates last entry's start address, copy from that address to the end of file into a separate binary file. This file would be the encoded game script you want to translate.
2. Use `parse.exe` to extract the subtitle lines from the script.
3. Translate. Make sure you keep the lines in the order of the original one with same line number.
4. Process the translated file manually. Search for and replace the special items `<ENDFILE> <LASTNAME> <FIRSTNAME> <UNKNOWN9802>` with line break symbol (`\n`), and then remove all the blank lines.
5. Use `make.exe` to generate encoded binary format of translated file.
6. Use `generate_ins_offset_table.exe` and the `ins_length_list.txt` which is from Step 5's output to generate a new ins/opcode offset table.
7. Combine all these files, including header, entry offset table, assets file list, new ins offset table, and encoded translated game script.

### FWD archive format

```c
#pragma pack(4)
struct fwd {
    struct fwd_header {
        unsigned char magic[12];
        unsigned char checksum[4];
        unsigned char unknown[16];
    } header;
    struct fwd_entry_offset {
        unsigned long padding;
        unsigned long offset;
    } entry_offsets [6];
};
```

### Other
`generate_header_code.exe` is used for calculating the checksum (File offset: `0x0c`) in `fwd_header` based on `Game.Title` and `Game.Author` in `setting.txt`.
This checksum is for file completion validation, copyright control or so. `arc.fwd` has this checksum too, same with `src.fwd`'s.
