(define (problem p0_18-problem)
 (:domain p0_18-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 9) (= (capacity c2) 6) (= (capacity c3) 2) (= (capacity c4) 11) (base b1) (in b1 c1) (on b1 c1) (on b16 b1) (in b16 c1) (on b13 b16) (in b13 c1) (clear b13) (base b11) (in b11 c2) (on b11 c2) (on b14 b11) (in b14 c2) (on b17 b14) (in b17 c2) (on b9 b17) (in b9 c2) (on b2 b9) (in b2 c2) (on b8 b2) (in b8 c2) (clear b8) (base b15) (in b15 c3) (on b15 c3) (on b6 b15) (in b6 c3) (on b10 b6) (in b10 c3) (on b4 b10) (in b4 c3) (on b19 b4) (in b19 c3) (on b12 b19) (in b12 c3) (on b7 b12) (in b7 c3) (on b18 b7) (in b18 c3) (on b5 b18) (in b5 c3) (on b3 b5) (in b3 c3) (clear b3) (clear c4))
 (:goal (and (on b6 c1) (on b5 b6) (on b19 b5) (clear b19) (on b1 c2) (on b17 b1) (on b12 b17) (on b9 b12) (on b15 b9) (on b7 b15) (on b14 b7) (on b11 b14) (clear b11) (on b13 c3) (on b18 b13) (on b3 b18) (on b2 b3) (on b4 b2) (on b10 b4) (on b16 b10) (on b8 b16) (clear b8)))
)
