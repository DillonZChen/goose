(define (problem p0_16-problem)
 (:domain p0_16-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 13) (= (capacity c2) 3) (= (capacity c3) 1) (= (capacity c4) 8) (base b7) (in b7 c1) (on b7 c1) (on b9 b7) (in b9 c1) (on b12 b9) (in b12 c1) (on b13 b12) (in b13 c1) (clear b13) (base b5) (in b5 c2) (on b5 c2) (on b14 b5) (in b14 c2) (on b15 b14) (in b15 c2) (on b10 b15) (in b10 c2) (on b6 b10) (in b6 c2) (on b2 b6) (in b2 c2) (clear b2) (base b16) (in b16 c3) (on b16 c3) (on b4 b16) (in b4 c3) (on b1 b4) (in b1 c3) (on b3 b1) (in b3 c3) (on b11 b3) (in b11 c3) (on b8 b11) (in b8 c3) (on b17 b8) (in b17 c3) (clear b17) (clear c4))
 (:goal (and (on b16 c1) (on b12 b16) (on b8 b12) (on b9 b8) (on b15 b9) (on b10 b15) (on b2 b10) (on b11 b2) (on b17 b11) (on b4 b17) (on b6 b4) (on b5 b6) (on b7 b5) (on b14 b7) (on b1 b14) (on b13 b1) (on b3 b13) (clear b3)))
)
