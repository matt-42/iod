sed -e 's/^\([[:alnum:]_]\+\)/#ifndef IOD_SYMBOL__\1\n\#define IOD_SYMBOL__\1\E\n    iod_define_symbol(\1, _\1)\n#endif/' $1
