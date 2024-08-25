(progn

  (defvar  program_line 0)

;  Print program to GFX stream for Program-Editor
  (defun draw-program  (lst0 list_level len scrolly)
   (let ((index 0) (lst_len (length lst0))
         (WindowHeight (- ListEditorHeight (* (program-line-Height) 1)))
         (posy (- (* (program-line-Height) program_line) scrolly))
         (columns (/ ListEditorWidth FontWidth)) (column_bgn 0))

      (if (>= len list_level)
         (setq program_line (1+ program_line))
         (setq posy (- (* (program-line-Height) (1- program_line)) scrolly)))
      (setq len list_level)
      (if (and  (<  posy  WindowHeight)
                   (>= posy  0) )
         (progn
           (set-cursor (* list_level FontWidth) posy)
           (item-color-set lst0 ColorBk_Editor)
           (draw-string "(")
         )
      )

      (dolist (lst lst0)
       (cond
        ((atom lst)
         (let* ((str_item
                  (if (stringp lst)
                     (format nil "~a~a~a" (code-char 34) lst (code-char 34))
                     (string  lst)
                  ))
                (item_len (length str_item)) )

           (setq column_bgn len)
           (setq len (+ len item_len))
           (if (>= len columns)
            (progn
              (setq len list_level)
              (setq column_bgn len)
              (setq program_line (1+ program_line))
              (setq posy (- (* (program-line-Height) (1- program_line)) scrolly))
              (if (<  posy  ListEditorHeight)
               (set-cursor (* list_level FontWidth) posy)
              )
              (setq len (+ len item_len))
            )
           )

           (if (and  (<  posy  WindowHeight)
                     (>= posy  0) )
            (progn
             (set-cursor (* (1+ column_bgn) FontWidth) posy)
             (item-color-set lst ColorBk_Editor)
             (if (> index 0)
               (progn
                (setq len (1+ len))
                (setq column_bgn (1+ column_bgn))
                (item-color-set lst ColorBk_Editor)
                (draw-string " ")
             ))
             (draw-string str_item))
           )
         )
        )
        (t
          (setq len (draw-program lst (+ list_level 2) (+ len 0) scrolly))
          (setq posy (- (* (program-line-Height) (- program_line 1)) scrolly))
        )
       )
       (setq index (1+ index))
     )

     (setq posy (- (* (program-line-Height) (- program_line 1)) scrolly))
     (if (>= (+ len 2) columns)
      (progn
        (setq len list_level)
        (setq program_line (1+ program_line))
        (setq posy (- (* (program-line-Height) (1- program_line)) scrolly))
        (if (<  posy  ListEditorHeight)
         (set-cursor (* list_level FontWidth) posy)
        )
     ))
     (if (and  (< posy  (- ListEditorHeight (* (program-line-Height) 2)))
               (>= posy  0) )
       (progn
        (item-color-set lst0 ColorBk_Editor)
        (draw-string ")"))
     )
     (1+ len)
    )
  )


;  Set Line editor Height for FontHeight + cursor underline
  (defun program-line-Height () (+ FontHeight 4))
  (defun program-cursory (posy) (+ FontHeight (* posy (+ FontHeight 4))))


  (defvar current_object -1)
  (defvar current_list_prev nil)
  (defvar current_object_prev -1)
  (defvar cursor_list nil)    ; the list which is conained the cursor object
  (defvar cursor_object -1)   ; index of the cursor object
  (defvar cursor_list_prev nil)  ; The external list which is conained the cursor list
  (defvar cursor_object_prev -1) ; index of the cursor list in the external list
  (defvar cursor_endoflist -1) ;  Indicate that cursor at end of the list
  (defvar cursor_objectX 0)
  (defvar cursor_objectXl 0)
  (defvar cursor_Linebgn 0)
  (defvar cursor_Lineend 0)


; Determine the list item selected by the cursor for the program editor.

  (defun detect-cursor  (lst0 lst_prev index_prev  list_level len scrolly cursor_col cursor_row highliting)
   (let ((index 0) (lst_len (length lst0)) (column_bgn 0)
         (WindowHeight (- ListEditorHeight (* (program-line-Height) 1)))
         (posy (- (* (program-line-Height) program_line) scrolly))
         (columns (/ ListEditorWidth FontWidth))
         (cursor_y (* cursor_row  (program-line-Height))))

      (if (and (eq len 0) (eq list_level 0))
        (progn
          (setq program_line 0)
          (setq cursor_endoflist -1)
          (setq current_list_prev lst0)
          (setq cursor_list nil)
          (setq cursor_list_prev nil)
          (setq current_object -1)
          (setq current_object_prev -1)
          (setq cursor_object -1)
          (setq cursor_object_prev -1)
          (setq cursor_objectX 0)
          (setq cursor_objectXl 0)
          (defvar cursor_Linebgn 0)
          (defvar cursor_Lineend 0)
      ))

      (if (>= len list_level)
        (setq program_line (1+ program_line))
        (setq posy (- (* (program-line-Height) (1- program_line)) scrolly)))
      (setq len list_level)
      (if (and  (<  posy  WindowHeight)
                   (>= posy  0)
                   (= posy cursor_y) (> len 0) )
         (progn
           (defvar cursor_Linebgn list_level)
           (set-cursor (* list_level FontWidth) posy)
           (if (= cursor_col list_level)
             (progn
              (setq cursor_list lst_prev)
              (setq cursor_list_prev current_list_prev)
              (setq cursor_object index_prev)
              (setq cursor_object_prev current_object_prev)
              (if (eq highliting t)
               (item-color-set lst0 ColorBk_EditorH)
               (item-color-set lst0 ColorBk_Editor))
              (draw-string "(")
              (setq cursor_objectX  list_level)
              (setq cursor_objectXl (1+ list_level))
           ) )
         )
      )


      (dolist (lst lst0)
       (cond
        ((atom lst)
         (let* ((str_item
                  (if (stringp lst)
                     (format nil "~a~a~a" (code-char 34) lst (code-char 34))
                     (string  lst)
                  ))
                (item_len (length str_item)) )

           (setq column_bgn len)
           (setq len (+ len item_len))
           (if (>= len columns)
            (progn
              (setq len list_level)
              (setq column_bgn len)
              (setq program_line (1+ program_line))
              (setq posy (- (* (program-line-Height) (1- program_line)) scrolly))
              (if (<  posy  ListEditorHeight)
               (set-cursor (* list_level FontWidth) posy)
              )
              (setq len (+ len item_len))
            )
           )

           (if (> index 0)
             (progn
              (setq len (1+ len))
              (setq column_bgn (1+ column_bgn))
           ))

           (if (and (= posy cursor_y) (<  posy  WindowHeight)
                     (>= posy  0))
            (progn
             (defvar cursor_Lineend len)
             (if (and (> cursor_col column_bgn) (<= cursor_col len))
               (progn
                (setq cursor_list_prev lst_prev)
                (setq cursor_list lst0)
                (setq cursor_object index)
                (setq cursor_object_prev index_prev)
                (if (eq highliting t)
                 (item-color-set lst ColorBk_EditorH)
                 (item-color-set lst ColorBk_Editor))

                (set-cursor (* (1+ column_bgn) FontWidth) posy)
                (setq cursor_objectX (1+ column_bgn))
                (setq cursor_objectXl len)

                (draw-string str_item)
              )
             )
            )
           )
         )
        )
        (t
          (setq current_object_prev index_prev)
          (setq current_list_prev lst_prev)
          (setq len (detect-cursor lst lst0 index (+ list_level 2) (+ len 0) scrolly  cursor_col cursor_row highliting))
          (setq posy (- (* (program-line-Height) (- program_line 1)) scrolly))
        )
       )
       (setq index (1+ index))
     )

     (setq len (1+ len))
     (setq posy (- (* (program-line-Height) (- program_line 1)) scrolly))
     (if (>= (+ len 1) columns)
      (progn
        (setq len list_level)
        (setq program_line (1+ program_line))
        (setq posy (- (* (program-line-Height) (1- program_line)) scrolly))
     ))

     (if (and  (< posy  (- ListEditorHeight (* (program-line-Height) 2)))
               (>= posy  0))
       (progn

         (if (= posy cursor_y) (defvar cursor_Lineend len))

         (if  (and (= cursor_col len)
                   (= posy cursor_y))
          (progn
            (if (eq highliting t)
              (item-color-set lst0 ColorBk_EditorH)
              (item-color-set lst0 ColorBk_Editor)
            )
            (setq cursor_list_prev lst_prev)
            (setq cursor_object_prev index_prev)
            (setq cursor_list lst0)
            (setq cursor_object 0)
            (setq cursor_endoflist t)
            (if (<  posy  ListEditorHeight)
             (set-cursor (* len FontWidth) posy)
            )
            (setq cursor_objectX len)
            (setq cursor_objectXl (1+ len))

            (draw-string ")")

           )
         )
       )
     )

     (+ len 0)
    )
  )



 (defun get-previouse-list (lst0)
   (let ((current_list lst0))

     (dolist (current_item lst0)
        (cond
          (t nil)
        )
     )
   )
 )



; Copy the object or list into buffer

 (defVar object_buffer nil)

 (defun copy-to-buffer (lst delf)
    (cond
      ( (atom lst)
        (setq object_buffer lst)
      )
      (t
        (if (eq delf 0)
            (setf object_buffer (copy-list lst))
            (setf object_buffer lst))
      )
    )
 )



; Program list editor

 (defun program-editor  (lst)
   (let* ((scrolly 0) (cursor_col 0) (cursor_row 0)
         (changed_flag 0) (current_item 0) (current_index 0) (key 0)
         (bkcolor0 ColorBk_Editor) (bkcolor1 ColorBk_EditorH)
         (rows (- (/ ListEditorHeight (program-line-Height)) 1))
         (columns (/ ListEditorWidth FontWidth))
         (rowpage (1- rows)) (item_edit 0) (debug 0) )

     (fill-rect 0 0 ListEditorWidth ListEditorHeight ColorBk_Editor )
     (setq program_line 0)
     (draw-program lst 0 0 scrolly)

     (loop
        (setq program_line 0)
        (detect-cursor lst nil -1 0 0 scrolly cursor_col cursor_row t)
        (fill-rect (* cursor_col FontWidth) (program-cursory cursor_row) FontWidth 2 Color_CursorEditor )
        (set-cursor 0 (- ListEditorHeight FontHeight))

        (fill-rect (* FontWidth 5) (- ListEditorHeight FontHeight) (- ListEditorWidth (* FontWidth 5)) FontHeight bkcolor0 )
        (if (> cursor_object -1)
            (display-editor-item (* FontWidth 5)
                 (- ListEditorHeight FontHeight)
                 (nth cursor_object cursor_list) )
         )
         (set-cursor 0 (- ListEditorHeight FontHeight))


         (cond
          ((eq debug 1)
            (fill-rect (* FontWidth 5) (- ListEditorHeight 0) (- ListEditorWidth (* FontWidth 5)) FontHeight bkcolor0 )
            (if (> cursor_object_prev -1)
               (display-editor-item (* FontWidth 5)
                 (- ListEditorHeight 0)
                 (nth cursor_object_prev cursor_list_prev) ))
                 (set-cursor 0 (+ ListEditorHeight FontHeight))
                 (draw-string (format nil "room=~a " (room))))
          (t nil)
         )

        (setq key (read-key))

        (setq program_line 0)
        (detect-cursor lst nil -1 0 0 scrolly cursor_col cursor_row 0)

        (fill-rect (* cursor_col FontWidth) (program-cursory cursor_row) FontWidth 2 ColorBk_Editor )

        (set-text-color Color_CursorEditor ColorBk_Editor)
        (set-cursor 0 (- ListEditorHeight FontHeight))
        #| (format t "~a  " key) |#

        (if (key-forQuit key)
           (return )
        )


        (cond
          ((key-forLeft key)
           (let ((cobj0 cursor_object) (clist0 cursor_list) (ccol0 cursor_col)
                 (cursor_eol0 cursor_endoflist) (objx cursor_objectX))
            (loop
             (if (<= cursor_col 0)
               (progn
                 (setq cursor_col ccol0)
                 (return) )
             )
             (if (and (> cursor_object -1)
                      (or (not (eq cobj0 cursor_object))
                          (not (eq clist0 cursor_list))
                          (not (eq cursor_eol0 cursor_endoflist))))
                (return))

             (if (and (> cursor_col objx) (> objx 0))
               (setq cursor_col objx) )

             (setq cursor_col (1- cursor_col))
             (if (> cursor_col cursor_Lineend)
               (setq cursor_col cursor_Lineend))
             (setq program_line 0)
             (detect-cursor lst nil -1 0 0 scrolly cursor_col cursor_row 0)
            )
           )
          )
          ((key-forRight key)
            (let ((cobj0 cursor_object) (clist0 cursor_list) (ccol0 cursor_col)
                  (cursor_eol0 cursor_endoflist) (objx cursor_objectXl))
             (loop
              (if (>= cursor_col (1- columns))
                (progn
                  (setq cursor_col  ccol0)
                  (return) )
              )
              (if (and (> cursor_object -1)
                       (or (not (eq cobj0 cursor_object))
                           (not (eq clist0 cursor_list))
                           (not (eq cursor_eol0 cursor_endoflist))))
                 (return))

              (if (< cursor_col objx)
                (setq cursor_col objx)
                (setq cursor_col (1+ cursor_col)) )
              (if (< cursor_col cursor_Linebgn)
                  (setq cursor_col cursor_Linebgn))
              (setq program_line 0)
              (detect-cursor lst nil -1 0 0 scrolly cursor_col cursor_row 0)
             )
            )
          )
          ((key-forUp key)
           (if (> cursor_row 0)
              (setq cursor_row (1- cursor_row))
              (progn
                 (setq scrolly (- scrolly (program-line-Height)))
                 (if (< scrolly 0) (setq scrolly  0))
                 (fill-rect 0 0 ListEditorWidth ListEditorHeight ColorBk_Editor )
                 (setq program_line 0)
                 (draw-program lst 0 0 scrolly)
              )
           )
          )
          ((key-forDown key)
           (if (< cursor_row (1- rows))
               (setq cursor_row (1+ cursor_row))
               (progn
                  (setq scrolly (+ scrolly (program-line-Height)))
                  (fill-rect 0 0 ListEditorWidth ListEditorHeight ColorBk_Editor )
                  (setq program_line 0)
                  (draw-program lst 0 0 scrolly)
               )
           )
          )
          ((key-forPgUp key)
           (if (> cursor_row 0)
             (setq cursor_row 0)
             (progn
                (setq scrolly (- scrolly (* (program-line-Height) rowpage)))
                (if (< scrolly 0) (setq scrolly  0))
                (fill-rect 0 0 ListEditorWidth ListEditorHeight ColorBk_Editor )
                (setq program_line 0)
                (draw-program lst 0 0 scrolly)))
          )
          ((key-forPgDn key)
          (if (< cursor_row (1- rowpage))
              (setq cursor_row (1- rowpage))
              (progn
                (setq scrolly (+ scrolly (* (program-line-Height) rowpage)))
                (fill-rect 0 0 ListEditorWidth ListEditorHeight ColorBk_Editor )
                (setq program_line 0)
                (draw-program lst 0 0 scrolly)))
          )

          ((and (or (eq key 69) (eq key 101)) (> cursor_object -1))
           (if (< (length (string (nth cursor_object cursor_list))) (- columns 5))
            (progn
             (setq item_edit (line-editor (* FontWidth 5) (- ListEditorHeight (line-editor-Height))
                             (nth cursor_object cursor_list)))
             (if (not (eq item_edit nil))
              (progn
               (setq changed_flag 1)
               (setf (nth cursor_object cursor_list) item_edit)
             ))
             (fill-rect 0 0 ListEditorWidth ListEditorHeight ColorBk_Editor )
             (setq program_line 0)
             (draw-program lst 0 0 scrolly)
           ))
          )

          ;  Delete object
          ((and (or (eq key 68) (eq key 100)) (> cursor_object -1))

            (copy-to-buffer (nth cursor_object cursor_list) 1)

            (if (and (> cursor_object_prev -1) (consp cursor_list_prev))
              (setf (nth cursor_object_prev cursor_list_prev)
                  (append (subseq cursor_list 0 cursor_object)
                          (subseq cursor_list (1+ cursor_object))))
              (setf lst
                (append (subseq cursor_list 0 cursor_object)
                        (subseq cursor_list (1+ cursor_object)))))

            (setq changed_flag 1)
            (fill-rect 0 0 ListEditorWidth ListEditorHeight ColorBk_Editor )
            (setq program_line 0)
            (draw-program lst 0 0 scrolly)
          )

          ;  Insert or append object
          ((and (or (eq key 73) (eq key 105) (eq key 97) (eq key 65)) (> cursor_object -1))

            (if (and (> cursor_object_prev -1) (consp cursor_list_prev))
                (if (or (eq key 97) (eq key 65))
                  (setf (nth cursor_object_prev cursor_list_prev) (append (subseq cursor_list 0) '(nil)))
                  (setf (nth cursor_object_prev cursor_list_prev) (append (subseq cursor_list 0 cursor_object)
                          '(nil) (subseq cursor_list cursor_object)))
                )
                (if (or (eq key 97) (eq key 65))
                  (setq lst (append (subseq cursor_list 0) '(nil)))
                  (setq lst (append (subseq cursor_list 0 cursor_object) '(nil)
                                            (subseq cursor_list cursor_object)))
                )
            )

            (if (or (eq key 97) (eq key 65))
                (setq cursor_object (length cursor_list))
            )
            (if (and (> cursor_object_prev -1) (consp cursor_list_prev))
              (setq cursor_list (nth cursor_object_prev cursor_list_prev))
              (setq cursor_list lst))
            (setq item_edit (line-editor (* FontWidth 5) (- ListEditorHeight (line-editor-Height))
                    '()))
            (if (not (eq item_edit nil))
              (setf (nth cursor_object cursor_list) item_edit)
            )

            (setq changed_flag 1)
            (fill-rect 0 0 ListEditorWidth ListEditorHeight ColorBk_Editor )
            (setq program_line 0)
            (draw-program lst 0 0 scrolly)
          )

          ;  c - copy object o buffer
          ( (and (or (eq key 99) (eq key 67)) (> cursor_object -1))
            (copy-to-buffer (nth cursor_object cursor_list) 0)
          )

          ;  p - Paste object   t - paste-append to tail
          ((and (or (eq key 112) (eq key 80) (eq key 116) (eq key 74)) (> cursor_object -1) (not (eq object_buffer nil)) )

            (if (and (> cursor_object_prev -1) (consp cursor_list_prev))
              (if (eq cursor_endoflist t)
                (setf (nth cursor_object_prev cursor_list_prev) (append (subseq cursor_list 0) '(nil)))
                (if (or (eq key 112) (eq key 80))
                   (setf (nth cursor_object_prev cursor_list_prev) (append (subseq cursor_list 0 cursor_object)
                        '(nil) (subseq cursor_list cursor_object)))
                   (setf (nth cursor_object_prev cursor_list_prev) (append (subseq cursor_list 0)
                             '(nil))))
              )
              (if (eq cursor_endoflist t)
                (setq lst (append (subseq cursor_list 0) '(nil)))
                (if (or (eq key 112) (eq key 80))
                  (setq lst (append (subseq cursor_list 0 cursor_object) '(nil)
                                          (subseq cursor_list cursor_object)))
                  (setq lst (append (subseq cursor_list 0) '(nil) ))
                )
              )
            )

            (if (or (eq cursor_endoflist t) (eq key 116) (eq key 74))
                (setq cursor_object (length cursor_list))
            )
            (if (and (> cursor_object_prev -1) (consp cursor_list_prev))
              (setq cursor_list (nth cursor_object_prev cursor_list_prev))
              (setq cursor_list lst))

            (setf (nth cursor_object cursor_list) object_buffer)

            (if (consp object_buffer)
               (setq object_buffer nil)
            )

            (setq changed_flag 1)
            (fill-rect 0 0 ListEditorWidth ListEditorHeight ColorBk_Editor )
            (setq program_line 0)
            (draw-program lst 0 0 scrolly)
          )

          ;  Run and eval
          ((and (or (eq key 114) (eq key 82)) (> cursor_object -1))
            (ignore-errors
             (let* ( (str (string (eval (nth cursor_object cursor_list)))) (len (length str)))
               (if (> len 32) (setq len 32))
               (set-cursor 0 0)
               (set-text-color  ColorBk_Editor (rgb 255 127 255))
               (draw-string (format nil " Ret=~a " (subseq str 0 len)))
             )
            )

            (setq key (read-key))

            (fill-rect 0 0 ListEditorWidth ListEditorHeight ColorBk_Editor )
            (setq program_line 0)
            (draw-program lst 0 0 scrolly)
          )


          ((eq key 13)
           (if (= cursor_object -1) (draw-string "***")

           (if (not (atom (nth cursor_object cursor_list)))
             (progn
              (setq item_edit (program-editor (nth cursor_object cursor_list)))
              (if (not (eq item_edit nil))
               (progn
                (setf (nth cursor_object cursor_list) item_edit)
                (setq changed_flag 1))))))
           (fill-rect 0 0 ListEditorWidth ListEditorHeight ColorBk_Editor )
           (setq program_line 0)
           (draw-program lst 0 0 scrolly)
          )

          ((key-forHelp key)
           (PrintHelp)
           (fill-rect 0 0 ListEditorWidth ListEditorHeight ColorBk_Editor )
           (setq program_line 0)
           (draw-program lst 0 0 scrolly)
          )

          (t nil)
        )

        (if (or (eq key 113) (eq key 81) (eq key -23) (eq key -55) (eq key 57))
         (return )
        )
     )
     (set-text-color Color_CursorEditor ColorBk_Editor)
     (if (eq changed_flag 1) lst nil)
   )
 )
)
