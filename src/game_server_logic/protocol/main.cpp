

#include "BufferedParser.h"

int main(int argc, char **argv) {
    BufferedParser p;
    p.parse("1234567\r\n\r\nabcdef\r\n\r\nxyzh\r\n\r\n");
}