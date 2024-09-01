This contains a file extension for microlisp developed by David Johnson-Davies (http://www.ulisp.com),
for working with the file system. The file is intended for the Arduino environment for 
ESP32 processors with a connection to an SD card.



(probe-file pathspec)  tests whether a file exists.
Returns nil if there is no file named pathspec, and otherwise returns the truename of pathspec.

(delete-file pathspec)  delete specified file.
Returns true if success and otherwise returns nil.

(rename-file filespec newfile)  rename or moving specified file.
Returns true if success and otherwise returns nil.

(copy-file filespec newfile)   copy specified file.
Returns true if success and otherwise returns nil.

(ensure-directories-exist pathspec)  Tests whether the directories containing the specified file actually exist,
and attempts to create them if they do not. Returns true if success and otherwise returns nil.
