
#include "emulator.h"

#include <string>
#include <vector>
#include <zlib.h>

#include "config.h"
#include "fceu/driver.h"
#include "fceu/fceu.h"
#include "fceu/types.h"
#include "fceu/utils/md5.h"
#include "fceu/version.h"
#include "fceu/state.h"

// Joystick data. I think used for both controller 0 and 1. Part of
// the "API".
static uint32 joydata = 0;
static bool initialized = false;

uint64 Emulator::RamChecksum() {
  md5_context ctx;
  md5_starts(&ctx);
  md5_update(&ctx, RAM, 0x800);
  uint8 digest[16];
  md5_finish(&ctx, digest);
  return *(uint64*)digest;
}
/**
 * Initialize all of the subsystem drivers: video, audio, and joystick.
 */
static int DriverInitialize(FCEUGI *gi) {
  // Used to init video. I think it's safe to skip.

  // Here we initialized sound. Assuming it's safe to skip,
  // because of an early return if config turned it off.

  // Used to init joysticks. Don't care about that.

  // No fourscore support.
  // eoptions &= ~EO_FOURSCORE;

  // Why do both point to the same joydata? -tom
  FCEUI_SetInput (0, SI_GAMEPAD, &joydata, 0);
  FCEUI_SetInput (1, SI_GAMEPAD, &joydata, 0);

  FCEUI_SetInputFourscore (false);
  return 1;
}

/**
 * Closes a game.  Frees memory, and deinitializes the drivers.
 */
int CloseGame() {
  FCEUI_CloseGame();
  GameInfo = 0;
  return 1;
}

/**
 * Loads a game, given a full path/filename.  The driver code must be
 * initialized after the game is loaded, because the emulator code
 * provides data necessary for the driver code(number of scanlines to
 * render, what virtual input devices to use, etc.).
 */
int LoadGame(const char *path) {
  CloseGame();
  if(!FCEUI_LoadGame(path, 1)) {
    return 0;
  }

  // Here we used to do ParseGIInput, which allows the gameinfo
  // to override our input config, or something like that. No
  // weird stuff. Skip it.

  // RefreshThrottleFPS();

  if(!DriverInitialize(GameInfo)) {
    return(0);
  }
	
  // Set NTSC (1 = pal)
  FCEUI_SetVidSystem(GIV_NTSC);

  return 1;
}

void Emulator::Shutdown() {
  CloseGame();
}

bool Emulator::Initialize(const string &romfile) {
  if (initialized) {
    fprintf(stderr, "Already initialized.\n");
    abort();
    return false;
  }

  int error;

  fprintf(stderr, "Starting " FCEU_NAME_AND_VERSION "...\n");

  // (Here's where SDL was initialized.)

  // Initialize the configuration system
  InitConfig();
  if (!global_config) {
    return -1;
  }

  // initialize the infrastructure
  error = FCEUI_Initialize();
  if (error != 1) {
    fprintf(stderr, "Error initializing.\n");
    return -1;
  }

  // (init video was here.)
  // I don't think it's necessary -- just sets up the SDL window and so on.

  // (input config was here.) InputCfg(string value of --inputcfg)

  // UpdateInput(g_config) was here.
  // This is just a bunch of fancy stuff to choose which controllers we have
  // and what they're mapped to.
  // I think the important functions are FCEUD_UpdateInput()
  // and FCEUD_SetInput
  // Calling FCEUI_SetInputFC ((ESIFC) CurInputType[2], InputDPtr, attrib);
  //   and FCEUI_SetInputFourscore ((eoptions & EO_FOURSCORE) != 0);
	
  // No HUD recording to AVI.
  FCEUI_SetAviEnableHUDrecording(false);

  // No Movie messages.
  FCEUI_SetAviDisableMovieMessages(false);

  // defaults
  const int ntsccol = 0, ntsctint = 56, ntschue = 72;
  FCEUI_SetNTSCTH(ntsccol, ntsctint, ntschue);

  // Set NTSC (1 = pal)
  FCEUI_SetVidSystem(GIV_NTSC);

  FCEUI_SetGameGenie(0);

  // Default. Sound thing.
  FCEUI_SetLowPass(0);

  // Default.
  FCEUI_DisableSpriteLimitation(1);

  // Defaults.
  const int scanlinestart = 0, scanlineend = 239;

  FCEUI_SetRenderedLines(scanlinestart + 8, scanlineend - 8, 
			 scanlinestart, scanlineend);


  {
    extern int input_display, movieSubtitles;
    input_display = 0;
    extern int movieSubtitles;
    movieSubtitles = 0;
  }

  // Load the game.
  if (1 != LoadGame(romfile.c_str())) {
    fprintf(stderr, "Couldn't load [%s]\n", romfile.c_str());
    return false;
  }


  // Default.
  newppu = 0;

  initialized = true;
}

