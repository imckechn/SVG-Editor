This is a svg editor program.

This program has two parts. A Python interface and C Dynamic Library. The user runs the python project to run the program.

This program uses the libsxm2 library by GNOME (it's already inside the project) as well as the c linked list API by 'CIS*2750 S18 (based on the ListADT from CIS*2520, S17)'

This is program allows the user to: edit, create, delete, view rectangles and circles in an SVG image file. You can bring your own svg files by having them in the svgs directory or you can make your own through the program. The program will validate each file before running. All the edits and changes are tracked in a metaData file in JSON format which allows this to operater as a svg image repository. This repository contains some SVG files already to you don't have to find some for yourself, they are labeled a.svg through f.svg respectivly. You can validate each svg file on https://validator.w3.org/check if you wish.

To run!
type:
- $ make
- $ python3 src/main.py
