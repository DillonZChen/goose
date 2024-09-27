(define (problem p0_23-problem)
 (:domain p0_23-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 - block
   c1 c2 c3 c4 c5 c6 c7 c8 c9 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 5) (= (capacity c2) 3) (= (capacity c3) 2) (= (capacity c4) 1) (= (capacity c5) 0) (= (capacity c6) 5) (= (capacity c7) 8) (= (capacity c8) 5) (= (capacity c9) 5) (base b3) (in b3 c1) (on b3 c1) (clear b3) (base b4) (in b4 c2) (on b4 c2) (on b9 b4) (in b9 c2) (clear b9) (base b2) (in b2 c3) (on b2 c3) (on b16 b2) (in b16 c3) (on b11 b16) (in b11 c3) (clear b11) (base b8) (in b8 c4) (on b8 c4) (on b22 b8) (in b22 c4) (on b12 b22) (in b12 c4) (on b1 b12) (in b1 c4) (clear b1) (base b20) (in b20 c5) (on b20 c5) (on b10 b20) (in b10 c5) (on b23 b10) (in b23 c5) (on b17 b23) (in b17 c5) (on b6 b17) (in b6 c5) (on b18 b6) (in b18 c5) (on b21 b18) (in b21 c5) (on b14 b21) (in b14 c5) (on b15 b14) (in b15 c5) (on b5 b15) (in b5 c5) (on b13 b5) (in b13 c5) (on b7 b13) (in b7 c5) (on b19 b7) (in b19 c5) (clear b19) (clear c6) (clear c7) (clear c8) (clear c9))
 (:goal (and (on b3 c1) (clear b3) (on b12 c2) (clear b12) (on b9 c3) (clear b9) (on b17 c4) (on b8 b17) (clear b8) (on b6 c5) (on b20 b6) (on b1 b20) (on b13 b1) (on b22 b13) (clear b22) (on b18 c6) (on b19 b18) (on b2 b19) (on b4 b2) (on b7 b4) (clear b7) (on b23 c7) (on b10 b23) (on b11 b10) (on b14 b11) (on b15 b14) (on b21 b15) (on b16 b21) (on b5 b16) (clear b5)))
)
