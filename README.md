# FlashMandelNG
Short: Mandelbrot & Julia fractals AOS4
Uploader: Dino Papararo (Dino.Papararo@Gmail.Com)
Author: Dino Papararo (Dino.Papararo@Gmail.com)/Edgar Schwan (eds@rz-online.de)
Type: gfx/fract
Version: 4.4 (25-Apr-2022)
Architecture: ppc-amigaos => 4.1
Requires: AmigaOS4 or compatible PPC machine with/out Altivec unit
Replaces: FlashMandelNG_OS4.lha

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

Flashmandel  was  focused  on OS2.x and OS3.x with 68k cpus, main critical parts
were written in 68k assembler. I remember latest 68k version was developed on my
A4000  with  Cyberstorm  PPC 604e/68060 mixing binaries between 68k and ppc code
under WarpOS. Later I sold all my Amiga stuff but after many years I  wanted  to
come back to my preferred computer and hobby, programming in C/ASM under WinUAE.

Sadly  after first entusiastic days I had to admint experience was not the same.
So I decided to come back for real with a true AmigaNG platform!  Sooner I got a
2nd  hand "Pegasos 2" board. Today I can boot three different OS, Amiga OS4.1fe,
MorphOS and Linux Debian. It's really a funny machine, much much better than any
emulated one and obviously any pc :-)

So my actual development system  is ->  Pegasos II G4@1GHz 2GB Ram - Radeon 9250
AmigaOS4.1fe update 2 with latest OS4 SDK and GCC 8.4.0

FlashMandel uses handmade assembler routines for max speed!   ..and "SPEED" it's
main  feature.  With  new  implemented  altivec  routines,  for altivec version,
renderings are all near realtime! I think it's one of the fastest mandelbrot and
julia  sets  rendering  program  ever  seen  on  a  single  core  cpu.  Due it's
development  for  AmigaOS  it  lacks  of  multiprocessing  features.  But   once
implemented, it can be adapted for sure to work on more cpu cores.

FlashMandelNG primary features are:

Uses  Reaction  GUI,  Arexx port, Localization, Arbitrary precision math via GMP
Library and don't need any installer, just click 'n run! C and Asm  sources  are
also  included,  you  can  recompile  it  just  typing MAKE! It's faaast even on
emulated Amigas and can render even with direct  memory  access  (TURBO  OPTION)
..under  x86  emuation  it's faster than many others x86 native fractal programs
;-) Saved pictures can  be  reloaded  and  further  explored  at  all  supported
screen's resolutions. It's really cool and totally free ;-)

Additional  developing  notes:  FlasdhMandelNG is developed with GCC 8.4.0 under
Pegasos II and AmigaOS 4.1fe. Altivec code  was  firstly  developed  and  tested
under  Powermac  G5  Quad hardware ..Now I can test it directly on my Pegasos II
;-) Altivec code is the most optimized ever seen  and  could  be  used  as  nice
programming  guide.  Many  many  thanks and greetings to my friend Edgar Schwan,
FlashMandel CoDeveloper, for  his  great  efforts  in  Localization,  Arexx  and
Reaction integration, GMP Library port, etc...

In  archive  we have two binaries: 
1) FlashMandelNG_OS4 -> critical routines are written in  PPC handmade assembler   
to gain max speed on all powerpc cpus families from 603 to G5!
2) FlashMandelNG_OS4_altivec -> critical routines are written in C code but with 
full Altivec directives, engine processes 4 pixels per time! Sadly due limited 
hardware implementation, precision is limited to float32 datatypes.

I'd like to add another binary for Tabor and its SPE unit! ...For now it was not
possibble due GCC v8 incompatibilities with SPE code ..maybe in the next release
we got it!

..so what version is the faster  one?  On  a  G4  latest  FlashMandelNG  Altivec
versions  are about 3x faster than FPU version because it processes 4 pixels per
time! ..but it's also less accurate due Float32  single  precision  calculations
(limited  by  hardware)  vs  Float64  FPU doubles, so for deep zooms you need to
switch sooner to high precision math calculations.

About precision one of main features  of  FlashMandel  is  the  ability  to  use
arbitrary precision math. To archieve this I used GMP library, Edgar did a great
job porting it from linux sources ..and now we have it also on Amiga systems :-)

