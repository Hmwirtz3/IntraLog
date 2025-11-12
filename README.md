This project uses the intraFont library by BenHur, licensed under CC BY-SA 3.0.


<img width="480" height="274" alt="image" src="https://github.com/user-attachments/assets/692a1d90-1c94-43a0-a134-aaba93b5211a" />

IntraLog is a tiny wrapper for intraFont, that aids in logging for real hardware and PPSSPP. It prints word wrapped log lines and has an FPS overlay that prints directly on top of your frame using IntraFont. 

Features:
Drop in logging for C++ projects that supports printf stylle arguments.
Word wrapped overlay.
FPS overlay.
Easy stubbing to compile out Logs per file (#include "intraLogNULL.h" instead of #include "intraLog.h")

Requirements:
PSPSDK,
intraFont (included in repo)

See sample for usage. 

Thanks to BenHurr for intraFont! IntraFont was not altered in any way.
