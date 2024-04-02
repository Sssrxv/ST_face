# AI3101 memory map and flash partitions

<!-- vscode-markdown-toc -->
* 1. [AI3101 memory map](#AI3101memorymap)
	* 1.1. [Boot flow](#Bootflow)
* 2. [Flash Partition Introductions](#FlashPartitionIntroductions)
	* 2.1. [Fixed partitions](#Fixedpartitions)
	* 2.2. [Customized partitions](#Customizedpartitions)
* 3. [How to generate flash ROM image for factory product building?](#HowtogenerateflashROMimageforfactoryproductbuilding)
* 4. [How to dynamically update partition table?](#Howtodynamicallyupdatepartitiontable)

<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc -->


##  1. <a name='AI3101memorymap'></a>AI3101 memory map
The memory map layout of AI3101 is shown as below.
- `ROM`: non-volatile region in silicon for BOOT ROM, read only
- `SRAM0`: the first 256KB bank of SRAM
- `SRAM1`: the second 256KB bank of SARM
- `SRAM2`: the third 256KB bank of SRAM
- `HW registers`: HW register memory map region
- `DDR3`:  DDR memory region


###  1.1. <a name='Bootflow'></a>Boot flow
1. `BOOT ROM` loads `SPL(Secondary Program Loader)` into SRAM0 (0x10000000)
2. `SPL` loads firmware image into DDR3 (0x80000000)
3. Run firmware image in DDR3
 
```c
/*
0x00000000 -------> +-------------------------------------+
                    |     BOOT ROM      (8KB,   RO)       |
0x00002000 -------> +-------------------------------------+
                    .     (*)           (256M - 8KB)      .
0x10000000 -------> +-------------------------------------+
                    |     SRAM0         (256KB, RW)       |
0x10040000 -------> +-------------------------------------+
                    |     SRAM1         (256KB, RW)       |
0x10080000 -------> +-------------------------------------+
                    |     SRAM2         (256KB, RW)       |
0x100C0000 -------> +-------------------------------------+
                    .     (*)           (256M - 768KB)    .
0x20000000 -------> +-------------------------------------+
                    |     HW registers  (4MB, RO/WO/RW)   |
0x20400000 -------> +-------------------------------------+
                    .     (*)           (2GB- 4MB)        .
0x80000000 -------> +-------------------------------------+
                    |     DDR3          (128MB)           |
0x88000000 -------> +-------------------------------------+
                    .     (*)           (*)               .
0xFFFFFFFF -------> +-------------------------------------+
*/
```

##  2. <a name='FlashPartitionIntroductions'></a>Flash Partition Introductions

AIVA SDK splits the flash region into several partitions. Some of them are fixed, while the others could be customized.

###  2.1. <a name='Fixedpartitions'></a>Fixed partitions
  - `SPL (Secondary Program Loader)`. The address of SPL partition is fixed as 0x0. The first byte address of flash which will be automatically loaded by boot rom. The size of SPL is fixed as 256KB.
  - `Partition Tables`.  There are two partition tables, each of them is a reserve one to the other. The partition table describes the details of the customized partition layout.

```c
/*
0x00000000 -------> +-------------------------------------+
                    |     SPL           (256KB)           |
0x00040000 -------> +-------------------------------------+
                    |     Part Table0   (1KB)             |
                    +-------------------------------------+
                    .     (*)           (127KB)           .
0x00060000 -------> +-------------------------------------+
                    |     Part Table1   (1KB)             |
                    +-------------------------------------+
                    .     (*)           (127KB)           .
                    +-------------------------------------+
```
   
###  2.2. <a name='Customizedpartitions'></a>Customized partitions

Generally, the name and size of customized partitions could be changed according to the users' requirements except for the 'fw0', 'fw1' and 'fs'.

  - `fw0 (FW image0)`.  The SPL will try to find the partition named as `fw0` in the partition table. If it passed the verification, the SPL will boot from this firmware image. The size of `fw0` could be customized by configuration file.
  - `fw1 (FW image1)`.  The `fw1` works as the backup partition of `fw0`. It will be loaded by the SPL only if the first boot of `fw0` failed. The size of `fw1` could be customized by configuration file.
  - `fs (File System)`. If you are using the default file system of AIVA SDK, it will try to find the partition named as `fs` and mount it. If no partiton named as `fs` could be found, the mount operation would fail. The size of `fs` partition could be customized by configuration file.
  - For the other partitions, the name and size of these partitions are totally defined according to the users' requirements.

```c
/*
0x00080000 -------> +-------------------------------------+
                    |     fw0(FW image0)(xxMB)            |
0xXXX00000 -------> +-------------------------------------+
                    |     fw1(FW image1)(xxMB)            |
0xXXX00000 -------> +-------------------------------------+
                    |     fs(File system)(xxMB)           |
0xXXX00000 -------> +-------------------------------------+
                    |     cust_part0    (xxMB)            |
0xXXX00000 -------> +-------------------------------------+
                    |     cust_part1    (xxMB)            |
0xXXX00000 -------> +-------------------------------------+
                    |     (...)         (xxMB)            |
0xXXX00000 -------> +-------------------------------------+
*/
```

> NOTE: there should be no overlapping for these customized partitions. The user must calculate the offset and size of these partitions carefully!

##  3. <a name='HowtogenerateflashROMimageforfactoryproductbuilding'></a>How to generate flash ROM image for factory product building?
To build a factory flash image, you need 6 things:
1.  `SPL image`
2.  `FW images`
3.  `ROM configuration file`
4.  `swapdw tool`
5.  `bin2rom tool`
6.  `Linux OS PC`

The `SPL` image existed in the SDK folder. If you can't find it, ask your `FAE` to get it. 

The firmware images are files named as `fw0` and `fw1`. You need to use `swapdw` tool to generate them from your target image file which should be ended by '.img'.

> NOTE: there are two kinds of SPL. For NOR flash, please use `nor_spl.img`. For NAND flash, please use `nand_spl.img`

There is an example of ROM configuration file. The line starts with '#' will be considered as a comment line. The syntax of the usual line would be partition name, offset and size in bytes.

```sh
# `flash_part.cfg` file

# partition name, offset, size
# 1st part: fw region 0 (default)
fw0         0x00080000  0x00100000
# 2nd part: fw region 1 (backup)
fw1         0x00180000  0x00100000
# 3rd part: file system
fs          0x00280000  0x00100000
# 4th part: customized partition
cust0       0x00280000  0x00100000
```

Generate `fw0` and `fw1` files.
- assuming your project image is `a.img`, execute below command to generate `fw0` and `fw1`

```sh
$ ./swapdw a.img fw0
$ ./swapdw a.img fw1
```

Now copy all these files into one folder such as `ROM`, you should have below files in `ROM` folder.
```sh
- ROM
    |- nor_spl.img  # if using NAND flash, replace it with 'nand_spl.img'
    |- fw0
    |- fw1
    |- flash_part.cfg
    |- bin2rom
```

Execute below command in the `ROM` folder to generate ROM file.

If using NOR flash:

```sh
$ ./bin2rom nor_spl.img flash_part.cfg 0 nor_flash.rom
```

If using NAND flash:
```sh
$ ./bin2rom nand_spl.img flash_part.cfg 1 nor_flash.rom
```

> NOTE: generally, the ROM file generation script will be integrated into SDK Makefile. The user doesn't need to type these command to generate ROM file mannually. What the user needs to do is to change the ROM configuration files according to their flash layout designment.

##  4. <a name='Howtodynamicallyupdatepartitiontable'></a>How to dynamically update partition table?
If you want to change the default partition table designed by the ROM configuration file. You could run `part` command under your firmware shell.
- Create a new partition: run `part create [name] [offset] [size]`
- Update a existing partition: run `part update [name] [offset] [size]`
- Delete all partitions: `part del_all`
- List all partitions: `part ls`
- Reset all partitons to the SDK-defined default values: `part default`

> NOTE: please make sure that there is no partiton overlapping if you changed the default partition table.

```sh
    _    _____     ___        
   / \  |_ _\ \   / / \     
  / _ \  | | \ \ / / _ \    
 / ___ \ | |  \ V / ___ \    
/_/   \_\___|  \_/_/   \_\ 

Build:       May 11 2022 12:44:25
Version:     3.0.5
Copyright:   (c) 2020 aiva

aiva:/$ part
Usage: part [OPTIONS] [ARGS] 
   [OPTIONS]: 
      - create : create a partition,       ARGS: [name], [offset], [size] 
      - update : update a partition,       ARGS: [name], [offset], [size] 
      - del_all: delete all partitions,    ARGS: none
      - ls     : list all partions,        ARGS: none
      - default: init default part tables, ARGS: none

aiva:/$ part ls
---------- List partitions ----------
0x00080000-0x0017ffff: Name ('fw0     '), Size (1024KB)
0x00180000-0x0027ffff: Name ('fw1     '), Size (1024KB)
0x00f80000-0x00ffffff: Name ('fs      '), Size (512KB)
-------- Total 3 partitions ---------

```