FlashMandelNG_OS4 is Giftware, If you like it please sent me nice  goodies  ..An
email or common postcard is ok too! :-) It's possible to include it in magazines
and CDs, if so please let me to have a free copy of paper/media support ;-)

Version history:
New in 4.4  Now you can scroll fractal map with  keyboard arrow keys and zoom in  
and out with 'i' and 'o' keys.  Granularity can  be choosen pressing from '0' to
'9' on keyboard with a minimun of 1/12 up to 1/3.  Now LibGMP has been optimized 
for 'Altivec support', sadly speed increase is very minimal for Altivec version.
Small changes and enanchements.

New in 4.3  Maxed recursion algorithm up to 4x4 pixel blocks. Added new ToolType 
SMREQUESTER, set this to 0 to use ToolTypes parameters or 1 to choose resolution 
from Screenmode requester's list at startup.  Added new ToolType PLAYSOUND,  set 
this to "0" to avoid applause sound at end first rendering. Fixed mem allocation 
bug for orbit window. Linked against LibGMP 6.2.1 
Many other small internal changes, cleanups,small fixes/optimizations.

New in 4.2 Fixed Turbo Mode (rendering in memory) in 8 bit screens. Fixed bitmap 
scaling effect for zooms. Implemented use of Screen's "Custom Bitmaps"  and made 
some other internal changes.  OS4 "API calls" are used vs "standard C" functions 
whenever possible.

New in 4.1 Still many fixes and under the hood improvements.  Now a real "fading 
effect" is present also in 24bit screenmodes.

New in 4.0 Fixed ARexx functions and scripts. Fixed Load Palette function. Where 
possible OS4 API calls are replaced over C library standard functions.
Altivec binary doesn't crashes more on systems without an altivec unit and exits 
with a warning. In Altivec binary ARexx port is disabled and Altivec "Java/IEEE" 
mode is turned off because useless for our purposes, it was only an experiment.
Reindented, bugfixed and cleaned up code. Recompiled project against current SDK
V53.34 released for AmigaOS4.

New in 3.9 Bugfixed "Recalculation" and "Undo" functions, optimized Altivec code
and enabled Java/IEEE mode. Now Fading effect is present also in 24bit pictures.
Made other small fixes, enanchements and cleanups.
Raised default iterations from 320 to 4096, speed is only partially affected :-)

New in 3.8 Enabled Mouse Wheel "Zoom in" frame for a better granularity control.
Now you can press ESC key to exit when you have to choose Julia fractal type.

New in 3.7  Enabled  Middle  Mouse  Button to Zoom in the specified area. Better
handling of GT_GetIMsg and GT_ReplyIMsg intuition routines.

New in 3.6 Enabled GFX renderings also on 24bit screens, minor optimizations and
speedups.

New in 3.5  It's a bugfix release, now FlashMandel workbench startup and palette 
requester are fixed.

New in 3.4 Now load/save picture functions uses datatypes.library, an additional 
file with same picture name followed by ".fmng" extension is created to save and 
reload coordinates. Load picture is stil compatible with previous images. 

New in 3.3  Implemeted  load/save  picture  also for 24bit screens. Greetings to
to  Massimiliano Scarano  for his support into 24bit picture ilbm save function.

New in 3.2 Implemented 24 bit screens  support  for  truecolor  fractals.  Fixed
Histogram  coloring algorithm. Fixed and speeded up recursive drawing functions.
Minor code fixes and code clean up, now indentation follows GNU rules to 80 cols
To save 24 bit pictures you have to switch to 256 colors screen  due to a bug in
iff ilbm 24bit save function. It will be resoved hopefully in a next release.

New in 3.1 Implemented Histogram coloring algorithm  as  currently  decribed  on
wikipedia,  now  you can display a wider range of colors at same time. Recoded a
lot of stuff following the current best "C Programming practices", i.e. no  more
use  of  global  vars  for  inner functions. Fixed still some serious bugs, this
update is a "must have".

