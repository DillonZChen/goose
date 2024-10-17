(define (problem p0_29-problem)
 (:domain p0_29-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 - block
   c1 c2 c3 c4 c5 c6 c7 c8 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 7) (= (capacity c2) 5) (= (capacity c3) 5) (= (capacity c4) 4) (= (capacity c5) 7) (= (capacity c6) 0) (= (capacity c7) 7) (= (capacity c8) 7) (base b9) (in b9 c1) (on b9 c1) (clear b9) (base b23) (in b23 c2) (on b23 c2) (on b20 b23) (in b20 c2) (clear b20) (base b15) (in b15 c3) (on b15 c3) (on b5 b15) (in b5 c3) (clear b5) (base b25) (in b25 c4) (on b25 c4) (on b21 b25) (in b21 c4) (on b26 b21) (in b26 c4) (clear b26) (base b16) (in b16 c5) (on b16 c5) (on b10 b16) (in b10 c5) (on b11 b10) (in b11 c5) (on b19 b11) (in b19 c5) (on b27 b19) (in b27 c5) (clear b27) (base b6) (in b6 c6) (on b6 c6) (on b3 b6) (in b3 c6) (on b2 b3) (in b2 c6) (on b13 b2) (in b13 c6) (on b1 b13) (in b1 c6) (on b17 b1) (in b17 c6) (on b8 b17) (in b8 c6) (on b24 b8) (in b24 c6) (on b4 b24) (in b4 c6) (on b18 b4) (in b18 c6) (on b22 b18) (in b22 c6) (on b7 b22) (in b7 c6) (on b28 b7) (in b28 c6) (on b12 b28) (in b12 c6) (on b14 b12) (in b14 c6) (clear b14) (clear c7) (clear c8))
 (:goal (and (on b13 c1) (clear b13) (on b9 c2) (on b15 b9) (on b4 b15) (on b21 b4) (clear b21) (on b20 c3) (on b16 b20) (on b14 b16) (on b12 b14) (on b17 b12) (clear b17) (on b26 c4) (on b10 b26) (on b3 b10) (on b8 b3) (on b18 b8) (on b22 b18) (clear b22) (on b1 c5) (on b5 b1) (on b11 b5) (on b6 b11) (on b28 b6) (on b25 b28) (on b19 b25) (on b7 b19) (on b24 b7) (on b2 b24) (on b27 b2) (on b23 b27) (clear b23)))
)
