(define (problem p70-problem)
 (:domain p70-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 - block
   c1 c2 c3 c4 c5 c6 c7 c8 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 3) (= (capacity c2) 0) (= (capacity c3) 3) (= (capacity c4) 3) (= (capacity c5) 5) (= (capacity c6) 11) (= (capacity c7) 3) (= (capacity c8) 3) (base b7) (in b7 c1) (on b7 c1) (clear b7) (base b14) (in b14 c2) (on b14 c2) (on b12 b14) (in b12 c2) (on b2 b12) (in b2 c2) (on b1 b2) (in b1 c2) (on b18 b1) (in b18 c2) (on b17 b18) (in b17 c2) (on b19 b17) (in b19 c2) (on b5 b19) (in b5 c2) (on b6 b5) (in b6 c2) (on b15 b6) (in b15 c2) (on b16 b15) (in b16 c2) (on b8 b16) (in b8 c2) (on b9 b8) (in b9 c2) (on b20 b9) (in b20 c2) (on b11 b20) (in b11 c2) (on b3 b11) (in b3 c2) (on b10 b3) (in b10 c2) (on b4 b10) (in b4 c2) (on b21 b4) (in b21 c2) (on b13 b21) (in b13 c2) (clear b13) (clear c3) (clear c4) (clear c5) (clear c6) (clear c7) (clear c8))
 (:goal (and (on b19 c1) (clear b19) (on b16 c2) (clear b16) (on b14 c3) (clear b14) (on b17 c4) (on b20 b17) (clear b20) (on b13 c5) (on b21 b13) (on b12 b21) (on b11 b12) (on b7 b11) (clear b7) (on b15 c6) (on b3 b15) (on b8 b3) (on b6 b8) (on b1 b6) (on b5 b1) (on b9 b5) (on b4 b9) (on b18 b4) (on b2 b18) (on b10 b2) (clear b10)))
)
