
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <vector>

#include "FgAcqBlockExample.cpp"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <gbe.h>
#include <gbe_error.h>

class EventListener{
public:
    virtual int32_t onEvent(uint64_t events, const struct fg_event_info *info) = 0;
};

class FgEventExample: public FgAcqBlockExample, public EventListener{
public:
    FgEventExample();

    int32_t run();

    //this function interprets the event, and prints its information
    virtual int32_t onEvent(uint64_t events, const struct fg_event_info *info);
protected:
    std::vector<std::string> determineEvents(Fg_Struct* fgHandle);

    std::string selectEvent(Fg_Struct* fgHandle);
};