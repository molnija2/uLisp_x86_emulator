#ifndef FILESYS_H
#define FILESYS_H




const char string_probefile[] = "probe-file";
const char string_deletefile[] = "delete-file";
const char string_deletedir[] = "delete-dir";
const char string_renamefile[] = "rename-file";
const char string_copyfile[] = "copy-file";
const char string_ensuredirectoriesexist[] = "ensure-directories-exist";
const char string_uiopchdir[] = "uiop:chdir";
const char string_uiopgetcwd[] = "uiop:getcwd";

const char doc_probefile[] = "(probe-file pathspec)\n"
"tests whether a file exists.\n"
" Returns nil if there is no file named pathspec,"
" and otherwise returns the truename of pathspec.";


const char doc_deletefile[] = "(delete-file pathspec)\n"
"delete specified file.\n"
" Returns true if success and otherwise returns nil.";

const char doc_deletedir[] = "(delete-dir pathspec)\n"
"delete specified directory.\n"
" Returns true if success and otherwise returns nil.";

const char doc_renamefile[] = "(rename-file filespec newfile)\n"
"rename or moving specified file.\n"
" Returns true if success and otherwise returns nil.";

const char doc_copyfile[] = "(copy-file filespec newfile)\n"
"copy specified file.\n"
" Returns true if success and otherwise returns nil.";

const char doc_ensuredirectoriesexist[] = "(ensure-directories-exist pathspec)\n"
"Tests whether the directories containing the specified file actually exist,"
" and attempts to create them if they do not.\n"
" Returns true if success and otherwise returns nil.";

const char doc_uiopchdir[] = "(uiop:chdir pathspec)\n"
"Change current directory by specified directory.\n"
" Returns true if success and otherwise returns nil.";

const char doc_uiopgetcwd[] = "(uiop:getcwd)\n"
"Returns a string which contains current directory.";

#endif // FILESYS_H
