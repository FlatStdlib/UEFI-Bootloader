#include "init.h"

public bool set_new_variable(const string name, const string value)
{
    if(!name || !value)
        return false;

    
    map_append(_FSLEFI_.variables, name, value);
    return true;
}