Open-ED is a very simple and cheap open source Genesis/Megadrive cartridge. 
The cartridge does not have its own processor or FPGA, 
it is built in the old school style using only memory and discrete logic. 
Cartridge design uses an absolute minimum of inexpensive components.

Supported mappers:
ROM_4M		Most games work in this mode
ROM_2M+RAM	Some games with battery ram
ROM_4M+RAM	Beyond Oasis, Sonic-3 etc


Content description:
fabrication: pcb gerber files, bill of materials, menu binary
menu:	menu sources
pcb_src: pcb sources
schematic: cartridge schematics
flashkit-md.exe: flaskit software v1.0.0.1
open-ed.v: cartridge design in verilog
open-ed-mapper: cartridge mapper description
flashkit_mod.jpg: FlashKit mod required for open-ed programming
open-ed.jpg: cartridge photo


Menu programming
open-ed.md should be burned to the upper bank of flash memory (0x400000-0x40FFFF)
Menu can be programmed to the flash chip using any flash programmer or FlashKit.
FlashKit can program menu onboard (flashkit-md v1.0.0.1 required)
FlashKit have to be modified: 10k pullup to the cart pin B27(/VRES)
