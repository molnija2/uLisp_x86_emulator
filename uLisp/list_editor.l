(progn

; Define Editor screen size
 (defvar ListEditorWidth 320)
 (defvar ListEditorHeight 240)

; Define current font character size
 (defvar FontWidth 8)
 (defvar FontHeight 11)

 (defvar ColorBk_LineEditor (rgb 255 255 255))
 (defvar ColorBk_Editor (rgb 0 0 0))
 (defvar ColorBk_EditorH (rgb 200 100 100))
 (defvar Color_LineEditor (rgb 0 0 0))
 (defvar Color_CursorEditor (rgb 255 255 255))
 (defvar ColorBk_EditorH (rgb 180 50 50))

 (defun key-forUp (key)    (if (or (eq key 16) (eq key 16) (eq key 19)) t nil))
 (defun key-forDown (key)  (if (or (eq key 14) (eq key 14) (eq key 21)) t nil))
 (defun key-forLeft (key)  (if (or (eq key 2) (eq key 2) (eq key 18)) t nil))
 (defun key-forRight (key) (if (or (eq key 6) (eq key 6) (eq key 20)) t nil))
 (defun key-forPgUp  (key) (if (or (eq key 23) (eq key 23) (eq key 122)) t nil))
 (defun key-forPgDn  (key) (if (or (eq key 22) (eq key 22) (eq key 123)) t nil))
 (defun key-forHelp  (key) (if (or (eq key 104) (eq key 72) (eq key 48)) t nil))
 (defun key-forPrEd  (key) (if (or (eq key 108) (eq key 76) (eq key 51)) t nil))
 (defun key-forQuit  (key) (if (or (eq key 113) (eq key 81) (eq key 17) (eq key 57)) t nil))


 (defun program-editor  (lst) nil)


 (defun read-key ()
   (read-byte)
 )

 (defvar listedit_help '(
 "   Editor control keys:"
 "h - read this text"
 "Ctrl-b Backward, Ctrl-f Forward,"
 "Ctrl-p PrevLine, Ctrl-n NextLine,"
 "Ctrl-v PgDn, Ctrl-w PgUp"
 "e - edit element"
 "Backspace - delete character"
 "a - append element to current list"
 "i - insert element into current list"
 "d - delete element from current list"
 "    and save it in the buffer"
 "c - element (list) into the buffer"
 "p - paste-insert element from the buffer"
 "t - paste-append element from the buffer"
 "Enter - input into the list"
 "l - run program-editor for the list"
 "Ctrl-q - exit"
 " "
 "press any key to return"))


 (defun PrintHelp ()
  (let ((index 0) (colorbk (rgb 150 0 0)))
   (with-gfx (str)
     (fill-rect 0 0 ListEditorWidth (- ListEditorHeight FontHeight) colorbk )
     (set-text-color (rgb 255 255 255) colorbk)
       (dolist (x listedit_help)
         (set-cursor 0 (* index (+ FontHeight 2)))
         (write-string x str)
         (setq index (1+ index))
       )
     )
   )

   (read-key)
 )



; Draw string
  (defun draw-string (s)
    (if (stringp s)
       ( with-gfx (str) (write-string s str))
    )
  )


;  Set Line editor Height for FontHeight + cursor underline
  (defun line-editor-Height () (+ FontHeight 2))

