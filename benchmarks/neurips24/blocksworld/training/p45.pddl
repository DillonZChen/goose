(define (problem p45-problem)
 (:domain p45-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 4) (= (capacity c2) 8) (= (capacity c3) 1) (= (capacity c4) 6) (base b11) (in b11 c1) (on b11 c1) (on b8 b11) (in b8 c1) (on b5 b8) (in b5 c1) (clear b5) (base b13) (in b13 c2) (on b13 c2) (on b7 b13) (in b7 c2) (on b4 b7) (in b4 c2) (on b1 b4) (in b1 c2) (clear b1) (base b2) (in b2 c3) (on b2 c3) (on b10 b2) (in b10 c3) (on b3 b10) (in b3 c3) (on b6 b3) (in b6 c3) (on b9 b6) (in b9 c3) (on b12 b9) (in b12 c3) (clear b12) (clear c4))
 (:goal (and (on b6 c1) (clear b6) (on b12 c2) (on b1 b12) (on b7 b1) (on b5 b7) (on b9 b5) (on b3 b9) (on b10 b3) (on b2 b10) (on b13 b2) (on b8 b13) (on b11 b8) (on b4 b11) (clear b4)))
)
