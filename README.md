FlashMandel is "THE" Mandelbrot and Julia fractals  rendering  program  for  the
AmigaOS4 platforms :-) It's a very long time project, development started in the
end of '90 when I was sixteen and learned about fractals reading and looking  on
specialized newspapers some beautiful pictures.  Early develpoment started on an
Amiga 2000 with KS1.3 1MB Ram and SAS/C compiler. FlashMandel was  developed  by
me, Dino Papararo, in my freetime to learn and improve my skills with C/ASM code
and AmigaOS specific API functions. In a second time joined Edgar Schwan  adding
a  lot  of  interesting  features  like  reaction  GUI, arexx ports and scripts,
localization, MAPM arbitray precision math, installer script,  a  dedicated  web
page, and many various bug fixes.

So my actual development system  is ->  Pegasos II G4@1GHz 2GB Ram - Radeon 9250
AmigaOS4.1fe update 2 with latest OS4 SDK and GCC 11.2.0

FlashMandel uses handmade assembler routines for max speed!   ..and "SPEED" it's
main  feature.  With  new  implemented  altivec  routines,  for altivec version,
renderings are all near realtime! I think it's one of the fastest mandelbrot and
julia  sets  rendering  program  ever  seen  on  a  single  core  cpu.  Due it's
development  for  AmigaOS  it  lacks  of  multiprocessing  features.  But   once
implemented, it can be adapted for sure to work on more cpu cores.

FlashMandelNG primary features are:

Uses  Reaction  GUI,  Arexx port, Localization, Arbitrary precision math via GMP
Library and don't need any installer, just click 'n run! C and Asm  sources  are
also included.  You can recompile it just installing latest AmigaOS4 SDK and GMP 
static LIB included in FalshMandelNG. Copy content in GCC local newlib path just
as like any third party lib add on.  FlashMandelNG it's faaast even on  QEMU/UAE
emulated Amigas and can render even with direct  memory  access  (TURBO  OPTION)
..under  x86  emuation  it's faster than many others x86 native fractal programs
;-) Saved pictures can  be  reloaded  and  further  explored  at  all  supported
screen's resolutions. It's really cool and totally free ;-)

In archive we have four binaries: 
1) FlashMandelNG_OS4 -> critical routines are written in  PPC handmade assembler   
to gain max speed on all powerpc cpus families from 603 to G5!
2) FlashMandelNG_OS4_altivec -> critical routines are written in C code but with 
full Altivec directives, engine processes 4 pixels per time!  Sadly  due limited 
hardware implementation, precision is limited to float32 datatypes.  For Altivec
version ARexx port is disabled, please consider it as an experimental version.
3) FlashMandelNG_OS4_spe  critical routines are written in C code but transalted 
in SPE native code, it should be faster than FPU version. ** Untested version **
4) FlashMandel_OS4_lite  is like standard version but whitout Reaction and Arexx
support, it can have better compatibility under emulated environments like QEMU.

..so what version is the faster  one?  On  a  G4  latest  FlashMandelNG  Altivec
versions  are about 3x faster than FPU version because it processes 4 pixels per
time! ..but it's also less accurate due Float32  single  precision  calculations
(limited  by  hardware)  vs  Float64  FPU doubles, so for deep zooms you need to
switch sooner to higher precision GMP Library math calculations.
In version 4.5 I have introduced A1222/Tabor compatiiblity with full SPE support
this version should be faster than FPU one and slower than Altivec one.
Anyway it's only supposed because I'm not an owner of A1222/Tabor machine and so
it's all still untested and could not work as expected!

About precision, one of main features of FlashMandel,  is  the  ability  to  use
arbitrary precision math. To archieve this I used GMP library, Edgar did a great
job porting it from linux sources ..and now we have it also on Amiga systems :-)

FlashMandelNG_OS4 is Giftware, If you like it please sent me nice  goodies  ..An
email or common postcard is ok too! :-) It's possible to include it in magazines
and CDs, if so please let me to have a free copy of paper/media support ;-)

Version 4.9 changes:
Added 16 bit 'HiColor' screenmodes, mainly for QEMU and 'low end' systems users.

***

"Bottomless wonders spring from simple rules, which are repeated without end."
cit. Benoit Mandelbrot
 Enjoy,


Amiga Rulez!
