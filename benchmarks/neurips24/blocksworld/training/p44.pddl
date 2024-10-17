(define (problem p44-problem)
 (:domain p44-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 6) (= (capacity c2) 6) (= (capacity c3) 0) (= (capacity c4) 7) (base b6) (in b6 c1) (on b6 c1) (on b9 b6) (in b9 c1) (clear b9) (base b11) (in b11 c2) (on b11 c2) (on b4 b11) (in b4 c2) (on b12 b4) (in b12 c2) (on b13 b12) (in b13 c2) (clear b13) (base b10) (in b10 c3) (on b10 c3) (on b1 b10) (in b1 c3) (on b5 b1) (in b5 c3) (on b3 b5) (in b3 c3) (on b7 b3) (in b7 c3) (on b2 b7) (in b2 c3) (on b8 b2) (in b8 c3) (clear b8) (clear c4))
 (:goal (and (on b10 c1) (on b1 b10) (on b7 b1) (clear b7) (on b13 c2) (on b4 b13) (on b3 b4) (on b8 b3) (on b9 b8) (on b6 b9) (on b12 b6) (on b11 b12) (on b5 b11) (on b2 b5) (clear b2)))
)
