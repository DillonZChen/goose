(define (problem p57-problem)
 (:domain p57-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 5) (= (capacity c2) 12) (= (capacity c3) 2) (= (capacity c4) 0) (= (capacity c5) 6) (base b6) (in b6 c1) (on b6 c1) (on b4 b6) (in b4 c1) (clear b4) (base b9) (in b9 c2) (on b9 c2) (on b11 b9) (in b11 c2) (clear b11) (base b10) (in b10 c3) (on b10 c3) (on b2 b10) (in b2 c3) (on b8 b2) (in b8 c3) (on b1 b8) (in b1 c3) (clear b1) (base b15) (in b15 c4) (on b15 c4) (on b13 b15) (in b13 c4) (on b7 b13) (in b7 c4) (on b5 b7) (in b5 c4) (on b3 b5) (in b3 c4) (on b16 b3) (in b16 c4) (on b12 b16) (in b12 c4) (on b17 b12) (in b17 c4) (on b14 b17) (in b14 c4) (clear b14) (clear c5))
 (:goal (and (on b8 c1) (on b14 b8) (on b12 b14) (clear b12) (on b4 c2) (on b1 b4) (on b17 b1) (on b15 b17) (on b7 b15) (on b3 b7) (on b5 b3) (on b2 b5) (on b11 b2) (on b10 b11) (on b6 b10) (on b16 b6) (on b9 b16) (on b13 b9) (clear b13)))
)
