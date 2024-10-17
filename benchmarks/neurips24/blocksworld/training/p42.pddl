(define (problem p42-problem)
 (:domain p42-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 6) (= (capacity c3) 7) (= (capacity c4) 5) (base b11) (in b11 c1) (on b11 c1) (on b1 b11) (in b1 c1) (on b2 b1) (in b2 c1) (on b3 b2) (in b3 c1) (on b10 b3) (in b10 c1) (on b5 b10) (in b5 c1) (on b9 b5) (in b9 c1) (on b6 b9) (in b6 c1) (on b4 b6) (in b4 c1) (on b8 b4) (in b8 c1) (on b7 b8) (in b7 c1) (on b12 b7) (in b12 c1) (clear b12) (clear c2) (clear c3) (clear c4))
 (:goal (and (on b7 c1) (clear b7) (on b5 c2) (on b3 b5) (on b4 b3) (on b10 b4) (clear b10) (on b8 c3) (on b2 b8) (on b11 b2) (on b6 b11) (on b1 b6) (on b12 b1) (on b9 b12) (clear b9)))
)
