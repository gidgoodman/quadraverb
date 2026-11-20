// Alesis QuadraVerb factory program names (00-99) mapped onto this plugin's parameters.
// The unit's real program data is not public in the manual, so values are approximations from each name.
#pragma once

constexpr int kNumPresets = 100;
struct PV { const char* id; float v; };
struct Preset { const char* name; const PV* pv; int n; };

static const PV kP0[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 52.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 250.0f }, { "dtr", 250.0f }, { "dfl", 20.0f }, { "dfr", 20.0f }, { "dinmix", 0.0f }, { "dylevel", 99.0f }, { "pmode", 0.0f }, { "pspeed", 51.0f }, { "pdepth", 40.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f }, { "dsel", 1.0f }, { "eqlevel", 64.0f }, { "elf", 100.0f }, { "ela", 0.0f }, { "ehf", 6000.0f }, { "eha", 2.0f } };
static const PV kP1[] = { { "config", 4.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 0.0f }, { "rdecay", 59.0f }, { "rhf", -35.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "pdepth", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 64.0f }, { "elf", 100.0f }, { "ela", 2.0f }, { "ehf", 6000.0f }, { "eha", 3.0f } };
static const PV kP2[] = { { "config", 4.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 0.0f }, { "rdecay", 59.0f }, { "rhf", -35.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "pdepth", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 64.0f }, { "elf", 100.0f }, { "ela", -2.0f }, { "ehf", 6000.0f }, { "eha", -4.0f } };
static const PV kP3[] = { { "config", 0.0f }, { "dlevel", 54.0f }, { "master", 67.0f }, { "rtype", 1.0f }, { "rdecay", 42.0f }, { "rhf", -20.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rgate", 1.0f }, { "rghold", 195.0f }, { "rgrel", 40.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 54.0f }, { "elf", 100.0f }, { "ela", 2.0f }, { "ehf", 6000.0f }, { "eha", 3.0f } };
static const PV kP4[] = { { "config", 0.0f }, { "dlevel", 50.0f }, { "master", 74.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "pmode", 0.0f }, { "pspeed", 51.0f }, { "pdepth", 59.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP5[] = { { "config", 0.0f }, { "dlevel", 50.0f }, { "master", 74.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "pmode", 3.0f }, { "pspeed", 33.0f }, { "pdepth", 89.0f }, { "pfb", 50.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP6[] = { { "config", 0.0f }, { "dlevel", 59.0f }, { "master", 59.0f }, { "rlevel", 0.0f }, { "dtype", 0.0f }, { "dtl", 330.0f }, { "dtr", 330.0f }, { "dfl", 40.0f }, { "dfr", 40.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f } };
static const PV kP7[] = { { "config", 0.0f }, { "dlevel", 50.0f }, { "master", 74.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "pmode", 4.0f }, { "pspeed", 47.0f }, { "pdepth", 30.0f }, { "pfb", 20.0f }, { "pdet", 12.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP8[] = { { "config", 1.0f }, { "dlevel", 0.0f }, { "master", 74.0f }, { "rtype", 1.0f }, { "rdecay", 31.0f }, { "rhf", -40.0f }, { "rin1", 4.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "lzspeed", 2.0f }, { "lzout", 99.0f }, { "lzsep", 60.0f } };
static const PV kP9[] = { { "config", 3.0f }, { "dlevel", 0.0f }, { "master", 0.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 99.0f }, { "elf", 100.0f }, { "ela", 4.0f }, { "emf", 1000.0f }, { "ema", -2.0f }, { "ehf", 6000.0f }, { "eha", 4.0f } };
static const PV kP10[] = { { "config", 0.0f }, { "dlevel", 69.0f }, { "master", 45.0f }, { "rtype", 2.0f }, { "rdecay", 24.0f }, { "rhf", -50.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP11[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 2.0f }, { "rdecay", 34.0f }, { "rhf", -45.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP12[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 2.0f }, { "rdecay", 45.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP13[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 0.0f }, { "rdecay", 48.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP14[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 0.0f }, { "rdecay", 59.0f }, { "rhf", -35.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP15[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 0.0f }, { "rdecay", 70.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP16[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 0.0f }, { "rdecay", 73.0f }, { "rhf", -50.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP17[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 38.0f }, { "rhf", -15.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 64.0f }, { "elf", 100.0f }, { "ela", 0.0f }, { "ehf", 6000.0f }, { "eha", 3.0f } };
static const PV kP18[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 52.0f }, { "rhf", -15.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 64.0f }, { "elf", 100.0f }, { "ela", 0.0f }, { "ehf", 6000.0f }, { "eha", 3.0f } };
static const PV kP19[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 66.0f }, { "rhf", -15.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 64.0f }, { "elf", 100.0f }, { "ela", 0.0f }, { "ehf", 6000.0f }, { "eha", 3.0f } };
static const PV kP20[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 3.0f }, { "rdecay", 45.0f }, { "rhf", -59.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP21[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 3.0f }, { "rdecay", 52.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP22[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 3.0f }, { "rdecay", 59.0f }, { "rhf", -45.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP23[] = { { "config", 0.0f }, { "dlevel", 50.0f }, { "master", 74.0f }, { "rtype", 1.0f }, { "rdecay", 28.0f }, { "rhf", -20.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rgate", 1.0f }, { "rghold", 135.0f }, { "rgrel", 40.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP24[] = { { "config", 0.0f }, { "dlevel", 50.0f }, { "master", 74.0f }, { "rtype", 1.0f }, { "rdecay", 34.0f }, { "rhf", -15.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rgate", 1.0f }, { "rghold", 165.0f }, { "rgrel", 40.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 50.0f }, { "elf", 100.0f }, { "ela", 0.0f }, { "ehf", 6000.0f }, { "eha", 6.0f } };
static const PV kP25[] = { { "config", 0.0f }, { "dlevel", 54.0f }, { "master", 67.0f }, { "rtype", 1.0f }, { "rdecay", 38.0f }, { "rhf", -30.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rgate", 1.0f }, { "rghold", 180.0f }, { "rgrel", 40.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP26[] = { { "config", 0.0f }, { "dlevel", 50.0f }, { "master", 74.0f }, { "rtype", 1.0f }, { "rdecay", 31.0f }, { "rhf", -20.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rgate", 1.0f }, { "rghold", 150.0f }, { "rgrel", 40.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 50.0f }, { "elf", 100.0f }, { "ela", 3.0f }, { "ehf", 6000.0f }, { "eha", 0.0f } };
static const PV kP27[] = { { "config", 0.0f }, { "dlevel", 54.0f }, { "master", 67.0f }, { "rtype", 1.0f }, { "rdecay", 45.0f }, { "rhf", -30.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rgate", 1.0f }, { "rghold", 210.0f }, { "rgrel", 40.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP28[] = { { "config", 0.0f }, { "dlevel", 54.0f }, { "master", 67.0f }, { "rtype", 1.0f }, { "rdecay", 34.0f }, { "rhf", -20.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rgate", 1.0f }, { "rghold", 165.0f }, { "rgrel", 40.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "pmode", 2.0f }, { "pspeed", 37.0f }, { "pdepth", 79.0f }, { "pfb", 50.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP29[] = { { "config", 0.0f }, { "dlevel", 54.0f }, { "master", 67.0f }, { "rtype", 1.0f }, { "rdecay", 38.0f }, { "rhf", -25.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rgate", 1.0f }, { "rghold", 180.0f }, { "rgrel", 40.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "pmode", 2.0f }, { "pspeed", 28.0f }, { "pdepth", 89.0f }, { "pfb", 50.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP30[] = { { "config", 0.0f }, { "dlevel", 50.0f }, { "master", 74.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "pmode", 2.0f }, { "pspeed", 37.0f }, { "pdepth", 79.0f }, { "pfb", 50.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP31[] = { { "config", 0.0f }, { "dlevel", 50.0f }, { "master", 74.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "pmode", 2.0f }, { "pspeed", 22.0f }, { "pdepth", 99.0f }, { "pfb", 50.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP32[] = { { "config", 0.0f }, { "dlevel", 50.0f }, { "master", 74.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "pmode", 2.0f }, { "pspeed", 84.0f }, { "pdepth", 69.0f }, { "pfb", 50.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP33[] = { { "config", 0.0f }, { "dlevel", 50.0f }, { "master", 74.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "pmode", 1.0f }, { "pspeed", 47.0f }, { "pdepth", 50.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP34[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 2.0f }, { "rdecay", 31.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "pmode", 0.0f }, { "pspeed", 47.0f }, { "pdepth", 50.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP35[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 52.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "pmode", 0.0f }, { "pspeed", 47.0f }, { "pdepth", 50.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP36[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rlevel", 0.0f }, { "dtype", 2.0f }, { "dtl", 300.0f }, { "dtr", 300.0f }, { "dfl", 30.0f }, { "dfr", 30.0f }, { "dinmix", 0.0f }, { "dylevel", 99.0f }, { "pmode", 0.0f }, { "pspeed", 47.0f }, { "pdepth", 50.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP37[] = { { "config", 0.0f }, { "dlevel", 50.0f }, { "master", 74.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "pmode", 4.0f }, { "pspeed", 42.0f }, { "pdepth", 20.0f }, { "pfb", 20.0f }, { "pdet", 15.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP38[] = { { "config", 1.0f }, { "dlevel", 0.0f }, { "master", 74.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "lzspeed", 1.0f }, { "lzout", 99.0f }, { "lzsep", 60.0f } };
static const PV kP39[] = { { "config", 1.0f }, { "dlevel", 0.0f }, { "master", 74.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "lzspeed", 2.0f }, { "lzout", 99.0f }, { "lzsep", 60.0f } };
static const PV kP40[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rlevel", 0.0f }, { "dtype", 2.0f }, { "dtl", 167.0f }, { "dtr", 167.0f }, { "dfl", 30.0f }, { "dfr", 30.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f } };
static const PV kP41[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rlevel", 0.0f }, { "dtype", 2.0f }, { "dtl", 250.0f }, { "dtr", 250.0f }, { "dfl", 30.0f }, { "dfr", 30.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f } };
static const PV kP42[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rlevel", 0.0f }, { "dtype", 2.0f }, { "dtl", 500.0f }, { "dtr", 500.0f }, { "dfl", 30.0f }, { "dfr", 30.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f } };
static const PV kP43[] = { { "config", 2.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rlevel", 0.0f }, { "dtype", 2.0f }, { "dtl", 1500.0f }, { "dtr", 1500.0f }, { "dfl", 30.0f }, { "dfr", 30.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f } };
static const PV kP44[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rlevel", 0.0f }, { "dtype", 1.0f }, { "dtl", 300.0f }, { "dtr", 600.0f }, { "dfl", 35.0f }, { "dfr", 35.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f } };
static const PV kP45[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rlevel", 0.0f }, { "dtype", 2.0f }, { "dtl", 250.0f }, { "dtr", 250.0f }, { "dfl", 40.0f }, { "dfr", 40.0f }, { "dinmix", 0.0f }, { "dylevel", 99.0f }, { "pmode", 2.0f }, { "pspeed", 33.0f }, { "pdepth", 79.0f }, { "pfb", 50.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP46[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rlevel", 0.0f }, { "dtype", 2.0f }, { "dtl", 350.0f }, { "dtr", 350.0f }, { "dfl", 45.0f }, { "dfr", 45.0f }, { "dinmix", 0.0f }, { "dylevel", 99.0f }, { "pmode", 2.0f }, { "pspeed", 28.0f }, { "pdepth", 79.0f }, { "pfb", 50.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP47[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rlevel", 0.0f }, { "dtype", 2.0f }, { "dtl", 280.0f }, { "dtr", 280.0f }, { "dfl", 40.0f }, { "dfr", 40.0f }, { "dinmix", 0.0f }, { "dylevel", 99.0f }, { "pmode", 2.0f }, { "pspeed", 37.0f }, { "pdepth", 69.0f }, { "pfb", 50.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP48[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 4.0f }, { "rdecay", 45.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 400.0f }, { "dtr", 400.0f }, { "dfl", 50.0f }, { "dfr", 50.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f } };
static const PV kP49[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 4.0f }, { "rdecay", 38.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 450.0f }, { "dtr", 450.0f }, { "dfl", 50.0f }, { "dfr", 50.0f }, { "dinmix", 0.0f }, { "dylevel", 99.0f }, { "pmode", 4.0f }, { "pspeed", 47.0f }, { "pdepth", 30.0f }, { "pfb", 20.0f }, { "pdet", 50.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP50[] = { { "config", 0.0f }, { "dlevel", 74.0f }, { "master", 37.0f }, { "rtype", 1.0f }, { "rdecay", 20.0f }, { "rhf", -50.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 74.0f }, { "elf", 100.0f }, { "ela", 6.0f }, { "ehf", 6000.0f }, { "eha", 2.0f } };
static const PV kP51[] = { { "config", 0.0f }, { "dlevel", 59.0f }, { "master", 59.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "pmode", 0.0f }, { "pspeed", 42.0f }, { "pdepth", 30.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f }, { "dsel", 1.0f }, { "eqlevel", 59.0f }, { "elf", 100.0f }, { "ela", 4.0f }, { "ehf", 6000.0f }, { "eha", -2.0f } };
static const PV kP52[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 48.0f }, { "rhf", -30.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 64.0f }, { "elf", 100.0f }, { "ela", 0.0f }, { "ehf", 6000.0f }, { "eha", 3.0f } };
static const PV kP53[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 56.0f }, { "rhf", -25.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 64.0f }, { "elf", 100.0f }, { "ela", 0.0f }, { "ehf", 6000.0f }, { "eha", 3.0f } };
static const PV kP54[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 45.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 200.0f }, { "dtr", 200.0f }, { "dfl", 20.0f }, { "dfr", 20.0f }, { "dinmix", 0.0f }, { "dylevel", 99.0f }, { "pmode", 0.0f }, { "pspeed", 47.0f }, { "pdepth", 25.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP55[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 45.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 380.0f }, { "dtr", 380.0f }, { "dfl", 40.0f }, { "dfr", 40.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f } };
static const PV kP56[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 28.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 300.0f }, { "dtr", 300.0f }, { "dfl", 25.0f }, { "dfr", 25.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 64.0f }, { "elf", 100.0f }, { "ela", 0.0f }, { "ehf", 6000.0f }, { "eha", 3.0f } };
static const PV kP57[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 38.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 400.0f }, { "dtr", 400.0f }, { "dfl", 35.0f }, { "dfr", 35.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f } };
static const PV kP58[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 45.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 300.0f }, { "dtr", 300.0f }, { "dfl", 30.0f }, { "dfr", 30.0f }, { "dinmix", 0.0f }, { "dylevel", 99.0f }, { "pmode", 0.0f }, { "pspeed", 47.0f }, { "pdepth", 20.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP59[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 31.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 150.0f }, { "dtr", 150.0f }, { "dfl", 20.0f }, { "dfr", 20.0f }, { "dinmix", 0.0f }, { "dylevel", 99.0f }, { "pmode", 0.0f }, { "pspeed", 42.0f }, { "pdepth", 50.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP60[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 0.0f }, { "rdecay", 62.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "pmode", 0.0f }, { "pspeed", 37.0f }, { "pdepth", 30.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP61[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 38.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 64.0f }, { "elf", 100.0f }, { "ela", 0.0f }, { "ehf", 6000.0f }, { "eha", 4.0f } };
static const PV kP62[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 34.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 120.0f }, { "dtr", 120.0f }, { "dfl", 15.0f }, { "dfr", 15.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 64.0f }, { "elf", 100.0f }, { "ela", 2.0f }, { "ehf", 6000.0f }, { "eha", 3.0f } };
static const PV kP63[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 0.0f }, { "rdecay", 59.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "pmode", 0.0f }, { "pspeed", 40.0f }, { "pdepth", 69.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP64[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 0.0f }, { "rdecay", 66.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "pmode", 0.0f }, { "pspeed", 47.0f }, { "pdepth", 79.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP65[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 38.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 200.0f }, { "dtr", 200.0f }, { "dfl", 20.0f }, { "dfr", 20.0f }, { "dinmix", 0.0f }, { "dylevel", 99.0f }, { "pmode", 0.0f }, { "pspeed", 45.0f }, { "pdepth", 59.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP66[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 62.0f }, { "rhf", -30.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 64.0f }, { "elf", 100.0f }, { "ela", 0.0f }, { "ehf", 6000.0f }, { "eha", 2.0f } };
static const PV kP67[] = { { "config", 1.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 45.0f }, { "rhf", -40.0f }, { "rin1", 4.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "lzspeed", 2.0f }, { "lzout", 99.0f }, { "lzsep", 60.0f } };
static const PV kP68[] = { { "config", 0.0f }, { "dlevel", 50.0f }, { "master", 74.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "pmode", 4.0f }, { "pspeed", 37.0f }, { "pdepth", 30.0f }, { "pfb", 20.0f }, { "pdet", 10.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP69[] = { { "config", 0.0f }, { "dlevel", 50.0f }, { "master", 74.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "pmode", 4.0f }, { "pspeed", 37.0f }, { "pdepth", 50.0f }, { "pfb", 20.0f }, { "pdet", 18.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP70[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 0.0f }, { "rdecay", 59.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rpre", 120.0f }, { "rpremix", 99.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 120.0f }, { "dtr", 120.0f }, { "dfl", 0.0f }, { "dfr", 0.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f } };
static const PV kP71[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 0.0f }, { "rdecay", 73.0f }, { "rhf", -50.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 600.0f }, { "dtr", 600.0f }, { "dfl", 54.0f }, { "dfr", 54.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f } };
static const PV kP72[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 4.0f }, { "rdecay", 59.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 80.0f }, { "dtr", 80.0f }, { "dfl", 10.0f }, { "dfr", 10.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f } };
static const PV kP73[] = { { "config", 0.0f }, { "dlevel", 50.0f }, { "master", 74.0f }, { "rtype", 0.0f }, { "rdecay", 62.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "pmode", 4.0f }, { "pspeed", 47.0f }, { "pdepth", 30.0f }, { "pfb", 20.0f }, { "pdet", 50.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP74[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 42.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "pmode", 4.0f }, { "pspeed", 51.0f }, { "pdepth", 79.0f }, { "pfb", 20.0f }, { "pdet", 15.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP75[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 0.0f }, { "rdecay", 79.0f }, { "rhf", -30.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "pmode", 0.0f }, { "pspeed", 28.0f }, { "pdepth", 30.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP76[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rlevel", 0.0f }, { "dtype", 2.0f }, { "dtl", 350.0f }, { "dtr", 350.0f }, { "dfl", 40.0f }, { "dfr", 40.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f } };
static const PV kP77[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rlevel", 0.0f }, { "dtype", 2.0f }, { "dtl", 350.0f }, { "dtr", 350.0f }, { "dfl", 35.0f }, { "dfr", 35.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f } };
static const PV kP78[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 45.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 300.0f }, { "dtr", 300.0f }, { "dfl", 35.0f }, { "dfr", 35.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f } };
static const PV kP79[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 52.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 300.0f }, { "dtr", 300.0f }, { "dfl", 30.0f }, { "dfr", 30.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f } };
static const PV kP80[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 31.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 64.0f }, { "elf", 100.0f }, { "ela", 3.0f }, { "ehf", 6000.0f }, { "eha", 5.0f } };
static const PV kP81[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 45.0f }, { "rhf", -59.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f }, { "dsel", 1.0f }, { "eqlevel", 64.0f }, { "elf", 100.0f }, { "ela", 4.0f }, { "ehf", 6000.0f }, { "eha", -2.0f } };
static const PV kP82[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 0.0f }, { "rdecay", 66.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 450.0f }, { "dtr", 450.0f }, { "dfl", 59.0f }, { "dfr", 59.0f }, { "dinmix", 0.0f }, { "dylevel", 99.0f }, { "pmode", 4.0f }, { "pspeed", 47.0f }, { "pdepth", 30.0f }, { "pfb", 20.0f }, { "pdet", 50.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP83[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 52.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 350.0f }, { "dtr", 350.0f }, { "dfl", 35.0f }, { "dfr", 35.0f }, { "dinmix", -99.0f }, { "dylevel", 99.0f }, { "plevel", 0.0f } };
static const PV kP84[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 0.0f }, { "rdecay", 70.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 300.0f }, { "dtr", 300.0f }, { "dfl", 30.0f }, { "dfr", 30.0f }, { "dinmix", 0.0f }, { "dylevel", 99.0f }, { "pmode", 4.0f }, { "pspeed", 42.0f }, { "pdepth", 50.0f }, { "pfb", 20.0f }, { "pdet", 10.0f }, { "pin", 1.0f }, { "plevel", 99.0f }, { "dsel", 1.0f }, { "eqlevel", 64.0f }, { "elf", 100.0f }, { "ela", 0.0f }, { "ehf", 6000.0f }, { "eha", 2.0f } };
static const PV kP85[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 48.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 220.0f }, { "dtr", 220.0f }, { "dfl", 30.0f }, { "dfr", 30.0f }, { "dinmix", 0.0f }, { "dylevel", 99.0f }, { "pmode", 0.0f }, { "pspeed", 47.0f }, { "pdepth", 59.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f }, { "dsel", 1.0f }, { "eqlevel", 64.0f }, { "elf", 100.0f }, { "ela", 0.0f }, { "ehf", 6000.0f }, { "eha", 4.0f } };
static const PV kP86[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 52.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", -99.0f }, { "rlevel", 99.0f }, { "dylevel", 0.0f }, { "pmode", 4.0f }, { "pspeed", 47.0f }, { "pdepth", 30.0f }, { "pfb", 20.0f }, { "pdet", 12.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP87[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 52.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 300.0f }, { "dtr", 300.0f }, { "dfl", 30.0f }, { "dfr", 30.0f }, { "dinmix", 0.0f }, { "dylevel", 99.0f }, { "pmode", 0.0f }, { "pspeed", 47.0f }, { "pdepth", 59.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP88[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 52.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 300.0f }, { "dtr", 300.0f }, { "dfl", 30.0f }, { "dfr", 30.0f }, { "dinmix", 0.0f }, { "dylevel", 99.0f }, { "pmode", 0.0f }, { "pspeed", 33.0f }, { "pdepth", 84.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f } };
static const PV kP89[] = { { "config", 0.0f }, { "dlevel", 64.0f }, { "master", 52.0f }, { "rtype", 1.0f }, { "rdecay", 52.0f }, { "rhf", -40.0f }, { "rin1", 0.0f }, { "rinmix", 0.0f }, { "rlevel", 99.0f }, { "dtype", 2.0f }, { "dtl", 250.0f }, { "dtr", 250.0f }, { "dfl", 25.0f }, { "dfr", 25.0f }, { "dinmix", 0.0f }, { "dylevel", 99.0f }, { "pmode", 0.0f }, { "pspeed", 47.0f }, { "pdepth", 40.0f }, { "pfb", 20.0f }, { "pin", 1.0f }, { "plevel", 99.0f }, { "dsel", 1.0f }, { "eqlevel", 64.0f }, { "elf", 100.0f }, { "ela", 0.0f }, { "ehf", 6000.0f }, { "eha", 2.0f } };
static const PV kP90[] = { { "config", 0.0f }, { "dlevel", 99.0f }, { "master", 0.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP91[] = { { "config", 0.0f }, { "dlevel", 99.0f }, { "master", 0.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP92[] = { { "config", 0.0f }, { "dlevel", 99.0f }, { "master", 0.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP93[] = { { "config", 0.0f }, { "dlevel", 99.0f }, { "master", 0.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP94[] = { { "config", 0.0f }, { "dlevel", 99.0f }, { "master", 0.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP95[] = { { "config", 0.0f }, { "dlevel", 99.0f }, { "master", 0.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP96[] = { { "config", 0.0f }, { "dlevel", 99.0f }, { "master", 0.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP97[] = { { "config", 0.0f }, { "dlevel", 99.0f }, { "master", 0.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP98[] = { { "config", 0.0f }, { "dlevel", 99.0f }, { "master", 0.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };
static const PV kP99[] = { { "config", 0.0f }, { "dlevel", 99.0f }, { "master", 0.0f }, { "rlevel", 0.0f }, { "dylevel", 0.0f }, { "plevel", 0.0f } };

static const Preset kPresets[kNumPresets] = {
    { "EQ+CRS+DLY+RVB", kP0, 28 },
    { "PRE EQ REVERB", kP1, 18 },
    { "POST EQ REVERB", kP2, 18 },
    { "GATED REVERB", kP3, 20 },
    { "STEREO CHORUS", kP4, 11 },
    { "STEREO FLANGE", kP5, 11 },
    { "PINGPONG DELAY", kP6, 12 },
    { "DETUNE +12", kP7, 12 },
    { "LEZLY aftr tch", kP8, 14 },
    { "5 BAND PARA EQ", kP9, 14 },
    { "SMALL ROOM", kP10, 11 },
    { "MEDIUM ROOM", kP11, 11 },
    { "LARGE ROOM", kP12, 11 },
    { "SMALL HALL", kP13, 11 },
    { "MEDIUM HALL", kP14, 11 },
    { "Concert Hall 1", kP15, 11 },
    { "Concert Hall 2", kP16, 11 },
    { "SML VOCALPLATE", kP17, 17 },
    { "MED VOCALPLATE", kP18, 17 },
    { "LRG VOCALPLATE", kP19, 17 },
    { "LO DIFUZ CHMBR", kP20, 11 },
    { "RICH CHAMBER", kP21, 11 },
    { "KINGS CHAMBER", kP22, 11 },
    { "SML TIGHT GATE", kP23, 14 },
    { "BRITELITE GATE", kP24, 20 },
    { "SMOOTH GATE", kP25, 14 },
    { "MEAN GATE", kP26, 20 },
    { "GATE PLUS VERB", kP27, 14 },
    { "FLANGE/GATE", kP28, 19 },
    { "FLANGE/GATE 2", kP29, 19 },
    { "MONO FLANGE", kP30, 11 },
    { "TRIGGER FLANGE", kP31, 11 },
    { "TWITCH FLANGER", kP32, 11 },
    { "MONO CHORUS", kP33, 11 },
    { "CHORUS ROOM", kP34, 16 },
    { "CHORUS REVERB", kP35, 16 },
    { "CHORUS DELAY", kP36, 17 },
    { "STEREO PITCH", kP37, 12 },
    { "LOOSE LEZLIE", kP38, 9 },
    { "P.BEND LEZLIE", kP39, 9 },
    { "167 ms DELAY", kP40, 12 },
    { "250 ms DELAY", kP41, 12 },
    { "500 ms DELAY", kP42, 12 },
    { "1500 ms DELAY", kP43, 12 },
    { "ECHO 2the LEFT", kP44, 12 },
    { "ECHO-FLANGE", kP45, 17 },
    { "FLANGED ECHOES", kP46, 17 },
    { "FLANGED ECHO 2", kP47, 17 },
    { "REVERSE ECHO", kP48, 17 },
    { "PichReversEcho", kP49, 23 },
    { "RAD KICK", kP50, 17 },
    { "AMAZING BASS", kP51, 17 },
    { "BALLAD SNARE 1", kP52, 17 },
    { "BALLAD SNARE 2", kP53, 17 },
    { "WET RHYTHM GTR", kP54, 22 },
    { "WET ECHO STRAT", kP55, 17 },
    { "SKANK GUITAR", kP56, 23 },
    { "LEAD GUITAR 1", kP57, 17 },
    { "Lead Guitar 2", kP58, 22 },
    { "GREASE GUITAR", kP59, 22 },
    { "CONCERT GUITAR", kP60, 16 },
    { "ACOUSTIC GTR", kP61, 17 },
    { "ROCK ACOUSTIC", kP62, 23 },
    { "CHORUSTRINGS 1", kP63, 16 },
    { "CHORUSTRINGS 2", kP64, 16 },
    { "Roadz Chorus+", kP65, 22 },
    { "HOLMS' Plate", kP66, 17 },
    { "SUSPEDAL LEZLY", kP67, 14 },
    { "STEREO SPREAD1", kP68, 12 },
    { "STEREO SPREAD2", kP69, 12 },
    { "DELAYED REVERB", kP70, 19 },
    { "DEEP SPACE EKO", kP71, 17 },
    { "RONG WAY VERB", kP72, 17 },
    { "VERB SHIFTER", kP73, 17 },
    { "MULTI VOCAL", kP74, 17 },
    { "TAJ MAHAL modw", kP75, 16 },
    { "VELOCITY ECHO+", kP76, 12 },
    { "VELOCITY ECHO-", kP77, 12 },
    { "VELOCITY+REVRB", kP78, 17 },
    { "VELOCITY-REVRB", kP79, 17 },
    { "ROUGH TUBE !", kP80, 17 },
    { "ROUGH PIPE !", kP81, 17 },
    { "BEAM ME UP !!!", kP82, 23 },
    { "VERBATUM DELI", kP83, 17 },
    { "QUADREAMVERB", kP84, 29 },
    { "ELECTRIC BLUE1!", kP85, 28 },
    { "DTUNE>SPRD>RVB", kP86, 17 },
    { "CHRUS>ECHO>RVB", kP87, 22 },
    { "FLANG>ECHO>RVB", kP88, 22 },
    { "DEMO CONFIG", kP89, 28 },
    { "BLANK PROGRAM", kP90, 6 },
    { "BLANK PROGRAM", kP91, 6 },
    { "BLANK PROGRAM", kP92, 6 },
    { "BLANK PROGRAM", kP93, 6 },
    { "BLANK PROGRAM", kP94, 6 },
    { "BLANK PROGRAM", kP95, 6 },
    { "BLANK PROGRAM", kP96, 6 },
    { "BLANK PROGRAM", kP97, 6 },
    { "BLANK PROGRAM", kP98, 6 },
    { "BLANK PROGRAM", kP99, 6 },
};