// Make one emulator step with the given input.
// Bits from MSB to LSB are
//    RLDUTSBA (Right, Left, Down, Up, sTart, Select, B, A)
void Emulator::Step(uint8 inputs) {
  int32 *sound;
  int32 ssize;

  // The least significant byte is player 0 and
  // the bits are in the same order as in the fm2 file.
  joydata = (uint32) inputs;

  // Limited ability to skip video and sound.
  #define SKIP_VIDEO_AND_SOUND 2

  // Emulate a single frame.
  FCEUI_Emulate(NULL, &sound, &ssize, SKIP_VIDEO_AND_SOUND);
}

void Emulator::Save(vector<uint8> *out) {
  SaveEx(out, NULL);
}

void Emulator::GetBasis(vector<uint8> *out) {
  FCEUSS_SaveRAW(out);
}

void Emulator::Load(vector<uint8> *state) {
  LoadEx(state, NULL);
}

// Compression yields 2x slowdown, but states go from ~80kb to 1.4kb
// Without screenshot, ~1.3kb and only 40% slowdown
#define USE_COMPRESSION 1

#if USE_COMPRESSION

void Emulator::SaveEx(vector<uint8> *state, const vector<uint8> *basis) {
  // TODO
  // Saving is not as efficient as we'd like for a pure in-memory operation
  //  - uses tags to tell you what's next, even though we could already know
  //  - takes care for endianness; no point
  //  - might save some other write-only data (sound?)

  vector<uint8> raw;
  FCEUSS_SaveRAW(&raw);

  // Encode.
  int blen = (basis == NULL) ? 0 : (min(basis->size(), raw.size()));
  for (int i = 0; i < blen; i++) {
    raw[i] -= (*basis)[i];
  }

  // Compress.
  int error = Z_OK;
  uint8* cbuf = &raw[0];
  int len = raw.size();
  // worst case compression: zlib says "0.1% larger than sourceLen plus 12 bytes"
  uLongf comprlen = (len >> 9) + 12 + len;

  // Make sure there is contiguous space. Need room for header too.
  state->resize(4 + comprlen);

  if (Z_OK != compress2(&(*state)[4], &comprlen, &raw[0], len, Z_DEFAULT_COMPRESSION)) {
    fprintf(stderr, "Couldn't compress.\n");
    abort();
  }

  *(uint32*)&(*state)[0] = len;
  // fprintf(stderr, "comprlen: %d\n", comprlen);

  // Trim to what we actually needed.
  // PERF: This almost certainly does not actually free the memory. Might need to copy.
  state->resize(4 + comprlen);
}

void Emulator::LoadEx(vector<uint8> *state, const vector<uint8> *basis) {
  // Decompress. First word tells us the decompressed size.
  int uncomprlen = *(uint32*)&(*state)[0];
  vector<uint8> uncompressed;
  uncompressed.resize(uncomprlen);
  // fprintf(stderr, "uncompressed length: %d\n", uncomprlen);
 
  switch (uncompress(&uncompressed[0], (uLongf*)&uncomprlen,
		     &(*state)[4], state->size() - 4)) {
  case Z_OK: break;
  case Z_BUF_ERROR:
    fprintf(stderr, "Not enough room in output\n");
    abort();
    break;
  case Z_MEM_ERROR:
    fprintf(stderr, "Not enough memory\n");
    abort();
    break;
  default:
    fprintf(stderr, "Unknown decompression error\n");
    abort();
    break;
  }
  // fprintf(stderr, "After uncompression: %d\n", uncomprlen);
  
  // Why doesn't this equal the result from before?
  uncompressed.resize(uncomprlen);

  // Decode.
  int blen = (basis == NULL) ? 0 : (min(basis->size(), uncompressed.size()));
  for (int i = 0; i < blen; i++) {
    uncompressed[i] += (*basis)[i];
  }

  if (!FCEUSS_LoadRAW(&uncompressed)) {
    fprintf(stderr, "Couldn't restore from state\n");
    abort();
  }
}

#else

// When compression is disabled, we ignore the basis (no point) and
// don't store any size header. These functions become very simple.
void Emulator::SaveEx(vector<uint8> *state, const vector<uint8> *basis) {
  FCEUSS_SaveRAW(out);
}

void Emulator::LoadEx(vector<uint8> *state, const vector<uint8> *basis) {
  if (!FCEUSS_LoadRAW(state)) {
    fprintf(stderr, "Couldn't restore from state\n");
    abort();
  }
}


#endif