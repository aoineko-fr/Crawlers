// ____________________________
// ██▀▀█▀▀██▀▀▀▀▀▀▀█▀▀█        │  ▄▄▄       ▄  ▄▄    ▄▄   ▄▄▄▄           ▄▄
// ██  ▀  █▄  ▀██▄ ▀ ▄█ ▄▀▀ █  │  ██▄▀ ██ █ ▄  ██   ▄██    ██  ▄█▀▄ ▄█▀▄ ██
// █  █ █  ▀▀  ▄█  █  █ ▀▄█ █▄ │  ██▄▀ ▀█▄█ ██ ▀█▄ ▀▄██    ██  ▀█▄▀ ▀█▄▀ ▀█▄
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀────────┘  
//  by Guillaume 'Aoineko' Blanchard under CC BY-SA license
//─────────────────────────────────────────────────────────────────────────────

//*****************************************************************************
// TOOLS SETTINGS
//*****************************************************************************

Emulator = `${ToolsDir}OpenMSX/openmsx`;
// Emulator = `${ToolsDir}Emulicious/Emulicious`;
// Emulator = `${ToolsDir}BlueMSX/blueMSX`;
// Emulator = `${ToolsDir}MEISEI/meisei`;
// Emulator = `${ToolsDir}fMSX/fMSX`;
// Emulator = `${ToolsDir}RuMSX/MSX`;
// Debugger = `${ToolsDir}OpenMSX/Debugger/openmsx-debugger`;

//*****************************************************************************
// PROJECT SETTINGS
//*****************************************************************************

//-- Project name (will be use for output filename)
ProjName = "crawlers";

//-- Project modules to build (use ProjName if not defined)
ProjModules = [ ProjName ];

//-- List of modules to link
LibModules = [ "compress/pletter", "arkos/akg_player", "game_menu", "device/ninjatap", "fsm", "vdp", "input", "memory", "math", "print" ];

//-- MSX machine version:
// - 1		MSX 1
// - 2		MSX 2
// - 12		MSX 1 or 2 (dual support)
// - 2K		Korean MSX 2 (SC9 support)
// - 2P		MSX 2+
// - TR		MSX Turbo-R
// - 3		MSX 3 (reserved)
Machine = "12";

//-- Program media target:
// - BIN			.bin	BASIC binary program (8000h~)
// - ROM_8K			.rom	8KB ROM in page 1 (4000h ~ 5FFFh)
// - ROM_8K_P2		.rom	8KB ROM in page 2 (8000h ~ 9FFFh)
// - ROM_16K		.rom	16KB ROM in page 1 (4000h ~ 7FFFh)
// - ROM_16K_P2		.rom	16KB ROM in page 2 (8000h ~ BFFFh)
// - ROM_32K		.rom	32KB ROM in page 1-2 (4000h ~ BFFFh)
// - ROM_48K		.rom	48KB ROM in page 0-2 (0000h ~ BFFFh). Pages 1-2 visible at start
// - ROM_48K_ISR	.rom	48KB ROM in page 0-2 (0000h ~ BFFFh). Pages 0-2 visible at start
// - ROM_64K		.rom	64KB ROM in page 0-3 (0000h ~ FFFFh). Pages 1-2 visible at start
// - ROM_64K_ISR	.rom	64KB ROM in page 0-3 (0000h ~ FFFFh). Pages 0-2 visible at start
// - ROM_ASCII8		.rom	128KB ROM using ASCII-8 mapper
// - ROM_ASCII16	.rom	128KB ROM using ASCII-16 mapper
// - ROM_KONAMI		.rom	128KB ROM using Konami mapper (8KB segments)
// - ROM_KONAMI_SCC	.rom	128KB ROM using Konami SCC mapper (8KB segments)
// - DOS1			.com	MSX-DOS 1 program (0100h~) No direct acces to Main-ROM
// - DOS2			.com	MSX-DOS 2 program (0100h~) No direct acces to Main-ROM
Target = "ROM_48K_ISR";

//-- ROM mapper size (from 64 to 4096). Must be a multiple of 8 or 16 depending on the mapper type
ROMSize = 0;

//-- Postpone the ROM startup to let the other ROMs initialize (BDOS for example) (true or false)
ROMDelayBoot = false;

//-- RAM in slot 0 and install ISR there (true or false)
InstallRAMISR = false;

//-- Type of custom ISR (for RAM or ROM)
// - VBLANK		V-blank handler
// - VHBLANK	V-blank and h-blank handler (V9938 or V9958)
// - V9990		v-blank, h-blank and command end handler (V9990)
CustomISR = "VBLANK";

//-- Use banked call and trampoline functions (true or false)
BankedCall = false;

//-- Overwrite RAM starting address (e.g. 0xE0000 for 8K RAM machine)
ForceRamAddr = 0;

//-- Data to copy to disk (comma separated list)
DiskFiles = [];

//-- Add application signature to binary data (true or false)
AppSignature = true;

//-- Application company
AppCompany = "PP";

//-- Application ID (0~65535)
AppID = "CR";

//*******************************************************************************
// MAKE SETTINGS
//*******************************************************************************

//-- Use static MSXgl library (true or false)
BuildLibrary = true;

//-- Debug flag (true or false)
Debug = false;

//-- Move debug symbols do binary folder (true or false)
DebugSymbols = true;

//-- Assembler code optimizer
// - None
// - PeepHole	SDCC otpimizer
// - MDL		MDL z80 otpimizer
AsmOptim = "None";

//-- Optim:
// - Default
// - Speed
// - Size
Optim = "Speed";

//-- Additionnal compilation flag
CompileOpt = "";

//-- Skip file if compile data is newer than the (true or false)
CompileSkipOld = false;

//-- Compile verbose mode (true or false)
Verbose = true;

//*******************************************************************************
// EMULATOR SETINGS
//*******************************************************************************

//-- Emulator options (true or false):
EmulMachine = false;
Emul60Hz = false;
EmulFullScreen = false;
EmulMute = false;
EmulDebug = false

//-- Emulator extensions (true or false):
EmulSCC = false;
EmulMSXMusic = false;
EmulMSXAudio = false;
EmulPSG2 = false;
EmulV9990 = false;

//-- Emulator port: joystick, mouse, keyboard (fake joystick)
EmulPortA = "";
EmulPortB = "";

//-- Emulator extra parameters to be add to command-line (emulator sotfware specific)
// EmulExtraParam=-command "plug joyporta ninjatap" -command "plug ninjatap_port_1 joystick1"
EmulExtraParam = '-command "plug joyporta ninjatap"';

//*******************************************************************************
// BUILD STEPS
//*******************************************************************************

DoClean   = false;
DoCompile = true;
DoMake    = true;
DoPackage = true;
DoDeploy  = true;
DoRun     = true;

//*****************************************************************************
// START BUILD
//*****************************************************************************

LogFile = true;
LogFileName = "log.txt";