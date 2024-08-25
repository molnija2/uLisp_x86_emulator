(progn
 (defvar TestList '(1 2 3))
 (defvar Test 0)
 (defvar ListEditorWidth 320)
 (defvar ListEditorHeight 240)
 (defvar FontWidth 8)
 (defvar FontHeight 11)
 (defvar ColorBk_Editor0 (rgb 255 255 255))
 (defvar ColorBk_Editor1 (rgb 0 0 0))
 (defvar Color_LineEditor (rgb 0 0 0))

  (defun line-editor-Height () (+ FontHeight 2))


  (defun line-editor  (posx posy item_org)
    (let* ((str_item (string item_org)) (key 0) (len (length str_item))
           (cursor_char len) (return_flag 0))
      (set-cursor posx posy)
      (fill-rect posx posy (- ListEditorWidth posx) (line-editor-Height) ColorBk_Editor0 )
      (set-text-color Color_LineEditor ColorBk_Editor0)
      (write-string (subseq str_item 0 len) )
      (loop
        (fill-rect (+ posx (* cursor_char FontWidth)) (+ posy FontHeight) FontWidth 2 Color_LineEditor )
        (set-cursor 0 (- ListEditorHeight FontHeight))
        (with-serial (str 0) (setq key (read-byte str)))
        (set-cursor 0 (- ListEditorHeight FontHeight))
        (format t "~a  " key)
        (fill-rect (+ posx (* cursor_char FontWidth)) (+ posy FontHeight) FontWidth 2 ColorBk_Editor0 )

        (set-cursor posx posy)
        (if (eq key 0)
         (progn
          (set-cursor 0 (- ListEditorHeight FontHeight))
          (with-serial (str 0) (setq key (read-byte str)))
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
             (fill-rect 0 (- ListEditorHeight FontHeight) ListEditorWidth (line-editor-Height) ColorBk_Editor0 )
             (setq len (length str_item))
             (set-cursor posx posy)
             (set-text-color Color_LineEditor ColorBk_Editor0)
             (write-string (subseq str_item 0 len) )
           )
           (t (set-text-color Color_LineEditor ColorBk_Editor0)
              (set-cursor 0 (- ListEditorHeight FontHeight))
              (format t "0+~a " key))
          )
         )
         (progn
          (cond
           ((eq key 8)
            (if (> cursor_char 0)
             (setq str_item
               (concatenate 'string
                 (subseq str_item 0 (1- cursor_char))
                 (subseq str_item cursor_char len))))
             (setq cursor_char (1- cursor_char))
             (fill-rect 0 (- ListEditorHeight (line-editor-Height)) ListEditorWidth (line-editor-Height) ColorBk_Editor0 )
           )
           (t
              (if (eq key 13)
                (progn
                  (setq return_flag 2) (setq cursor_char len) )
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
          (set-text-color Color_LineEditor ColorBk_Editor0)
          (write-string (subseq str_item 0 len) )
         )
        )
        (if (not (eq return_flag 0)) (return))
      )
      (if (eq return_flag 1) nil)
      (if (eq return_flag 2) (read-from-string str_item))
    )
  )

  (defun display-editor-item  (posx posy item)
     (let* ((str_item (string item)) (key 0) (len (length str_item)) (len_org len))
      (set-cursor posx posy)
      (set-text-color Color_LineEditor ColorBk_Editor0)
      (if (> len 32) (setq len 32))
      (write-string (subseq str_item 0 len) )
      len_org
    )
  )

  (defun item-color-set (item bkcolor)
    (cond
     ( (atom item)
       (set-text-color (rgb 255 255 255) bkcolor)
       (if (boundp (quote item))
        (if (consp (quote item))
         (set-text-color (rgb 0 255 0) bkcolor)
         (set-text-color (rgb 255 255 255) bkcolor)
        )
        (set-text-color (rgb 0 0 255) bkcolor)
       )
       (if (numberp  item)
             (set-text-color (rgb 255 255 180) bkcolor)
       )
       (if (floatp  item)
           (set-text-color (rgb 255 255 180) bkcolor)
       )
     )
     (t (set-text-color (rgb 0 255 0) bkcolor))
    )
  )

  (defun item-get-string (item)
   (cond
    ( (atom item)
     (string  item)
    )
    (t
      (if (atom (car  item))
        (format nil "(~a)"  (car item) )
        (format nil "((...)" )
      )
    )
   )
  )

  (defun calc-list-coords (lst posx posy)
    (let ((len (length lst)) (a nil)
          (curx 0) (cury 0)
          (item_width 0) (index 0) (items_in_line 0) (cursor_item 0))

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

  (defun print-list-editor (lst posx posy)
    (let ((len (length lst)) (a nil)
        (curx 0) (cury 0)
        (item_width 0) (index 0) (items_in_line 0) (cursor_item 0))

     (fill-rect 0 0 ListEditorWidth ListEditorHeight bkcolor0 )

     (if (> len 0)
      (progn
        (setq index 0)
        (dolist (x lst)
         (set-cursor (aref posx index) (aref posy index))
         (item-color-set x bkcolor0)
         (write-string (item-get-string x))
         (setq index (1+ index))
       )))
     )
   )


   (defun edit-list (lst)
       (let ((len (length lst)) (a nil) (item_edit 0) (changed_flag 0)
             (posx 0) (posy 0) (curx 0) (cury 0) (key 0) (bkcolor0 (rgb 50 50 50)) (bkcolor1 (rgb 200 100 100))
             (item_width 0) (index 0) (items_in_line 0) (cursor_item 0) (item_str_length 0))

      (if (> len 0)
       (progn
            (setq posx (make-array len ))
            (setq posy (make-array len ))
      ))

      (calc-list-coords lst posx posy)

     (print-list-editor lst posx posy)

     (loop
       (if (> len 0)
         (set-cursor (aref posx cursor_item) (aref posy cursor_item)))
        (item-color-set (nth cursor_item lst) bkcolor1)
        (write-string (item-get-string (nth cursor_item lst)))

        (fill-rect (* FontWidth 5) (- ListEditorHeight FontHeight) (- ListEditorWidth (* FontWidth 5)) FontHeight bkcolor0 )
        (setq item_str_length (display-editor-item (* FontWidth 5) (- ListEditorHeight FontHeight) (nth cursor_item lst)))

        (set-cursor 0 (- ListEditorHeight FontHeight))
        (item-color-set (nth cursor_item lst) bkcolor0)

        (ignore-errors (with-serial (str 0)
          (setq key (read-byte str))
        ))
        (set-cursor 0 (- ListEditorHeight FontHeight))
        (item-color-set (nth cursor_item lst) bkcolor0)
        (format t "~a  " key)

        (if (> len 0) (set-cursor (aref posx cursor_item) (aref posy cursor_item)))
        (item-color-set (nth cursor_item lst) bkcolor0)
        (write-string (item-get-string (nth cursor_item lst)))


        (if (or (eq key 113) (eq key 81) (eq key -23) (eq key -55) (eq key 57))
           (return )
        )

        (cond
          ((eq key 18) (if (> cursor_item 0) (setq cursor_item (1- cursor_item))))
          ((eq key 20) (if (< cursor_item (1- len)) (setq cursor_item (1+ cursor_item))))
          ((eq key 19)
            (if (> cursor_item 0)
              (let ((cursor cursor_item))
               (loop
                (setq cursor_item (1- cursor_item))
                (if (eq cursor_item 0) (return nil))
                (if (and (not (eq (aref posy cursor_item) (aref posy cursor)))
                      (<= (aref posx cursor_item) (aref posx cursor)))
                  (return nil))
               )
              )
            )
          )
          ((eq key 21)
            (if (< cursor_item (1- len))
             (let ((cursor cursor_item))
              (loop
               (setq cursor_item (1+ cursor_item))
               (if (eq cursor_item (1- len)) (return nil))
               (if (and (not (eq (aref posy cursor_item) (aref posy cursor)))
                        (>= (aref posx cursor_item) (aref posx cursor)))
                 (return nil))
              )
             )
            )
          )
          ((eq key 13)
            (if (not (atom (nth cursor_item lst)))
             (progn
              (setq item_edit (edit-list (nth cursor_item lst)))
              (if (not (eq item_edit nil))
                (setf (nth cursor_item lst) item_edit)
               )
              )
            )
            (calc-list-coords lst posx posy)

            (print-list-editor lst posx posy)
          )
          ((or (eq key 68) (eq key 100))
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

            (print-list-editor lst posx posy)
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

            (print-list-editor lst posx posy)
          )
          ((and (or (eq key 69) (eq key 101)) (< item_str_length 128))
            (setq item_edit (line-editor (* FontWidth 5) (- ListEditorHeight (line-editor-Height)) (nth cursor_item lst)))
            (if (not(eq item_edit nil))
                (setf (nth cursor_item lst) item_edit) )

            (calc-list-coords lst posx posy)

            (print-list-editor lst posx posy)
          )
          (t nil)
        )

     )
     (set-cursor 0 (- ListEditorHeight FontHeight))

     (if (> len 0)
      (progn
        (makunbound 'posx)
        (makunbound 'posy)
     ))
     (if (eq changed_flag 1) lst nil)
    )
  )


 (defvar MainProgram
  '(progn
     TestList
     Test
    (with-gfx (str)
     (cls)
     (fill-rect 10 90 200 100 (rgb 255 0 255) )
     (fill-circle 200 100 50 (rgb 255 0 0) )
     (draw-circle 100 130 50 (rgb 0 0 255) )
     (set-text-color (rgb 255 255 255) 0)
     (set-cursor 20 20)
     (set-text-color (rgb 0 255 0) 0)
     (write-string "=== micro-Lisp ===" str)
     (set-cursor 0 200)
     (edit-list knowleges)
     (if (consp knowleges_new) (format t "Return: ~a" knowleges_new))
    )
  )
 )
)