; Text line editor for atom or list with limited lenght of string
  (defun line-editor  (posx posy item_org)
    (let* ((str_item
             (if (stringp item_org)
                 (format nil "~a~a~a" (code-char 34) item_org (code-char 34))
                 (string  item_org)))
           (key 0) (len (length str_item))
           (cursor_char len) (return_flag 0) (first_key 1))
      (set-cursor posx posy)
      (fill-rect posx posy (- ListEditorWidth posx) (line-editor-Height) ColorBk_LineEditor )
      (set-text-color ColorBk_LineEditor Color_LineEditor)
      (draw-string (subseq str_item 0 len) )
      (loop
        (fill-rect (+ posx (* cursor_char FontWidth)) (+ posy FontHeight) FontWidth 2 Color_LineEditor )
        (set-cursor 0 (- ListEditorHeight FontHeight))
        (setq key (read-key))

        (fill-rect (+ posx (* cursor_char FontWidth)) (+ posy FontHeight) FontWidth 2 ColorBk_LineEditor )

        (if (eq first_key 1)
         (progn
            (set-cursor posx posy)
            (set-text-color Color_LineEditor ColorBk_LineEditor)
            (draw-string (subseq str_item 0 len) )
         )
        )

        (set-cursor posx posy)
        (if (eq key 0)
         (progn
          (set-cursor 0 (- ListEditorHeight FontHeight))
          (setq key (read-key))

          (cond
           ((eq key 18) (if (> cursor_char 0) (setq cursor_char (1- cursor_char))))
           ((eq key 20) (if (< cursor_char len) (setq cursor_char (1+ cursor_char))))
           ((eq key 57) (setq return_flag 1) )
           ((eq key 7)
             (if (< cursor_char len)
              (setq str_item
               (concatenate 'string
                 (subseq str_item 0  cursor_char)
                 (subseq str_item (1+ cursor_char) len))))
             (if (eq first_key 1)
               (progn
                 (setq first_key 0)
                 (setq str_item "")
                 (setq cursor_char 0)
               )
             )

             (fill-rect 0 posy ListEditorWidth (line-editor-Height) ColorBk_LineEditor )
             (setq len (length str_item))
             (set-cursor posx posy)
             (set-text-color Color_LineEditor ColorBk_LineEditor)
             (draw-string (subseq str_item 0 len) )
           )
           (t (set-text-color ColorBk_LineEditor ColorBk_Editor)
              (set-cursor 0 (- ListEditorHeight FontHeight))
              (format t "0+~a " key))
          )
          (setq first_key 0)
         )
         (progn
          (if (and (eq first_key 1) (not (eq key 13)) (not (eq key 8)) (not (key-forLeft key)) (not (key-forRight key)) (not (key-forQuit key)))
           (progn
            (setq str_item "")
            (setq cursor_char 0)
            (setq len 0)
            (fill-rect 0 (- ListEditorHeight (line-editor-Height)) ListEditorWidth (line-editor-Height) ColorBk_LineEditor )
           )
          )
          (setq first_key 0)
          (cond
           ((eq key 17) (setq return_flag 1) )
           ((key-forLeft key) (if (> cursor_char 0) (setq cursor_char (1- cursor_char))))
           ((key-forRight key) (if (< cursor_char len) (setq cursor_char (1+ cursor_char))))
           ((eq key 8)
            (if (> cursor_char 0)
             (progn
              (setq str_item
               (concatenate 'string
                 (subseq str_item 0 (1- cursor_char))
                 (subseq str_item cursor_char len)))
              (setq cursor_char (1- cursor_char))
              (fill-rect 0 (- ListEditorHeight (line-editor-Height)) ListEditorWidth (line-editor-Height) ColorBk_LineEditor )
             ))
           )
           (t
              (if (eq key 13)
                (progn
                  (setq return_flag 2)
                  (setq cursor_char len) )
              )
              (setq str_item
               (concatenate 'string
                 (subseq str_item 0 cursor_char)
                 (string (code-char key))
                 (subseq str_item cursor_char len)))
                 (setq cursor_char (1+ cursor_char))
           )
          )
          (setq len (length str_item))
          (set-cursor posx posy)
          (set-text-color Color_LineEditor ColorBk_LineEditor)
          (draw-string (subseq str_item 0 len) )
         )
        )
        (if (not (eq return_flag 0)) (return))
      )
      (if (eq return_flag 1) nil)
      (if (eq return_flag 2) (read-from-string str_item))
    )
  )

; Bottom line displays current list which is selected by cursor
  (defun display-editor-item  (posx posy item)
     (let* ((str_item (string item)) (key 0) (len (length str_item)) (len_org len))
      (set-cursor posx posy)
      (set-text-color Color_LineEditor ColorBk_LineEditor)
      (if (> len 32) (setq len 32))
      (draw-string (subseq str_item 0 len) )
      len_org
    )
  )

  (defvar Color_atom  (rgb 255 255 255))
  (defvar Color_list  (rgb 0 255 0))
  (defvar Color_unbound (rgb 0 0 255))
  (defvar Color_int  (rgb 255 255 180))
  (defvar Color_float  (rgb 255 180 255))
  (defvar Color_string (rgb 255 180 180))




