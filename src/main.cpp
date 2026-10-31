#include "DigitalLedModule.h"
#include "OpenKNX.h"
#include "hardware.h"

void setup()
{
    Serial.begin(115200);
    openknx.init(1);
    openknx.addModule(1, digitalLedModule);
    openknx.setup();
}

void loop()
{
    openknx.loop();
}

#if defined(OPENKNX_DUALCORE)
void setup1()
{
    openknx.setup1();
}

void loop1()
{
    openknx.loop1();
}
#endif
