#ifndef FILESYS_H
#define FILESYS_H




const char string_probefile[] = "probe-file";
const char string_deletefile[] = "delete-file";


const char doc_probefile[] = "(probe-file pathspec)\n"
"tests whether a file exists.\n"
" Returns false if there is no file named pathspec,"
" and otherwise returns the truename of pathspec.";


const char doc_deletefile[] = "(delete-file pathspec)\n"
"delete specified file.\n"
" Returns false if success, and otherwise returns nil.";

#endif // FILESYS_H
