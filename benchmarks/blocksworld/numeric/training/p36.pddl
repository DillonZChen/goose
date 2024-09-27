(define (problem p36-problem)
 (:domain p36-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 3) (= (capacity c2) 0) (= (capacity c3) 7) (= (capacity c4) 6) (base b8) (in b8 c1) (on b8 c1) (on b11 b8) (in b11 c1) (on b5 b11) (in b5 c1) (on b9 b5) (in b9 c1) (clear b9) (base b4) (in b4 c2) (on b4 c2) (on b6 b4) (in b6 c2) (on b1 b6) (in b1 c2) (on b7 b1) (in b7 c2) (on b3 b7) (in b3 c2) (on b10 b3) (in b10 c2) (on b2 b10) (in b2 c2) (clear b2) (clear c3) (clear c4))
 (:goal (and (on b10 c1) (clear b10) (on b4 c2) (on b5 b4) (on b1 b5) (on b9 b1) (clear b9) (on b6 c3) (on b2 b6) (on b7 b2) (on b3 b7) (on b11 b3) (on b8 b11) (clear b8)))
)
