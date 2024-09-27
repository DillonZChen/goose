(define (problem p39-problem)
 (:domain p39-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 11) (= (capacity c2) 0) (= (capacity c3) 4) (= (capacity c4) 3) (base b2) (in b2 c1) (on b2 c1) (clear b2) (base b5) (in b5 c2) (on b5 c2) (on b7 b5) (in b7 c2) (on b4 b7) (in b4 c2) (on b8 b4) (in b8 c2) (on b3 b8) (in b3 c2) (on b10 b3) (in b10 c2) (on b11 b10) (in b11 c2) (on b1 b11) (in b1 c2) (on b6 b1) (in b6 c2) (on b12 b6) (in b12 c2) (on b9 b12) (in b9 c2) (clear b9) (clear c3) (clear c4))
 (:goal (and (on b9 c1) (on b2 b9) (on b7 b2) (on b3 b7) (on b6 b3) (on b1 b6) (on b10 b1) (on b11 b10) (on b4 b11) (on b12 b4) (on b8 b12) (on b5 b8) (clear b5)))
)
