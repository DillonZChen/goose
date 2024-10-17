(define (problem p0_07-problem)
 (:domain p0_07-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 7) (= (capacity c2) 0) (= (capacity c3) 4) (= (capacity c4) 4) (base b6) (in b6 c1) (on b6 c1) (on b2 b6) (in b2 c1) (on b10 b2) (in b10 c1) (clear b10) (base b8) (in b8 c2) (on b8 c2) (on b5 b8) (in b5 c2) (on b3 b5) (in b3 c2) (on b9 b3) (in b9 c2) (on b7 b9) (in b7 c2) (on b1 b7) (in b1 c2) (on b4 b1) (in b4 c2) (clear b4) (clear c3) (clear c4))
 (:goal (and (on b4 c1) (on b5 b4) (on b9 b5) (on b6 b9) (on b7 b6) (on b2 b7) (on b10 b2) (on b8 b10) (on b3 b8) (on b1 b3) (clear b1)))
)
