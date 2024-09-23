;; base case
;; graph: l1 - l2
(define (problem transport-02)
 (:domain transport)
 (:objects
    v1 - vehicle
    p1 - package
    l1 l2 - location
    c0 c1 - size
    )
 (:init
    (capacity v1 c1)
    (capacity-predecessor c0 c1)
    (at p1 l1)
    (at v1 l2)
    (road l1 l2)
    (road l2 l1)
    )
 (:goal  (and
    (at p1 l2)
 )))
