#include "StringId.h"

#include <unordered_map>

static std::unordered_map<StringId, const char*> gStringIdTable;

StringId internString(const char* str) {
    StringId sid = SID(str);
    
    if (auto it = gStringIdTable.find(sid); it == gStringIdTable.end()) {
        gStringIdTable[sid] = _strdup(str);
    }

    return sid;
}
