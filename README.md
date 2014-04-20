ANSI-ficator
============

###DESCRIPTION

ANSI-ficator is a program that will convert images to ANSI graphics with colors,
displaying the generated ANSI image in the program's window.
You can then save the generated image to a file.
	
It's being developed in C++ with Qt 4.8.1.
	
It supports input of all common image formats, including PNG, JPG, GIF and BMP.
It stretches the image you load to the needed resolution, optionally, you can
tell the program not to do this.
Currently, converted images may be saved as image files. (other kinds of output is planned.)

The utility converts images to a matrix of ANSI characters, using different background
and foreground colors for each character, of a palette of 16 colors. It's based upon the text
mode display commonly associated with the "DOS era".
	
In more detail, the converter creates graphics based upon the text modes on
VGA display devices, or devices compatible with the VGA standard. But the text mode format is also found
on earlier IBM standards like the EGA and CGA, the EGA expanded the features of the CGA and the VGA
expanded on the VGA, in turn.
The VGA standard is still a "lowest common denominator" on graphics devices on today's computers,
in other words, they more or less support displaying the modes of the VGA standard. Which is why you see
"DOS"-like text upon a computer booting, which is done in VGA text mode, it's also used for example, in the
first phase of the Windows XP installer. This program convert images to graphics that could be displayed
on the VGA text mode.

The default setting creates an image using a set of 256 characters, in 16 color resolution,
with a screen resolution of 80 x 25 characters (Width x Height).
Each character can have one of the 16 colors for Foreground and Background.
The palette is based on the default 16-color palette of the VGA device.

Under the "Format" menu there are currently two rows to choose between two different
color formats. The lower option  "80 x 25 - (16 FG / 8 BG color)" is the same as the upper,
except it's based on the case where each character is limited to 8 different BG colors.
A VGA device can be configured so a certain bit of data for each character,
is either used to describe the 4th (upper) bit of the BG color, or a On/Off state about
the character blinking. You can't have both at the same time. The VGA setting defines
this for the whole screen. The 8-color setting in this program's menus corresponds to
the VGA configuration where the bit tells the blinking state of characters. Incidently,
this VGA setting is the one used by MS QuickBasic, where you can only pick between 8 BG
colors when printing characters in "Screen 0" mode (a VGA text mode).

External links for reading, to learn more about IBM's VGA (and other) standards:

- https://en.wikipedia.org/wiki/Video_Graphics_Array 
- https://en.wikipedia.org/wiki/Enhanced_Graphics_Adapter 
- https://en.wikipedia.org/wiki/Color_Graphics_Adapter

###SOURCE CODE NOTES

The files "chart.bmp" and "trees.jpg" found in /ansificator must
be placed in the path that the program starts in. When running from Qt this seems to be in
the base path of the active build setting. When running the binary outside Qt they must
be in the binary's folder. "chart.bmp" is loaded on startup, "trees.jpg" is just a sample
image and is not vital.

###OTHER

This is my first programming project for a very long while that is meant for mainstream use,
and the first project I wrote in Qt, that I've released.

Enjoy the software!
	
###CONTACT

ingegjoestoel@hotmail.com 
https://github.com/Ingeager

####*(to be updated with binary downloads)*