; Mark list items by colors depend of is it atom or list
  (defun item-color-set (item bkcolor)
    (cond
     ((atom item)
       (if (boundp (quote item))
         (cond
          ((numberp  item) (set-text-color Color_int bkcolor))
          ((floatp  item)  (set-text-color Color_float bkcolor))
          ((stringp  item) (set-text-color Color_string bkcolor))
          ((consp (quote item)) (set-text-color Color_list bkcolor))
          (t   (set-text-color Color_atom bkcolor))
         )
         (set-text-color Color_unbound  bkcolor)
       )
     )
     (t (set-text-color Color_list bkcolor))
    )
  )

  (defun item-get-string (item)
   (cond
    ( (atom item)
      (if (stringp item)
         (format nil "~a~a~a" (code-char 34) item (code-char 34))
         (string  item)
      )
    )
    (t
      (if (atom (car  item))
        (format nil "(~a>)"  (car item) )
        (format nil "((...)" )
      )
    )
   )
  )

; Calculate the list items positions evenly for one column of fixed width
  (defun calc-list-coords (lst posx posy)
    (let ((len (length lst)) (a nil)
          (curx 0) (cury 0)
          (item_width 0) (index 0) (items_in_line 0) )

   (if (> len 0)
    (progn
     (setq index 0)
     (setq items_in_line 0)
     (dolist (x lst)
      (setq item_width (* (1+ (length (item-get-string x))) FontWidth))
      (cond
        ((>= (+ curx item_width) ListEditorWidth)
         (let ( (LineFreeSpace (- ListEditorWidth curx))
               (LineSpaceStep 0) (j 0) (k 0))
           (setq LineSpaceStep (truncate (/ LineFreeSpace (1- items_in_line))) )
           (dotimes (i (1- items_in_line))
              (setq k (1- (- index i)))
              (setq j (aref posx k ) )
              (setq j (+ j (- LineFreeSpace (* LineSpaceStep i))))
              (setf (aref posx k) j)
            )
          )

          (setq curx 0)
          (setq cury (+ cury FontHeight))
          (setf (aref posx index) curx)
          (setf (aref posy index) cury)
          (setq items_in_line 1)
          (setq curx item_width)
        )
        (t
          (setq items_in_line (1+ items_in_line))
          (setf (aref posx index) curx)
          (setf (aref posy index) cury)
          (setq curx (+ curx item_width)))
      )
      (setq index (1+ index))
     )
   )))
  )

  (defun print-list-editor (lst posx posy scrolly)
    (let ((len (length lst)) (a nil)
        (curx 0) (cury 0)
        (item_width 0) (index 0) (items_in_line 0)
        (WindowHeight (- ListEditorHeight (* FontHeight 2))) )

     (fill-rect 0 0 ListEditorWidth ListEditorHeight bkcolor0 )

     (if (> len 0)
      (progn
        (setq index 0)
        (dolist (x lst)
          (cond
           ((and  (<  (- (aref posy index) scrolly) WindowHeight)
                  (>= (- (aref posy index) scrolly) 0))

             (set-cursor (aref posx index) (- (aref posy index) scrolly))
             (item-color-set x bkcolor0)
             (draw-string (item-get-string x))
           )
           (t nil)
          )
          (setq index (1+ index))
        )
      )
    ))
   )


   (defun list-editor (lst)
       (let ((len (length lst)) (item_edit 0) (changed_flag 0)
             (posx 0) (posy 0) (curx 0) (cury 0) (key 0) (scrolly 0)
             (bkcolor0 ColorBk_Editor) (bkcolor1 ColorBk_EditorH)
             (item_width 0) (index 0) (items_in_line 0) (cursor_item 0)
             (item_str_length 0) (scrolly 0) (page 0))

      (if (> len 0)
       (progn
            (setq posx (make-array len ))
            (setq posy (make-array len ))
      ))

      (calc-list-coords lst posx posy)

     (print-list-editor lst posx posy scrolly)

     (loop
        (if (> len 0)
           (set-cursor (aref posx cursor_item) (- (aref posy cursor_item) scrolly)))
        (item-color-set (nth cursor_item lst) bkcolor1)
        (draw-string (item-get-string (nth cursor_item lst)))

        (fill-rect (* FontWidth 5) (- ListEditorHeight FontHeight) (- ListEditorWidth (* FontWidth 5)) FontHeight bkcolor0 )
        (setq item_str_length (display-editor-item (* FontWidth 5) (- ListEditorHeight FontHeight) (nth cursor_item lst)))

        (set-cursor 0 (- ListEditorHeight FontHeight))
        (item-color-set (nth cursor_item lst) bkcolor0)

        (setq key (read-key))
        (set-cursor 0 (- ListEditorHeight FontHeight))
        (item-color-set (nth cursor_item lst) bkcolor0)
        (draw-string (format nil "~a  " key))

        (if (> len 0) (set-cursor (aref posx cursor_item) (- (aref posy cursor_item) scrolly)))
        (item-color-set (nth cursor_item lst) bkcolor0)
        (draw-string (item-get-string (nth cursor_item lst)))


        (if (key-forQuit key)
           (return )
        )

        (cond
          ((key-forLeft key) (if (> cursor_item 0) (setq cursor_item (1- cursor_item))))
          ((key-forRight key) (if (< cursor_item (1- len)) (setq cursor_item (1+ cursor_item))))
          ((or (key-forUp key) (key-forPgUp key))
            (if (key-forPgUp key)
              (setq page (1- (/ ListEditorHeight FontHeight)))
              (setq page 1)
            )
            (if (> cursor_item 0)
              (let ((cursor cursor_item))
               (loop
                (setq cursor_item (1- cursor_item))
                (if (eq cursor_item 0) (return nil))
                (if (and (not (eq (aref posy cursor_item) (aref posy cursor)))
                      (<= (aref posx cursor_item) (aref posx cursor)))
                  (setq page (1- page)))
                (if (<= page 0) (return))
               )
               (loop
                 (if (< (- (aref posy cursor_item) scrolly) 0)
                   (progn
                     (setq scrolly (- scrolly FontHeight))
                     (print-list-editor lst posx posy scrolly))
                   (return)
               ) )
              )
            )
          )
          ((or (key-forDown key) (key-forPgDn key))
            (if (key-forPgDn key)
              (setq page (1- (/ ListEditorHeight FontHeight)))
              (setq page 1)
            )
            (if (< cursor_item (1- len))
             (let ((cursor cursor_item))
              (loop
               (setq cursor_item (1+ cursor_item))
               (if (eq cursor_item (1- len)) (return nil))
               (if (and (not (eq (aref posy cursor_item) (aref posy cursor)))
                        (>= (aref posx cursor_item) (aref posx cursor)))
                 (setq page (1- page)))
               (if (<= page 0) (return))
              )
              (loop
                (if (> (- (aref posy cursor_item) scrolly)
                     (- ListEditorHeight (* FontHeight 2)))
                  (progn
                   (setq scrolly (+ scrolly FontHeight))
                   (print-list-editor lst posx posy scrolly))
                  (return)
              ))
             )
            )
          )
          ((eq key 13)
            (if (not (atom (nth cursor_item lst)))
             (progn
              (setq item_edit (list-editor (nth cursor_item lst)))
              (if (not (eq item_edit nil))
               (progn
                (setf (nth cursor_item lst) item_edit)
                (setq changed_flag 1)
               )
              )
             )
            )
            (calc-list-coords lst posx posy)

            (print-list-editor lst posx posy scrolly)
          )

          ;  Run and eval
          ((or (eq key 114) (eq key 82))
            (ignore-errors
             (let* ( (str (string (eval (nth cursor_item lst)))) (len (length str)))
               (if (> len 32) (setq len 32))
               (set-cursor 0 0)
               (set-text-color  ColorBk_Editor (rgb 255 127 255))
               (draw-string (format nil " Ret=~a " (subseq str 0 len)))
             )
            )

            (setq key (read-key))

            (calc-list-coords lst posx posy)

            (print-list-editor lst posx posy scrolly)
          )

          ; program-editor launch
          ((key-forPrEd key)
            (if (not (atom (nth cursor_item lst)))
             (progn
              (setq item_edit (program-editor (nth cursor_item lst)))
              (if (not (eq item_edit nil))
               (progn
                (setf (nth cursor_item lst) item_edit)
                (setq changed_flag 1)
               )
              )
             )
            )
            (calc-list-coords lst posx posy)

            (print-list-editor lst posx posy scrolly)
          )

          ((or (eq key 68) (eq key 100))
            (copy-to-buffer (nth cursor_item lst) 0)
            (setq lst (append (subseq lst 0 cursor_item) (subseq lst (1+ cursor_item))
                      )
            )
            (setq changed_flag 1)
            (setq len (length lst))

            (if (and (>= cursor_item len) (> cursor_item 0)) (setq cursor_item (1- cursor_item)))
            (if (> len 0)
             (progn
                  (makunbound 'posx)
                  (makunbound 'posy)
                  (setq posx (make-array len ))
                  (setq posy (make-array len ))
            ))
            (calc-list-coords lst posx posy)
            (print-list-editor lst posx posy scrolly)
          )

          ;  c - copy object o buffer
          ( (or (eq key 99) (eq key 67))
            (copy-to-buffer (nth cursor_item lst) 0)
          )


          ;  p - Paste object   t - paste-append to tail
          ((and (or (eq key 112) (eq key 80) (eq key 116) (eq key 74)) (not (eq object_buffer nil)) )

            (if (or (eq key 116) (eq key 74))
                (setf lst (append (subseq lst 0) '(nil)))
                (setf lst (append (subseq lst 0 cursor_item)
                        '(nil) (subseq lst cursor_item)))
            )

            (if (or (eq key 116) (eq key 74))
                (setq cursor_item (1- (length lst)))
            )
            (setf (nth cursor_item lst) object_buffer)

            (if (consp object_buffer)
               (setq object_buffer nil)
            )

            (setq changed_flag 1)
            (setq len (length lst))

            (if (> len 0)
             (progn
                  (makunbound 'posx)
                  (makunbound 'posy)
                  (setq posx (make-array len ))
                  (setq posy (make-array len ))
            ))
            (calc-list-coords lst posx posy)
            (print-list-editor lst posx posy scrolly)
          )


          ((or (eq key 73) (eq key 105) (eq key 97) (eq key 65))

            (if (or (eq key 97) (eq key 65))
              (progn
                (setq cursor_item len)
                (setq lst (append (subseq lst 0) '(nil)))
              )
              (setq lst (append (subseq lst 0 cursor_item) '(nil) (subseq lst cursor_item)))
            )
            (setq item_edit (line-editor (* FontWidth 5) (- ListEditorHeight (line-editor-Height)) (nth cursor_item '())))
            (if (not (eq item_edit nil))
              (setf (nth cursor_item lst) item_edit)
            )

            (setq changed_flag 1)
            (setq len (length lst))

            (if (> len 0)
             (progn
                  (makunbound 'posx)
                  (makunbound 'posy)
                  (setq posx (make-array len ))
                  (setq posy (make-array len ))
            ))
            (calc-list-coords lst posx posy)
            (print-list-editor lst posx posy scrolly)
          )

          ((and (or (eq key 69) (eq key 101)) (< item_str_length 128))
            (setq item_edit (line-editor (* FontWidth 5) (- ListEditorHeight (line-editor-Height)) (nth cursor_item lst)))
            (if (not(eq item_edit nil))
              (progn
                (setq changed_flag 1)
                (setf (nth cursor_item lst) item_edit)
              ))

            (calc-list-coords lst posx posy)
            (print-list-editor lst posx posy scrolly)
          )
          ((key-forHelp key)
           (PrintHelp)
           (print-list-editor lst posx posy scrolly)
          )
          (t nil)
        )

      )
      (set-cursor 0 (- ListEditorHeight FontHeight))

      (if (> len 0)
       (progn
        (makunbound 'posx)
        (makunbound 'posy)
        (setq posx 0)
        (setq posy 0)
       )
      )
      (if (eq changed_flag 1) lst nil)
    )
  )



;  Print program into GFX or SDCADR stream
  (defun print-list  (lst0 list_level len s)
    (progn
       (write-byte 13 s)
       (print-spaces list_level s)
       (setq len list_level)
       (write-string "(" s)
    )
    (let ((index 0) (lst_len (length lst0)))
     (dolist (lst lst0)
      (cond
        ((atom lst)
         (let* ((str_item (string  lst))
             (item_len (length str_item)))
          (setq len (+ len item_len))
          (if (>= len (+ list_level 64))
            (progn
              (setq len list_level)
              (write-byte 13 s)
              (print-spaces list_level s))
          )
          (if (> index 0) (write-string " " s))
          (if (stringp lst) (write-byte 34 s))
          (write-string str_item s)
          (if (stringp lst) (write-byte 34 s))
         )
        )
        (t
          (print-list  lst (1+ list_level) (1+ len) s)
        )
     )
     (setq index (1+ index)))
    )
    (write-string ")" s)
  )


 (defun save-program (lst filename)
  (if (consp lst)
   (with-sd-card (s filename 2) (print-list lst 0 0 s))
  )
 )

 (defvar prog_editor (load-program "prog_editor.l"))
 (eval prog_editor)

 (defun key-test ()
  (loop
    (let ((k (read-key)))
      (set-cursor 0 0)
      (set-text-color (rgb 255 255 255) 0)
      (draw-string (format t " ~a" k))
      (if (eq k 113) (return)))))

)
