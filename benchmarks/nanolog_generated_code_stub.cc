#include "GeneratedCode.h"

namespace GeneratedFunctions {
size_t numLogIds = 0;
LogMetadata logId2Metadata[] = {};

ssize_t (*compressFnArray[])(NanoLogInternal::Log::UncompressedEntry *, char *) = {};

void (*decompressAndPrintFnArray[])(const char **, FILE *,
                                    void (*)(const char *, ...)) = {};

long int writeDictionary(char *, char *) { return 0; }
} // namespace GeneratedFunctions
