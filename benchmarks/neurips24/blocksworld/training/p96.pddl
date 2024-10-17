(define (problem p96-problem)
 (:domain p96-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 - block
   c1 c2 c3 c4 c5 c6 c7 c8 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 7) (= (capacity c2) 6) (= (capacity c3) 4) (= (capacity c4) 4) (= (capacity c5) 5) (= (capacity c6) 0) (= (capacity c7) 8) (= (capacity c8) 8) (base b2) (in b2 c1) (on b2 c1) (clear b2) (base b7) (in b7 c2) (on b7 c2) (on b5 b7) (in b5 c2) (clear b5) (base b15) (in b15 c3) (on b15 c3) (on b8 b15) (in b8 c3) (on b10 b8) (in b10 c3) (on b12 b10) (in b12 c3) (clear b12) (base b23) (in b23 c4) (on b23 c4) (on b16 b23) (in b16 c4) (on b22 b16) (in b22 c4) (on b21 b22) (in b21 c4) (clear b21) (base b11) (in b11 c5) (on b11 c5) (on b17 b11) (in b17 c5) (on b24 b17) (in b24 c5) (on b1 b24) (in b1 c5) (on b26 b1) (in b26 c5) (on b20 b26) (in b20 c5) (on b6 b20) (in b6 c5) (clear b6) (base b9) (in b9 c6) (on b9 c6) (on b3 b9) (in b3 c6) (on b25 b3) (in b25 c6) (on b14 b25) (in b14 c6) (on b27 b14) (in b27 c6) (on b4 b27) (in b4 c6) (on b28 b4) (in b28 c6) (on b13 b28) (in b13 c6) (on b18 b13) (in b18 c6) (on b19 b18) (in b19 c6) (clear b19) (clear c7) (clear c8))
 (:goal (and (on b13 c1) (clear b13) (on b4 c2) (on b23 b4) (on b7 b23) (on b15 b7) (clear b15) (on b8 c3) (on b5 b8) (on b10 b5) (on b19 b10) (on b6 b19) (clear b6) (on b9 c4) (on b1 b9) (on b28 b1) (on b11 b28) (on b3 b11) (on b2 b3) (clear b2) (on b25 c5) (on b21 b25) (on b17 b21) (on b14 b17) (on b22 b14) (on b18 b22) (on b26 b18) (on b20 b26) (on b16 b20) (on b27 b16) (on b24 b27) (on b12 b24) (clear b12)))
)
