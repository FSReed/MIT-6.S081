# Notes, in commits:

1. [The boot of xv6](https://github.com/FSReed/MIT-6.S081/commit/fbdc15afe0bb752b38cf0efc00f50e58dadea051)
2. ![How users use system calls](https://github.com/FSReed/MIT-6.S081/commit/c277e73108fbfc53e72f754daf2a43eb15c1ce21) on branch `syscall`
3. [Setup kernel pagetable on boot](https://github.com/FSReed/MIT-6.S081/commit/f01f7f25d69795594c813c9b301574c84c1fa23f) on branch `pgtbl`
4. [Setup kernel address space](https://github.com/FSReed/MIT-6.S081/commit/93ac884e3f0129f6b96edaa83dc219800b96e5f9) on branch `pgtbl`
5. [Memory Allocator](https://github.com/FSReed/MIT-6.S081/commit/abe554a7e6fc5a551e63502bf728a467ad4b1d34) on branch `pgtbl`
6. [Syscall sbrk](https://github.com/FSReed/MIT-6.S081/commit/d01d4802bf6a42fc096cd74d62312435a0c00bb0) on branch `pgtbl`
7. *(To be reviewed)* [`exec`](https://github.com/FSReed/MIT-6.S081/commit/384052d0a5e4b9c8945f2ffde19a5efc6d8c4530) on branch `pgtbl`
8. [Traps from user space](https://github.com/FSReed/MIT-6.S081/commit/ecbbdb4b8207a0ed23aa18259a4bee4ec7d79e19) on branch `traps`
9. [System call arguments](https://github.com/FSReed/MIT-6.S081/commit/57ab726948c480f50afcc1e8cfbff173cffff2e1) on branch `traps`
10. [Traps from kernel space](https://github.com/FSReed/MIT-6.S081/commit/1ce3073b14fb976dabb996e54a1577bd8bd74fb0) on branch `traps`
11. [Copy-On-Write](https://github.com/FSReed/MIT-6.S081/commit/0a4142f4c40cf745b319be0d1533d904f669ddd0), `cow` lab of 2024! It took me about 9 hours in total to finish this lab for the second time. The given commit gathers all the changes to the project.  
    The progress of me doing this lab of 2020 can be seen on branch `cow`.
12. *(To be reviewed)* [Lazy Allocation (2020 Lab)](https://github.com/FSReed/MIT-6.S081/commit/330ee169ca092ee87cf934409d00f10dd596696f) on branch `lazy`.
    Pass `lazytests`, but some tests in `usertests` failed: `sbrkarg` and `sbrk8000`. `sbrkarg` failed on `write()` call, but it seems like no page-faults are captured by `usertrap`. I don't know why.
13. [Console input](https://github.com/FSReed/MIT-6.S081/commit/1e64b691928f43da128e15a2d7b852d984c6f702) on branch `net`

> xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix  
> Version 6 (v6).  xv6 loosely follows the structure and style of v6,  
> but is implemented for a modern RISC-V multiprocessor using ANSI C.  
>   
> ACKNOWLEDGMENTS  
>   
> xv6 is inspired by John Lions's Commentary on UNIX 6th Edition (Peer  
> to Peer Communications; ISBN: 1-57398-013-7; 1st edition (June 14,  
> 2000)). See also https://pdos.csail.mit.edu/6.828/, which  
> provides pointers to on-line resources for v6.  
>   
> The following people have made contributions: Russ Cox (context switching,  
> locking), Cliff Frey (MP), Xiao Yu (MP), Nickolai Zeldovich, and Austin  
> Clements.  
>   
> We are also grateful for the bug reports and patches contributed by  
> Takahiro Aoyagi, Silas Boyd-Wickizer, Anton Burtsev, Ian Chen, Dan  
> Cross, Cody Cutler, Mike CAT, Tej Chajed, Asami Doi, eyalz800, Nelson  
> Elhage, Saar Ettinger, Alice Ferrazzi, Nathaniel Filardo, flespark,  
> Peter Froehlich, Yakir Goaron,Shivam Handa, Matt Harvey, Bryan Henry,  
> jaichenhengjie, Jim Huang, Matúš Jókay, Alexander Kapshuk, Anders  
> Kaseorg, kehao95, Wolfgang Keller, Jungwoo Kim, Jonathan Kimmitt,  
> Eddie Kohler, Vadim Kolontsov , Austin Liew, l0stman, Pavan  
> Maddamsetti, Imbar Marinescu, Yandong Mao, , Matan Shabtay, Hitoshi  
> Mitake, Carmi Merimovich, Mark Morrissey, mtasm, Joel Nider,  
> OptimisticSide, Greg Price, Jude Rich, Ayan Shafqat, Eldar Sehayek,  
> Yongming Shen, Fumiya Shigemitsu, Cam Tenny, tyfkda, Warren Toomey,  
> Stephen Tu, Rafael Ubal, Amane Uehara, Pablo Ventura, Xi Wang, Keiichi  
> Watanabe, Nicolas Wolovick, wxdao, Grant Wu, Jindong Zhang, Icenowy  
> Zheng, ZhUyU1997, and Zou Chang Wei.  
>   
> The code in the files that constitute xv6 is  
> Copyright 2006-2020 Frans Kaashoek, Robert Morris, and Russ Cox.  
>   
> ERROR REPORTS  
>   
> Please send errors and suggestions to Frans Kaashoek and Robert Morris  
> (kaashoek,rtm@mit.edu). The main purpose of xv6 is as a teaching  
> operating system for MIT's 6.S081, so we are more interested in  
> simplifications and clarifications than new features.  
>   
> BUILDING AND RUNNING XV6  
>   
> You will need a RISC-V "newlib" tool chain from  
> https://github.com/riscv/riscv-gnu-toolchain, and qemu compiled for  
> riscv64-softmmu. Once they are installed, and in your shell  
> search path, you can run "make qemu".  
