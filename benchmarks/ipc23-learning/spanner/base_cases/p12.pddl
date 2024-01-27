(define (problem spanner-12)
 (:domain spanner)
 (:objects
    bob - man
    spanner1 spanner2 spanner3 - spanner
    nut1 nut2 nut3 - nut
    shed location1 location2 location3 gate - location
 )
 (:init
    (at bob shed)
    (at spanner1 location1)
    (at spanner2 location1)
    (at spanner3 location1)
    (usable spanner1)
    (usable spanner2)
    (usable spanner3)
    (at nut1 gate)
    (loose nut1)
    (at nut2 gate)
    (loose nut2)
    (at nut3 gate)
    (loose nut3)
    (link shed location1)
    (link location1 location2)
    (link location2 location3)
    (link location3 gate)
)
 (:goal  (and
    (tightened nut1)
    (tightened nut2)
    (tightened nut3))))
