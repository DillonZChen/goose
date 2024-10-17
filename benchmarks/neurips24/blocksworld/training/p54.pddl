(define (problem p54-problem)
 (:domain p54-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 13) (= (capacity c3) 6) (= (capacity c4) 5) (base b15) (in b15 c1) (on b15 c1) (on b6 b15) (in b6 c1) (on b9 b6) (in b9 c1) (on b14 b9) (in b14 c1) (on b7 b14) (in b7 c1) (on b8 b7) (in b8 c1) (on b1 b8) (in b1 c1) (on b3 b1) (in b3 c1) (on b13 b3) (in b13 c1) (on b4 b13) (in b4 c1) (on b16 b4) (in b16 c1) (on b5 b16) (in b5 c1) (on b10 b5) (in b10 c1) (on b2 b10) (in b2 c1) (on b11 b2) (in b11 c1) (on b12 b11) (in b12 c1) (clear b12) (clear c2) (clear c3) (clear c4))
 (:goal (and (on b12 c1) (on b2 b12) (on b10 b2) (clear b10) (on b8 c2) (on b11 b8) (on b14 b11) (on b16 b14) (on b4 b16) (on b13 b4) (on b1 b13) (on b9 b1) (on b7 b9) (on b3 b7) (on b15 b3) (on b6 b15) (on b5 b6) (clear b5)))
)
