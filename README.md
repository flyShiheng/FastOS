# FastOS




# Debug
    make debug  
    # then auto in gdb  
    target remote localhost:1234
    file kernel.elf
    b kernel_main
    c
