# 一个将hex文件，或s19文件格式转换成bin文件，并输出段信息
## 使用方法
> $./ToBin -?
>
> Usage: hex [OPTION]... [FILE]...
> [option]
>   -?, --help                    Display this help and exit
>   -v, --version                 Display version information and exit
>   -h,           Specify input hex file
>   -s,           Specify input S-record file
>   [FILE]...                     Specify input file

* 使用```-h or -s```选项指定输入文件格式是HEX还是s19格式
* 后加需要转换的文件名
* 将会生成同名加后缀的```.bin```和```.info```文件
  * 其中```.bin```文件为转换后的二进制文件
  * ```.info```文件中以文本的形式储存了文件的地址及数据大小

