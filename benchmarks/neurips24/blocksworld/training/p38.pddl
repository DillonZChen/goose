(define (problem p38-problem)
 (:domain p38-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 5) (= (capacity c2) 0) (= (capacity c3) 6) (= (capacity c4) 5) (base b7) (in b7 c1) (on b7 c1) (clear b7) (base b5) (in b5 c2) (on b5 c2) (on b10 b5) (in b10 c2) (on b4 b10) (in b4 c2) (on b8 b4) (in b8 c2) (on b11 b8) (in b11 c2) (on b9 b11) (in b9 c2) (on b6 b9) (in b6 c2) (on b3 b6) (in b3 c2) (on b2 b3) (in b2 c2) (on b1 b2) (in b1 c2) (clear b1) (clear c3) (clear c4))
 (:goal (and (on b5 c1) (clear b5) (on b2 c2) (on b3 b2) (on b9 b3) (on b7 b9) (on b6 b7) (on b1 b6) (on b10 b1) (on b8 b10) (on b4 b8) (on b11 b4) (clear b11)))
)
