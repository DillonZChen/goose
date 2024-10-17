(define (problem p40-problem)
 (:domain p40-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 6) (= (capacity c2) 5) (= (capacity c3) 0) (= (capacity c4) 7) (base b11) (in b11 c1) (on b11 c1) (on b1 b11) (in b1 c1) (clear b1) (base b4) (in b4 c2) (on b4 c2) (on b6 b4) (in b6 c2) (on b9 b6) (in b9 c2) (clear b9) (base b2) (in b2 c3) (on b2 c3) (on b12 b2) (in b12 c3) (on b7 b12) (in b7 c3) (on b8 b7) (in b8 c3) (on b10 b8) (in b10 c3) (on b3 b10) (in b3 c3) (on b5 b3) (in b5 c3) (clear b5) (clear c4))
 (:goal (and (on b5 c1) (on b4 b5) (on b7 b4) (clear b7) (on b8 c2) (on b11 b8) (on b9 b11) (on b3 b9) (clear b3) (on b1 c3) (on b10 b1) (on b6 b10) (on b12 b6) (on b2 b12) (clear b2)))
)
