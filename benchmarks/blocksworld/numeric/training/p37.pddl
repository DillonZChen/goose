(define (problem p37-problem)
 (:domain p37-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 8) (= (capacity c2) 3) (= (capacity c3) 0) (= (capacity c4) 5) (base b11) (in b11 c1) (on b11 c1) (on b4 b11) (in b4 c1) (on b6 b4) (in b6 c1) (clear b6) (base b3) (in b3 c2) (on b3 c2) (on b9 b3) (in b9 c2) (on b5 b9) (in b5 c2) (clear b5) (base b8) (in b8 c3) (on b8 c3) (on b2 b8) (in b2 c3) (on b7 b2) (in b7 c3) (on b1 b7) (in b1 c3) (on b10 b1) (in b10 c3) (clear b10) (clear c4))
 (:goal (and (on b1 c1) (on b10 b1) (on b5 b10) (on b4 b5) (on b9 b4) (on b2 b9) (on b11 b2) (on b7 b11) (on b6 b7) (on b3 b6) (on b8 b3) (clear b8)))
)
