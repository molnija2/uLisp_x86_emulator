;  Simpe example program

(progn
 (defvar TestList '(1 2 3))
 (defvar Test 0)
 (defvar knowleges_new nil)

  (defun print-spaces (n s)
    (dotimes (i n) (write-string " " s) )
  )

 (defun program-line-Height ( ) (+ FontHeight 4))

 (defun editprg () (setq knowleges_new (list-editor knowleges)))
 (defun loadwsp () (load-image))

 (defvar MainProgram
  '(progn
     TestList
     Test
    (with-gfx (str)
     (cls)
     (defvar Test2 "TestString")

     (fill-rect 10 90 200 100 (rgb 255 0 255) )
     (fill-circle 200 100 50 (rgb 255 0 0) )
     (draw-circle 100 130 50 (rgb 0 0 255) )
     (set-text-color (rgb 255 255 255) 0)
     (set-cursor 20 20)
     (set-text-color (rgb 0 255 0) 0)
     (write-string "=== micro-Lisp ===" str)
     (set-cursor 0 200)

;    Call list-editor
     (setq knowleges_new (list-editor knowleges))

;    Test result
     (if (consp knowleges_new)
        (save-program knowleges_new "test_program_ret.l")
     )
    )
   )
  )
 )
)

