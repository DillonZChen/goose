(define (problem p49-problem)
 (:domain p49-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 8) (= (capacity c2) 3) (= (capacity c3) 3) (= (capacity c4) 8) (base b3) (in b3 c1) (on b3 c1) (clear b3) (base b9) (in b9 c2) (on b9 c2) (on b14 b9) (in b14 c2) (on b11 b14) (in b11 c2) (on b10 b11) (in b10 c2) (on b15 b10) (in b15 c2) (on b2 b15) (in b2 c2) (clear b2) (base b6) (in b6 c3) (on b6 c3) (on b8 b6) (in b8 c3) (on b7 b8) (in b7 c3) (on b4 b7) (in b4 c3) (on b5 b4) (in b5 c3) (on b1 b5) (in b1 c3) (on b13 b1) (in b13 c3) (on b12 b13) (in b12 c3) (clear b12) (clear c4))
 (:goal (and (on b9 c1) (clear b9) (on b4 c2) (on b5 b4) (on b10 b5) (clear b10) (on b8 c3) (on b1 b8) (on b2 b1) (on b11 b2) (on b12 b11) (on b7 b12) (on b3 b7) (on b13 b3) (on b15 b13) (on b6 b15) (on b14 b6) (clear b14)))
)
