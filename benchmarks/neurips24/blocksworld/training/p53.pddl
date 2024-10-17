(define (problem p53-problem)
 (:domain p53-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 - block
   c1 c2 c3 c4 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 4) (= (capacity c2) 0) (= (capacity c3) 10) (= (capacity c4) 10) (base b1) (in b1 c1) (on b1 c1) (on b5 b1) (in b5 c1) (on b4 b5) (in b4 c1) (on b10 b4) (in b10 c1) (on b8 b10) (in b8 c1) (on b2 b8) (in b2 c1) (clear b2) (base b9) (in b9 c2) (on b9 c2) (on b7 b9) (in b7 c2) (on b16 b7) (in b16 c2) (on b12 b16) (in b12 c2) (on b15 b12) (in b15 c2) (on b6 b15) (in b6 c2) (on b11 b6) (in b11 c2) (on b14 b11) (in b14 c2) (on b13 b14) (in b13 c2) (on b3 b13) (in b3 c2) (clear b3) (clear c3) (clear c4))
 (:goal (and (on b12 c1) (on b14 b12) (on b7 b14) (clear b7) (on b6 c2) (on b11 b6) (on b4 b11) (on b5 b4) (clear b5) (on b13 c3) (on b2 b13) (on b16 b2) (on b15 b16) (on b1 b15) (on b10 b1) (on b8 b10) (on b9 b8) (on b3 b9) (clear b3)))
)
