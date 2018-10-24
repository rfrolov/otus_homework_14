#include "ParseCmdLine.h"
#include "MapReduce.h"
#include <iostream>

int main(int argc, char *argv[]) {
    ParseCmdLine parser(argc, argv);
    if(!parser.is_valid()) {
        std::cerr << parser.error();
    }

    MapReduce app{parser.file_name(), parser.mnum(), parser.rnum()};
    app.run();

    return 0;
}