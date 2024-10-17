(define (problem p92-problem)
 (:domain p92-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 9) (= (capacity c2) 7) (= (capacity c3) 12) (= (capacity c4) 0) (= (capacity c5) 12) (base b9) (in b9 c1) (on b9 c1) (on b11 b9) (in b11 c1) (on b2 b11) (in b2 c1) (on b23 b2) (in b23 c1) (clear b23) (base b10) (in b10 c2) (on b10 c2) (on b6 b10) (in b6 c2) (on b24 b6) (in b24 c2) (on b18 b24) (in b18 c2) (on b22 b18) (in b22 c2) (clear b22) (base b27) (in b27 c3) (on b27 c3) (on b1 b27) (in b1 c3) (on b7 b1) (in b7 c3) (on b16 b7) (in b16 c3) (on b19 b16) (in b19 c3) (on b12 b19) (in b12 c3) (clear b12) (base b25) (in b25 c4) (on b25 c4) (on b14 b25) (in b14 c4) (on b4 b14) (in b4 c4) (on b3 b4) (in b3 c4) (on b21 b3) (in b21 c4) (on b8 b21) (in b8 c4) (on b26 b8) (in b26 c4) (on b5 b26) (in b5 c4) (on b13 b5) (in b13 c4) (on b15 b13) (in b15 c4) (on b20 b15) (in b20 c4) (on b17 b20) (in b17 c4) (clear b17) (clear c5))
 (:goal (and (on b6 c1) (on b11 b6) (on b25 b11) (on b16 b25) (clear b16) (on b12 c2) (on b13 b12) (on b20 b13) (on b23 b20) (on b7 b23) (clear b7) (on b17 c3) (on b8 b17) (on b18 b8) (on b19 b18) (on b26 b19) (on b3 b26) (on b15 b3) (on b5 b15) (on b9 b5) (on b1 b9) (on b10 b1) (on b4 b10) (on b21 b4) (on b24 b21) (on b14 b24) (on b2 b14) (on b27 b2) (on b22 b27) (clear b22)))
)
