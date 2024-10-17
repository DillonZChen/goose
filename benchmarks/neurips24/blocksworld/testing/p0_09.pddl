(define (problem p0_09-problem)
 (:domain p0_09-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 10) (= (capacity c3) 3) (= (capacity c4) 3) (base b2) (in b2 c1) (on b2 c1) (on b5 b2) (in b5 c1) (on b8 b5) (in b8 c1) (on b7 b8) (in b7 c1) (on b3 b7) (in b3 c1) (on b4 b3) (in b4 c1) (on b10 b4) (in b10 c1) (on b9 b10) (in b9 c1) (on b11 b9) (in b11 c1) (on b1 b11) (in b1 c1) (on b6 b1) (in b6 c1) (clear b6) (clear c2) (clear c3) (clear c4))
 (:goal (and (on b1 c1) (clear b1) (on b4 c2) (on b7 b4) (on b2 b7) (on b11 b2) (on b3 b11) (on b10 b3) (on b6 b10) (on b5 b6) (on b8 b5) (on b9 b8) (clear b9)))
)
