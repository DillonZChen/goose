(define (problem p35-problem)
 (:domain p35-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 6) (= (capacity c2) 2) (= (capacity c3) 1) (= (capacity c4) 6) (base b10) (in b10 c1) (on b10 c1) (clear b10) (base b3) (in b3 c2) (on b3 c2) (on b2 b3) (in b2 c2) (on b7 b2) (in b7 c2) (on b8 b7) (in b8 c2) (clear b8) (base b4) (in b4 c3) (on b4 c3) (on b9 b4) (in b9 c3) (on b5 b9) (in b5 c3) (on b6 b5) (in b6 c3) (on b1 b6) (in b1 c3) (clear b1) (clear c4))
 (:goal (and (on b8 c1) (on b10 b8) (on b1 b10) (on b7 b1) (clear b7) (on b3 c2) (on b5 b3) (on b2 b5) (on b6 b2) (on b9 b6) (on b4 b9) (clear b4)))
)
