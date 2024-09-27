(define (problem p0_20-problem)
 (:domain p0_20-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 - block
   c1 c2 c3 c4 c5 c6 c7 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 4) (= (capacity c2) 2) (= (capacity c3) 0) (= (capacity c4) 6) (= (capacity c5) 8) (= (capacity c6) 5) (= (capacity c7) 5) (base b5) (in b5 c1) (on b5 c1) (clear b5) (base b14) (in b14 c2) (on b14 c2) (on b17 b14) (in b17 c2) (on b12 b17) (in b12 c2) (clear b12) (base b1) (in b1 c3) (on b1 c3) (on b16 b1) (in b16 c3) (on b9 b16) (in b9 c3) (on b13 b9) (in b13 c3) (on b18 b13) (in b18 c3) (on b4 b18) (in b4 c3) (on b10 b4) (in b10 c3) (on b7 b10) (in b7 c3) (on b8 b7) (in b8 c3) (on b19 b8) (in b19 c3) (on b6 b19) (in b6 c3) (on b11 b6) (in b11 c3) (on b2 b11) (in b2 c3) (on b20 b2) (in b20 c3) (on b15 b20) (in b15 c3) (on b3 b15) (in b3 c3) (clear b3) (clear c4) (clear c5) (clear c6) (clear c7))
 (:goal (and (on b17 c1) (clear b17) (on b16 c2) (clear b16) (on b11 c3) (on b9 b11) (on b7 b9) (on b13 b7) (clear b13) (on b6 c4) (on b8 b6) (on b3 b8) (on b2 b3) (on b20 b2) (on b15 b20) (clear b15) (on b5 c5) (on b19 b5) (on b4 b19) (on b1 b4) (on b14 b1) (on b18 b14) (on b12 b18) (on b10 b12) (clear b10)))
)
