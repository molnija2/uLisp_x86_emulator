uLisp x86_64 emulator is console application created by QtCreator.

This program is an emulator of the lisp interpreter for
microcontrollers uLisp developed by David Johnson-Davies (http://www.ulisp.com/)

The interpreter connects to the qDisplayServer graphics server if
the server was previously running.

Two archives are separate projects that are built by QtCreator.

1. Download uLisp and qDisplayServer directories (or unpack uLisp.zip and qDisplayServer archives separately).
2. Build them using QtCreator.
3. Run qDisplayServer.
4. Copy the "*.l" files (examples in Lisp) to the directory where the uLisp executable is located.
5. Run uLisp.
6. Look at DisplayServer, try pressing an arrows or 'h' for a hint.  Emacs arrow keys are supported.
   

The program run lisp writed list-editor which allows to view and change lists and sub-lists.
For easy program-style view "l"-key can be pressed when the cursor is at list-type variable. 

![изображение](https://github.com/user-attachments/assets/b2e23c9f-c9c1-4342-a7f5-51acd94c3fe5)

'l' (small L) key calls advanced program-elitor for selected list.

![изображение](https://github.com/user-attachments/assets/56cafdee-7145-4955-9389-0feb119ca4d2)

