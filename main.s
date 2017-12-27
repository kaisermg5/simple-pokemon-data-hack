        .gba
        .thumb
        .open input_game,"build/rom.gba", 0x08000000
        
        .org 0x08800000
        .importobj "build/linked.o"
        .close
