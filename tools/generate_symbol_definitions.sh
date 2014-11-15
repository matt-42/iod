sed -e 's/^\([a-zA-Z1-9_]\)\([a-zA-Z1-9_]*\)/#ifndef IOD_SYMBOL__\U\1\L\2\E\n\#define IOD_SYMBOL__\U\1\L\2\E\n    iod_define_symbol(\1\2, _\U\1\L\2\E)\n#endif/' $1
