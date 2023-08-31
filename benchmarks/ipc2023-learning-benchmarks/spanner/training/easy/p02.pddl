(define (problem spanner-02)
 (:domain spanner)
 (:objects 
    bob - man
    spanner1 spanner2 - spanner
    nut1 - nut
    shed location1 gate - location
 )
 (:init 
    (at bob shed)
    (at spanner1 location1)
    (usable spanner1)
    (at spanner2 location1)
    (usable spanner2)
    (at nut1 gate)
    (loose nut1)
    (link shed location1)
    (link location1 gate)
)
 (:goal  (and (tightened nut1))))
