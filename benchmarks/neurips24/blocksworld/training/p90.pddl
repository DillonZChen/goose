(define (problem p90-problem)
 (:domain p90-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 11) (= (capacity c2) 4) (= (capacity c3) 0) (= (capacity c4) 14) (= (capacity c5) 11) (base b4) (in b4 c1) (on b4 c1) (clear b4) (base b8) (in b8 c2) (on b8 c2) (on b6 b8) (in b6 c2) (on b26 b6) (in b26 c2) (on b7 b26) (in b7 c2) (on b13 b7) (in b13 c2) (on b25 b13) (in b25 c2) (on b18 b25) (in b18 c2) (on b14 b18) (in b14 c2) (clear b14) (base b17) (in b17 c3) (on b17 c3) (on b11 b17) (in b11 c3) (on b12 b11) (in b12 c3) (on b27 b12) (in b27 c3) (on b23 b27) (in b23 c3) (on b21 b23) (in b21 c3) (on b1 b21) (in b1 c3) (on b3 b1) (in b3 c3) (on b22 b3) (in b22 c3) (on b24 b22) (in b24 c3) (on b2 b24) (in b2 c3) (on b15 b2) (in b15 c3) (on b19 b15) (in b19 c3) (on b10 b19) (in b10 c3) (on b5 b10) (in b5 c3) (on b9 b5) (in b9 c3) (on b16 b9) (in b16 c3) (on b20 b16) (in b20 c3) (clear b20) (clear c4) (clear c5))
 (:goal (and (on b21 c1) (clear b21) (on b13 c2) (clear b13) (on b24 c3) (on b14 b24) (on b7 b14) (on b3 b7) (on b25 b3) (on b4 b25) (on b27 b4) (on b26 b27) (on b19 b26) (on b16 b19) (on b17 b16) (clear b17) (on b18 c4) (on b11 b18) (on b8 b11) (on b23 b8) (on b5 b23) (on b12 b5) (on b1 b12) (on b9 b1) (on b10 b9) (on b2 b10) (on b15 b2) (on b6 b15) (on b20 b6) (on b22 b20) (clear b22)))
)
