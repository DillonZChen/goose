(define (problem p0_15-problem)
 (:domain p0_15-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 7) (= (capacity c2) 9) (= (capacity c3) 0) (= (capacity c4) 8) (base b16) (in b16 c1) (on b16 c1) (on b14 b16) (in b14 c1) (clear b14) (base b2) (in b2 c2) (on b2 c2) (on b13 b2) (in b13 c2) (on b8 b13) (in b8 c2) (clear b8) (base b6) (in b6 c3) (on b6 c3) (on b10 b6) (in b10 c3) (on b4 b10) (in b4 c3) (on b5 b4) (in b5 c3) (on b9 b5) (in b9 c3) (on b7 b9) (in b7 c3) (on b15 b7) (in b15 c3) (on b11 b15) (in b11 c3) (on b12 b11) (in b12 c3) (on b3 b12) (in b3 c3) (on b1 b3) (in b1 c3) (clear b1) (clear c4))
 (:goal (and (on b6 c1) (on b9 b6) (on b13 b9) (on b4 b13) (clear b4) (on b15 c2) (on b16 b15) (on b8 b16) (on b7 b8) (on b1 b7) (on b12 b1) (on b3 b12) (on b14 b3) (on b2 b14) (on b11 b2) (on b5 b11) (on b10 b5) (clear b10)))
)
