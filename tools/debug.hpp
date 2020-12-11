char normal[]   = { 0x1b, '[', '0', ';', '3', '9', 'm', 0 };
char red[]      = { 0x1b, '[', '1', ';', '4', '1', 'm', 0 };
char blue[]     = { 0x1b, '[', '1', ';', '3', '4', 'm', 0 };

#define ERROR(file, message) std::cerr << file << " " << red << "error" << normal << ": " << message << std::endl;
#define WARNING(s) 