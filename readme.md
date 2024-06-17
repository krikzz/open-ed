# Open-ED

Open-ED is a very simple and cheap open source Genesis/Megadrive cartridge.
The cartridge does not have its own processor or FPGA, 
it is built in the old school style using only memory and discrete logic. 
Cartridge design uses an absolute minimum of inexpensive components.

Supported mappers:<br/>
ROM_4M		Most games work in this mode<br/>
ROM_2M+RAM	Some games with battery ram<br/>
ROM_4M+RAM	Beyond Oasis, Sonic-3 etc<br/>


Content description:<br/>
fabrication: pcb gerber files, bill of materials, menu binary<br/>
menu:	menu sources<br/>
pcb_src: pcb sources<br/>
schematic: cartridge schematics<br/>
flashkit-md.exe: flaskit software v1.0.0.1<br/>
open-ed.v: cartridge design in verilog<br/>
open-ed-mapper: cartridge mapper description<br/>
flashkit_mod.jpg: FlashKit mod required for open-ed programming<br/>
open-ed.jpg: cartridge photo<br/>


Menu programming<br/>
open-ed.md should be burned to the upper bank of flash memory (0x400000-0x40FFFF)<br/>
Menu can be programmed to the flash chip using any flash programmer or FlashKit<br/>
FlashKit can program menu onboard (flashkit-md v1.0.0.1 required)<br/>
FlashKit have to be modified: 10k pullup to the cart pin B27(/VRES)<br/>

![Open-ED PCB](https://raw.githubusercontent.com/krikzz/open-ed/main/open-ed.jpg)