New in 3.0 Finally Reaction GUI is  fully  reimplemented,  Edgar  did  really  a
superb  work  about  this!  I  reworked  also  color remapping to gave some nice
alternative, i.e. try GUI remapping. A lot of  bugs  were  fixed  some  of  them
really dirty, I have also cleanup the code and gained some small speedup!

New in 2.9 Now Altivec C routines uses full intrinsics, generated code should be
very mear to full handmade asm. Small bugfixes and code cleanup.

New in 2.8 These are last days of COVID-19 quarantine in Italy, next  week  I'll
come  back  to  my daily work, ..so I decided to try to unlock once for all full
potential of Altivec  vector  unit  and...  I  did  it!  :-)  Now  FlashMandelNG
processes  4 pixels per time in the altivec version and it's nearly realtime for
zooms with low/mid iterations. One of the  most  important  issues  was  how  to
control Altivec power without loosing speed. So I took a challenge vs myself and
I won it! The my Altivec algorithm is fastest ever seen, preserving  full  power
and using a smart and efficient control for results! :-) Altivec version speeded
up to 3x!! Have fun!

New in v2.7 Minor speed improvements, I think to have reached tha max  possibile
performances  on  a G4. Only Altivec code can be highly speedup until to 4x, I'm
working on it! Fixed running multiple instances of  FlashMandel.  Now  only  RTG
screens  are  supported.  Recompiled  with GCC 8.4.0 and latest adtools. Various
minor bugfixes and improvements.

New in v2.6 Raised up GMP Library to actual latest version 6.2.0 (Edgar  Schwan)
Implemented periodicity checking to speed up renderig in black areas (Mandelbrot
set). Speeded up a bit Altivec version, we still calc one pixel  per  time,  I'd
like  to do 4 pixels per time using full Altivec power. Bugfixed rendering Z^n+C
with of power set to 16 or more. Raised max iterations to 2^31-1. Fixed Load and
Save pictures. Removed all debug symbols.

New  in v2.5 Due Covid19 virus in few weeks I did what I used to do in years due
my lack of time.. Finally I have implemented GMP Library  numeric  support.  Now
you  can  zoom  into  Mandelbrot  and Julia sets until 4096 precision's bits are
saturated. On current Amiga generations maybe you need to live for 500 years  to
saturate  all  precision  bits..  Eventually  I  can improve precision bits even
furter :-) I changed a bit the main recursion algorithm and now calculations are
about  10%  faster! I discovered and fixed some other annoyng bugs, at this time
only the Arexx and Reaction code parts and needs to  be  revisioned,  all  other
code  is all updated. At this time Reaction GUI isn't implemented because it was
developed with Reactor Tool. Sadly Reactor doesn't support PowerPc but only 68k,
so  object  file  generated  can't  be  linked  with  other sources and than GUI
fallback to system GadTools. If there will be ever a new release of Reactor with
PPC/OS4  support  I'll  be  happy  to reintroduce Reaction GUI and give again to
FlashMandel the right look! I decided also to renew iff chunk  format  of  saved
pictures, so now you can't load old fractals and continue zooming inside.

New  in  v2.3 Old heavy bug fixed - if Flashmandel didn't found a locale catalog
crashed at startup Other fixes and little improvements applied

New in v2.2 Now finally with a real Amiga and sadly in Covid-19 era in Italy,  I
had  a  lot  cpu  power, and a lot of time, to test and fully bugfix and cleanup
nearly all the code. Lowered resolution of all pictures to  1280x720  to  reduce
archive  size, old pictures are incompatible to this new version. Added some new
palettes and changed default one. Reviewed some Arexx scripts. Refreshed english
docs.

New in v2.1 Partial MorphOS compatibility via OS4EMU - Load and save didn't work
Reworked and eliminated all pre AGA stuff. Now we  have  3  different  binaries,
normal  GCC code, Altivec code and New PowerPC handmade asm version. Added DUTCH
catalog language - thanks to Bennymee for translation Minor fixes and changes.

New in v2.0 (Re)Enabled AREXX support (Please download and install AWNP_2-54.lha
archive from Aminet to let it run) Fixed tooltypes

To Do:
AmigaOne Tabor A1222 specific SPE version 

"Bottomless wonders spring from simple rules, which are repeated without end."
cit. Benoit Mandelbrot

 Enjoy,

Amiga Rulez!

